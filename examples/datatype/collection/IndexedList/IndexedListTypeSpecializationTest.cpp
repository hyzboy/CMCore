/**
 * IndexedList 类型特化测试
 *
 * 测试目标：
 * 1. 验证 IndexedList 对 C 数组类型的支持 (e.g., uint64[256])
 * 2. 测试大对象的索引管理
 * 3. 字节级数据完整性验证
 * 4. 空间重用和 Shrink 操作
 */

#include<hgl/type/IndexedList.h>
#include<iostream>
#include<iomanip>
#include<cstring>
#include<cassert>
#include<chrono>

using namespace hgl;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  [PASS]: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  [FAIL]: " << message << std::endl; \
        } \
    } while(0)

// 定义各种数组类型别名
namespace {
    using uint128 = uint64[2];      // 16 字节
    using uint256 = uint64[4];      // 32 字节
    using uint512 = uint64[8];      // 64 字节
    using uint1k = uint64[16];      // 128 字节
    using uint2k = uint64[32];      // 256 字节
    using uint4k = uint64[64];      // 512 字节
    using uint8k = uint64[128];     // 1024 字节
    using uint16k = uint64[256];    // 2048 字节

    // 模式填充：用于验证数据完整性
    void fill_pattern(uint64* arr, size_t count, uint64 seed)
    {
        for (size_t i = 0; i < count; ++i)
            arr[i] = seed ^ i;
    }

    bool verify_pattern(const uint64* arr, size_t count, uint64 seed)
    {
        for (size_t i = 0; i < count; ++i)
        {
            if (arr[i] != (seed ^ i))
                return false;
        }
        return true;
    }
}

// ============================================================================
// TEST 1: IndexedList<uint256> 索引操作
// ============================================================================

void test_indexedlist_uint256_index()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: IndexedList<uint256> index operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedList<uint256> indexed;

    std::cout << "\n[1.1] Basic index add:" << std::endl;
    uint256 data1, data2, data3;

    fill_pattern(data1, 4, 0xAAAAAAAAAAAAAAAAULL);
    fill_pattern(data2, 4, 0xBBBBBBBBBBBBBBBBULL);
    fill_pattern(data3, 4, 0xCCCCCCCCCCCCCCCCULL);

    indexed.Add(data1);
    indexed.Add(data2);
    indexed.Add(data3);

    TEST_ASSERT(indexed.GetCount() == 3, "Index array Add succeeded");

    // 使用索引运算符访问
    std::cout << "\n[1.2] Index operator access:" << std::endl;
    uint256 retrieved;
    memcpy(retrieved, indexed[0], sizeof(uint256));
    TEST_ASSERT(verify_pattern(retrieved, 4, 0xAAAAAAAAAAAAAAAAULL), "operator[0] data correct");

    memcpy(retrieved, indexed[1], sizeof(uint256));
    TEST_ASSERT(verify_pattern(retrieved, 4, 0xBBBBBBBBBBBBBBBBULL), "operator[1] data correct");

    // Insert 操作
    std::cout << "\n[1.3] Insert operation:" << std::endl;
    uint256 data_insert;
    fill_pattern(data_insert, 4, 0xDDDDDDDDDDDDDDDDULL);

    TEST_ASSERT(indexed.Insert(1, data_insert), "Insert(1) succeeded");
    TEST_ASSERT(indexed.GetCount() == 4, "GetCount == 4 after Insert");

    memcpy(retrieved, indexed[1], sizeof(uint256));
    TEST_ASSERT(verify_pattern(retrieved, 4, 0xDDDDDDDDDDDDDDDDULL), "Inserted data at correct position");

    // Delete 操作
    std::cout << "\n[1.4] Delete operation:" << std::endl;
    TEST_ASSERT(indexed.Delete(1), "Delete(1) succeeded");
    TEST_ASSERT(indexed.GetCount() == 3, "GetCount == 3 after Delete");

    memcpy(retrieved, indexed[1], sizeof(uint256));
    TEST_ASSERT(verify_pattern(retrieved, 4, 0xBBBBBBBBBBBBBBBBULL), "Indices correct after Delete");
}

