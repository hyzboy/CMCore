#include <hgl/time/Time.h>
#include <chrono>
#include <thread>
#include <ctime>

namespace hgl
{
    long GetGMTOff();

    namespace
    {
        uint64 program_start_time=0;

        long gmt_off=0;            //windows下为long

        struct TimeInit
        {
            TimeInit()
            {
                program_start_time=GetMicroTime();

                gmt_off=GetGMTOff();
            }
        };//struct TimeInit

        static TimeInit time_init;
    }//namespace

    /**
     * 返回时区时差
     */
    int GetTimeZone()
    {
        return gmt_off;
    }
    
    // 使用 C++20 std::chrono 重新实现，保持原有接口与语义

    /**
    * 取得当前与GMT的偏移(utc-local)单位:微秒 (保持原Win实现的符号与单位)
    */
    long GetGMTOff()
    {
        using namespace std;
        using namespace std::chrono;

        // 取当前时间点
        const system_clock::time_point now = system_clock::now();
        time_t tt = system_clock::to_time_t(now);

        std::tm local_tm{};
        std::tm gm_tm{};
#if defined(_WIN32)
        localtime_s(&local_tm, &tt);
        gmtime_s(&gm_tm, &tt);
        std::tm local_copy = local_tm;              // mktime/_mkgmtime 可能改写内容，所以复制
        std::tm gm_copy = gm_tm;
        time_t local_sec = std::mktime(&local_copy);    // 解释为本地时间 -> epoch(本地转换到UTC秒值)
        time_t gm_sec = _mkgmtime(&gm_copy);            // 解释为UTC时间
#else
        local_tm = *std::localtime(&tt);
        gm_tm    = *std::gmtime(&tt);
        std::tm local_copy = local_tm;
        std::tm gm_copy = gm_tm;
        time_t local_sec = std::mktime(&local_copy);    // 本地
        time_t gm_sec = timegm(&gm_copy);               // UTC
#endif
        // Windows旧实现返回: utc_time - local_time (微秒)
        const long diff_sec = static_cast<long>(gm_sec - local_sec); // = -(local_offset_seconds)
        return diff_sec * 1000000L; // 微秒
    }

    /**
    * 取得当前时间
    * @return 当前时间(单位：百万分之一秒)
    */
    uint64 GetMicroTime()
    {
        using namespace std::chrono;
        return static_cast<uint64>(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count());
    }

    /**
    * 取得当前时间
    * @return 当前时间(单位：千分之一秒)
    */
    uint64 GetTime()
    {
        return GetMicroTime()/1000ULL;
    }

    /**
    * 取得当前时间(双精度)
    * @return 当前时间(单位：秒)
    */
    PreciseTime GetPreciseTime()                                                                        ///<取得当前时间(双精度，单位秒)
    {
        return PreciseTime(GetMicroTime())/HGL_MICRO_SEC_PER_SEC;
    }

    /**
    * 等待指定时间
    * @param t 时间(单位：秒)
    */
    void WaitTime(const PreciseTime &t)
    {
        if(t<=0) return;
        std::this_thread::sleep_for(std::chrono::duration<PreciseTime>(t));
    }

    /**
    * 取得本地当前时间(双精度)
    * @return 当前时间(单位：秒)
    */
    double GetLocaPreciseTime()
    {
        return GetPreciseTime()+GetTimeZone();
    }

    uint64 GetMilliStartTime()
    {
        return program_start_time/1000;
    }

    uint64 GetMicroStartTime()
    {
        return program_start_time;
    }

    double GetPreciseStartTime()
    {
        return double(program_start_time)/HGL_MICRO_SEC_PER_SEC;
    }
}//namespace hgl
