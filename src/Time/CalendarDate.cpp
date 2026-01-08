#include<hgl/type/DataType.h>
#include<hgl/time/DateTime.h>

#include<time.h>
#include<chrono>
#include<cmath>

//--------------------------------------------------------------------------------------------------
namespace hgl
{
    // Helper: day of year
    int DayOfYear(int year,int month,int day)
    {
        static const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        int doy = 0;
        for(int m=1;m<month && m<=12;++m)
        {
            doy += mdays[m-1];
            if(m==2)
            {
                // February: add leap day if leap year
                bool leap = (year%4==0 && (year%100!=0 || year%400==0));
                if(leap) doy += 1;
            }
        }
        doy += day;
        return doy;
    }

    CalendarDate::CalendarDate(const double t)
    {
        Sync(t);
    }

    CalendarDate::CalendarDate(const CalendarDate &t)
    {
        operator = (t);
    }

    CalendarDate &CalendarDate::operator=(const CalendarDate &t)
    {
        year    =t.year;
        month    =t.month;
        day        =t.day;
        max_days_per_month=t.max_days_per_month;
        week_day=t.week_day;
        year_day=t.year_day;

        return(*this);
    }

    const int CalendarDate::compare(const CalendarDate &t)const
    {
        if(year!=t.year)
            return year-t.year;

        if(month!=t.month)
            return month-t.month;

        return day-t.day;
    }

    CalendarDate::CalendarDate()
    {
        year=2000;
        month=1;
        day=1;
        week_day=0;
        year_day=0;
        max_days_per_month=0;
    }

    CalendarDate::CalendarDate(int y,int m,int d)
    {
        Set(y,m,d);
    }

    void CalendarDate::Set(int y,int m,int d,int wd,int yd)
    {
        day=d;
        month=m;
        year=y;
        week_day=wd;
        year_day=yd;
        max_days_per_month=0;
    }

    void CalendarDate::SetMonth(int m)
    {
        if(m<1)
        {
            while(m<1)
            {
                m+=HGL_MONTH_ONE_YEAR;
                year--;
            }
        }
        else
        {
            while(m>HGL_MONTH_ONE_YEAR)
            {
                m-=HGL_MONTH_ONE_YEAR;
                year++;
            }
        }

        month=m;

        const int md[HGL_MONTH_ONE_YEAR]={31,0,31,30,31,30,31,31,30,31,30,31};

        if(month==2)
        {
            // Use full Gregorian leap year rules
            bool leap = (year%4==0 && (year%100!=0 || year%400==0));
            max_days_per_month = leap ? 29 : 28;
        }
        else
            max_days_per_month=md[month-1];
    }

    void CalendarDate::SetDay(int d)
    {
        int m=month;

        if(d<1)
        {
            while(d<1)
            {
                d+=max_days_per_month;
                m--;
            }
        }
        else
        {
            while(d>max_days_per_month)
            {
                d-=max_days_per_month;
                m++;
            }
        }

        day=d;
        SetMonth(m);
    }

    int CalendarDate::DayOfYear()const
    {
        return hgl::DayOfYear(year,month,day);
    }

    void CalendarDate::Sync(const double t)
    {
        double use_time = t;
        using namespace std::chrono;
        if(use_time==0)
        {
            auto now = system_clock::now();
            auto sec = time_point_cast<seconds>(now).time_since_epoch().count();
            use_time = double(sec);
        }
        time_t sec_part = time_t(std::floor(use_time));
        std::tm lt{};
#if defined(_WIN32)
        localtime_s(&lt,&sec_part);
#else
        localtime_r(&sec_part,&lt);
#endif
        year = lt.tm_year + 1900;
        month = lt.tm_mon + 1;
        day = lt.tm_mday;
        week_day = int8(lt.tm_wday); // 0 Sunday
        year_day = int16(hgl::DayOfYear(year,month,day));
        // compute max days of this month
        const int mdays[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
        if(month==2)
        {
            bool leap = (year%4==0 && (year%100!=0 || year%400==0));
            max_days_per_month = leap ? 29 : 28;
        }
        else
            max_days_per_month = mdays[month-1];
    }
}//namespace hgl
