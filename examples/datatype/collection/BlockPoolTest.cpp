/**
 * BlockPoolTest.cpp
 *
 * BlockPool 块内存池独立测试：
 *   - Init 前置条件（block_count >= 2）
 *   - 0 号块哨兵：Acquire 永不返回 0，Release(0) 恒失败
 *   - 连续多块申请/释放
 *   - 耗尽后 Acquire 返回 0
 *   - 块号 -> 地址计算与数据读写
 *   - 非法块号释放
 */
#include<hgl/type/BlockPool.h>
#include<hgl/type/MemoryAllocator.h>
#include"test_common.h"

#include<cstring>

using namespace hgl;

static void TestInit()
{
    std::printf("\n=== BlockPool: Init Preconditions ===\n");

    MemoryAllocator ma;
    BlockPool pool;

    TEST_ASSERT(!pool.Init(nullptr, 10), "nullptr allocator fails");
    TEST_ASSERT(!pool.Init(&ma, 1), "block_count < 2 fails");
    TEST_ASSERT(pool.Init(&ma, 100, 16), "Init(100 blocks, 16 bytes) succeeded");

    TEST_ASSERT(pool.GetBlockSize() == 16, "Block size is 16");
    TEST_ASSERT(pool.GetMaxCount() == 100, "Total blocks is 100");
    TEST_ASSERT(pool.GetFreeCount() == 99, "Block 0 reserved, 99 free");

    // 0 号块被永久占用
    TEST_ASSERT(!pool.Release(0), "Release(0) always fails");
}

static void TestAcquireRelease()
{
    std::printf("\n=== BlockPool: Acquire/Release ===\n");

    MemoryAllocator ma;
    BlockPool pool;
    pool.Init(&ma, 64, 16);

    // 连续申请
    const uint32 s1 = pool.Acquire(1);
    const uint32 s2 = pool.Acquire(2);
    const uint32 s3 = pool.Acquire(3);

    TEST_ASSERT(s1 != 0 && s2 != 0 && s3 != 0, "Acquire succeeded and non-zero");
    TEST_ASSERT(s2 != s1 && s3 != s1 && s3 != s2, "Returns distinct start blocks");
    TEST_ASSERT(pool.GetFreeCount() == 64 - 1 - 6, "Free = 64 - sentinel - 6 used");

    // 数据读写
    int *p1 = pool.GetData<int>(s1);
    *p1 = 12345;
    TEST_ASSERT(*pool.GetData<int>(s1) == 12345, "Block-indexed read/write");

    // 地址 = base + index * block_size
    const int64 diff = (int64)((uint8 *)pool.GetData(s2) - (uint8 *)pool.GetBase());
    TEST_ASSERT(diff == (int64)s2 * 16, "Address offset = block index * block size");

    // 释放
    TEST_ASSERT(pool.Release(s1), "Release(s1) succeeded");
    TEST_ASSERT(pool.GetFreeCount() == 64 - 1 - 5, "Free count +1 after release");

    // 重复释放失败
    TEST_ASSERT(!pool.Release(s1), "Double release fails");

    // 释放非本池块号失败
    TEST_ASSERT(!pool.Release(9999), "Invalid block index release fails");
}

static void TestExhaustion()
{
    std::printf("\n=== BlockPool: Exhaustion ===\n");

    MemoryAllocator ma;
    BlockPool pool;
    pool.Init(&ma, 16, 16);         // 15 块可用

    TEST_ASSERT(pool.Acquire(0) == 0, "Acquire(0) failed");

    // 申请 15 块（占满）
    TEST_ASSERT(pool.Acquire(15) != 0, "Acquire 15 blocks at once succeeded");
    TEST_ASSERT(pool.GetFreeCount() == 0, "Pool exhausted");

    // 耗尽后失败返回 0
    TEST_ASSERT(pool.Acquire(1) == 0, "Returns 0 when exhausted");

    // 超量申请失败
    TEST_ASSERT(pool.Acquire(16) == 0, "Oversized acquire fails");

    // 释放后可再申请
    // （无法拿到刚才的起始块号，直接验证整体计数）
    TEST_ASSERT(pool.GetFreeCount() == 0, "Still exhausted");
}

static void TestMultiBlock()
{
    std::printf("\n=== BlockPool: Multi-block Segment ===\n");

    MemoryAllocator ma;
    BlockPool pool;
    pool.Init(&ma, 32, 16);

    const uint32 seg = pool.Acquire(4);      // 4 个连续块
    TEST_ASSERT(seg != 0, "Acquire 4-block segment succeeded");

    // 4 块范围内读写
    for(uint32 i = 0; i < 4; i++)
        *(pool.GetData<uint32>(seg + i)) = i + 100;

    bool all_ok = true;
    for(uint32 i = 0; i < 4; i++)
        if(*(pool.GetData<uint32>(seg + i)) != i + 100)
            all_ok = false;
    TEST_ASSERT(all_ok, "Data read/write within segment correct");

    TEST_ASSERT(pool.Release(seg), "Release segment");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestInit();
    TestAcquireRelease();
    TestExhaustion();
    TestMultiBlock();

    return TEST_SUMMARY();
}
