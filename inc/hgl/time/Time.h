#pragma once

#include<hgl/type/DataType.h>

namespace hgl   //时间
{
    using PreciseTime=double;

    int GetTimeZone();                                                                              ///<返回时区的时差(单位：秒)

    uint64 GetMilliStartTime();                                                                     ///<取得毫秒程序启动时间(单位：1/1000秒)
    uint64 GetMicroStartTime();                                                                     ///<取得微秒程序启动时间(单位：1/1000000秒)
    PreciseTime GetPreciseStartTime();                                                              ///<取得秒程序启动时间(单位：秒)

    uint64 GetTime();                                                                               ///<取得当前时间(单位：1/1000秒)
    uint64 GetMicroTime();                                                                          ///<取得当前时间(单位：1/1000000秒)
    PreciseTime GetPreciseTime();                                                                   ///<取得当前时间(单位：秒)

    PreciseTime GetLocaPreciseTime();                                                               ///<取得本地当前时间(单位：秒)

    void WaitTime(const PreciseTime &);                                                             ///<等待一定时间(单位：秒)
}//namespace hgl
