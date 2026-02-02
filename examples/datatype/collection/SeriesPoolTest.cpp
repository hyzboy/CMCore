/**
 * SeriesPoolTest.cpp
 *
 * 全面测试 SeriesPool<T> 类
 * 测试场景：
 *   - 基本操作（初始化、分配、释放）
 *   - 边界条件（满池、空池、无效序号）
 *   - 双重释放检测
 *   - 随机序号初始化
 *   - 内存泄漏检测
 *   - 跟踪功能验证
 *   - 大容量测试
 */

#include<hgl/type/SeriesPool.h>
#include<iostream>
#include<cassert>
#include<vector>
#include<algorithm>
#include<set>

using namespace hgl;
using namespace std;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  ✓ PASS: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  ✗ FAIL: " << message << std::endl; \
        } \
    } while(0)

// ==================== TEST 1: 基本操作 ====================

void test_basic_operations()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: SeriesInt - Basic Operations" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;

    cout << "[1.1] Initialize pool with capacity 10:" << endl;
    bool init_result = pool.Init(10);
    TEST_ASSERT(init_result, "Pool initialization succeeded");
    TEST_ASSERT(pool.GetMaxCount() == 10, "Max count is 10");
    TEST_ASSERT(pool.GetFreeCount() == 10, "Free count is 10");
    TEST_ASSERT(pool.GetUseCount() == 0, "Use count is 0");

    cout << "\n[1.2] Acquire series numbers:" << endl;
    int s1, s2, s3;

    bool acquire1 = pool.Acquire(&s1);
    TEST_ASSERT(acquire1, "First acquire succeeded");
    TEST_ASSERT(s1 >= 0 && s1 < 10, "First series in valid range");
    TEST_ASSERT(pool.GetFreeCount() == 9, "Free count decreased to 9");
    TEST_ASSERT(pool.GetUseCount() == 1, "Use count increased to 1");

    bool acquire2 = pool.Acquire(&s2);
    TEST_ASSERT(acquire2, "Second acquire succeeded");
    TEST_ASSERT(s2 != s1, "Different series numbers");

    bool acquire3 = pool.Acquire(&s3);
    TEST_ASSERT(acquire3, "Third acquire succeeded");
    TEST_ASSERT(s3 != s1 && s3 != s2, "All series numbers unique");

    cout << "\n[1.3] Release series numbers:" << endl;
    bool release1 = pool.Release(s1);
    TEST_ASSERT(release1, "First release succeeded");
    TEST_ASSERT(pool.GetFreeCount() == 8, "Free count increased to 8");

    bool release2 = pool.Release(s2);
    TEST_ASSERT(release2, "Second release succeeded");

    cout << "\n[1.4] Reacquire (should reuse released):" << endl;
    int s4;
    bool acquire4 = pool.Acquire(&s4);
    TEST_ASSERT(acquire4, "Reacquire succeeded");
    TEST_ASSERT(s4 == s2 || s4 == s1, "Reacquired a previously released series");

    pool.Release(s3);
    pool.Release(s4);
}

// ==================== TEST 2: 边界条件 ====================

