#include <hgl/time/Timestamp.h>
#include <hgl/type/String.h>
#include <iostream>
#include <iomanip>

using namespace hgl;
using namespace std;

static void PrintSeparator(const char* title = nullptr)
{
    cout << "============================================================" << endl;
    if(title)
        cout << title << endl;
}

/**
 * 测试基本构造和转换
 */
void TestBasicConstruction()
{
    PrintSeparator("Basic Construction & Conversion Tests");

    // 测试默认构造（当前时间）
    Timestamp ts_now;
    cout << "Current time (Unix us): " << ts_now.ToUnixMicroseconds() << endl;
    cout << "Current time (Unix ms): " << ts_now.ToUnixMilliseconds() << endl;
    cout << "Current time (Unix s):  " << ts_now.ToUnixSeconds() << endl;

    // 测试静态工厂方法
    Timestamp ts_from_sec = Timestamp::FromUnixSeconds(1737945600);
    cout << "\nFrom Unix seconds (1737945600):" << endl;
    cout << "  To Unix us: " << ts_from_sec.ToUnixMicroseconds() << endl;
    cout << "  To Unix ms: " << ts_from_sec.ToUnixMilliseconds() << endl;
    cout << "  To Unix s:  " << ts_from_sec.ToUnixSeconds() << endl;

    // 测试Windows格式转换
    uint64 win_ts = ts_from_sec.ToWindows();
    Timestamp ts_from_win = Timestamp::FromWindows(win_ts);
    cout << "\nWindows timestamp roundtrip:" << endl;
    cout << "  Original (s): " << ts_from_sec.ToUnixSeconds() << endl;
    cout << "  Windows:      " << win_ts << endl;
    cout << "  Back (s):     " << ts_from_win.ToUnixSeconds() << endl;
    cout << "  Match: " << (ts_from_sec == ts_from_win ? "YES" : "NO") << endl;

    // 测试UUIDv7格式
    uint64 uuid7_ts = ts_from_sec.ToUUIDv7();
    Timestamp ts_from_uuid7 = Timestamp::FromUUIDv7(uuid7_ts);
    cout << "\nUUIDv7 timestamp roundtrip:" << endl;
    cout << "  Original (ms): " << ts_from_sec.ToUnixMilliseconds() << endl;
    cout << "  UUIDv7:        " << uuid7_ts << endl;
    cout << "  Back (ms):     " << ts_from_uuid7.ToUnixMilliseconds() << endl;
    cout << "  Match: " << (ts_from_sec.ToUnixMilliseconds() == ts_from_uuid7.ToUnixMilliseconds() ? "YES" : "NO") << endl;
}

/**
 * 测试时间加法
 */
void TestAddition()
{
    PrintSeparator("Time Addition Tests");

    Timestamp base = Timestamp::FromUnixSeconds(1737945600); // 2024-01-27 00:00:00

    cout << "Base timestamp (s): " << base.ToUnixSeconds() << endl;

    // 测试加秒
    Timestamp plus_60s = base.AddSeconds(60);
    cout << "\nAfter +60 seconds:  " << plus_60s.ToUnixSeconds() << " (diff: " << (plus_60s - base) << ")" << endl;

    // 测试加分钟
    Timestamp plus_5m = base.AddMinutes(5);
    cout << "After +5 minutes:   " << plus_5m.ToUnixSeconds() << " (diff: " << (plus_5m - base) << ")" << endl;

    // 测试加小时
    Timestamp plus_2h = base.AddHours(2);
    cout << "After +2 hours:     " << plus_2h.ToUnixSeconds() << " (diff: " << (plus_2h - base) << ")" << endl;

    // 测试加天
    Timestamp plus_1d = base.AddDays(1);
    cout << "After +1 day:       " << plus_1d.ToUnixSeconds() << " (diff: " << (plus_1d - base) << ")" << endl;

    // 测试加周
    Timestamp plus_1w = base.AddWeeks(1);
    cout << "After +1 week:      " << plus_1w.ToUnixSeconds() << " (diff: " << (plus_1w - base) << ")" << endl;

    // 测试加月
    Timestamp plus_1mo = base.AddMonths(1);
    cout << "After +1 month:     " << plus_1mo.ToUnixSeconds() << " (diff: " << (plus_1mo - base) << ")" << endl;

    // 测试加年
    Timestamp plus_1y = base.AddYears(1);
    cout << "After +1 year:      " << plus_1y.ToUnixSeconds() << " (diff: " << (plus_1y - base) << ")" << endl;

    // 验证计算的正确性
    cout << "\nValidation:" << endl;
    cout << "  60 seconds = 1 minute? " << (plus_60s.ToUnixSeconds() == base.ToUnixSeconds() + 60 ? "YES" : "NO") << endl;
    cout << "  5 minutes = 300 seconds? " << (plus_5m.ToUnixSeconds() == base.ToUnixSeconds() + 300 ? "YES" : "NO") << endl;
    cout << "  2 hours = 7200 seconds? " << (plus_2h.ToUnixSeconds() == base.ToUnixSeconds() + 7200 ? "YES" : "NO") << endl;
    cout << "  1 day = 86400 seconds? " << (plus_1d.ToUnixSeconds() == base.ToUnixSeconds() + 86400 ? "YES" : "NO") << endl;
}

