#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <numeric>
#include <unordered_set>
#include <hgl/type/ActiveIDManager.h>
#include <random>

using namespace hgl;
using namespace std;

static void ExpectCounts(const ActiveIDManager& aim, int active, int idle)
{
    assert(aim.GetActiveCount() == active);
    assert(aim.GetIdleCount() == idle);
    assert(aim.GetTotalCount() == active + idle);
}

static bool AllValid(const ActiveIDManager& aim, const int* ids, int n)
{
    for (int i = 0; i < n; ++i)
    {
        if (!aim.IsValid(ids[i])) return false;
    }
    return true;
}

int main()
{
    cout << "========================================" << endl;
    cout << "ActiveIDManager Comprehensive Test Suite" << endl;
    cout << "========================================" << endl << endl;

    ActiveIDManager aim;
    aim.Reserve(64);

    // --- 初始状态 ---
    cout << "=== Test 1: Initial State ===" << endl;
    cout << "Testing: Empty manager, no active/idle IDs, Get should fail" << endl;
    ExpectCounts(aim, 0, 0);
    assert(!aim.HasIdleID());

    int tmp[2];
    bool result = aim.Get(tmp, 2);
    cout << "  Get(tmp, 2) returned: " << result << " (expected: false)" << endl;
    assert(!result);                  // 无闲置，获取失败
    ExpectCounts(aim, 0, 0);
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    cout << "Test 1 passed." << endl << endl;

    // --- 参数校验：非法/空请求 ---
    cout << "=== Test 2: Invalid Parameter Validation ===" << endl;
    cout << "Testing: Negative/zero counts and null pointers should be rejected" << endl;

    cout << "  CreateIdle(-1) returned: " << aim.CreateIdle(-1) << " (expected: 0)" << endl;
    assert(aim.CreateIdle(-1) == 0);            // count<=0 返回0

    cout << "  CreateActive(nullptr, 0) returned: " << aim.CreateActive(nullptr, 0) << " (expected: 0)" << endl;
    assert(aim.CreateActive(nullptr, 0) == 0);  // count<=0 返回0

    result = aim.Get(tmp, 0);
    cout << "  Get(tmp, 0) returned: " << result << " (expected: false)" << endl;
    assert(!result);                   // count<=0 返回false

    cout << "  Release(nullptr, 0) returned: " << aim.Release(nullptr, 0) << " (expected: 0)" << endl;
    assert(aim.Release(nullptr, 0) == 0);       // count<=0 返回0

    ExpectCounts(aim, 0, 0);
    cout << "  State unchanged: Active=" << aim.GetActiveCount() << ", Idle=" << aim.GetIdleCount() << endl;
    cout << "Test 2 passed." << endl << endl;

    // --- 创建闲置ID ---
    cout << "=== Test 3: Create Idle IDs ===" << endl;
    cout << "Testing: CreateIdle(3) should create 3 idle IDs" << endl;
    int idle_batch[3];
    int created = aim.CreateIdle(idle_batch, 3);
    cout << "  Created: " << created << " IDs" << endl;
    cout << "  IDs: [" << idle_batch[0] << ", " << idle_batch[1] << ", " << idle_batch[2] << "]" << endl;
    assert(created == 3);
    ExpectCounts(aim, 0, 3);
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    assert(AllValid(aim, idle_batch, 3));
    assert(aim.HasIdleID());
    cout << "  HasIdleID: true" << endl;
    cout << "Test 3 passed." << endl << endl;

    // --- 从闲置获取（不创建新ID） ---
    cout << "=== Test 4: Get from Idle (LIFO) ===" << endl;
    cout << "Testing: Get(2) should pop 2 IDs from idle stack" << endl;
    int userA[2];
    bool full = aim.Get(userA, 2);
    cout << "  Get(userA, 2) returned: " << full << endl;
    cout << "  Retrieved IDs: [" << userA[0] << ", " << userA[1] << "]" << endl;
    assert(full);
    ExpectCounts(aim, 2, 1);
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    for (int v : userA) {
        cout << "  ID " << v << " - IsActive: " << aim.IsActive(v) << ", IsIdle: " << aim.IsIdle(v) << endl;
        assert(aim.IsActive(v));
        assert(!aim.IsIdle(v));
    }
    cout << "Test 4 passed." << endl << endl;

    // --- GetOrCreate：部分来自Idle，其余新建 ---
    cout << "=== Test 5: GetOrCreate (Mix Reuse + New) ===" << endl;
    cout << "Testing: GetOrCreate(4) with only 1 idle should reuse 1 and create 3 new" << endl;
    int userB[4];
    full = aim.GetOrCreate(userB, 4);
    cout << "  GetOrCreate(userB, 4) returned: " << full << endl;
    cout << "  Retrieved IDs: [" << userB[0] << ", " << userB[1] << ", " << userB[2] << ", " << userB[3] << "]" << endl;
    assert(full);
    ExpectCounts(aim, 6, 0); // active=之前2 + 本次4；idle被取空
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    for (int v : userB) { assert(aim.IsActive(v)); }
    // 验证唯一性
    {
        unordered_set<int> uniq;
        for (int v : userA) uniq.insert(v);
        for (int v : userB) uniq.insert(v);
        cout << "  Unique IDs total: " << uniq.size() << " (expected: 6)" << endl;
        assert((int)uniq.size() == 6);
    }
    cout << "Test 5 passed." << endl << endl;

    // --- 释放部分ID ---
    cout << "=== Test 6: Release Partial Active IDs ===" << endl;
    cout << "Testing: Release(userA, 2) should move 2 IDs back to idle" << endl;
    cout << "  Releasing IDs: [" << userA[0] << ", " << userA[1] << "]" << endl;
    int released = aim.Release(userA, 2);
    cout << "  Released: " << released << " IDs" << endl;
    assert(released == 2);
    ExpectCounts(aim, 4, 2);
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    for (int v : userA) {
        cout << "  ID " << v << " - IsActive: " << aim.IsActive(v) << ", IsIdle: " << aim.IsIdle(v) << endl;
        assert(!aim.IsActive(v));
        assert(aim.IsIdle(v));
    }
    cout << "Test 6 passed." << endl << endl;

    // --- 释放所有活跃ID ---
    cout << "=== Test 7: Release All Active IDs ===" << endl;
    cout << "Testing: ReleaseAllActive() should move all active to idle" << endl;
    cout << "  Before: Active=" << aim.GetActiveCount() << ", Idle=" << aim.GetIdleCount() << endl;
    released = aim.ReleaseAllActive();
    cout << "  Released: " << released << " IDs" << endl;
    ExpectCounts(aim, 0, 6);   // 全部转为空闲
    cout << "  After: Active=" << aim.GetActiveCount() << ", Idle=" << aim.GetIdleCount() << endl;
    assert(released == 4);     // 释放了剩余的4个活跃ID
    cout << "Test 7 passed." << endl << endl;

    // --- 释放无效ID不应改变状态 ---
    cout << "=== Test 8: Release Invalid IDs ===" << endl;
    cout << "Testing: Releasing non-existent IDs should have no effect" << endl;
    int bogus[2] = {9999, 10000};
    cout << "  Trying to release invalid IDs: [" << bogus[0] << ", " << bogus[1] << "]" << endl;
    int before_idle = aim.GetIdleCount();
    released = aim.Release(bogus, 2);
    cout << "  Released: " << released << " (expected: 0)" << endl;
    assert(released == 0);
    ExpectCounts(aim, 0, before_idle);
    cout << "  State unchanged: Active=" << aim.GetActiveCount() << ", Idle=" << aim.GetIdleCount() << endl;
    cout << "Test 8 passed." << endl << endl;

    // --- 清理但保留历史计数 ---
    cout << "=== Test 9: Clear(false) - Keep History ===" << endl;
    cout << "Testing: Clear(false) should reset counts but preserve HistoryMaxId" << endl;
    int history_before = aim.GetHistoryMaxId();
    cout << "  HistoryMaxId before clear: " << history_before << endl;
    aim.Clear(false);
    ExpectCounts(aim, 0, 0);
    cout << "  After clear: Active=" << aim.GetActiveCount() << ", Idle=" << aim.GetIdleCount() << endl;
    cout << "  HistoryMaxId after clear: " << aim.GetHistoryMaxId() << " (should be " << history_before << ")" << endl;
    assert(aim.GetHistoryMaxId() == history_before);
    cout << "Test 9 passed." << endl << endl;

    // --- 清理并重置计数器 ---
    cout << "=== Test 10: Clear(true) - Reset Everything ===" << endl;
    cout << "Testing: Clear(true) should reset all counters to 0" << endl;
    aim.CreateIdle(2);
    cout << "  Created 2 idle IDs, HistoryMaxId: " << aim.GetHistoryMaxId() << endl;
    assert(aim.GetHistoryMaxId() > 0);
    aim.Clear(true);
    ExpectCounts(aim, 0, 0);
    cout << "  After Clear(true): Active=" << aim.GetActiveCount() << ", Idle=" << aim.GetIdleCount() << endl;
    cout << "  HistoryMaxId: " << aim.GetHistoryMaxId() << " (expected: 0)" << endl;
    cout << "  ReleasedCount: " << aim.GetReleasedCount() << " (expected: 0)" << endl;
    assert(aim.GetHistoryMaxId() == 0);
    assert(aim.GetReleasedCount() == 0);
    cout << "Test 10 passed." << endl << endl;

    // --- 再次创建，验证重置后从0开始 ---
    cout << "=== Test 11: Create After Reset ===" << endl;
    cout << "Testing: After reset, IDs should start from 0" << endl;
    int after_reset[2];
    created = aim.CreateActive(after_reset, 2);
    cout << "  Created: " << created << " active IDs" << endl;
    cout << "  IDs: [" << after_reset[0] << ", " << after_reset[1] << "]" << endl;
    assert(created == 2);
    ExpectCounts(aim, 2, 0);
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    cout << "  Verifying IDs are 0 and 1..." << endl;
    assert(after_reset[0] == 0 && after_reset[1] == 1);
    cout << "Test 11 passed." << endl << endl;

    // --- 大规模随机压力测试：10万级别 ---
    {
        cout << "========================================" << endl;
        cout << "=== Test 12: Large Scale Stress Test ===" << endl;
        cout << "========================================" << endl;
        cout << "Testing: 100,000 IDs with 20,000 random operations" << endl;
        cout << "Operations: Get (from idle only), GetOrCreate (mix), Release" << endl;

        const int target_size = 100000;
        ActiveIDManager big;
        big.Reserve(target_size + 1000);

        std::mt19937 rng(123456789);
        std::uniform_int_distribution<int> action_dist(0, 2);      // 0:Get, 1:GetOrCreate, 2:Release
        std::uniform_int_distribution<int> batch_dist(1, 200);     // 每次操作的批量范围

        std::unordered_set<int> active_ids;
        std::unordered_set<int> idle_ids;

        auto assert_state = [&](const char* msg)
        {
            assert((int)active_ids.size() == big.GetActiveCount());
            assert((int)idle_ids.size() == big.GetIdleCount());
            assert((int)(active_ids.size() + idle_ids.size()) == big.GetTotalCount());
            int max_id = -1;
            for (int id : active_ids) { assert(big.IsValid(id)); if (id > max_id) max_id = id; }
            for (int id : idle_ids) { assert(big.IsValid(id)); if (id > max_id) max_id = id; }
            assert(big.GetHistoryMaxId() >= max_id + 1 || max_id == -1);
            (void)msg; // quiet unused warning in release builds
        };

        // 初始化：创建10万个闲置ID
        cout << "  Initializing: Creating " << target_size << " idle IDs..." << endl;
        std::vector<int> buffer(target_size);
        int created = big.CreateIdle(buffer.data(), target_size);
        assert(created == target_size);
        for (int i = 0; i < target_size; ++i)
        {
            idle_ids.insert(buffer[i]);
        }
        cout << "  Initial state: Active=" << big.GetActiveCount() << ", Idle=" << big.GetIdleCount() << endl;
        assert_state("after init idle");

        // 进行一系列随机操作
        const int iterations = 20000;
        buffer.resize(500);

        cout << "  Running " << iterations << " random operations..." << endl;
        int get_count = 0, get_or_create_count = 0, release_count = 0;

        for (int iter = 0; iter < iterations; ++iter)
        {
            // 每1000次操作输出一次进度
            if ((iter + 1) % 2000 == 0)
            {
                cout << "    Progress: " << (iter + 1) << "/" << iterations
                     << " - Active=" << active_ids.size()
                     << ", Idle=" << idle_ids.size() << endl;
            }

            int op = action_dist(rng);
            int count = batch_dist(rng);

            switch (op)
            {
            case 0: // Get from idle only
            {
                get_count++;
                bool ok = big.Get(buffer.data(), count);
                if ((int)idle_ids.size() >= count)
                {
                    assert(ok);
                    for (int i = 0; i < count; ++i)
                    {
                        int id = buffer[i];
                        assert(idle_ids.erase(id) == 1);
                        bool inserted = active_ids.insert(id).second;
                        assert(inserted);
                    }
                }
                else
                {
                    assert(!ok);
                }
                break;
            }
            case 1: // Get or create (mix reuse + new)
            {
                get_or_create_count++;
                bool ok = big.GetOrCreate(buffer.data(), count);
                assert(ok);
                for (int i = 0; i < count; ++i)
                {
                    int id = buffer[i];
                    if (idle_ids.erase(id) == 0)
                    {
                        // 新ID
                        assert(!active_ids.count(id));
                    }
                    bool inserted = active_ids.insert(id).second;
                    assert(inserted);
                }
                break;
            }
            case 2: // Release some active IDs back to idle
            {
                release_count++;
                if (active_ids.empty())
                {
                    // 没有可释放的，验证Release忽略空输入
                    int released = big.Release(nullptr, 0);
                    assert(released == 0);
                    break;
                }

                int real_count = std::min<int>(count, (int)active_ids.size());
                for (int i = 0; i < real_count; ++i)
                {
                    buffer[i] = *active_ids.begin();
                    active_ids.erase(active_ids.begin());
                }

                int released = big.Release(buffer.data(), real_count);
                assert(released == real_count);
                for (int i = 0; i < real_count; ++i)
                {
                    int id = buffer[i];
                    bool inserted = idle_ids.insert(id).second;
                    assert(inserted);
                }
                break;
            }
            }

            assert_state("after op");
        }

        cout << "  Operation statistics:" << endl;
        cout << "    Get operations: " << get_count << endl;
        cout << "    GetOrCreate operations: " << get_or_create_count << endl;
        cout << "    Release operations: " << release_count << endl;

        // 最终再释放所有活跃ID并核对计数
        cout << "  Final cleanup: Releasing all active IDs..." << endl;
        int released_all = big.ReleaseAllActive();
        cout << "  Released: " << released_all << " IDs" << endl;
        assert(released_all == (int)active_ids.size());
        for (int id : active_ids)
        {
            bool inserted = idle_ids.insert(id).second;
            assert(inserted);
        }
        active_ids.clear();
        cout << "  Final state: Active=" << big.GetActiveCount() << ", Idle=" << big.GetIdleCount() << endl;
        cout << "  Total IDs: " << big.GetTotalCount() << endl;
        cout << "  HistoryMaxId: " << big.GetHistoryMaxId() << endl;
        cout << "  ReleasedCount: " << big.GetReleasedCount() << endl;
        assert_state("after release all");
        cout << "Test 12 passed." << endl << endl;
    }

    cout << "========================================" << endl;
    cout << "All ActiveIDManager tests passed." << endl;
    cout << "========================================" << endl;
    return 0;
}
