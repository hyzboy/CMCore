#pragma once

#include<hgl/type/DataType.h>
#include<hgl/time/TimeConst.h>
#include<hgl/type/String.h>

namespace hgl
{
    /**
    * 时间类，这个类用来保存和计算时间。
    *
    * 当您对Hours,Minutes,Seconds中任何一个值进行加减时，其它值都会自动计算。如：11:30这个值，使用Minutes+=55。会自动计算出55分钟后的时间，值为12:25
    */
    class TimeOfDay:public Comparator<TimeOfDay>                                                                                  ///时间类
    {
        int32 gmt_off;      ///<当前时区与UTC时间的差值

        int8 hours;
        int8 minutes;
        int8 seconds;
        int32 micro_seconds;
        int8 week_day;      ///<为了取这个星期过了多少秒用的

    public:

        int GetGMT          ()const{return gmt_off;}                                                                    ///<取得当前时间与UTC时间的时差

        int GetHour         ()const{return hours;}                                                                      ///<时
        int GetMinute       ()const{return minutes;}                                                                    ///<分
        int GetSecond       ()const{return seconds;}                                                                    ///<秒
        int GetMicroSecond  ()const{return micro_seconds;}                                                              ///<微秒(1/1,000,000秒)

        int GetDaySeconds   ()const{return (hours*HGL_TIME_ONE_HOUR)+(minutes*HGL_TIME_ONE_MINUTE)+seconds;}            ///<今天过了多少秒

        int GetWeekSeconds  ()const                                                                                     ///<这个星期过了多少秒
        {
            int week = week_day;
            if(week_day > 0)
                --week;

            if(week_day == 0)
                week=(HGL_DAY_ONE_WEEK-1);

            return ((week*HGL_HOUR_ONE_DAY+hours)*HGL_TIME_ONE_HOUR)+(minutes*HGL_TIME_ONE_MINUTE)+seconds;
        }

//      int GetPastWeekSeconds()const{return int(GetPreciseTime()-GetWeekSeconds());}                                    ///<过去那些年到这周一0点过了多少秒

        void SetGMT(int go){gmt_off=go;}
        void SetHour(int);
        void SetMinute(int);
        void SetSecond(int);
        void SetMicroSecond(int);

    public:

        TimeOfDay();
        TimeOfDay(const double);
        TimeOfDay(int,int,int,int);
        TimeOfDay(const TimeOfDay &);

        void Clear()
        {
            gmt_off=0;
            hours=0;
            minutes=0;
            seconds=0;
            micro_seconds=0;
            week_day=0;
        }

        void Set(int h,int m=0,int s=0,int ms=0,int wd=0);

        TimeOfDay &operator = (const TimeOfDay &);

        const int compare(const TimeOfDay &)const override;

        void Sync(const double=0);                                                                  ///<和系统时间同步
    };//class TimeOfDay

    /**
    * 日期类，这个类用来保存和计算日期。带有闰年的处理
    *
    * 当您对Year,Month,Day中任意一个值进行修改时，其它值都会自动跟着计算。如1981-4-17,如果使用Day+=400，会自动计算出400天之后的日期，结果是1982-5-21
    */
    class CalendarDate:public Comparator<CalendarDate>                                                              ///日期类
    {
        int32 year;
        int8 month;
        int8 day;

        int8 week_day;
        int16 year_day;

        int8 max_days_per_month;

    public:

        int GetYear     ()const{return year;}                                                       ///<年
        int GetMonth    ()const{return month;}                                                      ///<月
        int GetDay      ()const{return day;}                                                        ///<日
        int GetMaxDays  ()const{return max_days_per_month;}                                         ///<本月天数
        int GetWeekDay  ()const{return week_day;}                                                   ///<星期,0为星期天
        int GetYearDay  ()const{return year_day;}                                                   ///<今天为这一年的第几天

        void SetYear    (int y){year=y;}
        void SetMonth   (int);
        void SetDay     (int);

    public:

        CalendarDate();
        CalendarDate(const double);
        CalendarDate(int,int,int);
        CalendarDate(const CalendarDate &);