// ============================================================================
// TEST 2: IndexedList<uint16k> 大对象索引管理
// ============================================================================

void test_indexedlist_uint16k_management()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: IndexedList<uint16k> large object management" << std::endl;
    std::cout << "========================================\n" << std::endl;

    IndexedList<uint16k> large_indexed;
    large_indexed.Reserve(50);

    std::cout << "\n[2.1] Add large objects:" << std::endl;
    const int ADD_COUNT = 30;

    for (int i = 0; i < ADD_COUNT; ++i)
    {
        uint16k data;
        fill_pattern(data, 256, 0x2000000000000000ULL + i);
        large_indexed.Add(data);
    }

    TEST_ASSERT(large_indexed.GetCount() == ADD_COUNT, "Added 30 large objects succeeded");

    // 验证空间重用
    std::cout << "\n[2.2] Space reuse detection:" << std::endl;
    int initial_alloc = large_indexed.GetAllocCount();

    // 删除一些元素
    for (int i = 0; i < 10; ++i)
        large_indexed.Delete(0);

    int free_count = large_indexed.GetFreeCount();
    TEST_ASSERT(free_count > 0, "FreeCount > 0 after Delete");

    // 再添加新元素，应该重用空闲空间
    for (int i = 0; i < 5; ++i)
    {
        uint16k data;
        fill_pattern(data, 256, 0x3000000000000000ULL + i);
        large_indexed.Add(data);
    }

    int new_alloc = large_indexed.GetAllocCount();
    TEST_ASSERT(new_alloc <= initial_alloc, "Space reused, allocation did not grow");

    // Shrink 操作
    std::cout << "\n[2.3] Shrink operation:" << std::endl;
    int before_count = large_indexed.GetCount();
    large_indexed.Shrink();
    int after_count = large_indexed.GetCount();

    TEST_ASSERT(before_count == after_count, "Element count unchanged after Shrink");
    TEST_ASSERT(large_indexed.GetFreeCount() == 0, "No free index after Shrink");
}

// ============================================================================
// TEST 3: 多种数组大小类型测试
// ============================================================================

void test_various_array_sizes()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Multiple array-size types" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[3.1] uint128 (16 bytes):" << std::endl;
    {
        IndexedList<uint128> small_array;
        uint128 data;
        fill_pattern(data, 2, 0x1111111111111111ULL);
        small_array.Add(data);

        uint128 retrieved;
        memcpy(retrieved, small_array[0], sizeof(uint128));
        TEST_ASSERT(verify_pattern(retrieved, 2, 0x1111111111111111ULL), "uint128 data correct");
    }

    std::cout << "\n[3.2] uint512 (64 bytes):" << std::endl;
    {
        IndexedList<uint512> medium_array;
        uint512 data;
        fill_pattern(data, 8, 0x2222222222222222ULL);
        medium_array.Add(data);

        uint512 retrieved;
        memcpy(retrieved, medium_array[0], sizeof(uint512));
        TEST_ASSERT(verify_pattern(retrieved, 8, 0x2222222222222222ULL), "uint512 data correct");
    }

    std::cout << "\n[3.3] uint4k (512 bytes):" << std::endl;
    {
        IndexedList<uint4k> large_array;
        uint4k data;
        fill_pattern(data, 64, 0x4444444444444444ULL);
        large_array.Add(data);

        uint4k retrieved;
        memcpy(retrieved, large_array[0], sizeof(uint4k));
        TEST_ASSERT(verify_pattern(retrieved, 64, 0x4444444444444444ULL), "uint4k data correct");
    }
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  IndexedList Type Specialization Test  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    try
    {
        test_indexedlist_uint256_index();
        test_indexedlist_uint16k_management();
        test_various_array_sizes();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║        Test Result Summary            ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << "  Passed: " << tests_passed << std::endl;
    std::cout << "  Failed: " << tests_failed << std::endl;

    if (tests_failed > 0)
    {
        std::cout << "\n❌ Not all tests passed" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "\n✅ All tests passed" << std::endl;
        return tests_failed == 0 ? 0 : 1;
    }
}
