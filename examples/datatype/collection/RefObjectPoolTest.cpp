/**
 * RefObjectPoolTest.cpp
 *
 * RefObjectPool<K,V> 引用计数对象池测试：
 *   - Add/Get 增加引用计数
 *   - Release 递减，归零后移入闲置表
 *   - idle -> active 的重新激活
 *   - GetRefCount 三态（活跃/闲置/不存在）
 *   - Contains* 系列查询
 *   - Stats 统计
 */
#include<hgl/type/RefObjectPool.h>
#include"test_common.h"

using namespace hgl;

static void TestAddGetRelease()
{
    std::printf("\n=== RefObjectPool: Add/Get/Release ===\n");

    RefObjectPool<int, int> pool;

    TEST_ASSERT(pool.GetActiveCount() == 0 && pool.GetIdleCount() == 0, "Initially empty");

    int v100 = 100;
    pool.Add(1, v100);
    TEST_ASSERT(pool.GetActiveCount() == 1, "Active count is 1 after Add");
    TEST_ASSERT(pool.GetRefCount(1) == 1, "Ref count is 1 after Add");

    // Get 增加引用计数
    int v = 0;
    TEST_ASSERT(pool.Get(1, v) && v == 100, "Get succeeds and returns value");
    TEST_ASSERT(pool.GetRefCount(1) == 2, "Ref count is 2 after Get");

    // Get 不存在的 key
    TEST_ASSERT(!pool.Get(99, v), "Get on missing key fails");
    TEST_ASSERT(pool.GetRefCount(99) == -1, "Not found returns -1");

    // 逐个 Release
    pool.Release(1);
    TEST_ASSERT(pool.GetRefCount(1) == 1, "Ref count is 1 after Release");
    TEST_ASSERT(pool.GetActiveCount() == 1, "Still active");

    pool.Release(1);
    TEST_ASSERT(pool.GetRefCount(1) == 0, "Ref count reaches zero");
    TEST_ASSERT(pool.GetActiveCount() == 0, "Moved out of active map at zero");
    TEST_ASSERT(pool.GetIdleCount() == 1, "Moved to idle map at zero");

    // 对闲置/不存在 key 的 Release 是无害的
    pool.Release(1);
    pool.Release(99);
    TEST_ASSERT(pool.GetIdleCount() == 1, "Extra Release has no effect");

    // 从闲置表重新激活
    v = 0;
    TEST_ASSERT(pool.Get(1, v) && v == 100, "Get on idle key succeeds (reactivated)");
    TEST_ASSERT(pool.GetActiveCount() == 1 && pool.GetIdleCount() == 0, "Moved back to active map after activation");
    TEST_ASSERT(pool.GetRefCount(1) == 1, "Ref count is 1 after reactivation");
}

static void TestContains()
{
    std::printf("\n=== RefObjectPool: Contains Series ===\n");

    RefObjectPool<int, int> pool;

    int v10 = 10;
    pool.Add(1, v10);
    TEST_ASSERT(pool.ContainsKey(1), "ContainsKey on active key");
    TEST_ASSERT(pool.ContainsActiveKey(1), "ContainsActiveKey");
    TEST_ASSERT(!pool.ContainsIdleKey(1), "Not idle");
    TEST_ASSERT(!pool.ContainsKey(2), "Key does not exist");

    pool.Release(1);
    TEST_ASSERT(pool.ContainsIdleKey(1), "Idle after Release");
    TEST_ASSERT(!pool.ContainsActiveKey(1), "Not active after Release");
    TEST_ASSERT(pool.ContainsKey(1), "Idle counts as existing");
}

static void TestMultipleKeys()
{
    std::printf("\n=== RefObjectPool: Independent Counts ===\n");

    RefObjectPool<int, int> pool;

    int v10 = 10;
    pool.Add(1, v10);
    int v20 = 20;
    pool.Add(2, v20);
    int v30 = 30;
    pool.Add(3, v30);

    TEST_ASSERT(pool.GetActiveCount() == 3, "Three active keys");

    int v = 0;
    pool.Get(1, v);
    pool.Get(1, v);
    pool.Get(2, v);

    TEST_ASSERT(pool.GetRefCount(1) == 3, "key1 count 3");
    TEST_ASSERT(pool.GetRefCount(2) == 2, "key2 count 2");
    TEST_ASSERT(pool.GetRefCount(3) == 1, "key3 count 1");

    pool.Release(1);
    pool.Release(1);
    pool.Release(1);
    TEST_ASSERT(pool.GetIdleCount() == 1, "key1 zeroed and moved to idle");

    pool.Release(2);
    pool.Release(2);
    TEST_ASSERT(pool.GetIdleCount() == 2, "key2 zeroed and moved to idle");

    // key3 仍活跃
    TEST_ASSERT(pool.GetActiveCount() == 1, "key3 still active");
    TEST_ASSERT(pool.GetRefCount(3) == 1, "key3 count 1");
}

static void TestStats()
{
    std::printf("\n=== RefObjectPool: Stats ===\n");

    RefObjectPool<int, int> pool;

    int v10 = 10, v20 = 20;
    pool.Add(1, v10);
    pool.Add(2, v20);

    int v = 0;
    pool.Get(1, v);         // key1 ref=2
    pool.Release(2);        // key2 -> idle

    const int keys[4] = {1, 2, 3, 4};
    RefPoolStats st;
    pool.Stats(st, keys, 4);

    TEST_ASSERT(st.active == 1, "Stats active == 1 (key1)");
    TEST_ASSERT(st.idle == 1, "Stats idle == 1 (key2)");
    TEST_ASSERT(st.not_found == 2, "Stats not_found == 2 (key3,4)");
    TEST_ASSERT(st.can_free >= 0, "Stats can_free >= 0");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestAddGetRelease();
    TestContains();
    TestMultipleKeys();
    TestStats();

    return TEST_SUMMARY();
}
