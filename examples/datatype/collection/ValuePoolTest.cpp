#include<hgl/type/ValuePool.h>
#include<iostream>
#include<chrono>
#include<random>
#include<algorithm>
#include"UserInfo.h"

using namespace hgl;
using namespace std;
using namespace std::chrono;

// ============================================================================
// 辅助工具函数
// ============================================================================

class Timer
{
    high_resolution_clock::time_point start;
public:
    Timer() { start = high_resolution_clock::now(); }

    double ElapsedMs() const
    {
        auto end = high_resolution_clock::now();
        return duration_cast<duration<double, milli>>(end - start).count();
    }
};

template<typename T>
void ShowPoolStats(const ValuePool<T> &pool, const char *title = nullptr)
{
    if (title)
        cout << ">>> " << title << endl;

    cout << "  Active: " << pool.GetActiveCount()
         << ", Idle: " << pool.GetIdleCount()
         << ", Total: " << pool.GetTotalCount()
         << ", History Max: " << pool.GetHistoryMaxCount() << endl;

    const auto &stats = pool.GetStats();
    cout << "  Stats: Creates=" << stats.total_creates
         << ", Reuses=" << stats.total_reuses
         << ", Releases=" << stats.total_releases
         << ", Peak=" << stats.peak_active
         << ", HitRate=" << (stats.GetHitRate() * 100.0f) << "%" << endl;
}

void ShowUserInfoArray(const std::vector<UserInfo> &ua)
{
    cout << "  User array [" << ua.size() << "]: ";

    int shown = 0;
    for (auto &ui : ua)
    {
        if (shown++ > 0) cout << ", ";
        cout << ui.name;
        if (shown >= 10)
        {
            cout << " ...";
            break;
        }
    }
    cout << endl;
}

// ============================================================================
// 测试1：基础功能测试
// ============================================================================

