#pragma once

namespace hgl
{
    /**
     * 星期每天枚举
     */
    enum class Weekday
    {
        Sunday=0,   ///<星期天
        Monday,     ///<星期一
        Tuesday,    ///<星期二
        Wednesday,  ///<星期三
        Thursday,   ///<星期四
        Friday,     ///<星期五
        Saturday,   ///<星期六

        BEGIN_RANGE =Sunday,
        END_RANGE   =Saturday,
        RANGE_SIZE  =END_RANGE-BEGIN_RANGE+1
    };

    /**
     * 月份枚举
     */
    enum class Month
    {
        January=1, ///<一月
        February,  ///<二月
        March,     ///<三月
        April,     ///<四月
        May,       ///<五月
        June,      ///<六月
        July,      ///<七月
        August,    ///<八月
        September, ///<九月
        October,   ///<十月
        November,  ///<十一月
        December,  ///<十二月

        BEGIN_RANGE =January,
        END_RANGE   =December,
        RANGE_SIZE  =END_RANGE-BEGIN_RANGE+1
    };//enum class Month

    constexpr uint  HGL_TIME_ONE_SECOND     =1;
    constexpr uint  HGL_TIME_HALF_MINUTE    =30;
    constexpr uint  HGL_TIME_ONE_MINUTE     =60;
    constexpr uint  HGL_TIME_HALF_HOUR      =30*HGL_TIME_ONE_MINUTE;
    constexpr uint  HGL_TIME_ONE_HOUR       =60*HGL_TIME_ONE_MINUTE;
    constexpr uint  HGL_TIME_HALF_DAY       =12*HGL_TIME_ONE_HOUR;
    constexpr uint  HGL_TIME_ONE_DAY        =24*HGL_TIME_ONE_HOUR;
    constexpr uint  HGL_TIME_ONE_WEEK       =7*HGL_TIME_ONE_DAY;
    constexpr uint  HGL_TIME_ONE_YEAR       =365*HGL_TIME_ONE_DAY;

    constexpr uint  HGL_HOUR_HALF_DAY       =12;
    constexpr uint  HGL_HOUR_ONE_DAY        =24;

    constexpr uint  HGL_DAY_ONE_WEEK        =7;
    constexpr uint  HGL_DAY_ONE_YEAR        =365;

    constexpr uint  HGL_MONTH_ONE_YEAR      =12;

    constexpr uint  HGL_MILLI_SEC_PRE_SEC   =1000;              //毫秒
    constexpr uint  HGL_MICRO_SEC_PER_SEC   =1000*1000;         //微秒
    constexpr uint  HGL_NANO_SEC_PER_SEC    =1000*1000*1000;    //纳秒
}//namespace hgl
