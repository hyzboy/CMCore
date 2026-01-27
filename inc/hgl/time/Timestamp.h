#pragma once

#include<hgl/time/Time.h>
#include<hgl/time/TimeConst.h>
#include<compare>

namespace hgl
{
    /**
     * @class Timestamp
     * @brief 时间戳类，支持Unix、Windows、UUIDv7三种格式的存储、转换和算术运算
     * 
     * 该类提供了统一的时间戳接口，可以方便地在不同格式之间转换，
     * 并提供明确的时间加减函数（按秒、分、时、天、年等）
     */
    class Timestamp
    {
    private:
        uint64 unix_us;     ///<内部统一使用Unix微秒时间戳存储
        
    public:
        // ========================================================================
        // 构造函数 (Constructors)
        // ========================================================================
        
        Timestamp();                                                                ///<默认构造，使用当前时间
        explicit Timestamp(uint64 unix_microseconds);                          ///<从Unix微秒时间戳构造
        Timestamp(const Timestamp &other);                                          ///<拷贝构造
        
        // ========================================================================
        // 静态工厂方法 (Static Factory Methods)
        // ========================================================================
        
        static Timestamp Now();                                                     ///<创建当前时间的时间戳对象
        static Timestamp FromUnixSeconds(uint64 seconds);                           ///<从Unix秒创建
        static Timestamp FromUnixMilliseconds(uint64 milliseconds);                 ///<从Unix毫秒创建
        static Timestamp FromUnixMicroseconds(uint64 microseconds);                 ///<从Unix微秒创建
        static Timestamp FromWindows(uint64 windows_timestamp);                     ///<从Windows FILETIME创建
        static Timestamp FromUUIDv7(uint64 uuid7_timestamp);                        ///<从UUIDv7时间戳创建
        
        // ========================================================================
        // 格式转换输出 (Format Conversion Output)
        // ========================================================================
        
        uint64 ToUnixSeconds() const;                                               ///<转换为Unix秒
        uint64 ToUnixMilliseconds() const;                                          ///<转换为Unix毫秒
        uint64 ToUnixMicroseconds() const;                                          ///<转换为Unix微秒
        uint64 ToWindows() const;                                                   ///<转换为Windows FILETIME
        uint64 ToUUIDv7() const;                                                    ///<转换为UUIDv7时间戳
        double ToDouble() const;                                                    ///<转换为浮点秒（兼容现有API）
        
        // ========================================================================
        // 时间加法 - 返回新对象 (Time Addition - Return New Object)
        // ========================================================================
        
        Timestamp AddSeconds(int64 seconds) const;                                  ///<加秒数
        Timestamp AddMinutes(int64 minutes) const;                                  ///<加分钟数
        Timestamp AddHours(int64 hours) const;                                      ///<加小时数
        Timestamp AddDays(int64 days) const;                                        ///<加天数
        Timestamp AddWeeks(int64 weeks) const;                                      ///<加周数
        Timestamp AddMonths(int64 months) const;                                    ///<加月数(按30天计算)
        Timestamp AddYears(int64 years) const;                                      ///<加年数(按365天计算)
        
        // ========================================================================
        // 时间减法 - 返回新对象 (Time Subtraction - Return New Object)
        // ========================================================================
        
        Timestamp SubSeconds(int64 seconds) const;                                  ///<减秒数
        Timestamp SubMinutes(int64 minutes) const;                                  ///<减分钟数
        Timestamp SubHours(int64 hours) const;                                      ///<减小时数
        Timestamp SubDays(int64 days) const;                                        ///<减天数
        Timestamp SubWeeks(int64 weeks) const;                                      ///<减周数
        Timestamp SubMonths(int64 months) const;                                    ///<减月数(按30天计算)
        Timestamp SubYears(int64 years) const;                                      ///<减年数(按365天计算)
        
        // ========================================================================
        // 时间修改 - 修改自身 (Time Modification - Modify Self)
        // ========================================================================
        
        void AddSecondsInplace(int64 seconds);                                      ///<原地加秒数
        void AddMinutesInplace(int64 minutes);                                      ///<原地加分钟数
        void AddHoursInplace(int64 hours);                                          ///<原地加小时数
        void AddDaysInplace(int64 days);                                            ///<原地加天数
        void AddWeeksInplace(int64 weeks);                                          ///<原地加周数
        void AddMonthsInplace(int64 months);                                        ///<原地加月数
        void AddYearsInplace(int64 years);                                          ///<原地加年数
        
        void SubSecondsInplace(int64 seconds);                                      ///<原地减秒数
        void SubMinutesInplace(int64 minutes);                                      ///<原地减分钟数
        void SubHoursInplace(int64 hours);                                          ///<原地减小时数
        void SubDaysInplace(int64 days);                                            ///<原地减天数
        void SubWeeksInplace(int64 weeks);                                          ///<原地减周数
        void SubMonthsInplace(int64 months);                                        ///<原地减月数
        void SubYearsInplace(int64 years);                                          ///<原地减年数
        
        // ========================================================================
        // 时间差计算 (Time Difference Calculation)
        // ========================================================================
        
        int64 DiffSeconds(const Timestamp &other) const;                            ///<计算秒数差(this - other)
        int64 DiffMinutes(const Timestamp &other) const;                            ///<计算分钟数差(this - other)
        int64 DiffHours(const Timestamp &other) const;                              ///<计算小时数差(this - other)
        int64 DiffDays(const Timestamp &other) const;                               ///<计算天数差(this - other)
        
        // ========================================================================
        // 比较操作符 (Comparison Operators)
        // ========================================================================
        
        bool operator==(const Timestamp &other) const;
        bool operator!=(const Timestamp &other) const;
        std::strong_ordering operator<=>(const Timestamp &other) const;
        
        // ========================================================================
        // 赋值操作符 (Assignment Operators)
        // ========================================================================
        
        Timestamp& operator=(const Timestamp &other);
        
        // ========================================================================
        // 算术操作符 (Arithmetic Operators)
        // ========================================================================
        
        Timestamp operator+(int64 seconds) const;                                   ///<加秒数（操作符重载）
        Timestamp operator-(int64 seconds) const;                                   ///<减秒数（操作符重载）
        int64 operator-(const Timestamp &other) const;                              ///<时间戳相减，返回秒数差
        
        Timestamp& operator+=(int64 seconds);                                       ///<加秒数（原地）
        Timestamp& operator-=(int64 seconds);                                       ///<减秒数（原地）
        
    };//class Timestamp
    
}//namespace hgl