void test_boundary_conditions()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Boundary Conditions" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;
    pool.Init(5);

    cout << "[2.1] Exhaust pool capacity:" << endl;
    vector<int> series;
    for (int i = 0; i < 5; i++) {
        int s;
        bool acquired = pool.Acquire(&s);
        TEST_ASSERT(acquired, "Acquire " + to_string(i + 1) + " succeeded");
        if (acquired) {
            series.push_back(s);
        }
    }
    TEST_ASSERT(pool.GetFreeCount() == 0, "Pool is empty");
    TEST_ASSERT(pool.GetUseCount() == 5, "All series in use");

    cout << "\n[2.2] Attempt to acquire from empty pool:" << endl;
    int overflow;
    bool overflow_acquire = pool.Acquire(&overflow);
    TEST_ASSERT(!overflow_acquire, "Acquire from empty pool fails");

    cout << "\n[2.3] Release one and reacquire:" << endl;
    bool release_ok = pool.Release(series[2]);
    TEST_ASSERT(release_ok, "Release succeeded");
    TEST_ASSERT(pool.GetFreeCount() == 1, "Free count is 1");

    int reacquired;
    bool reacquire_ok = pool.Acquire(&reacquired);
    TEST_ASSERT(reacquire_ok, "Reacquire succeeded");
    TEST_ASSERT(reacquired == series[2], "Reacquired same series number");

    cout << "\n[2.4] Release invalid series numbers:" << endl;
    bool invalid_negative = pool.Release(-1);
    TEST_ASSERT(!invalid_negative, "Release negative number fails");

    bool invalid_large = pool.Release(999);
    TEST_ASSERT(!invalid_large, "Release out-of-range number fails");

    bool invalid_nullptr = pool.Acquire(nullptr);
    TEST_ASSERT(!invalid_nullptr, "Acquire with nullptr fails");

    // Cleanup
    for (auto s : series) {
        pool.Release(s);
    }
    pool.Release(reacquired);
}

// ==================== TEST 3: 双重释放检测 ====================

void test_double_release()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Double Release Detection" << endl;
    cout << "========================================\n" << endl;

#ifdef SERIES_POOL_ENABLE_TRACKING
    SeriesInt pool;
    pool.Init(10);

    cout << "[3.1] Acquire and release normally:" << endl;
    int s;
    pool.Acquire(&s);
    bool first_release = pool.Release(s);
    TEST_ASSERT(first_release, "First release succeeded");

    cout << "\n[3.2] Attempt double release:" << endl;
    bool second_release = pool.Release(s);
    TEST_ASSERT(!second_release, "Double release detected and prevented");

    cout << "\n[3.3] Acquire again and verify:" << endl;
    int s2;
    pool.Acquire(&s2);
    TEST_ASSERT(s2 == s, "Previously released series can be reacquired");

    bool third_release = pool.Release(s2);
    TEST_ASSERT(third_release, "Release after reacquire succeeds");

    cout << "\n[3.4] Release unacquired series:" << endl;
    bool release_never_acquired = pool.Release(5);
    TEST_ASSERT(!release_never_acquired, "Releasing never-acquired series fails");
#else
    cout << "[3.0] Tracking disabled (SERIES_POOL_NO_TRACKING defined)" << endl;
    cout << "  Skipping double release detection tests" << endl;
#endif
}

// ==================== TEST 4: 随机序号初始化 ====================

void test_random_series()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Random Series Initialization" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;
    pool.Init(20);

    cout << "[4.1] Initialize with random series in range [0, 50]:" << endl;
    pool.InitRandomSeries(0, 50);

    cout << "\n[4.2] Acquire all and verify uniqueness:" << endl;
    set<int> acquired_series;
    for (int i = 0; i < 20; i++) {
        int s;
        bool ok = pool.Acquire(&s);
        TEST_ASSERT(ok, "Acquire succeeded");
        if (ok) {
            TEST_ASSERT(s >= 0 && s <= 50, "Series in specified range");
            acquired_series.insert(s);
        }
    }
    TEST_ASSERT(acquired_series.size() == 20, "All series numbers are unique");

    cout << "\n[4.3] Test with insufficient range (fallback to sequential):" << endl;
    SeriesInt pool2;
    pool2.Init(10);
    pool2.InitRandomSeries(0, 5);  // Range only has 6 values, need 10

    set<int> series2;
    for (int i = 0; i < 10; i++) {
        int s;
        pool2.Acquire(&s);
        series2.insert(s);
    }
    TEST_ASSERT(series2.size() == 10, "Fallback to sequential initialization works");
}

// ==================== TEST 5: IsInFreeList 诊断 ====================

