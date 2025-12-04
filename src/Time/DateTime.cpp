#include<hgl/time/TimeConst.h>
#include<hgl/time/DateTime.h>

#include<chrono>
#include<cmath>

namespace hgl
{
    double FromDateTime(const Date &d,const Time &t)
    {
        return FromDateTime(d.GetYear(),d.GetMonth(),d.GetDay(),
                            t.GetHour(),t.GetMinute(),t.GetSecond(),t.GetMicroSecond());
    }

    double FromDateTime(const int year,const int month,const int day,
                        const int hour,const int minute,const int second,const int micro_second)
    {
        std::tm tmv{};
        tmv.tm_year = year - 1900;
        tmv.tm_mon  = month - 1;
        tmv.tm_mday = day;
        tmv.tm_hour = hour;
        tmv.tm_min  = minute;
        tmv.tm_sec  = second;
        tmv.tm_isdst = -1; // let system determine DST
        time_t base = std::mktime(&tmv); // local time to epoch
        if(base == time_t(-1))
            return 0.0;
        double result = double(base) + double(micro_second)/double(HGL_MICRO_SEC_PER_SEC);
        return result;
    }

    void ToDateTime(Date &d,Time &t,const double cur_time)
    {
        double use_time = cur_time;
        if(use_time==0)
        {
            using namespace std::chrono;
            auto now = system_clock::now();
            auto us  = time_point_cast<microseconds>(now).time_since_epoch().count();
            use_time = double(us)/1'000'000.0;
        }
        d.Sync(use_time);
        t.Sync(use_time);
    }
}//namespace hgl
