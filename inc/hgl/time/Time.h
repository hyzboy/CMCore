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
}//namespace hgl
