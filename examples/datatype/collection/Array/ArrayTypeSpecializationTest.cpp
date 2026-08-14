/**
 * ValueArray 类型特化测试
 *
 * 测试目标：
 * 1. 验证 ValueArray 对 C 数组类型的支持 (e.g., uint64[256])
 * 2. 测试大对象的内存操作正确性
 * 3. 性能对比：数组类型 vs 标量类型
 * 4. 字节级数据完整性验证
 */

#include<hgl/type/ValueArray.h>
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
// TEST 1: ValueArray<uint256> 基础操作
// ============================================================================

void test_valuearray_uint256_basic()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: ValueArray<uint256> basic operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ValueArray<uint256> array;
    TEST_ASSERT(array.IsEmpty(), "Newly created ValueArray is empty");
    TEST_ASSERT(array.GetCount() == 0, "GetCount() == 0");

    // 添加元素
    std::cout << "\n[1.1] Add operation:" << std::endl;
    uint256 data1, data2, data3;

    fill_pattern(data1, 4, 0x1111111111111111ULL);
    fill_pattern(data2, 4, 0x2222222222222222ULL);
    fill_pattern(data3, 4, 0x3333333333333333ULL);

    array.Add(data1);
    array.Add(data2);
    array.Add(data3);

    TEST_ASSERT(array.GetCount() == 3, "GetCount() == 3 after Add");
    TEST_ASSERT(!array.IsEmpty(), "Array not empty");

    // 获取元素
    std::cout << "\n[1.2] Get operation:" << std::endl;
    uint256 retrieved;

    TEST_ASSERT(array.Get(0, retrieved), "Get(0) succeeded");
    TEST_ASSERT(verify_pattern(retrieved, 4, 0x1111111111111111ULL), "Get(0) data correct");

    TEST_ASSERT(array.Get(1, retrieved), "Get(1) succeeded");
    TEST_ASSERT(verify_pattern(retrieved, 4, 0x2222222222222222ULL), "Get(1) data correct");

    TEST_ASSERT(array.Get(2, retrieved), "Get(2) succeeded");
    TEST_ASSERT(verify_pattern(retrieved, 4, 0x3333333333333333ULL), "Get(2) data correct");

    TEST_ASSERT(!array.Get(3, retrieved), "Get(3) out of range returns false");
    TEST_ASSERT(!array.Get(-1, retrieved), "Get(-1) out of range returns false");

    // 设置元素
    std::cout << "\n[1.3] Set operation:" << std::endl;
    uint256 new_data;
    fill_pattern(new_data, 4, 0x4444444444444444ULL);

    TEST_ASSERT(array.Set(1, new_data), "Set(1) succeeded");
    TEST_ASSERT(array.Get(1, retrieved), "Get(1) succeeds after Set");
    TEST_ASSERT(verify_pattern(retrieved, 4, 0x4444444444444444ULL), "Data correct after Set");
}

// ============================================================================
// TEST 2: ValueArray<uint16k> 大对象操作
// ============================================================================

void test_valuearray_uint16k_large()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: ValueArray<uint16k> large object operations (2048 bytes)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ValueArray<uint16k> large_array;

    std::cout << "\n[2.1] Large object batch operations:" << std::endl;
    const int LARGE_COUNT = 100;

    large_array.Reserve(LARGE_COUNT);
    TEST_ASSERT(large_array.GetAllocCount() >= LARGE_COUNT, "Reserve pre-allocation succeeded");

    // 添加 100 个 2KB 对象
    for (int i = 0; i < LARGE_COUNT; ++i)
    {
        uint16k data;
        fill_pattern(data, 256, 0x1000000000000000ULL + i);
        large_array.Add(data);
    }

    TEST_ASSERT(large_array.GetCount() == LARGE_COUNT, "Bulk Add succeeded");

    // 验证内存占用
    int expected_bytes = LARGE_COUNT * sizeof(uint16k);
    TEST_ASSERT(large_array.GetTotalBytes() == expected_bytes, "GetTotalBytes correct");

    std::cout << "  Total memory usage: " << large_array.GetTotalBytes() / 1024 << " KB" << std::endl;

    // 随机访问验证
    std::cout << "\n[2.2] Random access verification:" << std::endl;
    uint16k retrieved;

    for (int idx : {0, 50, 99})
    {
        TEST_ASSERT(large_array.Get(idx, retrieved), "Get(" << idx << ") succeeded");
        TEST_ASSERT(verify_pattern(retrieved, 256, 0x1000000000000000ULL + idx),
                   "Get(" << idx << ") data pattern correct");
    }

    // 删除操作
    std::cout << "\n[2.3] Delete operation:" << std::endl;
    TEST_ASSERT(large_array.Delete(50), "Delete(50) succeeded");
    TEST_ASSERT(large_array.GetCount() == LARGE_COUNT - 1, "GetCount correct after delete");
    TEST_ASSERT(!large_array.Get(50, retrieved) ||
                !verify_pattern(retrieved, 256, 0x1000000000000000ULL + 50),
               "Deleted elements cannot be verified");
}

