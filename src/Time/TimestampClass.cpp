#include <hgl/time/Timestamp.h>
#include <compare>

namespace hgl
{
    // ========================================================================
    // 构造函数 (Constructors)
    // ========================================================================

    Timestamp::Timestamp()
    {
        unix_us = GetTimeUs();
    }

    Timestamp::Timestamp(uint64 unix_microseconds)
    {
        unix_us = unix_microseconds;
    }

    Timestamp::Timestamp(const Timestamp &other)
    {
        unix_us = other.unix_us;
    }

    // ========================================================================
    // 静态工厂方法 (Static Factory Methods)
    // ========================================================================

    Timestamp Timestamp::Now()
    {
        return Timestamp(GetTimeUs());
    }

    Timestamp Timestamp::FromUnixSeconds(uint64 seconds)
    {
        return Timestamp(seconds * HGL_MICRO_SEC_PER_SEC);
    }

    Timestamp Timestamp::FromUnixMilliseconds(uint64 milliseconds)
    {
        return Timestamp(milliseconds * 1000);
    }

    Timestamp Timestamp::FromUnixMicroseconds(uint64 microseconds)
    {
        return Timestamp(microseconds);
    }

    Timestamp Timestamp::FromWindows(uint64 windows_timestamp)
    {
        return Timestamp(WindowsTimestampToUnix(windows_timestamp));
    }

    Timestamp Timestamp::FromUUIDv7(uint64 uuid7_timestamp)
    {
        uint64 unix_ms = UUIDv7TimestampToUnix(uuid7_timestamp);
        return Timestamp(unix_ms * 1000);
    }

    // ========================================================================
    // 格式转换输出 (Format Conversion Output)
    // ========================================================================

    uint64 Timestamp::ToUnixSeconds() const
    {
        return unix_us / HGL_MICRO_SEC_PER_SEC;
    }

    uint64 Timestamp::ToUnixMilliseconds() const
    {
        return unix_us / 1000;
    }

    uint64 Timestamp::ToUnixMicroseconds() const
    {
        return unix_us;
    }

    uint64 Timestamp::ToWindows() const
    {
        return UnixTimestampToWindows(unix_us);
    }

    uint64 Timestamp::ToUUIDv7() const
    {
        uint64 unix_ms = unix_us / 1000;
        return UnixTimestampToUUIDv7(unix_ms);
    }

    double Timestamp::ToDouble() const
    {
        return static_cast<double>(unix_us) / HGL_MICRO_SEC_PER_SEC;
    }

    // ========================================================================
    // 时间加法 - 返回新对象 (Time Addition - Return New Object)
    // ========================================================================

    Timestamp Timestamp::AddSeconds(int64 seconds) const
    {
        return Timestamp(unix_us + seconds * HGL_MICRO_SEC_PER_SEC);
    }

    Timestamp Timestamp::AddMinutes(int64 minutes) const
    {
        return Timestamp(unix_us + minutes * HGL_TIME_ONE_MINUTE * HGL_MICRO_SEC_PER_SEC);
    }

    Timestamp Timestamp::AddHours(int64 hours) const
    {
        return Timestamp(unix_us + hours * HGL_TIME_ONE_HOUR * HGL_MICRO_SEC_PER_SEC);
    }

    Timestamp Timestamp::AddDays(int64 days) const
    {
        return Timestamp(unix_us + days * HGL_TIME_ONE_DAY * HGL_MICRO_SEC_PER_SEC);
    }

    Timestamp Timestamp::AddWeeks(int64 weeks) const
    {
        return AddDays(weeks * HGL_DAY_ONE_WEEK);
    }

    Timestamp Timestamp::AddMonths(int64 months) const
    {
        // 简化处理：按30天计算
        return AddDays(months * 30);
    }

    Timestamp Timestamp::AddYears(int64 years) const
    {
        // 简化处理：按365天计算
        return AddDays(years * 365);
    }

    // ========================================================================
    // 时间减法 - 返回新对象 (Time Subtraction - Return New Object)
    // ========================================================================

    Timestamp Timestamp::SubSeconds(int64 seconds) const
    {
        return AddSeconds(-seconds);
    }

    Timestamp Timestamp::SubMinutes(int64 minutes) const
    {
        return AddMinutes(-minutes);
    }

    Timestamp Timestamp::SubHours(int64 hours) const
    {
        return AddHours(-hours);
    }

    Timestamp Timestamp::SubDays(int64 days) const
    {
        return AddDays(-days);
    }

    Timestamp Timestamp::SubWeeks(int64 weeks) const
    {
        return AddWeeks(-weeks);
    }