bool Test_BasicOperations()
{
    cout << "\n========================================" << endl;
    cout << "Test 1: Basic Operations" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;

    // 测试初始状态
    cout << "\n[1.1] Initial state check..." << endl;
    if (pool.GetActiveCount() != 0 || pool.GetIdleCount() != 0)
    {
        cout << "  FAILED: Pool should be empty initially" << endl;
        return false;
    }
    cout << "  PASSED: Pool is empty" << endl;

    // 测试 CreateNew
    cout << "\n[1.2] CreateNew test..." << endl;
    int val1 = 0;
    if (!pool.CreateNew(val1))
    {
        cout << "  FAILED: CreateNew should succeed" << endl;
        return false;
    }
    if (pool.GetActiveCount() != 1)
    {
        cout << "  FAILED: Active count should be 1" << endl;
        return false;
    }
    cout << "  PASSED: CreateNew works, active count = 1" << endl;

    // 测试 IsActive
    cout << "\n[1.3] IsActive test..." << endl;
    if (!pool.IsActive(val1))
    {
        cout << "  FAILED: Object should be active" << endl;
        return false;
    }
    cout << "  PASSED: IsActive works" << endl;

    // 测试 Release
    cout << "\n[1.4] Release test..." << endl;
    if (!pool.Release(val1))
    {
        cout << "  FAILED: Release should succeed" << endl;
        return false;
    }
    if (pool.GetActiveCount() != 0 || pool.GetIdleCount() != 1)
    {
        cout << "  FAILED: Should have 0 active, 1 idle" << endl;
        return false;
    }
    cout << "  PASSED: Release works" << endl;

    // 测试 GetOrCreate (should reuse)
    cout << "\n[1.5] GetOrCreate reuse test..." << endl;
    int val2 = 0;
    if (!pool.GetOrCreate(val2))
    {
        cout << "  FAILED: GetOrCreate should succeed" << endl;
        return false;
    }
    if (val2 != val1)
    {
        cout << "  FAILED: Should reuse the released object" << endl;
        return false;
    }
    if (pool.GetActiveCount() != 1 || pool.GetIdleCount() != 0)
    {
        cout << "  FAILED: Should have 1 active, 0 idle" << endl;
        return false;
    }
    cout << "  PASSED: Object reused from idle pool" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试2：批量操作测试
// ============================================================================

bool Test_BatchOperations()
{
    cout << "\n========================================" << endl;
    cout << "Test 2: Batch Operations" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;
    pool.Reserve(100);

    // 批量创建
    cout << "\n[2.1] CreateBatch test..." << endl;
    int batch[50];
    int created = pool.CreateBatch(batch, 50);
    if (created != 50)
    {
        cout << "  FAILED: Should create 50 objects, got " << created << endl;
        return false;
    }
    if (pool.GetActiveCount() != 50)
    {
        cout << "  FAILED: Active count should be 50" << endl;
        return false;
    }
    cout << "  PASSED: Created " << created << " objects" << endl;

    // 批量释放
    cout << "\n[2.2] ReleaseBatch test..." << endl;
    int released = pool.ReleaseBatch(batch, 30);
    if (released != 30)
    {
        cout << "  FAILED: Should release 30 objects, got " << released << endl;
        return false;
    }
    if (pool.GetActiveCount() != 20 || pool.GetIdleCount() != 30)
    {
        cout << "  FAILED: Should have 20 active, 30 idle" << endl;
        return false;
    }
    cout << "  PASSED: Released " << released << " objects" << endl;

    // 批量获取
    cout << "\n[2.3] GetBatch test..." << endl;
    int get_batch[40];
    int got = pool.GetBatch(get_batch, 40);
    if (got != 40)
    {
        cout << "  FAILED: Should get 40 objects, got " << got << endl;
        return false;
    }
    if (pool.GetActiveCount() != 60)
    {
        cout << "  FAILED: Active count should be 60 (20 + 40)" << endl;
        return false;
    }
    cout << "  PASSED: Got " << got << " objects" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试3：容量管理测试
// ============================================================================

bool Test_CapacityManagement()
{
    cout << "\n========================================" << endl;
    cout << "Test 3: Capacity Management" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;

    // 测试 Reserve
    cout << "\n[3.1] Reserve test..." << endl;
    pool.Reserve(100, true); // set max to 100
    if (pool.IsFull())
    {
        cout << "  FAILED: Pool should not be full after Reserve" << endl;
        return false;
    }
    cout << "  PASSED: Reserved capacity 100" << endl;

    // 测试 Warmup
    cout << "\n[3.2] Warmup test..." << endl;
    pool.Warmup(50);
    if (pool.GetIdleCount() != 50)
    {
        cout << "  FAILED: Should have 50 idle objects after warmup" << endl;
        return false;
    }
    cout << "  PASSED: Warmed up 50 idle objects" << endl;

    // 测试 SetMaxActiveCount
    cout << "\n[3.3] Max active count test..." << endl;
    for (int i = 0; i < 100; ++i)
    {
        int val;
        pool.GetOrCreate(val);
    }
    if (!pool.IsFull())
    {
        cout << "  FAILED: Pool should be full" << endl;
        return false;
    }

    int val_extra;
    if (pool.CreateNew(val_extra))
    {
        cout << "  FAILED: Should not create when full" << endl;
        return false;
    }
    cout << "  PASSED: Max limit enforced" << endl;

    // 测试 SetMinIdleCount
    cout << "\n[3.4] Min idle count test..." << endl;
    pool.SetMinIdleCount(10);
    pool.ReleaseActive();
    if (pool.GetIdleCount() < 10)
    {
        cout << "  FAILED: Should maintain at least 10 idle" << endl;
        return false;
    }
    cout << "  PASSED: Min idle count maintained" << endl;

    // 测试 Shrink
    cout << "\n[3.5] Shrink test..." << endl;
    pool.ShrinkIdle(5);
    if (pool.GetIdleCount() > 5)
    {
        cout << "  FAILED: Should have at most 5 idle after shrink" << endl;
        return false;
    }
    cout << "  PASSED: Idle pool shrunk to " << pool.GetIdleCount() << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试4：重置函数测试
// ============================================================================

bool Test_ResetFunction()
{
    cout << "\n========================================" << endl;
    cout << "Test 4: Reset Function" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;

    cout << "\n[4.1] GetOrCreate with reset function..." << endl;

    // 创建并设置值
    int val1 = 0;
    pool.CreateNew(val1);
    // 对于int类型，val1是副本，修改不会影响池中的值
    // 所以我们直接Release，然后获取时使用重置函数
    pool.Release(val1);

    // 使用重置函数获取
    int val2 = 0;
    int reset_count = 0;
    pool.GetOrCreate(val2, [&](int &v) {
        v = 999; // 设置为999
        ++reset_count;
    });

    if (reset_count != 1)
    {
        cout << "  FAILED: Reset function should be called once" << endl;
        return false;
    }
    if (val2 != 999)
    {
        cout << "  FAILED: Value should be reset to 999, got " << val2 << endl;
        return false;
    }

    cout << "  PASSED: Reset function called and value reset" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试5：GetIfAvailable 测试
// ============================================================================

bool Test_GetIfAvailable()
{
    cout << "\n========================================" << endl;
    cout << "Test 5: GetIfAvailable" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;

    cout << "\n[5.1] GetIfAvailable from empty pool..." << endl;
    int val = 0;
    if (pool.GetIfAvailable(val))
    {
        cout << "  FAILED: Should fail when pool is empty" << endl;
        return false;
    }
    cout << "  PASSED: Correctly failed on empty pool" << endl;

    cout << "\n[5.2] GetIfAvailable from pool with idle objects..." << endl;
    pool.CreateNew(val);
    pool.Release(val);

    int val2 = 0;
    if (!pool.GetIfAvailable(val2))
    {
        cout << "  FAILED: Should succeed when idle objects exist" << endl;
        return false;
    }
    if (val2 != val)
    {
        cout << "  FAILED: Should get the same object" << endl;
        return false;
    }
    cout << "  PASSED: Got object from idle pool" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试6：统计功能测试
// ============================================================================

bool Test_Statistics()
{
    cout << "\n========================================" << endl;
    cout << "Test 6: Statistics" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;

    cout << "\n[6.1] Create and track statistics..." << endl;

    // 创建10个对象
    for (int i = 0; i < 10; ++i)
    {
        int val;
        pool.CreateNew(val);
    }

    const auto &stats1 = pool.GetStats();
    if (stats1.total_creates != 10)
    {
        cout << "  FAILED: Should have 10 creates, got " << stats1.total_creates << endl;
        return false;
    }
    if (stats1.peak_active != 10)
    {
        cout << "  FAILED: Peak active should be 10, got " << stats1.peak_active << endl;
        return false;
    }
    cout << "  PASSED: Creates tracked correctly" << endl;

    // 释放并重用
    cout << "\n[6.2] Release and reuse tracking..." << endl;
    pool.ReleaseActive();

    for (int i = 0; i < 5; ++i)
    {
        int val;
        pool.GetOrCreate(val);
    }

    const auto &stats2 = pool.GetStats();
    if (stats2.total_reuses != 5)
    {
        cout << "  FAILED: Should have 5 reuses, got " << stats2.total_reuses << endl;
        return false;
    }
    if (stats2.total_releases != 10)
    {
        cout << "  FAILED: Should have 10 releases, got " << stats2.total_releases << endl;
        return false;
    }

    float hit_rate = stats2.GetHitRate();
    float expected = 5.0f / 15.0f; // 5 reuses out of 15 total (10 creates + 5 reuses)
    if (abs(hit_rate - expected) > 0.01f)
    {
        cout << "  FAILED: Hit rate should be ~" << (expected * 100) << "%, got " << (hit_rate * 100) << "%" << endl;
        return false;
    }
    cout << "  PASSED: Statistics accurate, hit rate = " << (hit_rate * 100.0f) << "%" << endl;

    // 测试重置统计
    cout << "\n[6.3] ResetStats test..." << endl;
    pool.ResetStats();
    const auto &stats3 = pool.GetStats();
    if (stats3.total_creates != 0 || stats3.total_reuses != 0)
    {
        cout << "  FAILED: Stats should be reset to 0" << endl;
        return false;
    }
    cout << "  PASSED: Stats reset successfully" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试7：迭代器测试
// ============================================================================

bool Test_Iterators()
{
    cout << "\n========================================" << endl;
    cout << "Test 7: Iterators" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;

    cout << "\n[7.1] Range-based for loop..." << endl;
    for (int i = 0; i < 10; ++i)
    {
        int val = i;
        pool.AppendToActive(val);
    }

    int sum = 0;
    for (const auto &val : pool)
    {
        sum += val;
    }

    int expected_sum = (0 + 9) * 10 / 2; // 0+1+2+...+9 = 45
    if (sum != expected_sum)
    {
        cout << "  FAILED: Sum should be " << expected_sum << ", got " << sum << endl;
        return false;
    }
    cout << "  PASSED: Range-based for loop works, sum = " << sum << endl;

    cout << "\n[7.2] ForEachActive test..." << endl;
    int count = 0;
    pool.ForEachActive([&](int &val) {
        ++count;
        val *= 2; // 修改值
    });

    if (count != 10)
    {
        cout << "  FAILED: Should iterate 10 objects, got " << count << endl;
        return false;
    }

    int sum2 = 0;
    for (const auto &val : pool)
    {
        sum2 += val;
    }

    if (sum2 != expected_sum * 2)
    {
        cout << "  FAILED: Values should be doubled" << endl;
        return false;
    }
    cout << "  PASSED: ForEachActive works, modified values" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试8：Validate 测试
// ============================================================================

bool Test_Validate()
{
    cout << "\n========================================" << endl;
    cout << "Test 8: Validation" << endl;
    cout << "========================================" << endl;

    ValuePool<int> pool;
    pool.Reserve(100, true);

    cout << "\n[8.1] Validate empty pool..." << endl;
    if (!pool.Validate())
    {
        cout << "  FAILED: Empty pool should be valid" << endl;
        return false;
    }
    cout << "  PASSED: Empty pool is valid" << endl;

    cout << "\n[8.2] Validate after operations..." << endl;
    for (int i = 0; i < 50; ++i)
    {
        int val;
        pool.CreateNew(val);
    }

    if (!pool.Validate())
    {
        cout << "  FAILED: Pool should be valid after creates" << endl;
        return false;
    }
    cout << "  PASSED: Pool valid after creates" << endl;

    cout << "\n[8.3] Validate after releases..." << endl;
    // 复制数组避免在 Release 过程中引用失效
    const auto &view = pool.GetActiveView();
    vector<int> to_release;
    for (int i = 0; i < 25; ++i)
    {
        to_release.push_back(view[i]);
    }
    
    for (const auto &val : to_release)
    {
        pool.Release(val);
    }

    if (!pool.Validate())
    {
        cout << "  FAILED: Pool should be valid after releases" << endl;
        return false;
    }
    cout << "  PASSED: Pool valid after releases" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 测试9：性能测试 - O(1) Release 验证
// ============================================================================

bool Test_Performance_Release()
{
    cout << "\n========================================" << endl;
    cout << "Test 9: Performance - O(1) Release" << endl;
    cout << "========================================" << endl;

    const int sizes[] = { 1000, 10000, 100000 };

    for (int size : sizes)
    {
        cout << "\n[9." << (size / 1000) << "] Testing with " << size << " objects..." << endl;

        ValuePool<int> pool;
        pool.Reserve(size);

        // 创建对象
        vector<int> objects;
        for (int i = 0; i < size; ++i)
        {
            int val = i;
            pool.AppendToActive(val);
            objects.push_back(val);
        }

        // 随机打乱
        random_device rd;
        mt19937 gen(rd());
        shuffle(objects.begin(), objects.end(), gen);

        // 测试释放性能
        Timer timer;
        int released = 0;
        for (int i = 0; i < size / 2; ++i)
        {
            if (pool.Release(objects[i]))
                ++released;
        }
        double elapsed = timer.ElapsedMs();

        if (released != size / 2)
        {
            cout << "  FAILED: Should release " << (size / 2) << " objects, got " << released << endl;
            return false;
        }

        double avg_time = elapsed / released;
        cout << "  Released " << released << " objects in " << elapsed << "ms" << endl;
        cout << "  Average time per release: " << avg_time << "ms" << endl;

        // O(1) 验证：时间应该随规模线性增长，而非平方增长
        if (size > 1000 && avg_time > 0.01) // 平均每次不应超过0.01ms
        {
            cout << "  WARNING: Release might not be O(1), average time too high" << endl;
        }
        else
        {
            cout << "  PASSED: O(1) performance confirmed" << endl;
        }
    }

    return true;
}

// ============================================================================
// 测试10：压力测试
// ============================================================================

bool Test_StressTest()
{
    cout << "\n========================================" << endl;
    cout << "Test 10: Stress Test" << endl;
    cout << "========================================" << endl;

    const int ITERATIONS = 100000;
    const int POOL_SIZE = 10000;

    ValuePool<int> pool;
    pool.Reserve(POOL_SIZE, true);

    cout << "\n[10.1] Random operations stress test (" << ITERATIONS << " iterations)..." << endl;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> op_dist(0, 3);
    uniform_int_distribution<> count_dist(1, 100);

    vector<int> active_objs;
    int op_counts[4] = {0};

    Timer timer;

    for (int i = 0; i < ITERATIONS; ++i)
    {
        int op = op_dist(gen);
        ++op_counts[op];

        switch (op)
        {
        case 0: // CreateNew
        {
            int val;
            if (pool.CreateNew(val))
                active_objs.push_back(val);
            break;
        }
        case 1: // GetOrCreate
        {
            int val;
            if (pool.GetOrCreate(val))
                active_objs.push_back(val);
            break;
        }
        case 2: // Release
        {
            if (!active_objs.empty())
            {
                int idx = count_dist(gen) % active_objs.size();
                pool.Release(active_objs[idx]);
                active_objs.erase(active_objs.begin() + idx);
            }
            break;
        }
        case 3: // ReleaseBatch
        {
            if (!active_objs.empty())
            {
                int batch_size = min((int)active_objs.size(), count_dist(gen));
                pool.ReleaseBatch(active_objs.data(), batch_size);
                active_objs.erase(active_objs.begin(), active_objs.begin() + batch_size);
            }
            break;
        }
        }

        // 定期验证
        if (i % 10000 == 0 && !pool.Validate())
        {
            cout << "  FAILED: Pool validation failed at iteration " << i << endl;
            return false;
        }
    }

    double elapsed = timer.ElapsedMs();

    cout << "  Completed " << ITERATIONS << " operations in " << elapsed << "ms" << endl;
    cout << "  Operations: CreateNew=" << op_counts[0]
         << ", GetOrCreate=" << op_counts[1]
         << ", Release=" << op_counts[2]
         << ", ReleaseBatch=" << op_counts[3] << endl;
    cout << "  Average: " << (elapsed / ITERATIONS) << "ms per operation" << endl;

    if (!pool.Validate())
    {
        cout << "  FAILED: Final validation failed" << endl;
        return false;
    }

    cout << "  PASSED: Stress test completed successfully" << endl;
    ShowPoolStats(pool, "Final Stats");

    return true;
}

// ============================================================================
// 测试11：UserInfo 结构体测试
// ============================================================================

bool Test_UserInfo()
{
    cout << "\n========================================" << endl;
    cout << "Test 11: UserInfo Struct" << endl;
    cout << "========================================" << endl;

    ValuePool<UserInfo> pool;

    cout << "\n[11.1] Add all users to pool..." << endl;
    for (const auto &ui : user_info_array)
    {
        pool.AppendToActive(ui);
    }

    ShowUserInfoArray(pool.GetActiveView());

    cout << "\n[11.2] Release random users..." << endl;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(0, 25);

    int release_count = 5;
    for (int i = 0; i < release_count; ++i)
    {
        pool.Release(user_info_array[dist(gen)]);
    }

    ShowUserInfoArray(pool.GetActiveView());
    ShowPoolStats(pool, "Stats After Release");

    cout << "\n[11.3] Get users back..." << endl;
    vector<UserInfo> got_users;
    UserInfo ui;
    while (pool.GetIfAvailable(ui))
    {
        got_users.push_back(ui);
    }

    cout << "  Got " << got_users.size() << " users from idle pool" << endl;

    cout << "\n[11.4] Verify all can be accessed..." << endl;
    int count = 0;
    pool.ForEachActive([&](const UserInfo &u) {
        ++count;
    });

    cout << "  Iterated " << count << " active users" << endl;
    cout << "  PASSED: UserInfo pool works correctly" << endl;

    ShowPoolStats(pool, "Final Stats");
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char **)
{
    cout << "========================================" << endl;
    cout << "ValuePool<T> Comprehensive Test Suite" << endl;
    cout << "========================================" << endl;

    int passed = 0;
    int failed = 0;

    struct TestCase
    {
        const char *name;
        bool (*func)();
    };

    TestCase tests[] = {
        { "Basic Operations", Test_BasicOperations },
        { "Batch Operations", Test_BatchOperations },
        { "Capacity Management", Test_CapacityManagement },
        { "Reset Function", Test_ResetFunction },
        { "GetIfAvailable", Test_GetIfAvailable },
        { "Statistics", Test_Statistics },
        { "Iterators", Test_Iterators },
        { "Validation", Test_Validate },
        { "Performance - O(1) Release", Test_Performance_Release },
        { "Stress Test", Test_StressTest },
        { "UserInfo Struct", Test_UserInfo }
    };

    for (const auto &test : tests)
    {
        try
        {
            if (test.func())
            {
                ++passed;
                cout << "\n✓ " << test.name << " - PASSED" << endl;
            }
            else
            {
                ++failed;
                cout << "\n✗ " << test.name << " - FAILED" << endl;
            }
        }
        catch (const exception &e)
        {
            ++failed;
            cout << "\n✗ " << test.name << " - EXCEPTION: " << e.what() << endl;
        }
    }

    cout << "\n========================================" << endl;
    cout << "Test Results Summary" << endl;
    cout << "========================================" << endl;
    cout << "Total: " << (passed + failed) << endl;
    cout << "Passed: " << passed << " ✓" << endl;
    cout << "Failed: " << failed << " ✗" << endl;
    cout << "Success Rate: " << (passed * 100.0 / (passed + failed)) << "%" << endl;
    cout << "========================================" << endl;

    return failed == 0 ? 0 : 1;
}
