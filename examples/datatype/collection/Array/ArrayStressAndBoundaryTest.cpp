/**
 * ValueArray 容器压力和边界测试
 *
 * 测试目标：
 * 1. 极限规模测试 (1M+ 元素)
 * 2. 边界条件测试 (空容器、单元素)
 * 3. 循环添加/删除压力
 * 4. 预分配效果验证
 * 5. 容器重用和复用模式
 */

#include<hgl/type/ValueArray.h>
#include<hgl/type/ManagedArray.h>
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
// TEST 1: 极端规模测试
// ============================================================================

void test_extreme_scale_valuearray()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: 极端规模测试 - ValueArray" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int MILLION = 1000000;
    const int HALF_MILLION = 500000;

    std::cout << "\n[1.1] 添加 1 百万个元素:" << std::endl;

    ValueArray<int> large_va;

    TIME_OPERATION(
    {
        large_va.Reserve(MILLION);
        for (int i = 0; i < MILLION; ++i)
            large_va.Add(i);
    },
    "添加 1,000,000 个元素"
    );

    TEST_ASSERT(large_va.GetCount() == MILLION, "包含 1,000,000 个元素");

    std::cout << "  内存占用: " << (large_va.GetTotalBytes() / (1024 * 1024)) << " MB" << std::endl;
    std::cout << "  已分配容量: " << large_va.GetAllocCount() << std::endl;

    std::cout << "\n[1.2] 随机访问 1,000,000 次:" << std::endl;

    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, MILLION - 1);

    TIME_OPERATION(
    {
        int sum = 0;
        for (int i = 0; i < 1000000; ++i)
        {
            int idx = dis(gen);
            int value;
            large_va.Get(idx, value);
            sum += value;
        }
    },
    "1,000,000 次随机访问"
    );

    std::cout << "\n[1.3] 删除 500K 个元素:" << std::endl;

    TIME_OPERATION(
    {
        for (int i = 0; i < HALF_MILLION; ++i)
        {
            large_va.Delete(0);
        }
    },
    "删除 500,000 个元素"
    );

    TEST_ASSERT(large_va.GetCount() == MILLION - HALF_MILLION, "删除后元素个数正确");
}

// ============================================================================
// TEST 2: 边界条件测试
// ============================================================================

void test_boundary_conditions()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: 边界条件测试" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[2.1] 空容器操作:" << std::endl;
    {
        ValueArray<int> empty_va;

        TEST_ASSERT(empty_va.IsEmpty(), "新容器为空");
        TEST_ASSERT(empty_va.GetCount() == 0, "GetCount() == 0");
        TEST_ASSERT(empty_va.GetAllocCount() == 0, "GetAllocCount() == 0");

        int dummy;
        TEST_ASSERT(!empty_va.Get(0, dummy), "Get(0) 在空容器上返回 false");
        TEST_ASSERT(!empty_va.Delete(0), "Delete(0) 在空容器上返回 false");
    }

    std::cout << "\n[2.2] 单元素容器操作:" << std::endl;
    {
        ValueArray<double> single_va;
        single_va.Add(3.14159);

        TEST_ASSERT(single_va.GetCount() == 1, "包含 1 个元素");

        double value;
        TEST_ASSERT(single_va.Get(0, value), "Get(0) 成功");
        TEST_ASSERT(abs(value - 3.14159) < 0.00001, "值正确");

        TEST_ASSERT(single_va.Delete(0), "Delete(0) 成功");
        TEST_ASSERT(single_va.IsEmpty(), "删除后为空");
    }

    std::cout << "\n[2.3] Clear vs Free 对比:" << std::endl;
    {
        ValueArray<int> va;
        va.Reserve(100);

        for (int i = 0; i < 50; ++i)
            va.Add(i);

        int before_alloc = va.GetAllocCount();

        va.Clear();
        int after_clear_alloc = va.GetAllocCount();
        TEST_ASSERT(after_clear_alloc == before_alloc, "Clear 后容量不变");
        TEST_ASSERT(va.GetCount() == 0, "Clear 后元素数为 0");

        va.Free();
        int after_free_alloc = va.GetAllocCount();
        TEST_ASSERT(after_free_alloc == 0, "Free 后容量为 0");
    }

    std::cout << "\n[2.4] 越界访问测试:" << std::endl;
    {
        ValueArray<int> va;
        va.Add(10);
        va.Add(20);
        va.Add(30);

        int dummy;
        TEST_ASSERT(!va.Get(-1, dummy), "Get(-1) 返回 false");
        TEST_ASSERT(!va.Get(3, dummy), "Get(3) 越界返回 false");
        TEST_ASSERT(!va.Get(100, dummy), "Get(100) 越界返回 false");

        TEST_ASSERT(!va.Delete(-1), "Delete(-1) 返回 false");
        TEST_ASSERT(!va.Delete(3), "Delete(3) 越界返回 false");
    }
}

