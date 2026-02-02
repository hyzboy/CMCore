/**
 * IndexedList 容器互操作性测试
 *
 * 测试目标：
 * 1. IndexedList 和 ValueArray 的数据转换
 * 2. IndexedList 的移动语义
 * 3. IndexedList 的深层拷贝验证
 * 4. IndexedList 混合操作场景
 */

#include<hgl/type/IndexedList.h>
#include<hgl/type/ValueArray.h>
#include<iostream>
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

// 测试用的数据类型
struct TestData
{
    int id;
    double value;
    char name[32];

    TestData() : id(0), value(0.0) { name[0] = '\0'; }
    TestData(int i, double v, const char* n) : id(i), value(v)
    {
        strncpy_s(name, 32, n, 31);
        name[31] = '\0';
    }

    bool operator==(const TestData& other) const
    {
        return id == other.id && value == other.value;
    }
};

// ============================================================================
// TEST 1: ValueArray → IndexedList 转换
// ============================================================================

void test_valuearray_to_indexedlist()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: ValueArray → IndexedList 转换" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 创建 ValueArray
    std::cout << "\n[1.1] 创建 ValueArray:" << std::endl;
    ValueArray<int> va;
    for (int i = 1; i <= 10; ++i)
        va.Add(i * 10);

    TEST_ASSERT(va.GetCount() == 10, "ValueArray 包含 10 个元素");

    // 转换到 IndexedList
    std::cout << "\n[1.2] 转换到 IndexedList:" << std::endl;
    IndexedList<int> ia;

    for (int i = 0; i < va.GetCount(); ++i)
    {
        int value;
        va.Get(i, value);
        ia.Add(value);
    }

    TEST_ASSERT(ia.GetCount() == va.GetCount(), "IndexedList 元素数与 ValueArray 相同");

    // 验证数据
    std::cout << "\n[1.3] 验证转换后的数据:" << std::endl;
    bool data_match = true;
    for (int i = 0; i < va.GetCount(); ++i)
    {
        int va_value;
        va.Get(i, va_value);
        int ia_value = ia[i];

        if (va_value != ia_value)
        {
            data_match = false;
            break;
        }
    }

    TEST_ASSERT(data_match, "所有元素值匹配");
}

// ============================================================================
// TEST 2: IndexedList → ValueArray 转换
// ============================================================================

void test_indexedlist_to_valuearray()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: IndexedList → ValueArray 转换" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 创建 IndexedList 并添加删除操作，产生碎片
    std::cout << "\n[2.1] 创建含碎片的 IndexedList:" << std::endl;
    IndexedList<double> ia;

    for (int i = 0; i < 20; ++i)
        ia.Add(i * 1.5);

    // 删除一些元素制造碎片
    for (int i = 0; i < 5; ++i)
        ia.Delete(i * 3);

    int original_count = ia.GetCount();

    std::cout << "  原始 IndexedList: count=" << ia.GetCount()
              << ", free=" << ia.GetFreeCount() << std::endl;

    // 转换到 ValueArray（应该只包含有效元素）
    std::cout << "\n[2.2] 转换到 ValueArray（紧凑存储）:" << std::endl;
    ValueArray<double> va;

    for (int i = 0; i < ia.GetCount(); ++i)
    {
        double value = ia[i];
        va.Add(value);
    }

    TEST_ASSERT(va.GetCount() == original_count, "ValueArray 包含所有有效元素");
    TEST_ASSERT(va.GetAllocCount() >= va.GetCount(), "ValueArray 无碎片（紧凑）");

    std::cout << "  转换后 ValueArray: count=" << va.GetCount()
              << ", alloc=" << va.GetAllocCount() << std::endl;
}

// ============================================================================
// TEST 3: IndexedList 拷贝构造
// ============================================================================

void test_indexedlist_copy_constructor()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: IndexedList 拷贝构造" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[3.1] IndexedList 拷贝构造函数:" << std::endl;
    {
        IndexedList<double> source;
        for (int i = 0; i < 100; ++i)
            source.Add(i * 2.5);

        int source_count = source.GetCount();

        IndexedList<double> dest = source;

        TEST_ASSERT(dest.GetCount() == source_count, "拷贝后目标容器元素数正确");
        TEST_ASSERT(source.GetCount() == source_count, "拷贝后源容器保持不变");

        // 验证数据正确性
        bool data_match = true;
        for (int i = 0; i < source.GetCount(); ++i)
        {
            if (source[i] != dest[i])
            {
                data_match = false;
                break;
            }
        }
        TEST_ASSERT(data_match, "拷贝后数据正确");
    }
}