        void Clear()
        {
            year=0;
            month=0;
            day=0;

            week_day=0;
            year_day=0;

            max_days_per_month=0;
        }

        void Set(int y,int m,int d,int wd=0,int yd=0);

        CalendarDate &operator = (const CalendarDate &);

        const int compare(const CalendarDate &)const override;

        void Sync(const double=0);                                                                  ///<和系统日期同步

        int DayOfYear()const;
    };//class CalendarDate

    /**
     * 根据当前时间，转换出Date/Time两个结构
     */
    void ToDateTime(CalendarDate &,TimeOfDay &,const double cur_time=0);

    int DayOfYear(int year,int month,int day);                                                      ///<计算某年某月某日是该年的第几天

    /**
     * 根据日期和时间，转换出以秒为单位的时间值
     * @param year 年
     * @param month 月
     * @param day 日
     * @param hour 小时
     * @param minute 分
     * @param second 秒
     * @param micro_second 微秒(1/1,000,000秒)
     * @return 转换出的值
     */
    double FromDateTime(const int year,const int month,const int day,
                        const int hour,const int minute=0,const int second=0,const int micro_second=0);

    /**
     * 根据Date/Time转换出以秒为单位的时间值
     */
    double FromDateTime(const CalendarDate &,const TimeOfDay &);

    struct DateTime
    {
        CalendarDate date;
        TimeOfDay time;
    };

    /**
     * @defgroup DateTimeFormat CalendarDate/TimeOfDay 格式化和字符串转换
     * @{
     */

    /**
     * 日期格式选项
     */
    struct DateFormat
    {
        AnsiString pattern;    ///< 格式模式，支持: YYYY(年), MM(月), DD(日)
                                ///< 示例: "YYYY-MM-DD", "DD/MM/YYYY", "YYYY/MM/DD"

        DateFormat(const AnsiString &p = "YYYY-MM-DD") : pattern(p) {}
    };

    /**
     * 时间格式选项
     */
    struct TimeFormat
    {
        AnsiString pattern;    ///< 格式模式，支持: HH(时), MM(分), SS(秒), ms(毫秒), us(微秒)
                                ///< 示例: "HH:MM:SS", "HH:MM:SS.ms", "HH:MM:SS.us"

        TimeFormat(const AnsiString &p = "HH:MM:SS") : pattern(p) {}
    };

    /**
     * 日期时间格式选项
     */
    struct DateTimeFormat
    {
        DateFormat dateFormat;
        TimeFormat timeFormat;
        AnsiString separator;  ///< 日期和时间的分隔符，默认为空格 " "

        DateTimeFormat(const DateFormat &df = DateFormat(),
                      const TimeFormat &tf = TimeFormat(),
                      const AnsiString &sep = " ")
            : dateFormat(df), timeFormat(tf), separator(sep) {}
    };

    /**
     * 将Date转换为字符串
     * @param date 日期对象
     * @param format 日期格式选项
     * @return 转换后的字符串，如: "2024-01-15"
     */
    AnsiString DateToString(const CalendarDate &date, const DateFormat &format = DateFormat());

    /**
     * 将Time转换为字符串
     * @param time 时间对象
     * @param format 时间格式选项
     * @return 转换后的字符串，如: "14:30:45"
     */
    AnsiString TimeToString(const TimeOfDay &time, const TimeFormat &format = TimeFormat());

    /**
     * 将Date和Time组合转换为字符串
     * @param date 日期对象
     * @param time 时间对象
     * @param format 日期时间格式选项
     * @return 转换后的字符串，如: "2024-01-15 14:30:45"
     */
    AnsiString DateTimeToString(const CalendarDate &date, const TimeOfDay &time, 
                                 const DateTimeFormat &format = DateTimeFormat());

    /**
     * 将DateTime结构转换为字符串
     * @param dt DateTime结构体
     * @param format 日期时间格式选项
     * @return 转换后的字符串
     */
    AnsiString DateTimeToString(const DateTime &dt, const DateTimeFormat &format = DateTimeFormat());
}//namespace hgl
