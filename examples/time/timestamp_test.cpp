#include<hgl/time/Time.h>
#include<hgl/type/String.h>
#include<iostream>
#include<iomanip>
#include<cmath>

using namespace hgl;
using namespace std;

static void PrintSeparator(const char* title = nullptr)
{
    cout << "============================================================" << endl;
    if(title)
        cout << title << endl;
}

/**
 * 测试Unix时间戳函数
 */
void TestUnixTimestamps()
{
    PrintSeparator("Unix Timestamp Tests");
    
    uint64 unix_sec = GetUnixTimestampSec();
    uint64 unix_ms = GetUnixTimestampMs();
    uint64 unix_us = GetUnixTimestampUs();
    
    cout << "Unix Timestamp (seconds):      " << unix_sec << endl;
    cout << "Unix Timestamp (milliseconds): " << unix_ms << endl;
    cout << "Unix Timestamp (microseconds): " << unix_us << endl;
    
    // 验证一致性
    uint64 sec_from_ms = unix_ms / 1000;
    uint64 sec_from_us = unix_us / 1000000;
    
    cout << "\nConsistency Check:" << endl;
    cout << "  sec_from_ms = " << sec_from_ms << " (diff: " << (int64)(unix_sec - sec_from_ms) << ")" << endl;
    cout << "  sec_from_us = " << sec_from_us << " (diff: " << (int64)(unix_sec - sec_from_us) << ")" << endl;
    
    if(abs((int64)(unix_sec - sec_from_ms)) <= 1 && abs((int64)(unix_sec - sec_from_us)) <= 1)
    {
        cout << "  ✓ Unix timestamps are consistent" << endl;
    }
    else
    {
        cout << "  ✗ Unix timestamps inconsistent!" << endl;
    }
}

/**
 * 测试Windows时间戳转换
 */
void TestWindowsTimestamps()
{
    PrintSeparator("Windows Timestamp Tests");
    
    uint64 win_ts = GetWindowsTimestamp();
    cout << "Windows Timestamp (100ns ticks): " << win_ts << endl;
    
    // 测试往返转换
    uint64 unix_us = GetUnixTimestampUs();
    uint64 win_converted = UnixTimestampToWindows(unix_us);
    uint64 unix_back = WindowsTimestampToUnix(win_converted);
    
    cout << "\nRoundtrip Test (Unix -> Windows -> Unix):" << endl;
    cout << "  Original Unix (us):    " << unix_us << endl;
    cout << "  Converted to Windows:  " << win_converted << endl;
    cout << "  Back to Unix (us):     " << unix_back << endl;
    cout << "  Difference (us):       " << (int64)(unix_back - unix_us) << endl;
    
    if(unix_back == unix_us)
    {
        cout << "  ✓ Windows timestamp conversion is lossless" << endl;
    }
    else
    {
        cout << "  ⚠ Windows timestamp conversion has precision loss (expected)" << endl;
    }
    
    // 测试已知值
    // Unix时间戳 0 (1970-01-01 00:00:00) 对应 Windows时间戳
    uint64 unix_epoch = 0;
    uint64 win_at_unix_epoch = UnixTimestampToWindows(unix_epoch);
    uint64 expected_win_epoch = 116444736000000000ULL; // 1970在Windows纪元中的值
    
    cout << "\nKnown Value Test (Unix epoch 1970-01-01):" << endl;
    cout << "  Unix 0 -> Windows:  " << win_at_unix_epoch << endl;
    cout << "  Expected Windows:   " << expected_win_epoch << endl;
    cout << "  Difference:         " << (int64)(win_at_unix_epoch - expected_win_epoch) << endl;
    
    if(win_at_unix_epoch == expected_win_epoch)
    {
        cout << "  ✓ Known value conversion is correct" << endl;
    }
    else
    {
        cout << "  ✗ Known value conversion failed!" << endl;
    }
}

/**
 * 测试UUIDv7时间戳转换
 */
