/**
 * 容器互操作性测试
 *
 * 测试目标：
 * 1. ValueArray 和 IndexedList 的数据转换
 * 2. ManagedArray 和 ValueArray<T*> 的兼容性
 * 3. 容器的移动语义
 * 4. 深层拷贝验证
 * 5. 容器间的混合操作
 */

#include<hgl/type/ValueArray.h>
#include<hgl/type/IndexedList.h>
#include<hgl/type/ManagedArray.h>
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
        return id == other.id && value == other.value && strcmp(name, other.name) == 0;
    }
};

// ============================================================================
// TEST 1: ValueArray → IndexedList 转换
// ============================================================================

void test_valuearray_to_indexedvaluearray()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: ValueArray -> IndexedList conversion" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 创建并填充 ValueArray
    std::cout << "\n[1.1] Create source ValueArray:" << std::endl;
    ValueArray<int> va;
    for (int i = 1; i <= 5; ++i)
        va.Add(i * 10);

    TEST_ASSERT(va.GetCount() == 5, "ValueArray contains 5 elements");

    // 转换到 IndexedList
    std::cout << "\n[1.2] Convert to IndexedList:" << std::endl;
    IndexedList<int> ia;
    ia.Reserve(va.GetCount());

    for (int i = 0; i < va.GetCount(); ++i)
    {
        int value;
        va.Get(i, value);
        ia.Add(value);
    }

    TEST_ASSERT(ia.GetCount() == va.GetCount(), "Same element count after conversion");

    // 验证数据一致性
    std::cout << "\n[1.3] Verify data consistency:" << std::endl;
    for (int i = 0; i < va.GetCount(); ++i)
    {
        int va_value = va[i];
        int ia_value = ia[i];

        TEST_ASSERT(va_value == ia_value,
                   "element " << i << " Data consistent (" << va_value << " == " << ia_value << ")");
    }
}

// ============================================================================
// TEST 2: IndexedList → ValueArray 转换
// ============================================================================

void test_indexedvaluearray_to_valuearray()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: IndexedList -> ValueArray conversion" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 创建 IndexedList 并添加删除操作，产生碎片
    std::cout << "\n[2.1] Create IndexedList with fragmentation:" << std::endl;
    IndexedList<double> ia;
    ia.Reserve(10);

    for (int i = 0; i < 10; ++i)
        ia.Add(static_cast<double>(i) * 1.5);

    // 删除奇数位置
    for (int i = 1; i < ia.GetCount(); i += 2)
        ia.Delete(i);

    std::cout << "  Original IndexedList: count=" << ia.GetCount()
              << ", free=" << ia.GetFreeCount() << std::endl;

    // 整理后转换
    std::cout << "\n[2.2] Reorder data:" << std::endl;
    ia.Reorder();
    TEST_ASSERT(ia.IsOrdered(), "Data ordered after Reorder");

    // 转换到 ValueArray
    std::cout << "\n[2.3] Convert to ValueArray:" << std::endl;
    ValueArray<double> va;
    va.Reserve(ia.GetCount());

    for (int i = 0; i < ia.GetCount(); ++i)
    {
        double value = ia[i];
        va.Add(value);
    }

    TEST_ASSERT(va.GetCount() == ia.GetCount(), "Same element count after conversion");

    // 验证数据一致性
    std::cout << "\n[2.4] Verify data integrity:" << std::endl;
    for (int i = 0; i < va.GetCount(); ++i)
    {
        double ia_value = ia[i];
        double va_value = va[i];

        TEST_ASSERT(va_value == ia_value, "element " << i << " Data consistent");
    }
}

// ============================================================================
// TEST 3: 对象指针容器兼容性 (ManagedArray vs ValueArray<T*>)
// ============================================================================

