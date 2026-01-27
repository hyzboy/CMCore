#include <hgl/time/Time.h>
#include <hgl/time/TimeConst.h>

namespace hgl
{
    // ========================================================================
    // Unix Timestamp Functions - Unix时间戳函数
    // ========================================================================

    /**
     * 获取当前Unix时间戳(秒)
     * @return Unix时间戳，单位：秒(自1970-01-01 00:00:00 UTC起)
     */
    uint64 GetUnixTimestampSec()
    {
        return GetTimeUs() / HGL_MICRO_SEC_PER_SEC;
    }

    /**
     * 获取当前Unix时间戳(毫秒)
     * @return Unix时间戳，单位：毫秒(自1970-01-01 00:00:00 UTC起)
     */
    uint64 GetUnixTimestampMs()
    {
        return GetTimeMs();
    }

    /**
     * 获取当前Unix时间戳(微秒)
     * @return Unix时间戳，单位：微秒(自1970-01-01 00:00:00 UTC起)
     */
    uint64 GetUnixTimestampUs()
    {
        return GetTimeUs();
    }

    // ========================================================================
    // Windows Timestamp Functions - Windows时间戳函数
    // ========================================================================

    /**
     * 将Unix时间戳(微秒)转换为Windows时间戳
     * @param unix_us Unix时间戳，单位：微秒
     * @return Windows时间戳，单位：100纳秒间隔
     * @note 转换公式: win_ticks = (unix_us + epoch_diff * 1000000) * 10
     */
    uint64 UnixTimestampToWindows(uint64 unix_us)
    {
        // Unix纪元(1970)到Windows纪元(1601)的秒数差: 11644473600秒
        // 转换步骤:
        // 1. 将Unix微秒时间戳转换为从Windows纪元开始的微秒数
        // 2. 将微秒转换为100纳秒单位(乘以10)
        uint64 unix_us_from_win_epoch = unix_us + (HGL_WIN_TO_UNIX_EPOCH_SEC * HGL_MICRO_SEC_PER_SEC);
        return unix_us_from_win_epoch * 10; // 微秒 -> 100纳秒
    }

    /**
     * 获取当前Windows FILETIME时间戳
     * @return Windows时间戳，单位：100纳秒间隔(自1601-01-01 00:00:00 UTC起)
     * @note Windows FILETIME是从1601年1月1日开始的100纳秒间隔数
     */
    uint64 GetWindowsTimestamp()
    {
        return UnixTimestampToWindows(GetTimeUs());
    }

    /**
     * 将Windows时间戳转换为Unix时间戳(微秒)
     * @param win_timestamp Windows时间戳，单位：100纳秒间隔
     * @return Unix时间戳，单位：微秒
     * @note 转换公式: unix_us = (win_ticks / 10) - (epoch_diff * 1000000)
     */
    uint64 WindowsTimestampToUnix(uint64 win_timestamp)
    {
        // 转换步骤:
        // 1. 将Windows 100纳秒单位转换为微秒(除以10)
        // 2. 减去Windows纪元到Unix纪元的微秒数差
        uint64 us_from_win_epoch = win_timestamp / 10; // 100纳秒 -> 微秒
        uint64 epoch_diff_us = HGL_WIN_TO_UNIX_EPOCH_SEC * HGL_MICRO_SEC_PER_SEC;
        return us_from_win_epoch - epoch_diff_us;
    }

    // ========================================================================
    // UUIDv7 Timestamp Functions - UUIDv7时间戳函数
    // ========================================================================

    /**
     * 将Unix时间戳(毫秒)转换为UUIDv7时间戳
     * @param unix_ms Unix时间戳，单位：毫秒
     * @return UUIDv7时间戳(48位有效)
     * @note UUIDv7时间戳直接使用Unix毫秒时间戳，但只保留低48位
     */
    uint64 UnixTimestampToUUIDv7(uint64 unix_ms)
    {
        // UUIDv7的时间戳就是Unix毫秒时间戳的低48位
        // 48位可以表示从1970到约8925年的时间
        return unix_ms & HGL_UUID7_TIMESTAMP_MASK;
    }

    /**
     * 获取当前UUIDv7时间戳
     * @return UUIDv7时间戳，单位：毫秒(自1970-01-01 00:00:00 UTC起，48位)
     * @note UUIDv7使用48位存储毫秒时间戳，可表示约8925年的时间范围
     */
    uint64 GetUUIDv7Timestamp()
    {
        return UnixTimestampToUUIDv7(GetTimeMs());
    }

    /**
     * 将UUIDv7时间戳转换为Unix时间戳(毫秒)
     * @param uuid7_timestamp UUIDv7时间戳
     * @return Unix时间戳，单位：毫秒
     * @note UUIDv7时间戳直接就是Unix毫秒时间戳(48位有效)
     */
    uint64 UUIDv7TimestampToUnix(uint64 uuid7_timestamp)
    {
        // UUIDv7时间戳本身就是Unix毫秒时间戳
        // 为了保证安全，屏蔽高位(虽然按规范应该为0)
        return uuid7_timestamp & HGL_UUID7_TIMESTAMP_MASK;
    }

    // ========================================================================
    // Cross-format Conversion Functions - 交叉格式转换函数
    // ========================================================================

    /**
     * 将Windows时间戳转换为UUIDv7时间戳
     * @param win_timestamp Windows时间戳，单位：100纳秒间隔
     * @return UUIDv7时间戳(毫秒，48位)
     * @note 转换路径: Windows -> Unix(微秒) -> Unix(毫秒) -> UUIDv7
     */
    uint64 WindowsTimestampToUUIDv7(uint64 win_timestamp)
    {
        uint64 unix_us = WindowsTimestampToUnix(win_timestamp);
        uint64 unix_ms = unix_us / 1000; // 微秒 -> 毫秒
        return UnixTimestampToUUIDv7(unix_ms);
    }

    /**
     * 将UUIDv7时间戳转换为Windows时间戳
     * @param uuid7_timestamp UUIDv7时间戳
     * @return Windows时间戳，单位：100纳秒间隔
     * @note 转换路径: UUIDv7 -> Unix(毫秒) -> Unix(微秒) -> Windows
     */
    uint64 UUIDv7TimestampToWindows(uint64 uuid7_timestamp)
    {
        uint64 unix_ms = UUIDv7TimestampToUnix(uuid7_timestamp);
        uint64 unix_us = unix_ms * 1000; // 毫秒 -> 微秒
        return UnixTimestampToWindows(unix_us);
    }

}//namespace hgl