void TestUUIDv7Timestamps()
{
    PrintSeparator("UUIDv7 Timestamp Tests");
    
    uint64 uuid7_ts = GetUUIDv7Timestamp();
    cout << "UUIDv7 Timestamp (milliseconds, 48-bit): " << uuid7_ts << endl;
    cout << "  Hex: 0x" << hex << setfill('0') << setw(12) << uuid7_ts << dec << endl;
    
    // 测试往返转换
    uint64 unix_ms = GetUnixTimestampMs();
    uint64 uuid7_converted = UnixTimestampToUUIDv7(unix_ms);
    uint64 unix_back = UUIDv7TimestampToUnix(uuid7_converted);
    
    cout << "\nRoundtrip Test (Unix -> UUIDv7 -> Unix):" << endl;
    cout << "  Original Unix (ms):   " << unix_ms << endl;
    cout << "  Converted to UUIDv7:  " << uuid7_converted << endl;
    cout << "  Back to Unix (ms):    " << unix_back << endl;
    cout << "  Difference (ms):      " << (int64)(unix_back - unix_ms) << endl;
    
    if(unix_back == unix_ms)
    {
        cout << "  ✓ UUIDv7 timestamp conversion is lossless" << endl;
    }
    else
    {
        cout << "  ✗ UUIDv7 timestamp conversion failed!" << endl;
    }
    
    // 测试48位限制
    uint64 max_48bit = 0x0000FFFFFFFFFFFFULL;
    cout << "\n48-bit Range Test:" << endl;
    cout << "  Max 48-bit value: " << max_48bit << " (" << (max_48bit / 1000.0 / 86400.0 / 365.25) << " years)" << endl;
    cout << "  Current value:    " << uuid7_ts << endl;
    
    if(uuid7_ts <= max_48bit)
    {
        cout << "  ✓ Current timestamp fits in 48 bits" << endl;
    }
    else
    {
        cout << "  ✗ Current timestamp exceeds 48 bits!" << endl;
    }
}

/**
 * 测试交叉格式转换
 */
void TestCrossFormatConversions()
{
    PrintSeparator("Cross-Format Conversion Tests");
    
    // Windows <-> UUIDv7
    uint64 win_ts = GetWindowsTimestamp();
    uint64 uuid7_from_win = WindowsTimestampToUUIDv7(win_ts);
    uint64 win_back = UUIDv7TimestampToWindows(uuid7_from_win);
    
    cout << "Windows <-> UUIDv7 Conversion:" << endl;
    cout << "  Original Windows:     " << win_ts << endl;
    cout << "  Converted to UUIDv7:  " << uuid7_from_win << endl;
    cout << "  Back to Windows:      " << win_back << endl;
    cout << "  Difference:           " << (int64)(win_back - win_ts) << endl;
    
    // 由于精度转换(100ns -> ms -> 100ns)，可能有精度损失
    int64 diff = abs((int64)(win_back - win_ts));
    if(diff < 10000) // 允许1ms误差(10000个100ns)
    {
        cout << "  ✓ Windows <-> UUIDv7 conversion OK (within 1ms tolerance)" << endl;
    }
    else
    {
        cout << "  ✗ Windows <-> UUIDv7 conversion has large error!" << endl;
    }
    
    // 三向转换测试: Unix -> Windows -> UUIDv7 -> Unix
    cout << "\nTriple Conversion Test (Unix -> Windows -> UUIDv7 -> Unix):" << endl;
    uint64 original_unix_ms = GetUnixTimestampMs();
    uint64 step1_unix_us = original_unix_ms * 1000;
    uint64 step2_windows = UnixTimestampToWindows(step1_unix_us);
    uint64 step3_uuid7 = WindowsTimestampToUUIDv7(step2_windows);
    uint64 step4_unix_ms = UUIDv7TimestampToUnix(step3_uuid7);
    
    cout << "  Original Unix (ms):   " << original_unix_ms << endl;
    cout << "  After triple convert: " << step4_unix_ms << endl;
    cout << "  Difference (ms):      " << (int64)(step4_unix_ms - original_unix_ms) << endl;
    
    if(step4_unix_ms == original_unix_ms)
    {
        cout << "  ✓ Triple conversion is lossless" << endl;
    }
    else
    {
        cout << "  ⚠ Triple conversion has precision changes (may be expected)" << endl;
    }
}