// ============================================================================
// TEST 3: 循环压力测试 (反复操作)
// ============================================================================

void test_cyclic_stress()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: 循环压力测试" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int CYCLE_COUNT = 1000;
    const int CYCLE_SIZE = 100;

    std::cout << "\n[3.1] " << CYCLE_COUNT << " 次循环 (每次 Add " << CYCLE_SIZE << " 个):" << std::endl;

    ValueArray<int> stress_va;

    TIME_OPERATION(
    {
        for (int cycle = 0; cycle < CYCLE_COUNT; ++cycle)
        {
            for (int i = 0; i < CYCLE_SIZE; ++i)
                stress_va.Add(cycle * CYCLE_SIZE + i);

            // 删除一半
            for (int i = 0; i < CYCLE_SIZE / 2; ++i)
                stress_va.Delete(0);
        }
    },
    CYCLE_COUNT << " 次循环 (Add/Delete)"
    );

    TEST_ASSERT(stress_va.GetCount() > 0, "循环后容器非空");
    std::cout << "  最终元素数: " << stress_va.GetCount() << std::endl;

    std::cout << "\n[3.2] 快速 Add/Delete 交替:" << std::endl;

    ValueArray<int> alternate_va;

    TIME_OPERATION(
    {
        for (int i = 0; i < 100000; ++i)
        {
            alternate_va.Add(i);
            if (i > 100 && i % 10 == 0)
                alternate_va.Delete(0);
        }
    },
    "100,000 次 Add/Delete 交替"
    );
}

// ============================================================================
// TEST 4: 预分配效果验证
// ============================================================================

void test_preallocation_benefit()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: 预分配效果验证" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int TEST_SIZE = 1000000;

    std::cout << "\n[4.1] 不预分配:" << std::endl;

    long long no_prealloc_time = 0;

    {
        ValueArray<int> va;
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < TEST_SIZE; ++i)
            va.Add(i);

        auto end = std::chrono::high_resolution_clock::now();
        no_prealloc_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    std::cout << "  耗时: " << no_prealloc_time / 1000.0 << " ms" << std::endl;

    std::cout << "\n[4.2] 有预分配:" << std::endl;

    long long prealloc_time = 0;

    {
        ValueArray<int> va;
        va.Reserve(TEST_SIZE);
        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < TEST_SIZE; ++i)
            va.Add(i);

        auto end = std::chrono::high_resolution_clock::now();
        prealloc_time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }

    std::cout << "  耗时: " << prealloc_time / 1000.0 << " ms" << std::endl;

    double improvement = ((double)(no_prealloc_time - prealloc_time) / no_prealloc_time) * 100.0;
    std::cout << "  性能提升: " << std::fixed << std::setprecision(2) << improvement << "%" << std::endl;

    TEST_ASSERT(prealloc_time < no_prealloc_time, "预分配更快");
}

// ============================================================================
// TEST 5: 容器复用模式
// ============================================================================

void test_container_reuse_pattern()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: 容器复用模式" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[5.1] Clear + Reuse:" << std::endl;

    ValueArray<int> reusable;
    reusable.Reserve(10000);

    for (int round = 0; round < 3; ++round)
    {
        std::cout << "  Round " << (round + 1) << ": ";

        // 添加数据
        for (int i = 0; i < 5000; ++i)
            reusable.Add(i + round * 5000);

        std::cout << "added " << reusable.GetCount() << " items, ";

        // 清空但保留容量
        reusable.Clear();
        std::cout << "cleared (alloc=" << reusable.GetAllocCount() << ")" << std::endl;

        TEST_ASSERT(reusable.IsEmpty(), "Clear 后为空");
    }
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  ValueArray 容器压力和边界测试       ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    try
    {
        test_extreme_scale_valuearray();
        test_boundary_conditions();
        test_cyclic_stress();
        test_preallocation_benefit();
        test_container_reuse_pattern();
    }
    catch (const std::exception& e)
    {
        std::cerr << "异常: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║           测试结果统计               ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;
    std::cout << "  通过: " << tests_passed << std::endl;
    std::cout << "  失败: " << tests_failed << std::endl;
    std::cout << "  总耗时: " << total_time_ms << " ms" << std::endl;

    if (tests_failed > 0)
    {
        std::cout << "\n❌ 测试未全部通过" << std::endl;
        return 1;
    }
    else
    {
        std::cout << "\n✅ 所有测试通过" << std::endl;
        return 0;
    }
}