/**
 * 测试时间减法
 */
void TestSubtraction()
{
    PrintSeparator("Time Subtraction Tests");

    Timestamp base = Timestamp::FromUnixSeconds(1737945600);

    cout << "Base timestamp (s): " << base.ToUnixSeconds() << endl;

    Timestamp minus_60s = base.SubSeconds(60);
    cout << "\nAfter -60 seconds:  " << minus_60s.ToUnixSeconds() << " (diff: " << (base - minus_60s) << ")" << endl;

    Timestamp minus_5m = base.SubMinutes(5);
    cout << "After -5 minutes:   " << minus_5m.ToUnixSeconds() << " (diff: " << (base - minus_5m) << ")" << endl;

    Timestamp minus_2h = base.SubHours(2);
    cout << "After -2 hours:     " << minus_2h.ToUnixSeconds() << " (diff: " << (base - minus_2h) << ")" << endl;

    Timestamp minus_1d = base.SubDays(1);
    cout << "After -1 day:       " << minus_1d.ToUnixSeconds() << " (diff: " << (base - minus_1d) << ")" << endl;

    Timestamp minus_1w = base.SubWeeks(1);
    cout << "After -1 week:      " << minus_1w.ToUnixSeconds() << " (diff: " << (base - minus_1w) << ")" << endl;
}

/**
 * 测试原地修改
 */
void TestInplaceModification()
{
    PrintSeparator("Inplace Modification Tests");

    Timestamp ts = Timestamp::FromUnixSeconds(1737945600);
    cout << "Initial timestamp (s): " << ts.ToUnixSeconds() << endl;

    ts.AddSecondsInplace(3600);
    cout << "After +3600s inplace:  " << ts.ToUnixSeconds() << endl;

    ts.AddHoursInplace(1);
    cout << "After +1h inplace:     " << ts.ToUnixSeconds() << endl;

    ts.SubDaysInplace(1);
    cout << "After -1d inplace:     " << ts.ToUnixSeconds() << endl;

    // 测试操作符
    ts += 7200; // +2小时
    cout << "After += 7200:         " << ts.ToUnixSeconds() << endl;

    ts -= 3600; // -1小时
    cout << "After -= 3600:         " << ts.ToUnixSeconds() << endl;
}

/**
 * 测试时间差计算
 */
void TestTimeDifference()
{
    PrintSeparator("Time Difference Tests");

    Timestamp t1 = Timestamp::FromUnixSeconds(1737945600);
    Timestamp t2 = Timestamp::FromUnixSeconds(1737945600 + 7200); // +2小时

    cout << "T1 (s): " << t1.ToUnixSeconds() << endl;
    cout << "T2 (s): " << t2.ToUnixSeconds() << endl;

    cout << "\nDifferences (T2 - T1):" << endl;
    cout << "  Seconds: " << t2.DiffSeconds(t1) << endl;
    cout << "  Minutes: " << t2.DiffMinutes(t1) << endl;
    cout << "  Hours:   " << t2.DiffHours(t1) << endl;
    cout << "  Days:    " << t2.DiffDays(t1) << endl;

    cout << "\nDifferences (T1 - T2):" << endl;
    cout << "  Seconds: " << t1.DiffSeconds(t2) << endl;
    cout << "  Minutes: " << t1.DiffMinutes(t2) << endl;
    cout << "  Hours:   " << t1.DiffHours(t2) << endl;

    // 使用操作符
    int64 diff_sec = t2 - t1;
    cout << "\nUsing operator (T2 - T1): " << diff_sec << " seconds" << endl;
}