    Timestamp Timestamp::SubMonths(int64 months) const
    {
        return AddMonths(-months);
    }

    Timestamp Timestamp::SubYears(int64 years) const
    {
        return AddYears(-years);
    }

    // ========================================================================
    // 时间修改 - 修改自身 (Time Modification - Modify Self)
    // ========================================================================

    void Timestamp::AddSecondsInplace(int64 seconds)
    {
        unix_us += seconds * HGL_MICRO_SEC_PER_SEC;
    }

    void Timestamp::AddMinutesInplace(int64 minutes)
    {
        unix_us += minutes * HGL_TIME_ONE_MINUTE * HGL_MICRO_SEC_PER_SEC;
    }

    void Timestamp::AddHoursInplace(int64 hours)
    {
        unix_us += hours * HGL_TIME_ONE_HOUR * HGL_MICRO_SEC_PER_SEC;
    }

    void Timestamp::AddDaysInplace(int64 days)
    {
        unix_us += days * HGL_TIME_ONE_DAY * HGL_MICRO_SEC_PER_SEC;
    }

    void Timestamp::AddWeeksInplace(int64 weeks)
    {
        AddDaysInplace(weeks * HGL_DAY_ONE_WEEK);
    }

    void Timestamp::AddMonthsInplace(int64 months)
    {
        AddDaysInplace(months * 30);
    }

    void Timestamp::AddYearsInplace(int64 years)
    {
        AddDaysInplace(years * 365);
    }

    void Timestamp::SubSecondsInplace(int64 seconds)
    {
        AddSecondsInplace(-seconds);
    }

    void Timestamp::SubMinutesInplace(int64 minutes)
    {
        AddMinutesInplace(-minutes);
    }

    void Timestamp::SubHoursInplace(int64 hours)
    {
        AddHoursInplace(-hours);
    }

    void Timestamp::SubDaysInplace(int64 days)
    {
        AddDaysInplace(-days);
    }

    void Timestamp::SubWeeksInplace(int64 weeks)
    {
        AddWeeksInplace(-weeks);
    }

    void Timestamp::SubMonthsInplace(int64 months)
    {
        AddMonthsInplace(-months);
    }

    void Timestamp::SubYearsInplace(int64 years)
    {
        AddYearsInplace(-years);
    }

    // ========================================================================
    // 时间差计算 (Time Difference Calculation)
    // ========================================================================

    int64 Timestamp::DiffSeconds(const Timestamp &other) const
    {
        return static_cast<int64>((unix_us - other.unix_us) / HGL_MICRO_SEC_PER_SEC);
    }

    int64 Timestamp::DiffMinutes(const Timestamp &other) const
    {
        return DiffSeconds(other) / HGL_TIME_ONE_MINUTE;
    }

    int64 Timestamp::DiffHours(const Timestamp &other) const
    {
        return DiffSeconds(other) / HGL_TIME_ONE_HOUR;
    }

    int64 Timestamp::DiffDays(const Timestamp &other) const
    {
        return DiffSeconds(other) / HGL_TIME_ONE_DAY;
    }

    // ========================================================================
    // 比较操作符 (Comparison Operators)
    // ========================================================================

    bool Timestamp::operator==(const Timestamp &other) const
    {
        return unix_us == other.unix_us;
    }

    bool Timestamp::operator!=(const Timestamp &other) const
    {
        return unix_us != other.unix_us;
    }

    std::strong_ordering Timestamp::operator<=>(const Timestamp &other) const
    {
        return unix_us <=> other.unix_us;
    }

    // ========================================================================
    // 赋值操作符 (Assignment Operators)
    // ========================================================================

    Timestamp& Timestamp::operator=(const Timestamp &other)
    {
        if (this != &other)
        {
            unix_us = other.unix_us;
        }
        return *this;
    }

    // ========================================================================
    // 算术操作符 (Arithmetic Operators)
    // ========================================================================

    Timestamp Timestamp::operator+(int64 seconds) const
    {
        return AddSeconds(seconds);
    }

    Timestamp Timestamp::operator-(int64 seconds) const
    {
        return SubSeconds(seconds);
    }

    int64 Timestamp::operator-(const Timestamp &other) const
    {
        return DiffSeconds(other);
    }

    Timestamp& Timestamp::operator+=(int64 seconds)
    {
        AddSecondsInplace(seconds);
        return *this;
    }

    Timestamp& Timestamp::operator-=(int64 seconds)
    {
        SubSecondsInplace(seconds);
        return *this;
    }

}//namespace hgl