void test_free_list_check()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: IsInFreeList Diagnostic" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;
    pool.Init(10);

    cout << "[5.1] Check initial state (all free):" << endl;
    for (int i = 0; i < 10; i++) {
        bool is_free = pool.IsInFreeList(i);
        TEST_ASSERT(is_free, "Series " + to_string(i) + " is in free list");
    }

    cout << "\n[5.2] Acquire some and check state:" << endl;
    int s1, s2, s3;
    pool.Acquire(&s1);
    pool.Acquire(&s2);
    pool.Acquire(&s3);

#ifdef SERIES_POOL_ENABLE_TRACKING
    bool s1_free = pool.IsInFreeList(s1);
    bool s2_free = pool.IsInFreeList(s2);
    bool s3_free = pool.IsInFreeList(s3);
    TEST_ASSERT(!s1_free, "Acquired series s1 not in free list");
    TEST_ASSERT(!s2_free, "Acquired series s2 not in free list");
    TEST_ASSERT(!s3_free, "Acquired series s3 not in free list");
#endif

    cout << "\n[5.3] Release and verify:" << endl;
    pool.Release(s1);

#ifdef SERIES_POOL_ENABLE_TRACKING
    bool s1_free_after = pool.IsInFreeList(s1);
    TEST_ASSERT(s1_free_after, "Released series s1 back in free list");
#endif

    pool.Release(s2);
    pool.Release(s3);
}

// ==================== TEST 6: 跟踪功能测试 ====================

void test_tracking_features()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Tracking Features" << endl;
    cout << "========================================\n" << endl;

#ifdef SERIES_POOL_ENABLE_TRACKING
    SeriesInt pool;
    pool.Init(20);

    cout << "[6.1] Initial allocated count:" << endl;
    size_t initial_count = pool.GetAllocatedCount();
    TEST_ASSERT(initial_count == 0, "Initial allocated count is 0");
    TEST_ASSERT(pool.IsFullyReleased(), "Pool is fully released initially");

    cout << "\n[6.2] Acquire several and check count:" << endl;
    vector<int> series;
    for (int i = 0; i < 5; i++) {
        int s;
        pool.Acquire(&s);
        series.push_back(s);
    }

    size_t after_acquire = pool.GetAllocatedCount();
    TEST_ASSERT(after_acquire == 5, "Allocated count is 5");
    TEST_ASSERT(!pool.IsFullyReleased(), "Pool is not fully released");

    cout << "\n[6.3] Release some and check count:" << endl;
    pool.Release(series[0]);
    pool.Release(series[1]);

    size_t after_partial = pool.GetAllocatedCount();
    TEST_ASSERT(after_partial == 3, "Allocated count is 3 after partial release");

    cout << "\n[6.4] Release all and verify:" << endl;
    for (size_t i = 2; i < series.size(); i++) {
        pool.Release(series[i]);
    }

    TEST_ASSERT(pool.GetAllocatedCount() == 0, "All series released");
    TEST_ASSERT(pool.IsFullyReleased(), "Pool is fully released");
#else
    cout << "[6.0] Tracking disabled (SERIES_POOL_NO_TRACKING defined)" << endl;
    cout << "  Skipping tracking feature tests" << endl;
#endif
}

// ==================== TEST 7: 大容量测试 ====================

void test_large_capacity()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Large Capacity Pool" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;
    pool.Init(10000);

    cout << "[7.1] Acquire 10000 series:" << endl;
    vector<int> series;
    int failed_count = 0;
    for (int i = 0; i < 10000; i++) {
        int s;
        if (pool.Acquire(&s)) {
            series.push_back(s);
        } else {
            failed_count++;
        }
    }
    TEST_ASSERT(series.size() == 10000, "All 10000 series acquired");
    TEST_ASSERT(failed_count == 0, "No acquisition failures");
    TEST_ASSERT(pool.GetFreeCount() == 0, "Pool is empty");

    cout << "\n[7.2] Verify uniqueness:" << endl;
    set<int> unique_series(series.begin(), series.end());
    TEST_ASSERT(unique_series.size() == 10000, "All series are unique");

    cout << "\n[7.3] Release all:" << endl;
    for (auto s : series) {
        pool.Release(s);
    }
    TEST_ASSERT(pool.GetFreeCount() == 10000, "All series released");

