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
        assert(aim.IsValid(ids[i]));
    }
    return true;
}

int main()
{
    cout << "========================================" << endl;
    cout << "ActiveIDManager FIFO ValueQueue Test Suite" << endl;
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
    assert(!result);
    ExpectCounts(aim, 0, 0);
    cout << "  Active: " << aim.GetActiveCount() << ", Idle: " << aim.GetIdleCount() << endl;
    cout << "Test 1 passed." << endl << endl;

    // --- 参数校验 ---
    cout << "=== Test 2: Invalid Parameter Validation ===" << endl;
    cout << "Testing: Negative/zero counts should be rejected" << endl;
    assert(aim.CreateIdle(-1) == 0);
    assert(aim.CreateActive(nullptr, 0) == 0);
    assert(!aim.Get(tmp, 0));
    assert(aim.Release(nullptr, 0) == 0);
    ExpectCounts(aim, 0, 0);
    cout << "Test 2 passed." << endl << endl;

    // --- 创建闲置ID ---
    cout << "=== Test 3: Create Idle IDs ===" << endl;
    cout << "Testing: CreateIdle(5) should create 5 idle IDs in order" << endl;
    int idle_batch[5];
    int created = aim.CreateIdle(idle_batch, 5);
    cout << "  Created IDs in order: [" << idle_batch[0] << ", " << idle_batch[1] 
         << ", " << idle_batch[2] << ", " << idle_batch[3] << ", " << idle_batch[4] << "]" << endl;
    assert(created == 5);
    ExpectCounts(aim, 0, 5);
    cout << "Test 3 passed." << endl << endl;

    // --- FIFO行为验证 ---
    cout << "=== Test 4: Get from Idle (FIFO ValueQueue Order) ===" << endl;
    cout << "Testing: Get(3) should pop 3 IDs in FIFO order (first-in-first-out)" << endl;
    cout << "  ValueQueue state: [" << idle_batch[0] << "(front), " << idle_batch[1] << ", " 
         << idle_batch[2] << ", " << idle_batch[3] << ", " << idle_batch[4] << "(rear)]" << endl;
    
    int userA[3];
    bool full = aim.Get(userA, 3);
    cout << "  Retrieved IDs: [" << userA[0] << ", " << userA[1] << ", " << userA[2] << "]" << endl;
    assert(full);
    ExpectCounts(aim, 3, 2);
    
    // FIFO保证：应该取出idle_batch的前3个
    cout << "  FIFO Verification (must match front 3):" << endl;
    for (int i = 0; i < 3; ++i) 
    {
        cout << "    userA[" << i << "]=" << userA[i] << " vs idle_batch[" << i << "]=" << idle_batch[i];
        assert(userA[i] == idle_batch[i]);
        cout << " ✓" << endl;
    }
    cout << "Test 4 passed." << endl << endl;

    // --- GetOrCreate验证 ---
    cout << "=== Test 5: GetOrCreate (FIFO Reuse + New) ===" << endl;
    cout << "Testing: GetOrCreate(4) should reuse 2 remaining idle, then create 2 new" << endl;
    cout << "  Current idle queue: [" << idle_batch[3] << "(front), " << idle_batch[4] << "(rear)]" << endl;
    
    int userB[4];
    full = aim.GetOrCreate(userB, 4);
    cout << "  Retrieved IDs: [" << userB[0] << ", " << userB[1] << ", " << userB[2] << ", " << userB[3] << "]" << endl;
    assert(full);
    ExpectCounts(aim, 7, 0);
    
    // 验证FIFO复用：前2个应该是idle_batch[3,4]
    cout << "  FIFO Reuse Verification:" << endl;
    cout << "    userB[0]=" << userB[0] << " (should be idle_batch[3]=" << idle_batch[3] << ")";
    assert(userB[0] == idle_batch[3]);
    cout << " ✓" << endl;
    cout << "    userB[1]=" << userB[1] << " (should be idle_batch[4]=" << idle_batch[4] << ")";
    assert(userB[1] == idle_batch[4]);
    cout << " ✓" << endl;
    cout << "    userB[2,3]=" << userB[2] << "," << userB[3] << " (new IDs created)" << endl;
    cout << "Test 5 passed." << endl << endl;

    // --- Release并验证FIFO继续 ---
    cout << "=== Test 6: Release and Verify FIFO Continues ===" << endl;
    cout << "Testing: Release userA back to idle, then Get should follow FIFO" << endl;
    cout << "  Releasing: [" << userA[0] << ", " << userA[1] << ", " << userA[2] << "]" << endl;
    
    int released = aim.Release(userA, 3);
    cout << "  Released " << released << " IDs" << endl;
    assert(released == 3);
    ExpectCounts(aim, 4, 3);
    
    // 现在idle queue应该有3个ID (userA的3个)
    cout << "  Idle queue now contains released IDs at rear" << endl;
    
    int userC[3];
    full = aim.Get(userC, 3);
    cout << "  Got 3 more IDs: [" << userC[0] << ", " << userC[1] << ", " << userC[2] << "]" << endl;
    assert(full);
    ExpectCounts(aim, 7, 0);
    
    // FIFO保证：应该取出刚释放的userA（按FIFO顺序）
    cout << "  FIFO After-Release Verification:" << endl;
    for (int i = 0; i < 3; ++i) 
    {
        cout << "    userC[" << i << "]=" << userC[i] << " vs userA[" << i << "]=" << userA[i];
        assert(userC[i] == userA[i]);
        cout << " ✓" << endl;
    }
    cout << "Test 6 passed." << endl << endl;

    // --- 对比碎片化改进 ---
    cout << "=== Test 7: FIFO Avoids ID Fragmentation ===" << endl;
    cout << "Testing: Create 10 IDs, release odd indices, verify FIFO reuse pattern" << endl;
    
    ActiveIDManager frag_test;
    frag_test.Reserve(100);
    
    int all_ids[10];
    [[maybe_unused]] int created_idle = frag_test.CreateIdle(all_ids, 10);
    cout << "  Created 10 IDs: [";
    for (int i = 0; i < 10; ++i) cout << all_ids[i] << (i<9?", ":"");
    cout << "]" << endl;
    
    int active_10[10];
    bool got_all = frag_test.Get(active_10, 10);
    assert(got_all);
    assert(frag_test.GetActiveCount() == 10 && frag_test.GetIdleCount() == 0);
    
    // 释放奇数索引：1,3,5,7,9
    int odd_ids[5] = {active_10[1], active_10[3], active_10[5], active_10[7], active_10[9]};
    cout << "  Releasing odd indices: [" << odd_ids[0] << ", " << odd_ids[1] << ", " 
         << odd_ids[2] << ", " << odd_ids[3] << ", " << odd_ids[4] << "]" << endl;
    
    int released_odd = frag_test.Release(odd_ids, 5);
    assert(released_odd == 5);
    cout << "  After release: Active=" << frag_test.GetActiveCount() << ", Idle=" << frag_test.GetIdleCount() << endl;
    
    // 获取5个新ID - FIFO保证应该取出刚释放的ID（按顺序）
    int reused_5[5];
    bool reuse_ok = frag_test.Get(reused_5, 5);
    assert(reuse_ok);
    cout << "  Got 5 reused IDs: [";
    for (int i = 0; i < 5; ++i) cout << reused_5[i] << (i<4?", ":"");
    cout << "]" << endl;
    
    // FIFO保证：reused_5应该与odd_ids完全相同
    cout << "  FIFO Guarantee Check:" << endl;
    bool fifo_match = true;
    for (int i = 0; i < 5; ++i) 
    {
        cout << "    reused_5[" << i << "]=" << reused_5[i] << " vs odd_ids[" << i << "]=" << odd_ids[i];
        if (reused_5[i] == odd_ids[i]) {
            cout << " ✓" << endl;
        } else {
            cout << " ✗ FRAGMENTATION!" << endl;
            fifo_match = false;
        }
    }
    assert(fifo_match);
    cout << "  Result: No fragmentation - FIFO ensures fair reuse!" << endl;
    cout << "Test 7 passed." << endl << endl;

    // --- 释放所有 ---
    cout << "=== Test 8: Release All Active ===" << endl;
    cout << "Testing: ReleaseAllActive()" << endl;
    released = aim.ReleaseAllActive();
    cout << "  Released: " << released << " IDs" << endl;
    ExpectCounts(aim, 0, 7);
    cout << "Test 8 passed." << endl << endl;

    // --- Clear测试 ---
    cout << "=== Test 9: Clear Operations ===" << endl;
    int history_before = aim.GetHistoryMaxId();
    cout << "  HistoryMaxId before: " << history_before << endl;
    
    aim.Clear(false);
    ExpectCounts(aim, 0, 0);
    cout << "  Clear(false): HistoryMaxId=" << aim.GetHistoryMaxId() << " (preserved)" << endl;
    assert(aim.GetHistoryMaxId() == history_before);
    
    aim.Clear(true);
    cout << "  Clear(true): HistoryMaxId=" << aim.GetHistoryMaxId() << " (reset)" << endl;
    assert(aim.GetHistoryMaxId() == 0);
    cout << "Test 9 passed." << endl << endl;

    // --- 大规模FIFO测试 ---
    cout << "=== Test 10: Large Scale FIFO (1000 IDs) ===" << endl;
    cout << "Testing: Bulk operations with FIFO verification" << endl;
    
    ActiveIDManager big;
    big.Reserve(2000);
    const int BIG_N = 1000;
    
    [[maybe_unused]] int idle_created = big.CreateIdle(BIG_N);
    cout << "  Created " << BIG_N << " idle IDs" << endl;
    
    vector<int> big_active(BIG_N);
    bool big_ok = big.Get(big_active.data(), BIG_N);
    assert(big_ok);
    cout << "  Got " << BIG_N << " active IDs" << endl;
    
    int big_released = big.ReleaseAllActive();
    assert(big_released == BIG_N);
    cout << "  Released " << BIG_N << " IDs back to queue" << endl;
    
    vector<int> big_reuse(BIG_N);
    big_ok = big.Get(big_reuse.data(), BIG_N);
    assert(big_ok);
    cout << "  Got " << BIG_N << " reused IDs" << endl;
    
    // 验证FIFO：重新获取的ID应该与原相同（FIFO保证）
    bool all_match = true;
    for (int i = 0; i < BIG_N; ++i) 
    {
        if (big_reuse[i] != big_active[i]) {
            all_match = false;
            cout << "  Mismatch at index " << i << ": " << big_reuse[i] << " != " << big_active[i] << endl;
            break;
        }
    }
    assert(all_match);
    cout << "  FIFO Verified: All " << BIG_N << " IDs reused in order ✓" << endl;
    cout << "Test 10 passed." << endl << endl;

    cout << "========================================" << endl;
    cout << "All Tests Passed!" << endl;
    cout << "ValueQueue (FIFO) Implementation Verified" << endl;
    cout << "No ID fragmentation, fair reuse pattern" << endl;
    cout << "========================================" << endl;

    return 0;
}
