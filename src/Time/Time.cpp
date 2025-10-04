#include<hgl/time/DateTime.h>
#include<hgl/platform/Platform.h>
#include<hgl/time/TimeConst.h>

#include <chrono>

namespace hgl
{
    Time::Time()
    {
        gmt_off=0;
        hours=0;
        minutes=0;
        seconds=0;
        micro_seconds=0;
    }

    Time::Time(const double t)
    {
        Sync(t);
    }

    Time::Time(int h,int m,int s,int ms)
    {
        gmt_off=0;
        hours=h;
        minutes=m;
        seconds=s;
        micro_seconds=ms;
    }

    Time::Time(const Time &t)
    {
        operator = (t);
    }

    /**
     * 设置时间
     * @param h 小时
     * @param m 分
     * @param s 秒
     * @param ms 微秒(百分分之一秒)
     */
    void Time::Set(int h,int m,int s,int ms,int wd)
    {
        hours=h;
        minutes=m;
        seconds=s;
        micro_seconds=ms;
        week_day=wd;
    }

    Time &Time::operator=(const Time &t)
    {
        hours            =t.hours;
        minutes            =t.minutes;
        seconds            =t.seconds;
        micro_seconds    =t.micro_seconds;

        return(*this);
    }

    const int Time::compare(const Time &t)const
    {
        if(hours!=t.hours)
            return hours-t.hours;

        if(minutes!=t.minutes)
            return minutes-t.minutes;

        if(seconds!=t.seconds)
            return seconds-t.seconds;

        return micro_seconds-t.micro_seconds;
    }

    void Time::SetHour(int h)
    {
        if(h<0)
        {
            while(h<0)
                h+=HGL_HOUR_ONE_DAY;
        }
        else if(h>0)
        {
            while(h>(HGL_HOUR_ONE_DAY-1))
                h-=HGL_HOUR_ONE_DAY;
        }

        hours=h;
    }

    void Time::SetMinute(int m)
    {
        int h=hours;

        if(m<0)
        {
            while(m<0)
            {
                m+=HGL_TIME_ONE_MINUTE;
                h--;
            }
        }
        else if(m>0)
        {
            while(m>(HGL_TIME_ONE_MINUTE-1))
            {
                m-=HGL_TIME_ONE_MINUTE;
                h++;
            }
        }

        minutes=m;
        SetHour(h);
    }

    void Time::SetSecond(int s)
    {
        int m=minutes;

        if(s<0)
        {
            while(s<0)
            {
                s+=HGL_TIME_ONE_MINUTE;
                m--;
            }
        }
        else if(s>0)
        {
            while(s>(HGL_TIME_ONE_MINUTE-1))
            {
                s-=HGL_TIME_ONE_MINUTE;
                m++;
            }
        }

        seconds=s;
        SetMinute(m);
    }

    void Time::SetMicroSecond(int ms)
    {
        int s=seconds;

        if(ms<0)
        {
            while(ms<0)
            {
                ms-=HGL_MICRO_SEC_PER_SEC;
                s--;
            }
        }
        else if(ms>0)
        {
            while(ms>(HGL_MICRO_SEC_PER_SEC-1))
            {
                ms-=HGL_MICRO_SEC_PER_SEC;
                s++;
            }
        }

        micro_seconds=ms;
        SetSecond(s);
    }

    // 获取与UTC偏移(秒)
    static int32 CalcGMTOffset(const std::tm &local_tm)
    {
        std::tm temp_local = local_tm;
        std::tm temp_gm    = local_tm;
        // mktime assumes tm is local time
        time_t local_sec = std::mktime(&temp_local);
        // Build gm tm by using gmtime_r / gmtime_s on local_sec
        std::tm gm_tm{};
#if defined(_WIN32)
        gmtime_s(&gm_tm,&local_sec);
#else
        gmtime_r(&local_sec,&gm_tm);
#endif
        time_t gm_sec = std::mktime(&gm_tm); // this interprets gm_tm as local, so diff gives offset
        return int32(difftime(local_sec, gm_sec));
    }

    void Time::Sync(const double t)
    {
        double use_time = t;
        using namespace std::chrono;
        if(use_time==0)
        {
            auto now = system_clock::now();
            auto us  = time_point_cast<microseconds>(now).time_since_epoch().count();
            use_time = double(us)/1'000'000.0;
        }

        time_t sec_part = time_t(std::floor(use_time));
        double frac = use_time - double(sec_part);
        if(frac<0){ frac+=1.0; --sec_part; }

        std::tm lt{};
#if defined(_WIN32)
        localtime_s(&lt,&sec_part);
#else
        localtime_r(&sec_part,&lt);
#endif
        hours = int8(lt.tm_hour);
        minutes = int8(lt.tm_min);
        seconds = int8(lt.tm_sec);
        micro_seconds = int32(std::lround(frac*HGL_MICRO_SEC_PER_SEC));
        if(micro_seconds>=HGL_MICRO_SEC_PER_SEC){micro_seconds-=HGL_MICRO_SEC_PER_SEC; seconds++;}
        week_day = int8(lt.tm_wday); // 0=Sunday
        gmt_off = CalcGMTOffset(lt);
    }
}//namespace hgl