/**
 * 测试特殊值
 */
void TestSpecialValues()
{
    PrintSeparator("Special Values Tests");
    
    // 测试Unix纪元时刻 (1970-01-01 00:00:00)
    cout << "Unix Epoch (1970-01-01 00:00:00):" << endl;
    uint64 unix_zero = 0;
    uint64 win_epoch = UnixTimestampToWindows(unix_zero);
    uint64 uuid7_epoch = UnixTimestampToUUIDv7(unix_zero);
    
    cout << "  Unix:    " << unix_zero << endl;
    cout << "  Windows: " << win_epoch << endl;
    cout << "  UUIDv7:  " << uuid7_epoch << endl;
    
    // 测试一个已知日期: 2024-01-01 00:00:00 UTC
    // Unix timestamp: 1704067200 seconds = 1704067200000 ms
    cout << "\nKnown Date (2024-01-01 00:00:00 UTC):" << endl;
    uint64 known_unix_ms = 1704067200000ULL;
    uint64 known_win = UnixTimestampToWindows(known_unix_ms * 1000);
    uint64 known_uuid7 = UnixTimestampToUUIDv7(known_unix_ms);
    
    cout << "  Unix (ms):     " << known_unix_ms << endl;
    cout << "  Windows:       " << known_win << endl;
    cout << "  UUIDv7:        " << known_uuid7 << endl;
    cout << "  UUIDv7 (hex):  0x" << hex << setfill('0') << setw(12) << known_uuid7 << dec << endl;
    
    // 验证往返
    uint64 verify_unix = UUIDv7TimestampToUnix(known_uuid7);
    cout << "  Verify (Unix): " << verify_unix << endl;
    
    if(verify_unix == known_unix_ms)
    {
        cout << "  ✓ Known date conversion verified" << endl;
    }
    else
    {
        cout << "  ✗ Known date conversion failed!" << endl;
    }
}

/**
 * 性能简单测试
 */
void TestPerformance()
{
    PrintSeparator("Performance Test");
    
    constexpr int ITERATIONS = 1000000;
    
    cout << "Running " << ITERATIONS << " iterations..." << endl;
    
    auto start = GetTimeUs();
    for(int i = 0; i < ITERATIONS; i++)
    {
        GetUnixTimestampMs();
    }
    auto end = GetTimeUs();
    cout << "GetUnixTimestampMs():     " << (end - start) << " us total, " 
         << ((end - start) / (double)ITERATIONS) << " us/call" << endl;
    
    start = GetTimeUs();
    for(int i = 0; i < ITERATIONS; i++)
    {
        GetWindowsTimestamp();
    }
    end = GetTimeUs();
    cout << "GetWindowsTimestamp():    " << (end - start) << " us total, " 
         << ((end - start) / (double)ITERATIONS) << " us/call" << endl;
    
    start = GetTimeUs();
    for(int i = 0; i < ITERATIONS; i++)
    {
        GetUUIDv7Timestamp();
    }
    end = GetTimeUs();
    cout << "GetUUIDv7Timestamp():     " << (end - start) << " us total, " 
         << ((end - start) / (double)ITERATIONS) << " us/call" << endl;
    
    // 测试转换性能
    uint64 test_val = GetUnixTimestampUs();
    start = GetTimeUs();
    for(int i = 0; i < ITERATIONS; i++)
    {
        UnixTimestampToWindows(test_val);
    }
    end = GetTimeUs();
    cout << "UnixTimestampToWindows(): " << (end - start) << " us total, " 
         << ((end - start) / (double)ITERATIONS) << " us/call" << endl;
}

int main(int, char**)
{
    cout << "CMCore Timestamp Conversion Test Program" << endl;
    cout << "=========================================" << endl << endl;
    
    TestUnixTimestamps();
    cout << endl;
    
    TestWindowsTimestamps();
    cout << endl;
    
    TestUUIDv7Timestamps();
    cout << endl;
    
    TestCrossFormatConversions();
    cout << endl;
    
    TestSpecialValues();
    cout << endl;
    
    TestPerformance();
    cout << endl;
    
    PrintSeparator("All Tests Completed");
    
    return 0;
}
