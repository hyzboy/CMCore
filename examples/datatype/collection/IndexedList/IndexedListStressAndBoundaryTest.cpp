/**
 * IndexedList 容器压力和边界测试
 *
 * 测试目标：
 * 1. 极限规模测试 (500K+ 元素)
 * 2. 边界条件测试 (满容量边界)
 * 3. 内存碎片化场景
 * 4. 空间重用模式
 * 5. Shrink 操作验证
 */

#include<hgl/type/IndexedList.h>
#include<iostream>
#include<iomanip>
#include<random>
#include<cassert>
#include<chrono>

using namespace hgl;

static int tests_passed = 0;
static int tests_failed = 0;
static long long total_time_ms = 0;

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

#define TIME_OPERATION(operation, message) \
    do { \
        auto start = std::chrono::high_resolution_clock::now(); \
        operation; \
        auto end = std::chrono::high_resolution_clock::now(); \
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count(); \
        total_time_ms += ms; \
        std::cout << "  ⏱️  " << message << ": " << ms << " ms" << std::endl; \
    } while(0)

// ============================================================================
// TEST 1: 极端规模测试 - IndexedList
// ============================================================================

void test_extreme_scale_indexedlist()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: Extreme scale test - IndexedList" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int HALF_MILLION = 500000;

    std::cout << "\n[1.1] Add 500K elements:" << std::endl;

    IndexedList<int> large_ia;

    TIME_OPERATION(
    {
        large_ia.Reserve(HALF_MILLION);
        for (int i = 0; i < HALF_MILLION; ++i)
            large_ia.Add(i);
    },
    "Added 500,000 elements"
    );

    TEST_ASSERT(large_ia.GetCount() == HALF_MILLION, "Contains 500,000 elements");

    std::cout << "\n[1.2] Cyclic delete and add (fragmentation):" << std::endl;

    int delete_count = 0;

    TIME_OPERATION(
    {
        // 删除偶数位置
        for (int i = 1; i < large_ia.GetCount(); i += 2)
        {
            large_ia.Delete(i);
            delete_count++;
        }
    },
    "Deleted even-position elements"
    );

    std::cout << "  Free index count: " << large_ia.GetFreeCount() << std::endl;

    std::cout << "\n[1.3] Re-add elements (space reuse):" << std::endl;

    TIME_OPERATION(
    {
        for (int i = 0; i < delete_count; ++i)
            large_ia.Add(HALF_MILLION + i);
    },
    "Re-added " << delete_count << " elements"
    );

    std::cout << "  Free index count: " << large_ia.GetFreeCount() << std::endl;
}

// ============================================================================
// TEST 2: 边界条件测试
// ============================================================================

void test_boundary_conditions()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: Boundary condition test" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[2.1] Full capacity boundary:" << std::endl;
    {
        IndexedList<int> ia;
        ia.Reserve(10);

        for (int i = 0; i < 10; ++i)
            ia.Add(i);

        TEST_ASSERT(ia.GetCount() == 10, "Filled container");
        TEST_ASSERT(ia.GetAllocCount() >= 10, "Allocated capacity sufficient");

        ia.Add(999);  // 超过预分配容量
        TEST_ASSERT(ia.GetCount() == 11, "Auto-expands beyond capacity");
    }
}

// ============================================================================
// TEST 3: 内存碎片化场景
// ============================================================================

void test_fragmentation_scenario()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Memory fragmentation scenario" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[3.1] Index array fragmentation:" << std::endl;

    IndexedList<int> fragmented;
    fragmented.Reserve(100);

    // 填充
    for (int i = 0; i < 100; ++i)
        fragmented.Add(i);

    std::cout << "  Initial: count=" << fragmented.GetCount()
              << ", free=" << fragmented.GetFreeCount() << std::endl;

    // 随机删除产生碎片
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, fragmented.GetCount() - 1);

    int delete_count = 30;
    for (int i = 0; i < delete_count; ++i)
    {
        int idx = dis(gen) % fragmented.GetCount();
        fragmented.Delete(idx);
    }

    std::cout << "  After fragmentation: count=" << fragmented.GetCount()
              << ", free=" << fragmented.GetFreeCount() << std::endl;

    TEST_ASSERT(fragmented.GetFreeCount() > 0, "Free indices exist");
    TEST_ASSERT(!fragmented.IsOrdered(), "Data out of order");

    // Shrink 操作
    std::cout << "\n[3.2] Shrink compaction:" << std::endl;

    TIME_OPERATION(
    {
        fragmented.Shrink();
    },
    "Shrink operation"
    );

    std::cout << "  After compaction: count=" << fragmented.GetCount()
              << ", free=" << fragmented.GetFreeCount() << std::endl;

    TEST_ASSERT(fragmented.GetFreeCount() == 0, "No free index after Shrink");
    // 注意: Shrink 只是填充空闲位置，不保证数据有序
}

// ============================================================================
// TEST 4: 容器空间重用模式
// ============================================================================

void test_space_reuse_pattern()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Container space reuse pattern" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[4.1] IndexedList space reuse:" << std::endl;

    IndexedList<int> reusable_indexed;
    reusable_indexed.Reserve(1000);

    for (int cycle = 0; cycle < 5; ++cycle)
    {
        // 添加
        for (int i = 0; i < 100; ++i)
            reusable_indexed.Add(cycle * 1000 + i);

        // 删除一半
        for (int i = 0; i < 50; ++i)
            reusable_indexed.Delete(0);

        std::cout << "  Cycle " << (cycle + 1) << ": count=" << reusable_indexed.GetCount()
                  << ", free=" << reusable_indexed.GetFreeCount() << std::endl;
    }

    TEST_ASSERT(reusable_indexed.GetCount() > 0, "Container not empty after multiple rounds");
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  IndexedList Stress and Boundary Test  ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    try
    {
        test_extreme_scale_indexedlist();
        test_boundary_conditions();
        test_fragmentation_scenario();
        test_space_reuse_pattern();
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
    std::cout << "  Total time: " << total_time_ms << " ms" << std::endl;

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