#ifdef SERIES_POOL_ENABLE_TRACKING
    TEST_ASSERT(pool.IsFullyReleased(), "No leaks detected");
#endif
}

// ==================== TEST 8: 交替操作模式 ====================

void test_alternating_pattern()
{
    cout << "\n========================================" << endl;
    cout << "TEST 8: Alternating Acquire/Release Pattern" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;
    pool.Init(50);

    cout << "[8.1] Pattern: acquire 5, release 3, repeat 100 times:" << endl;
    vector<int> active_series;
    int total_acquires = 0;
    int total_releases = 0;

    for (int cycle = 0; cycle < 100; cycle++) {
        // Acquire 5
        for (int i = 0; i < 5 && pool.GetFreeCount() > 0; i++) {
            int s;
            if (pool.Acquire(&s)) {
                active_series.push_back(s);
                total_acquires++;
            }
        }

        // Release 3
        for (int i = 0; i < 3 && !active_series.empty(); i++) {
            pool.Release(active_series.back());
            active_series.pop_back();
            total_releases++;
        }
    }

    cout << "    Total acquires: " << total_acquires << endl;
    cout << "    Total releases: " << total_releases << endl;
    cout << "    Still active: " << active_series.size() << endl;

    // 池大小 50，每次净增 2（获取5-释放3），约 25 次循环后池满
    // 之后只能维持现状，每次获取的数量会减少
    TEST_ASSERT(total_acquires >= 300 && total_acquires <= 400, "Acquires in expected range");
    TEST_ASSERT(total_releases == 300, "300 total releases");
    TEST_ASSERT(active_series.size() >= 45 && active_series.size() <= 50, "Pool near capacity");

    // Cleanup
    for (auto s : active_series) {
        pool.Release(s);
    }

#ifdef SERIES_POOL_ENABLE_TRACKING
    TEST_ASSERT(pool.IsFullyReleased(), "All series properly released");
#endif
}

// ==================== TEST 9: 多种类型测试 ====================

void test_different_types()
{
    cout << "\n========================================" << endl;
    cout << "TEST 9: Different Integer Types" << endl;
    cout << "========================================\n" << endl;

    cout << "[9.1] SeriesU8 (uint8):" << endl;
    SeriesU8 pool8;
    pool8.Init(100);
    uint8 s8;
    bool ok8 = pool8.Acquire(&s8);
    TEST_ASSERT(ok8, "SeriesU8 acquire succeeded");
    pool8.Release(s8);

    cout << "\n[9.2] SeriesI16 (int16):" << endl;
    SeriesI16 pool16;
    pool16.Init(1000);
    int16 s16;
    bool ok16 = pool16.Acquire(&s16);
    TEST_ASSERT(ok16, "SeriesI16 acquire succeeded");
    pool16.Release(s16);

    cout << "\n[9.3] SeriesI64 (int64):" << endl;
    SeriesI64 pool64;
    pool64.Init(10000);
    int64 s64;
    bool ok64 = pool64.Acquire(&s64);
    TEST_ASSERT(ok64, "SeriesI64 acquire succeeded");
    pool64.Release(s64);
}

// ==================== TEST 10: 边缘案例 ====================

