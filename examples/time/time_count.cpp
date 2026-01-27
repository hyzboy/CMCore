#include<hgl/time/Time.h>
#include<hgl/time/DateTime.h>
#include<hgl/type/String.h>
#include<iostream>
#include<iomanip>

using namespace hgl;
using namespace std;

constexpr const size_t TIME_OUT_FRAC=6;     //小数输出位数

static void PrintSeparator()
{
    cout << "------------------------------------------------------------" << endl;
}

static const char *WeekDayName(int wd)
{
    // CalendarDate::GetWeekDay: 0=Sunday
    static const char *names[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};
    if(wd<0 || wd>6) return "Unknown";
    return names[wd];
}

int main(int,char **)
{
    PrintSeparator();
    cout << "Basic current time / roundtrip test" << endl;

    const double cur_time=GetTimeSec();

    CalendarDate d;
    TimeOfDay t;

    ToDateTime(d,t,cur_time);

    AnsiString str=AnsiString::floatOf(cur_time,TIME_OUT_FRAC);

    cout<<"cur_time="<<str.c_str()<<endl;
    cout<<"\tyear="<<d.GetYear()<<" month="<<d.GetMonth()<<" day="<<d.GetDay()<<endl;
    cout<<"\thour="<<t.GetHour()<<" minute="<<t.GetMinute()<<" second="<<t.GetSecond()<<" micro_second="<<t.GetMicroSecond()<<endl;
    cout<<endl;

    const double new_time=FromDateTime(d,t);

    str=AnsiString::floatOf(new_time,TIME_OUT_FRAC);
    cout<<"new_time="<<str.c_str()<<endl;

    cout<<"TimeZone:"<<GetTimeZoneOffset()<<endl;
    PrintSeparator();

    // Roundtrip equality check (allow small epsilon due to possible DST/localtime rounding)
    double diff = fabs(new_time - cur_time);
    cout << "Roundtrip diff = " << setprecision(6) << fixed << diff << " seconds" << endl;

    PrintSeparator();
    cout << "Testing ToDateTime with cur_time == 0 (uses system now)" << endl;

    CalendarDate d_now;
    TimeOfDay t_now;
    ToDateTime(d_now, t_now, 0);
    cout << "now: " << d_now.GetYear() << '-' << d_now.GetMonth() << '-' << d_now.GetDay()
         << ' ' << int(t_now.GetHour()) << ':' << int(t_now.GetMinute()) << ':' << int(t_now.GetSecond())
         << " (micro=" << t_now.GetMicroSecond() << ")" << endl;
    PrintSeparator();

    // DayOfYear tests
    cout << "DayOfYear tests:" << endl;
    CalendarDate dy1(2021,1,1); cout << "2021-01-01 -> " << dy1.DayOfYear() << " (expect 1)" << endl;
    CalendarDate dy2(2021,2,28); cout << "2021-02-28 -> " << dy2.DayOfYear() << " (expect 59)" << endl;
    CalendarDate dy3(2020,2,29); cout << "2020-02-29 -> " << dy3.DayOfYear() << " (expect 60, leap year)" << endl;
    CalendarDate dy4(2019,12,31); cout << "2019-12-31 -> " << dy4.DayOfYear() << " (expect 365)" << endl;
    PrintSeparator();

    // DayOfWeek tests
    cout << "DayOfWeek tests (GetWeekDay):" << endl;
    CalendarDate wd1(2021,1,1); cout << "2021-01-01 -> " << WeekDayName(wd1.GetWeekDay()) << " (expect Friday)" << endl;
    CalendarDate wd2(2021,9,23); cout << "2021-09-23 -> " << WeekDayName(wd2.GetWeekDay()) << " (expect Thursday)" << endl;
    CalendarDate wd3(2000,1,1); cout << "2000-01-01 -> " << WeekDayName(wd3.GetWeekDay()) << " (expect Saturday)" << endl;
    CalendarDate wd4(1970,1,1); cout << "1970-01-01 -> " << WeekDayName(wd4.GetWeekDay()) << " (expect Thursday)" << endl;
    CalendarDate wd5(2020,2,29); cout << "2020-02-29 -> " << WeekDayName(wd5.GetWeekDay()) << " (expect Saturday)" << endl;
    PrintSeparator();

    // SetMonth / SetDay edge cases
    cout << "SetMonth / SetDay edge cases:" << endl;
    CalendarDate dm1(2021,1,31); cout << "start: 2021-01-31" << endl;
    dm1.SetMonth(2); // sets month to February but does not automatically adjust day
    cout << "after SetMonth(2): year="<<dm1.GetYear()<<" month="<<dm1.GetMonth()<<" day="<<dm1.GetDay()
         << " max_days="<<dm1.GetMaxDays() << " (day may be > max_days)" << endl;

    CalendarDate dd1(2021,1,31);
    dd1.SetDay(32); // should advance into next month
    cout << "SetDay(32) on 2021-01-31 -> "<< dd1.GetYear() << '-' << dd1.GetMonth() << '-' << dd1.GetDay() << endl;

    CalendarDate dd2(2020,1,31);
    dd2.SetDay(31 + 31 + 1); // advance by 62 days
    cout << "2020-01-31 SetDay(63) -> "<< dd2.GetYear() << '-' << dd2.GetMonth() << '-' << dd2.GetDay() << endl;
    PrintSeparator();

    // Leap year SetDay around feb
    cout << "Leap year handling:" << endl;
    CalendarDate ly1(2000,2,28); ly1.SetDay(29); cout << "2000-02-28 SetDay(29)-> "<<ly1.GetYear()<<'-'<<ly1.GetMonth()<<'-'<<ly1.GetDay()<<" (2000 is leap)"<<endl;
    CalendarDate ly2(1900,2,28); ly2.SetDay(29); cout << "1900-02-28 SetDay(29)-> "<<ly2.GetYear()<<'-'<<ly2.GetMonth()<<'-'<<ly2.GetDay()<<" (1900 not leap)"<<endl;
    PrintSeparator();

    // TimeOfDay microsecond and overflow tests
    cout << "TimeOfDay microsecond / overflow tests:" << endl;
    TimeOfDay tm1; tm1.Set(1,2,3,500000);
    cout << "tm1 initial: 01:02:03." << tm1.GetMicroSecond() << endl;

    TimeOfDay tm2; tm2.Set(23,59,59,999999);
    cout << "tm2 initial: 23:59:59."<<tm2.GetMicroSecond()<<" (max before roll)"<<endl;

    // Large microsecond (overflow into seconds)
    TimeOfDay tm3; tm3.Set(0,0,0,0);
    tm3.SetMicroSecond(1500000); // 1.5 seconds -> should increment seconds
    cout << "tm3 after SetMicroSecond(1500000): "<< tm3.GetHour() << ':'<< tm3.GetMinute() << ':'<< tm3.GetSecond() << "."<< tm3.GetMicroSecond() << endl;

    // Negative microsecond (underflow into previous second) -- behavior depends on implementation
    TimeOfDay tm4; tm4.Set(0,0,1,250000);
    tm4.SetMicroSecond(-500000); // expect to move to previous second (0.x)
    cout << "tm4 after SetMicroSecond(-500000): "<< tm4.GetHour() << ':'<< tm4.GetMinute() << ':'<< tm4.GetSecond() << "."<< tm4.GetMicroSecond() << endl;
    PrintSeparator();

    // FromDateTime explicit values and roundtrip checks
    cout << "Explicit FromDateTime tests and roundtrips:" << endl;
    CalendarDate ed(1999,12,31); TimeOfDay et(23,59,59,123456);
    double epoch_like = FromDateTime(ed, et);
    cout << "1999-12-31 23:59:59.123456 -> time="<< AnsiString::floatOf(epoch_like, TIME_OUT_FRAC).c_str() << endl;

    CalendarDate rd; TimeOfDay rt;
    ToDateTime(rd, rt, epoch_like);
    cout << "roundtrip -> "<< rd.GetYear()<<'-'<<rd.GetMonth()<<'-'<<rd.GetDay() << ' ' << rt.GetHour() << ':' << rt.GetMinute() << ':' << rt.GetSecond() << "." << rt.GetMicroSecond() << endl;
    PrintSeparator();

    cout << "All tests completed." << endl;
    return(0);
}
