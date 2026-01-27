#pragma once

#include<hgl/CoreType.h>

namespace hgl   //时间
{
    using PreciseTime=double;

    int GetTimeZoneOffset();                                                                        ///<返回时区偏移(单位：秒)

    // System Time (since Unix epoch) - 系统时间(自Unix纪元1970-01-01 00:00:00 UTC起)
    uint64 GetTimeMs();                                                                             ///<取得当前系统时间(单位：毫秒 millisecond, 1/1000秒)
    uint64 GetTimeUs();                                                                             ///<取得当前系统时间(单位：微秒 microsecond, 1/1000000秒)
    PreciseTime GetTimeSec();                                                                       ///<取得当前系统时间(单位：秒 second, 高精度浮点)
    PreciseTime GetLocalTimeSec();                                                                  ///<取得当前本地时间(单位：秒 second, 已应用时区偏移)

    // Program Uptime (since program start) - 程序运行时间(自程序启动起)
    uint64 GetUptimeMs();                                                                           ///<取得程序运行时间(单位：毫秒 millisecond, 1/1000秒)
    uint64 GetUptimeUs();                                                                           ///<取得程序运行时间(单位：微秒 microsecond, 1/1000000秒)
    PreciseTime GetUptimeSec();                                                                     ///<取得程序运行时间(单位：秒 second, 高精度浮点)

    // Utility - 工具函数
    void SleepSecond(PreciseTime seconds);                                                          ///<休眠指定时间(单位：秒 second)

    // ========================================================================
    // Timestamp Conversion Functions - 时间戳转换函数
    // ========================================================================
    
    /**
     * @defgroup TimestampConversion 时间戳转换函数组
     * @brief 支持Unix、Windows、UUIDv7三种时间戳格式的获取和相互转换
     * @{
     */
    
    // Unix Timestamps - Unix时间戳 (自1970-01-01 00:00:00 UTC起)
    uint64 GetUnixTimestampSec();                                                                   ///<获取Unix时间戳(单位：秒)
    uint64 GetUnixTimestampMs();                                                                    ///<获取Unix时间戳(单位：毫秒)
    uint64 GetUnixTimestampUs();                                                                    ///<获取Unix时间戳(单位：微秒)
    
    // Windows Timestamps - Windows时间戳 (自1601-01-01 00:00:00 UTC起的100纳秒间隔数)
    uint64 GetWindowsTimestamp();                                                                   ///<获取Windows FILETIME时间戳(100纳秒单位)
    uint64 UnixTimestampToWindows(uint64 unix_us);                                                  ///<Unix时间戳转Windows时间戳(输入：微秒)
    uint64 WindowsTimestampToUnix(uint64 win_timestamp);                                            ///<Windows时间戳转Unix时间戳(输出：微秒)
    
    // UUIDv7 Timestamps - UUIDv7时间戳 (Unix纪元起的毫秒数，48位)
    uint64 GetUUIDv7Timestamp();                                                                    ///<获取UUIDv7时间戳(毫秒，48位)
    uint64 UnixTimestampToUUIDv7(uint64 unix_ms);                                                   ///<Unix时间戳转UUIDv7时间戳(输入：毫秒)
    uint64 UUIDv7TimestampToUnix(uint64 uuid7_timestamp);                                           ///<UUIDv7时间戳转Unix时间戳(输出：毫秒)
    
    // Cross-format Conversions - 交叉格式转换
    uint64 WindowsTimestampToUUIDv7(uint64 win_timestamp);                                          ///<Windows时间戳转UUIDv7时间戳
    uint64 UUIDv7TimestampToWindows(uint64 uuid7_timestamp);                                        ///<UUIDv7时间戳转Windows时间戳
    
    /** @} */ // end of TimestampConversion group
}//namespace hgl