void test_managed_array_pointer_compatibility()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Object pointer container compatibility" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // 创建 ValueArray<TestData*>（TestData 不能用于 ManagedArray，因为它是非平凡类型）
    std::cout << "\n[3.1] Create ValueArray<TestData*>:" << std::endl;
    ValueArray<TestData*> va_ptr;

    va_ptr.Add(new TestData(1, 1.5, "First"));
    va_ptr.Add(new TestData(2, 2.5, "Second"));
    va_ptr.Add(new TestData(3, 3.5, "Third"));

    TEST_ASSERT(va_ptr.GetCount() == 3, "ValueArray<TestData*> contains 3 objects");

    // 创建等效的第二个 ValueArray<TestData*>
    std::cout << "\n[3.2] Create second ValueArray<TestData*>:" << std::endl;
    ValueArray<TestData*> va_ptr2;

    for (int i = 0; i < va_ptr.GetCount(); ++i)
    {
        TestData* ptr = va_ptr[i];
        va_ptr2.Add(ptr);
    }

    TEST_ASSERT(va_ptr2.GetCount() == va_ptr.GetCount(), "Pointer container sizes match");

    // 验证指向同一对象
    std::cout << "\n[3.3] Verify pointers to same object:" << std::endl;
    for (int i = 0; i < va_ptr.GetCount(); ++i)
    {
        TestData* va_ptr_value = va_ptr[i];
        TestData* va_ptr2_value = va_ptr2[i];

        TEST_ASSERT(va_ptr_value == va_ptr2_value, "Pointer " << i << " identical");
        TEST_ASSERT(va_ptr_value->id == i + 1, "Object content correct");
    }

    // 清理（仅清理容器，不删除对象）
    std::cout << "\n[3.4] Container cleanup:" << std::endl;
    for (int i = 0; i < va_ptr.GetCount(); ++i)
        delete va_ptr[i];
    va_ptr.Clear();
    va_ptr2.Clear();
    TEST_ASSERT(va_ptr.IsEmpty(), "ValueArray<T*> Clear succeeded");
}

// ============================================================================
// TEST 4: 容器移动语义测试
// ============================================================================

void test_container_move_semantics()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Container move semantics" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ValueArray 移动
    std::cout << "\n[4.1] ValueArray move construction:" << std::endl;
    {
        ValueArray<int> source;
        source.Add(10);
        source.Add(20);
        source.Add(30);

        int source_count = source.GetCount();

        ValueArray<int> dest = std::move(source);

        TEST_ASSERT(dest.GetCount() == source_count, "Target container has correct elements after move");

        int value;
        dest.Get(0, value);
        TEST_ASSERT(value == 10, "Data correct after move");
    }

    // IndexedList 移动
    std::cout << "\n[4.2] IndexedList move assignment:" << std::endl;
    {
        IndexedList<double> source;
        source.Add(1.5);
        source.Add(2.5);
        source.Add(3.5);

        int source_count = source.GetCount();

        IndexedList<double> dest;
        dest = std::move(source);

        TEST_ASSERT(dest.GetCount() == source_count, "Element count correct after move assignment");

        double value = dest[1];
        TEST_ASSERT(value == 2.5, "Data correct after move assignment");
    }
}

// ============================================================================
// TEST 5: 深层拷贝验证
// ============================================================================

void test_deep_copy_semantics()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: Deep copy verification" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ValueArray 拷贝
    std::cout << "\n[5.1] ValueArray copy independence:" << std::endl;
    {
        ValueArray<int> original;
        original.Add(100);
        original.Add(200);
        original.Add(300);

        ValueArray<int> copy = original;

        // 修改原容器
        original.Set(0, 999);

        // 验证拷贝不受影响
        int original_value, copy_value;
        original.Get(0, original_value);
        copy.Get(0, copy_value);

        TEST_ASSERT(original_value == 999, "Original container modified");
        TEST_ASSERT(copy_value == 100, "Copy unaffected by original modifications");
    }

    // IndexedList 拷贝
    std::cout << "\n[5.2] IndexedList copy independence:" << std::endl;
    {
        IndexedList<TestData> original;
        original.Add(TestData(1, 1.1, "Original"));
        original.Add(TestData(2, 2.2, "Copy"));

        IndexedList<TestData> copy = original;

        // 修改原容器
        TestData new_data(99, 99.9, "Modified");
        original[0] = new_data;  // 使用 operator[] 赋值

        // 验证拷贝不受影响
        TestData orig_value = original[0];
        TestData copy_value = copy[0];

        TEST_ASSERT(orig_value.id == 99, "Original container modified");
        TEST_ASSERT(copy_value.id == 1, "Copy unaffected");
    }
}