// ============================================================================
// TEST 3: 数组类型数据完整性 (Find 和 Delete by Value)
// ============================================================================

void test_array_type_find_delete()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Array type data integrity (Find/DeleteByValue)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    ValueArray<uint256> array;

    uint256 data1, data2, data3;
    fill_pattern(data1, 4, 0x1111111111111111ULL);
    fill_pattern(data2, 4, 0x2222222222222222ULL);
    fill_pattern(data3, 4, 0x3333333333333333ULL);

    array.Add(data1);
    array.Add(data2);
    array.Add(data1);  // 重复元素
    array.Add(data3);

    std::cout << "\n[3.1] Find operation:" << std::endl;

    int index = array.Find(data2);
    TEST_ASSERT(index == 1, "Find(data2) returns index 1");

    index = array.Find(data1);
    TEST_ASSERT(index == 0, "Find(data1) returns first matching index");

    // DeleteByValue
    std::cout << "\n[3.2] DeleteByValue operation:" << std::endl;

    TEST_ASSERT(array.Delete(data1), "DeleteByValue(data1) succeeded");
    TEST_ASSERT(array.GetCount() == 3, "GetCount == 3 after delete");

    int new_index = array.Find(data1);
    TEST_ASSERT(new_index != 0, "First data1 deleted");

    // 再次删除 data1
    TEST_ASSERT(array.Delete(data1), "Second DeleteByValue(data1) succeeded");
    TEST_ASSERT(array.Find(data1) == -1, "All data1 deleted");
}

// ============================================================================
// TEST 4: 性能对比 - 数组类型 vs 标量类型
// ============================================================================

void test_performance_comparison()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Performance comparison - array type vs scalar type" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int TEST_COUNT = 10000;

    std::cout << "\n[4.1] Add performance comparison:" << std::endl;

    // 标量类型
    {
        ValueArray<uint64> scalar_array;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < TEST_COUNT; ++i)
            scalar_array.Add(static_cast<uint64>(i));

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "  uint64 Add × " << TEST_COUNT
                  << ": " << duration.count() << " μs" << std::endl;
    }

    // 数组类型
    {
        ValueArray<uint256> array_type;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < TEST_COUNT; ++i)
        {
            uint256 data;
            fill_pattern(data, 4, static_cast<uint64>(i));
            array_type.Add(data);
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        std::cout << "  uint256 Add × " << TEST_COUNT
                  << ": " << duration.count() << " μs" << std::endl;
    }

    std::cout << "\n[4.2] Get performance comparison:" << std::endl;

    ValueArray<uint64> scalar_array;
    ValueArray<uint256> array_type;

    for (int i = 0; i < 1000; ++i)
    {
        scalar_array.Add(static_cast<uint64>(i));
        uint256 data;
        fill_pattern(data, 4, static_cast<uint64>(i));
        array_type.Add(data);
    }

    // 标量 Get
    {
        auto start = std::chrono::high_resolution_clock::now();
        uint64 value;
        for (int i = 0; i < TEST_COUNT; ++i)
            scalar_array.Get(i % 1000, value);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "  uint64 Get × " << TEST_COUNT << ": " << duration.count() << " μs" << std::endl;
    }

    // 数组类型 Get
    {
        auto start = std::chrono::high_resolution_clock::now();
        uint256 value;
        for (int i = 0; i < TEST_COUNT; ++i)
            array_type.Get(i % 1000, value);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "  uint256 Get × " << TEST_COUNT << ": " << duration.count() << " μs" << std::endl;
    }
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  ValueArray Type Specialization Test  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    try
    {
        test_valuearray_uint256_basic();
        test_valuearray_uint16k_large();
        test_array_type_find_delete();
        test_performance_comparison();
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
