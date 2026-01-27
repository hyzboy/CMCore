#include <hgl/time/Time.h>
#include <hgl/time/TimeConst.h>
#include <chrono>
#include <thread>
#include <ctime>

namespace hgl
{
    long GetGMTOff();

    namespace
    {
        uint64 program_start_time=0;        ///< 程序启动时的系统时间(单位：微秒 1/1000000秒)

        long gmt_off=0;                     ///< 时区偏移值(单位：微秒 1/1000000秒) windows下为long

        struct TimeInit
        {
            TimeInit()
            {
                program_start_time=GetTimeUs();     // 记录程序启动时间戳

                gmt_off=GetGMTOff();                // 计算时区偏移
            }
        };//struct TimeInit

        static TimeInit time_init;          ///< 静态初始化，程序启动时自动执行
    }//namespace

    /**
     * 返回时区偏移
     * @return 时区偏移(单位：秒 second)
     * @note 内部存储为微秒，返回时转换为秒
     */
    int GetTimeZoneOffset()
    {
        return gmt_off / HGL_MICRO_SEC_PER_SEC;     // 微秒转秒 (1/1000000 -> 1)
    }

    // ========================================================================
    // 使用 C++11/C++20 std::chrono 重新实现，保持原有接口与语义
    // ========================================================================

    /**
    * 取得当前与GMT的偏移(UTC-Local)
    * @return 时区偏移(单位：微秒 microsecond, 1/1000000秒)
    * @note 返回值 = GMT时间 - 本地时间 (东时区为负值，西时区为正值)
    * @example 北京时间(UTC+8)返回 -28800000000 微秒 (-8小时)
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
        localtime_s(&local_tm, &tt);                    // 转换为本地时间结构
        gmtime_s(&gm_tm, &tt);                          // 转换为UTC时间结构
        std::tm local_copy = local_tm;                  // mktime/_mkgmtime 可能改写内容，所以复制
        std::tm gm_copy = gm_tm;
        time_t local_sec = std::mktime(&local_copy);    // 将本地时间结构解释为本地时间戳
        time_t gm_sec = _mkgmtime(&gm_copy);            // 将UTC时间结构解释为UTC时间戳
#else
        local_tm = *std::localtime(&tt);                // POSIX线程不安全版本
        gm_tm    = *std::gmtime(&tt);
        std::tm local_copy = local_tm;
        std::tm gm_copy = gm_tm;
        time_t local_sec = std::mktime(&local_copy);    // 本地时间戳
        time_t gm_sec = timegm(&gm_copy);               // UTC时间戳(GNU扩展)
#endif
        // 计算偏移：UTC时间 - 本地时间 = -(本地时区偏移)
        // 例如：北京时间(UTC+8)，gm_sec比local_sec小8小时，diff_sec = -8*3600
        const long diff_sec = static_cast<long>(gm_sec - local_sec);
        return diff_sec * 1000000L;                     // 秒转微秒 (1 -> 1/1000000)
    }

    /**
    * 取得当前系统时间
    * @return 当前时间(单位：微秒 microsecond, 1/1000000秒)
    * @note 自Unix纪元(1970-01-01 00:00:00 UTC)起的微秒数
    */
    uint64 GetTimeUs()
    {
        using namespace std::chrono;
        return static_cast<uint64>(duration_cast<microseconds>(system_clock::now().time_since_epoch()).count());
    }

    /**
    * 取得当前系统时间
    * @return 当前时间(单位：毫秒 millisecond, 1/1000秒)
    * @note 自Unix纪元(1970-01-01 00:00:00 UTC)起的毫秒数
    */
    uint64 GetTimeMs()
    {
        return GetTimeUs()/1000ULL;                     // 微秒转毫秒 (1/1000000 -> 1/1000)
    }

    /**
    * 取得当前系统时间(高精度)
    * @return 当前时间(单位：秒 second, 双精度浮点)
    * @note 自Unix纪元(1970-01-01 00:00:00 UTC)起的秒数，保留小数精度
    */
    PreciseTime GetTimeSec()
    {
        return PreciseTime(GetTimeUs())/HGL_MICRO_SEC_PER_SEC;  // 微秒转秒 (1/1000000 -> 1)
    }

    /**
    * 休眠指定时间
    * @param seconds 时间(单位：秒 second, 支持小数)
    * @note 若参数<=0则不休眠
    */
    void SleepSecond(PreciseTime seconds)
    {
        if(seconds<=0) return;
        std::this_thread::sleep_for(std::chrono::duration<PreciseTime>(seconds));
    }

    /**
    * 取得本地当前时间(高精度)
    * @return 当前本地时间(单位：秒 second, 双精度浮点)
    * @note 返回值 = UTC时间 + 时区偏移
    * @example UTC时间为 1000.5秒，北京时区(+8小时)，返回 1000.5 + 28800 = 29800.5秒
    */
    PreciseTime GetLocalTimeSec()
    {
        return GetTimeSec()+GetTimeZoneOffset();        // UTC时间 + 时区偏移(秒)
    }

    /**
    * 取得程序运行时间
    * @return 程序运行时间(单位：毫秒 millisecond, 1/1000秒)
    * @note 自程序启动(TimeInit构造)起的毫秒数
    */
    uint64 GetUptimeMs()
    {
        return (GetTimeUs()-program_start_time)/1000ULL;    // 微秒差转毫秒 (1/1000000 -> 1/1000)
    }

    /**
    * 取得程序运行时间
    * @return 程序运行时间(单位：微秒 microsecond, 1/1000000秒)
    * @note 自程序启动(TimeInit构造)起的微秒数
    */
    uint64 GetUptimeUs()
    {
        return GetTimeUs()-program_start_time;              // 微秒差值
    }

    /**
    * 取得程序运行时间(高精度)
    * @return 程序运行时间(单位：秒 second, 双精度浮点)
    * @note 自程序启动(TimeInit构造)起的秒数，保留小数精度
    */
    PreciseTime GetUptimeSec()
    {
        return PreciseTime(GetTimeUs()-program_start_time)/HGL_MICRO_SEC_PER_SEC;  // 微秒差转秒 (1/1000000 -> 1)
    }
}//namespace hgl