// ============================================================================
// TEST 6: 容器操作的组合使用
// ============================================================================

void test_combined_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: Combined container operations" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "\n[6.1] ValueArray + IndexedList mixed operations:" << std::endl;

    // 步骤1: 用 ValueArray 积累数据
    ValueArray<int> va;
    for (int i = 0; i < 10; ++i)
        va.Add(i);

    // 步骤2: 转换到 IndexedList
    IndexedList<int> ia;
    ia.Reserve(va.GetCount());
    for (int i = 0; i < va.GetCount(); ++i)
    {
        int value;
        va.Get(i, value);
        ia.Add(value);
    }

    // 步骤3: 在 IndexedList 中删除
    for (int i = 0; i < 5; ++i)
        ia.Delete(0);

    TEST_ASSERT(ia.GetCount() == 5, "IndexedList contains 5 elements after delete");

    // 步骤4: 转换回 ValueArray
    ValueArray<int> va_final;
    for (int i = 0; i < ia.GetCount(); ++i)
    {
        int value = ia[i];
        va_final.Add(value);
    }

    TEST_ASSERT(va_final.GetCount() == 5, "Final ValueArray contains 5 elements");

    std::cout << "\n[6.2] Verify final data:" << std::endl;
    int expected[] = {5, 6, 7, 8, 9};
    for (int i = 0; i < va_final.GetCount(); ++i)
    {
        int value;
        va_final.Get(i, value);
        TEST_ASSERT(value == expected[i], "element " << i << " value is " << expected[i]);
    }
}

// ============================================================================
// TEST 7: 容器互转内存效率
// ============================================================================

void test_container_conversion_efficiency()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 7: Container conversion memory efficiency" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int LARGE_SIZE = 50000;

    std::cout << "\n[7.1] Large-scale conversion test (" << LARGE_SIZE << " elements):" << std::endl;

    // 创建源 ValueArray
    ValueArray<int> va_source;
    va_source.Reserve(LARGE_SIZE);
    for (int i = 0; i < LARGE_SIZE; ++i)
        va_source.Add(i);

    std::cout << "  Source ValueArray: " << va_source.GetTotalBytes() / 1024 << " KB" << std::endl;

    // 转换到 IndexedList
    auto start = std::chrono::high_resolution_clock::now();

    IndexedList<int> ia;
    ia.Reserve(LARGE_SIZE);
    for (int i = 0; i < va_source.GetCount(); ++i)
    {
        int value = va_source[i];
        ia.Add(value);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "  Target IndexedList: " << ia.GetTotalBytes() / 1024 << " KB" << std::endl;
    std::cout << "  Conversion time: " << duration.count() << " ms" << std::endl;

    TEST_ASSERT(ia.GetCount() == LARGE_SIZE, "Element count correct after conversion");
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char**)
{
    std::cout << "\n╔════════════════════════════════════════╗" << std::endl;
    std::cout << "║    Container Interoperability Test    ║" << std::endl;
    std::cout << "╚════════════════════════════════════════╝" << std::endl;

    try
    {
        test_valuearray_to_indexedvaluearray();
        test_indexedvaluearray_to_valuearray();
        test_managed_array_pointer_compatibility();
        test_container_move_semantics();
        test_deep_copy_semantics();
        test_combined_operations();
        test_container_conversion_efficiency();
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