void test_edge_cases()
{
    cout << "\n========================================" << endl;
    cout << "TEST 10: Edge Cases" << endl;
    cout << "========================================\n" << endl;

    cout << "[10.1] Size 1 pool:" << endl;
    SeriesInt pool1;
    pool1.Init(1);
    int s1;
    bool ok1 = pool1.Acquire(&s1);
    TEST_ASSERT(ok1, "Acquire from size-1 pool succeeded");
    TEST_ASSERT(s1 == 0, "Series number is 0");

    int s2;
    bool ok2 = pool1.Acquire(&s2);
    TEST_ASSERT(!ok2, "Second acquire from size-1 pool fails");

    pool1.Release(s1);
    bool ok3 = pool1.Acquire(&s2);
    TEST_ASSERT(ok3, "Reacquire after release succeeds");

    cout << "\n[10.2] Multiple Init attempts:" << endl;
    SeriesInt pool_multi;
    bool init1 = pool_multi.Init(10);
    TEST_ASSERT(init1, "First init succeeded");

    bool init2 = pool_multi.Init(20);
    TEST_ASSERT(!init2, "Second init fails (already initialized)");
    TEST_ASSERT(pool_multi.GetMaxCount() == 10, "Max count unchanged");

    cout << "\n[10.3] GetRawData verification:" << endl;
    SeriesInt pool_raw;
    pool_raw.Init(100);
    int* raw_data = pool_raw.GetRawData();
    TEST_ASSERT(raw_data != nullptr, "Raw data pointer is valid");

    // Verify raw data contains valid series numbers
    bool all_valid = true;
    for (int i = 0; i < 100; i++) {
        if (raw_data[i] < 0 || raw_data[i] >= 100) {
            all_valid = false;
            break;
        }
    }
    TEST_ASSERT(all_valid, "All raw data values are valid series numbers");
}

// ==================== TEST 11: 压力测试 ====================

void test_stress()
{
    cout << "\n========================================" << endl;
    cout << "TEST 11: Stress Test" << endl;
    cout << "========================================\n" << endl;

    SeriesInt pool;
    pool.Init(100);

    cout << "[11.1] Rapid acquire/release cycles (10000 iterations):" << endl;
    vector<int> active;
    int operations = 0;

    for (int i = 0; i < 10000; i++) {
        // Randomly acquire or release
        if ((i % 3 == 0) && pool.GetFreeCount() > 0) {
            int s;
            if (pool.Acquire(&s)) {
                active.push_back(s);
                operations++;
            }
        }

        if ((i % 2 == 0) && !active.empty()) {
            size_t idx = i % active.size();
            pool.Release(active[idx]);
            active.erase(active.begin() + idx);
            operations++;
        }
    }

    cout << "    Total operations: " << operations << endl;
    cout << "    Still active: " << active.size() << endl;
    TEST_ASSERT(operations > 5000, "Many operations performed");

    // Cleanup
    for (auto s : active) {
        pool.Release(s);
    }

#ifdef SERIES_POOL_ENABLE_TRACKING
    TEST_ASSERT(pool.IsFullyReleased(), "No leaks after stress test");
#endif
}

// ==================== Main ====================

int os_main(int, os_char**)
{
    cout << "\n=====================================" << endl;
    cout << "SeriesPool Test Suite" << endl;
    cout << "=====================================" << endl;

#ifdef SERIES_POOL_ENABLE_TRACKING
    cout << "Tracking: ENABLED (double-release detection active)" << endl;
#else
    cout << "Tracking: DISABLED (SERIES_POOL_NO_TRACKING defined)" << endl;
#endif

    test_basic_operations();
    test_boundary_conditions();
    test_double_release();
    test_random_series();
    test_free_list_check();
    test_tracking_features();
    test_large_capacity();
    test_alternating_pattern();
    test_different_types();
    test_edge_cases();
    test_stress();

    cout << "\n========================================" << endl;
    cout << "TEST SUMMARY" << endl;
    cout << "========================================" << endl;
    cout << "Passed: " << tests_passed << endl;
    cout << "Failed: " << tests_failed << endl;

    if (tests_failed == 0) {
        cout << "\n✅ ALL TESTS PASSED!" << endl;
    } else {
        cout << "\n❌ SOME TESTS FAILED!" << endl;
    }

    return tests_failed > 0 ? 1 : 0;
}