/**
 * 测试比较操作
 */
void TestComparison()
{
    PrintSeparator("Comparison Tests");

    Timestamp t1 = Timestamp::FromUnixSeconds(1737945600);
    Timestamp t2 = Timestamp::FromUnixSeconds(1737945600);
    Timestamp t3 = Timestamp::FromUnixSeconds(1737945700);

    cout << "T1 == T2? " << (t1 == t2 ? "YES" : "NO") << endl;
    cout << "T1 != T3? " << (t1 != t3 ? "YES" : "NO") << endl;
    cout << "T1 < T3?  " << (t1 < t3 ? "YES" : "NO") << endl;
    cout << "T1 > T3?  " << (t1 > t3 ? "YES" : "NO") << endl;
    cout << "T3 > T1?  " << (t3 > t1 ? "YES" : "NO") << endl;
}

/**
 * 测试链式操作
 */
void TestChaining()
{
    PrintSeparator("Chaining Operations Tests");

    Timestamp base = Timestamp::FromUnixSeconds(1737945600);

    cout << "Base: " << base.ToUnixSeconds() << endl;

    // 链式加法
    Timestamp result = base.AddDays(1).AddHours(2).AddMinutes(30);
    cout << "After +1d +2h +30m: " << result.ToUnixSeconds() << endl;
    cout << "Expected: " << (base.ToUnixSeconds() + 86400 + 7200 + 1800) << endl;
    cout << "Match? " << (result.ToUnixSeconds() == (base.ToUnixSeconds() + 86400 + 7200 + 1800) ? "YES" : "NO") << endl;

    // 混合操作
    Timestamp result2 = base.AddWeeks(1).SubDays(2).AddHours(3);
    cout << "\nAfter +1w -2d +3h: " << result2.ToUnixSeconds() << endl;
    int64 expected = base.ToUnixSeconds() + (7*86400) - (2*86400) + (3*3600);
    cout << "Expected: " << expected << endl;
    cout << "Match? " << (result2.ToUnixSeconds() == expected ? "YES" : "NO") << endl;
}

/**
 * 测试拷贝和赋值
 */
void TestCopyAndAssignment()
{
    PrintSeparator("Copy & Assignment Tests");

    Timestamp t1 = Timestamp::FromUnixSeconds(1737945600);

    // 拷贝构造
    Timestamp t2 = t1;
    cout << "Copy construction:" << endl;
    cout << "  T1: " << t1.ToUnixSeconds() << endl;
    cout << "  T2: " << t2.ToUnixSeconds() << endl;
    cout << "  T1 == T2? " << (t1 == t2 ? "YES" : "NO") << endl;

    // 赋值
    Timestamp t3 = Timestamp::FromUnixSeconds(0);
    t3 = t1;
    cout << "\nAssignment:" << endl;
    cout << "  T3: " << t3.ToUnixSeconds() << endl;
    cout << "  T1 == T3? " << (t1 == t3 ? "YES" : "NO") << endl;

    // 修改拷贝不影响原对象
    t2.AddHoursInplace(1);
    cout << "\nAfter modifying T2 (+1h):" << endl;
    cout << "  T1: " << t1.ToUnixSeconds() << endl;
    cout << "  T2: " << t2.ToUnixSeconds() << endl;
    cout << "  T1 == T2? " << (t1 == t2 ? "YES" : "NO") << endl;
}

int main(int, char**)
{
    cout << "CMCore Timestamp Class Test Program" << endl;
    cout << "====================================" << endl << endl;

    TestBasicConstruction();
    cout << endl;

    TestAddition();
    cout << endl;

    TestSubtraction();
    cout << endl;

    TestInplaceModification();
    cout << endl;

    TestTimeDifference();
    cout << endl;

    TestComparison();
    cout << endl;

    TestChaining();
    cout << endl;

    TestCopyAndAssignment();
    cout << endl;

    PrintSeparator("All Tests Completed");

    return 0;
}
