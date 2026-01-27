#pragma once

/**
 * @file TimeConst.h
 * @brief 时间常量定义 (Time Constants Definition)
 * 
 * 此文件定义了时间相关的常量，用于时间计算和转换
 * This file defines time-related constants for time calculations and conversions
 */

namespace hgl
{
    // 基础时间单位常量 (Basic Time Unit Constants)
    constexpr int64  HGL_MICRO_SEC_PER_SEC      = 1000000;          ///<微秒每秒 (microseconds per second)
    constexpr int64  HGL_MILLI_SEC_PER_SEC      = 1000;             ///<毫秒每秒 (milliseconds per second)
    constexpr int64  HGL_NANO_SEC_PER_SEC       = 1000000000;       ///<纳秒每秒 (nanoseconds per second)
    constexpr int64  HGL_NANO_SEC_PER_MICRO     = 1000;             ///<纳秒每微秒 (nanoseconds per microsecond)
    
    // 时间单位常量 (Time Unit Constants)
    constexpr int    HGL_TIME_ONE_MINUTE        = 60;               ///<一分钟的秒数 (seconds per minute)
    constexpr int    HGL_TIME_ONE_HOUR          = 3600;             ///<一小时的秒数 (seconds per hour)
    constexpr int    HGL_TIME_ONE_DAY           = 86400;            ///<一天的秒数 (seconds per day)
    
    // 日历单位常量 (Calendar Unit Constants)
    constexpr int    HGL_HOUR_ONE_DAY           = 24;               ///<一天的小时数 (hours per day)
    constexpr int    HGL_DAY_ONE_WEEK           = 7;                ///<一周的天数 (days per week)
    constexpr int    HGL_MONTH_ONE_YEAR         = 12;               ///<一年的月数 (months per year)
    
    // Windows FILETIME 时间戳常量 (Windows FILETIME Constants)
    // Windows FILETIME: 100纳秒单位，从1601年1月1日00:00:00 UTC开始
    // Windows FILETIME: 100-nanosecond intervals since January 1, 1601 00:00:00 UTC
    constexpr int64  HGL_WIN_TICKS_PER_SEC      = 10000000;         ///<Windows时间刻度每秒 (100ns ticks per second)
    constexpr int64  HGL_WIN_TO_UNIX_EPOCH_SEC  = 11644473600LL;    ///<Windows纪元到Unix纪元的秒数差 (1601->1970)
    
    // UUIDv7 时间戳常量 (UUIDv7 Timestamp Constants)  
    // UUIDv7: 毫秒时间戳，从Unix纪元开始
    // UUIDv7: millisecond timestamp since Unix epoch
    constexpr int    HGL_UUID7_TIMESTAMP_BITS   = 48;               ///<UUIDv7时间戳位数 (timestamp bits in UUIDv7)
}//namespace hgl
