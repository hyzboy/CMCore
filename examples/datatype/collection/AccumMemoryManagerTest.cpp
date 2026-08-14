/**
 * AccumMemoryManagerTest.cpp
 *
 * AccumMemoryManager 累计内存管理测试：
 *   - Acquire 分配块（偏移/大小记录）
 *   - Access 访问块内容
 *   - 多次分配互不重叠
 *   - GetTotalBytes / GetBlockCount
 *   - Clear 统一释放
 */
#include<hgl/platform/Platform.h>
#include<hgl/type/AccumMemoryManager.h>
#include"test_common.h"

#include<cstring>

using namespace hgl;

static void TestBasic()
{
    std::printf("\n=== AccumMemoryManager: Basic Allocation ===\n");

    AccumMemoryManager mgr;

    TEST_ASSERT(mgr.GetTotalBytes() == 0, "Initially 0 bytes");
    TEST_ASSERT(mgr.GetBlockCount() == 0, "Initially 0 blocks");

    // 注意：Acquire 返回的 Block* 指向 block_list（std::vector<Block>），
    // 后续 Acquire 触发 vector 扩容后旧指针会失效（类设计缺陷）。
    // 因此测试采用"Allocate one -> verify immediately -> allocate next"的顺序模式。

    // 第一块
    {
        auto *b = mgr.Acquire(16);
        TEST_ASSERT(b != nullptr && b->offset == 0 && b->size == 16, "b1 offset 0 size 16");
        std::memcpy(mgr.Access(b), "hello", 6);
        TEST_ASSERT(std::memcmp(mgr.Access(b), "hello", 5) == 0, "b1 content");
    }

    // 第二块（可能使上一块指针失效）
    {
        auto *b = mgr.Acquire(32);
        TEST_ASSERT(b != nullptr && b->offset == 16 && b->size == 32, "b2 offset 16 size 32");
        std::memcpy(mgr.Access(b), "world", 6);
        TEST_ASSERT(std::memcmp(mgr.Access(b), "world", 5) == 0, "b2 content");
    }

    // 第三块
    {
        auto *b = mgr.Acquire(8);
        TEST_ASSERT(b != nullptr && b->offset == 48 && b->size == 8, "b3 offset 48 size 8");
        std::memcpy(mgr.Access(b), "!", 2);
        TEST_ASSERT(*(char *)mgr.Access(b) == '!', "b3 content");
    }

    TEST_ASSERT(mgr.GetBlockCount() == 3, "3 blocks");
    TEST_ASSERT(mgr.GetTotalBytes() == 56, "Total bytes 16+32+8=56");
}

static void TestInvalid()
{
    std::printf("\n=== AccumMemoryManager: Invalid Parameters ===\n");

    AccumMemoryManager mgr;

    TEST_ASSERT(mgr.Acquire(0) == nullptr, "Acquire(0) failed");
    TEST_ASSERT(mgr.Acquire(-5) == nullptr, "Acquire(negative) fails");
    TEST_ASSERT(mgr.GetBlockCount() == 0, "Failed allocation creates no block");

    AccumMemoryManager::Block *b = mgr.Acquire(4);
    TEST_ASSERT(b != nullptr, "Normal allocation succeeded");
    TEST_ASSERT(mgr.Access(nullptr) == nullptr, "Access(nullptr) returns null");
}

static void TestClear()
{
    std::printf("\n=== AccumMemoryManager: Clear ===\n");

    AccumMemoryManager mgr;
    mgr.Acquire(10);
    mgr.Acquire(20);

    TEST_ASSERT(mgr.GetTotalBytes() == 30, "30 bytes");

    mgr.Clear();
    TEST_ASSERT(mgr.GetTotalBytes() == 0, "0 bytes after Clear");
    TEST_ASSERT(mgr.GetBlockCount() == 0, "0 blocks after Clear");

    // 清空后可继续使用
    mgr.Acquire(5);
    TEST_ASSERT(mgr.GetTotalBytes() == 5, "Can allocate after Clear");
}

int os_main(int, os_char **)
{
    hgltest::reset();

    TestBasic();
    TestInvalid();
    TestClear();

    return TEST_SUMMARY();
}