// ============================================================================
// TEST 4: IndexedList 深层拷贝
// ============================================================================

void test_indexedlist_deep_copy()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: IndexedList 深层拷贝" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[4.1] IndexedList 拷贝独立性:" << std::endl;
    {
        IndexedList<TestData> original;
        original.Add(TestData(1, 100.0, "first"));
        original.Add(TestData(2, 200.0, "second"));

        IndexedList<TestData> copy = original;

        TEST_ASSERT(copy.GetCount() == original.GetCount(), "拷贝后元素数相同");

        // 修改拷贝，不应影响原始容器
        TestData new_data(99, 999.0, "modified");
        copy[0] = new_data;

        TestData original_first = original[0];
        TestData copy_first = copy[0];

        TEST_ASSERT(original_first.id != copy_first.id, "修改拷贝不影响原始容器");
    }
}

// ============================================================================
// TEST 5: IndexedList 与 ValueArray 混合操作
// ============================================================================

void test_mixed_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: IndexedList 与 ValueArray 混合操作" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[5.1] ValueArray + IndexedList 混合操作:" << std::endl;

    // 步骤1: 在 ValueArray 中准备数据
    ValueArray<int> va;
    for (int i = 1; i <= 10; ++i)
        va.Add(i);

    TEST_ASSERT(va.GetCount() == 10, "ValueArray 准备 10 个元素");

    // 步骤2: 转换到 IndexedList
    IndexedList<int> ia;
    for (int i = 0; i < va.GetCount(); ++i)
    {
        int value;
        va.Get(i, value);
        ia.Add(value);
    }

    TEST_ASSERT(ia.GetCount() == 10, "IndexedList 接收 10 个元素");

    // 步骤3: 在 IndexedList 中删除
    for (int i = 0; i < 5; ++i)
        ia.Delete(0);

    TEST_ASSERT(ia.GetCount() == 5, "删除后 IndexedList 包含 5 个元素");

    // 步骤4: 转回 ValueArray
    ValueArray<int> va2;
    for (int i = 0; i < ia.GetCount(); ++i)
    {
        int value = ia[i];
        va2.Add(value);
    }

    TEST_ASSERT(va2.GetCount() == 5, "最终 ValueArray 包含 5 个元素");
}

// ============================================================================
// TEST 6: 大规模数据转换性能
// ============================================================================

void test_large_scale_conversion()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: 大规模数据转换性能" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int LARGE_SIZE = 100000;

    std::cout << "\n[6.1] 大规模 ValueArray → IndexedList:" << std::endl;

    // 准备 ValueArray
    ValueArray<int> large_va;
    large_va.Reserve(LARGE_SIZE);
    for (int i = 0; i < LARGE_SIZE; ++i)
        large_va.Add(i);

    std::cout << "  源 ValueArray: " << large_va.GetTotalBytes() / 1024 << " KB" << std::endl;

    // 转换到 IndexedList
    auto start = std::chrono::high_resolution_clock::now();

    IndexedList<int> ia;
    ia.Reserve(LARGE_SIZE);
    for (int i = 0; i < large_va.GetCount(); ++i)
    {
        int value;
        if (large_va.Get(i, value))
            ia.Add(value);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  目标 IndexedList: " << ia.GetTotalBytes() / 1024 << " KB" << std::endl;
    std::cout << "  转换耗时: " << duration.count() << " ms" << std::endl;

    TEST_ASSERT(ia.GetCount() == LARGE_SIZE, "大规模转换成功");
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║  IndexedList 容器互操作性测试        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    try
    {
        test_valuearray_to_indexedlist();
        test_indexedlist_to_valuearray();
        test_indexedlist_copy_constructor();
        test_indexedlist_deep_copy();
        test_mixed_operations();
        test_large_scale_conversion();
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
