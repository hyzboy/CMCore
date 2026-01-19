/**
 * DataArray 综合测试套件
 * 
 * 本测试套件全面覆盖 DataArray 模板类的所有功能：
 * 1. 基本操作（构造、析构、初始化）
 * 2. 内存管理（Reserve、Resize、Expand、Free、Clear）
 * 3. 数据添加和访问（Append、At、ReadAt、WriteAt）
 * 4. 数据删除（Delete、DeleteShift）
 * 5. 数据移动和交换（Move、Exchange、Insert）
 * 6. 查找和比较（Find、compare）
 * 7. 集合操作（WithoutList）
 * 8. 特殊类型（POD类型、非平凡类型）
 */

#include<hgl/type/DataArray.h>
#include<iostream>
#include<iomanip>
#include<string>
#include<cassert>
#include<vector>

using namespace hgl;

// ============================================================================
// 测试辅助工具
// ============================================================================

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "  ✗ FAILED: " << message << std::endl; \
            return false; \
        } \
    } while(0)

#define RUN_TEST(test_func, test_name) \
    do { \
        std::cout << "\n  " << test_name << std::endl; \
        if (test_func()) { \
            std::cout << "    ✓ PASSED" << std::endl; \
            passed++; \
        } else { \
            std::cout << "    ✗ FAILED" << std::endl; \
            failed++; \
        } \
        total++; \
    } while(0)

// ============================================================================
// 测试用例 1: 基本构造和初始化
// ============================================================================

bool test_basic_construction()
{
    // 默认构造
    DataArray<int> arr1;
    TEST_ASSERT(arr1.GetCount() == 0, "Default constructor: count should be 0");
    TEST_ASSERT(arr1.IsEmpty(), "Default constructor: should be empty");
    TEST_ASSERT(arr1.GetAllocCount() == 0, "Default constructor: alloc_count should be 0");
    
    // 带大小的构造
    DataArray<int> arr2(5);
    TEST_ASSERT(arr2.GetCount() == 5, "Constructor with size: count should be 5");
    TEST_ASSERT(arr2.GetAllocCount() >= 5, "Constructor with size: alloc_count >= 5");
    
    // 验证初始化的值
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr2[i] == 0, "Constructor with size: elements should be initialized to 0");
    
    return true;
}

// ============================================================================
// 测试用例 2: 内存管理 - Reserve
// ============================================================================

bool test_reserve()
{
    DataArray<int> arr;
    
    // 预留空间
    TEST_ASSERT(arr.Reserve(10), "Reserve(10) should succeed");
    TEST_ASSERT(arr.GetAllocCount() >= 10, "After Reserve(10), alloc_count >= 10");
    TEST_ASSERT(arr.GetCount() == 0, "Reserve should not change count");
    
    // 预留更小的空间（应该不缩小）
    arr.Reserve(5);
    TEST_ASSERT(arr.GetAllocCount() >= 10, "Reserve(5) after Reserve(10) should not shrink");
    
    // 预留更大的空间
    arr.Reserve(20);
    TEST_ASSERT(arr.GetAllocCount() >= 20, "Reserve(20) should increase alloc_count");
    
    return true;
}

// ============================================================================
// 测试用例 3: 内存管理 - Resize
// ============================================================================

bool test_resize()
{
    DataArray<int> arr;
    
    // 调整到5个元素
    arr.Resize(5);
    TEST_ASSERT(arr.GetCount() == 5, "Resize(5): count should be 5");
    
    // 填充数据
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // 扩展到10个元素
    arr.Resize(10);
    TEST_ASSERT(arr.GetCount() == 10, "Resize(10): count should be 10");
    
    // 验证旧数据保留
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr[i] == i * 10, "Resize: old data should be preserved");
    
    // 验证新数据初始化为0
    for (int i = 5; i < 10; i++)
        TEST_ASSERT(arr[i] == 0, "Resize: new elements should be initialized to 0");
    
    // 缩小到3个元素
    arr.Resize(3);
    TEST_ASSERT(arr.GetCount() == 3, "Resize(3): count should be 3");
    
    // 验证数据完整性
    for (int i = 0; i < 3; i++)
        TEST_ASSERT(arr[i] == i * 10, "Resize down: data should be intact");
    
    return true;
}

// ============================================================================
// 测试用例 4: 内存管理 - Expand
// ============================================================================

bool test_expand()
{
    DataArray<int> arr;
    arr.Resize(5);
    
    int original_count = arr.GetCount();
    arr.Expand(3);
    TEST_ASSERT(arr.GetCount() == original_count + 3, "Expand(3) should add 3 elements");
    
    arr.Expand(2);
    TEST_ASSERT(arr.GetCount() == original_count + 5, "Expand(2) should add 2 more elements");
    
    return true;
}

// ============================================================================
// 测试用例 5: 数据添加 - Append
// ============================================================================

bool test_append()
{
    DataArray<int> arr;
    
    // 添加元素
    for (int i = 0; i < 10; i++)
        arr.Append(i);
    
    TEST_ASSERT(arr.GetCount() == 10, "After 10 appends, count should be 10");
    
    // 验证数据顺序
    for (int i = 0; i < 10; i++)
        TEST_ASSERT(arr[i] == i, "Appended data should be in correct order");
    
    return true;
}

// ============================================================================
// 测试用例 6: 数据访问 - At
// ============================================================================

bool test_at()
{
    DataArray<int> arr;
    arr.Resize(5);
    
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // 有效索引
    TEST_ASSERT(arr.At(0) != nullptr, "At(0) should return valid pointer");
    TEST_ASSERT(*arr.At(2) == 20, "At(2) should return correct value");
    TEST_ASSERT(*arr.At(4) == 40, "At(4) should return correct value");
    
    // 无效索引
    TEST_ASSERT(arr.At(-1) == nullptr, "At(-1) should return nullptr");
    TEST_ASSERT(arr.At(5) == nullptr, "At(5) should return nullptr");
    TEST_ASSERT(arr.At(100) == nullptr, "At(100) should return nullptr");
    
    return true;
}

// ============================================================================
// 测试用例 7: 数据访问 - ReadAt/WriteAt
// ============================================================================

bool test_read_write_at()
{
    DataArray<int> arr;
    arr.Resize(5);
    
    // WriteAt - 单个元素
    TEST_ASSERT(arr.WriteAt(100, 0), "WriteAt(100, 0) should succeed");
    TEST_ASSERT(arr[0] == 100, "WriteAt should set value correctly");
    
    TEST_ASSERT(arr.WriteAt(50, 2), "WriteAt(50, 2) should succeed");
    TEST_ASSERT(arr[2] == 50, "WriteAt should set value correctly");
    
    // WriteAt - 无效索引
    TEST_ASSERT(!arr.WriteAt(999, -1), "WriteAt with negative index should fail");
    TEST_ASSERT(!arr.WriteAt(999, 5), "WriteAt with out-of-range index should fail");
    
    // ReadAt - 单个元素
    int value = 0;
    TEST_ASSERT(arr.ReadAt(value, 0), "ReadAt(value, 0) should succeed");
    TEST_ASSERT(value == 100, "ReadAt should get correct value");
    
    TEST_ASSERT(arr.ReadAt(value, 2), "ReadAt(value, 2) should succeed");
    TEST_ASSERT(value == 50, "ReadAt should get correct value");
    
    // ReadAt - 无效索引
    TEST_ASSERT(!arr.ReadAt(value, -1), "ReadAt with negative index should fail");
    TEST_ASSERT(!arr.ReadAt(value, 5), "ReadAt with out-of-range index should fail");
    
    // WriteAt/ReadAt - 多个元素
    DataArray<int> arr2;
    arr2.Resize(10);
    
    int write_data[] = {1, 2, 3, 4, 5};
    TEST_ASSERT(arr2.WriteAt(write_data, 2, 5), "WriteAt multiple should succeed");
    
    int read_data[5] = {0};
    TEST_ASSERT(arr2.ReadAt(read_data, 2, 5), "ReadAt multiple should succeed");
    
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(read_data[i] == write_data[i], "ReadAt should get correct values");
    
    return true;
}

// ============================================================================
// 测试用例 8: 数据删除 - Delete (不关心顺序)
// ============================================================================

bool test_delete()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // 删除单个元素
    TEST_ASSERT(arr.Delete(5), "Delete(5) should succeed");
    TEST_ASSERT(arr.GetCount() == 9, "After Delete(5), count should be 9");
    
    // 删除多个元素
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    TEST_ASSERT(arr.Delete(2, 3), "Delete(2, 3) should succeed");
    TEST_ASSERT(arr.GetCount() == 7, "After Delete(2, 3), count should be 7");
    
    // 删除无效范围
    TEST_ASSERT(!arr.Delete(-1), "Delete with negative index should fail");
    TEST_ASSERT(!arr.Delete(10), "Delete with out-of-range index should fail");
    TEST_ASSERT(!arr.Delete(0, -1), "Delete with negative count should fail");
    
    // 删除全部
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    TEST_ASSERT(arr.Delete(0, 5), "Delete all should succeed");
    TEST_ASSERT(arr.GetCount() == 0, "After deleting all, count should be 0");
    
    return true;
}

// ============================================================================
// 测试用例 9: 数据删除 - DeleteShift (关心顺序)
// ============================================================================

bool test_delete_shift()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // 删除单个元素
    arr.DeleteShift(3);
    TEST_ASSERT(arr.GetCount() == 9, "After DeleteShift(3), count should be 9");
    
    // 验证顺序保留：0,1,2,4,5,6,7,8,9
    int expected[] = {0, 1, 2, 4, 5, 6, 7, 8, 9};
    for (int i = 0; i < 9; i++)
        TEST_ASSERT(arr[i] == expected[i], "DeleteShift should preserve order");
    
    // 删除多个元素
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    arr.DeleteShift(2, 3);
    TEST_ASSERT(arr.GetCount() == 7, "After DeleteShift(2, 3), count should be 7");
    
    // 验证顺序保留：0,1,5,6,7,8,9
    int expected2[] = {0, 1, 5, 6, 7, 8, 9};
    for (int i = 0; i < 7; i++)
        TEST_ASSERT(arr[i] == expected2[i], "DeleteShift should preserve order");
    
    return true;
}

// ============================================================================
// 测试用例 10: 数据交换 - Exchange
// ============================================================================

bool test_exchange()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // 交换相邻元素
    arr.Exchange(0, 1);
    TEST_ASSERT(arr[0] == 10, "After Exchange(0, 1), arr[0] should be 10");
    TEST_ASSERT(arr[1] == 0, "After Exchange(0, 1), arr[1] should be 0");
    
    // 交换远距离元素
    arr.Exchange(0, 4);
    TEST_ASSERT(arr[0] == 40, "After Exchange(0, 4), arr[0] should be 40");
    TEST_ASSERT(arr[4] == 10, "After Exchange(0, 4), arr[4] should be 10");
    
    // 交换相同位置（无效操作，但应该不崩溃）
    arr.Exchange(2, 2);
    
    return true;
}

// ============================================================================
// 测试用例 11: 数据插入 - Insert
// ============================================================================

bool test_insert()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    // 在开始处插入
    int insert_data1[] = {100, 101};
    TEST_ASSERT(arr.Insert(0, insert_data1, 2), "Insert at beginning should succeed");
    TEST_ASSERT(arr.GetCount() == 7, "After insert, count should be 7");
    TEST_ASSERT(arr[0] == 100, "First inserted element should be correct");
    TEST_ASSERT(arr[1] == 101, "Second inserted element should be correct");
    TEST_ASSERT(arr[2] == 0, "Original first element should be at position 2");
    
    // 在中间插入
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    int insert_data2[] = {999};
    TEST_ASSERT(arr.Insert(3, insert_data2, 1), "Insert in middle should succeed");
    TEST_ASSERT(arr.GetCount() == 6, "After insert, count should be 6");
    TEST_ASSERT(arr[3] == 999, "Inserted element should be at correct position");
    TEST_ASSERT(arr[4] == 3, "Element after insert should shift");
    
    // 在末尾插入
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    int insert_data3[] = {555, 556, 557};
    TEST_ASSERT(arr.Insert(5, insert_data3, 3), "Insert at end should succeed");
    TEST_ASSERT(arr.GetCount() == 8, "After insert at end, count should be 8");
    TEST_ASSERT(arr[5] == 555, "First appended element should be correct");
    
    return true;
}

// ============================================================================
// 测试用例 12: 数据移动 - Move
// ============================================================================

bool test_move()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // 向后移动
    TEST_ASSERT(arr.Move(7, 2, 2), "Move(7, 2, 2) should succeed");
    // 期望：0,1,4,5,6,7,2,3,8,9 (不关心顺序)
    
    // 向前移动
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    TEST_ASSERT(arr.Move(0, 5, 3), "Move(0, 5, 3) should succeed");
    
    // 移动到末尾
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    TEST_ASSERT(arr.Move(8, 0, 2), "Move to end should succeed");
    
    return true;
}

// ============================================================================
// 测试用例 13: 查找 - Find
// ============================================================================

bool test_find()
{
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i * 10;
    
    // 查找存在的元素
    TEST_ASSERT(arr.Find(0) == 0, "Find(0) should return 0");
    TEST_ASSERT(arr.Find(30) == 3, "Find(30) should return 3");
    TEST_ASSERT(arr.Find(90) == 9, "Find(90) should return 9");
    
    // 查找不存在的元素
    TEST_ASSERT(arr.Find(999) < 0, "Find(999) should return < 0");
    TEST_ASSERT(arr.Find(5) < 0, "Find(5) should return < 0");
    
    // 从指定位置查找
    TEST_ASSERT(arr.Find(30, 3) == 3, "Find(30, 3) should return 3");
    TEST_ASSERT(arr.Find(30, 4) < 0, "Find(30, 4) should return < 0");
    
    return true;
}

// ============================================================================
// 测试用例 14: 比较 - Compare
// ============================================================================

bool test_compare()
{
    DataArray<int> arr1, arr2, arr3;
    
    arr1.Resize(3);
    arr1[0] = 1; arr1[1] = 2; arr1[2] = 3;
    
    arr2.Resize(3);
    arr2[0] = 1; arr2[1] = 2; arr2[2] = 3;
    
    arr3.Resize(4);
    arr3[0] = 1; arr3[1] = 2; arr3[2] = 3; arr3[3] = 4;
    
    // 相等的数组
    TEST_ASSERT(arr1.compare(arr2) == 0, "Identical arrays should compare equal");
    
    // 长度不同的数组
    int cmp_result = arr1.compare(arr3);
    TEST_ASSERT(cmp_result < 0, "Shorter array should compare less");
    
    cmp_result = arr3.compare(arr1);
    TEST_ASSERT(cmp_result > 0, "Longer array should compare greater");
    
    return true;
}

// ============================================================================
// 测试用例 15: 清空和释放
// ============================================================================

bool test_clear_and_free()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i;
    
    // Clear - 清空数据但保留内存
    arr.Clear();
    TEST_ASSERT(arr.GetCount() == 0, "After Clear, count should be 0");
    TEST_ASSERT(arr.IsEmpty(), "After Clear, should be empty");
    TEST_ASSERT(arr.GetAllocCount() >= 5, "After Clear, alloc_count should remain");
    
    // Free - 完全释放
    arr.Resize(10);
    arr.Free();
    TEST_ASSERT(arr.GetCount() == 0, "After Free, count should be 0");
    TEST_ASSERT(arr.GetAllocCount() == 0, "After Free, alloc_count should be 0");
    
    return true;
}

// ============================================================================
// 测试用例 16: 运算符重载
// ============================================================================

bool test_operators()
{
    DataArray<int> arr1, arr2;
    
    // 初始化arr1
    arr1.Resize(5);
    for (int i = 0; i < 5; i++)
        arr1[i] = i;
    
    // operator = (复制)
    arr2 = arr1;
    TEST_ASSERT(arr2.GetCount() == 5, "After assignment, count should match");
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr2[i] == i, "After assignment, data should match");
    
    // operator = (initializer_list)
    arr1 = {10, 20, 30};
    TEST_ASSERT(arr1.GetCount() == 3, "After list assignment, count should be 3");
    TEST_ASSERT(arr1[0] == 10, "List assignment should set first element");
    TEST_ASSERT(arr1[1] == 20, "List assignment should set second element");
    TEST_ASSERT(arr1[2] == 30, "List assignment should set third element");
    
    // operator [] (访问)
    arr1[1] = 99;
    TEST_ASSERT(arr1[1] == 99, "operator[] assignment should work");
    
    // operator T * (转换)
    int* ptr = arr1;
    TEST_ASSERT(ptr == arr1.GetData(), "operator T* should return data pointer");
    
    // operator -> (指针访问)
    int* data_ptr = arr1.GetData();
    TEST_ASSERT(data_ptr != nullptr, "GetData should return non-null pointer");
    
    return true;
}

// ============================================================================
// 测试用例 17: 迭代器接口
// ============================================================================

bool test_iterators()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i * 10;
    
    // begin() 和 end()
    int sum = 0;
    for (int* p = arr.begin(); p != arr.end(); ++p)
        sum += *p;
    
    TEST_ASSERT(sum == 100, "Iteration should sum correctly: 0+10+20+30+40=100");
    
    // last()
    TEST_ASSERT(arr.last() != nullptr, "last() should return non-null pointer");
    TEST_ASSERT(*arr.last() == 40, "last() should point to last element");
    
    // last() on empty array
    DataArray<int> empty;
    TEST_ASSERT(empty.last() == nullptr, "last() on empty array should return nullptr");
    
    return true;
}

// ============================================================================
// 测试用例 18: 集合操作 - WithoutList
// ============================================================================

bool test_without_list()
{
    DataArray<int> arr1, arr2, result;
    
    // arr1 = {1, 2, 3, 4, 5}
    arr1.Resize(5);
    for (int i = 0; i < 5; i++)
        arr1[i] = i + 1;
    
    // arr2 = {2, 4}
    arr2.Resize(2);
    arr2[0] = 2;
    arr2[1] = 4;
    
    // 计算 arr1 - arr2 = {1, 3, 5}
    arr1.WithoutList(result, arr2);
    
    TEST_ASSERT(result.GetCount() == 3, "Result should have 3 elements");
    TEST_ASSERT(result[0] == 1, "Result should contain 1");
    TEST_ASSERT(result[1] == 3, "Result should contain 3");
    TEST_ASSERT(result[2] == 5, "Result should contain 5");
    
    return true;
}

// ============================================================================
// 测试用例 19: 字节大小查询
// ============================================================================

bool test_byte_sizes()
{
    DataArray<int> arr;
    
    // 空数组
    TEST_ASSERT(arr.GetTotalBytes() == 0, "Empty array total bytes should be 0");
    TEST_ASSERT(arr.GetAllocBytes() == 0, "Empty array alloc bytes should be 0");
    
    // 有数据的数组
    arr.Resize(5);
    TEST_ASSERT(arr.GetTotalBytes() == 5 * sizeof(int), "Total bytes should match");
    TEST_ASSERT(arr.GetAllocBytes() >= 5 * sizeof(int), "Alloc bytes should be >= total");
    
    return true;
}

// ============================================================================
// 测试用例 20: SetData和Unlink
// ============================================================================

bool test_set_data_and_unlink()
{
    // 创建外部数据
    int external_data[] = {100, 200, 300};
    
    DataArray<int> arr;
    arr.SetData(external_data, 3);
    
    TEST_ASSERT(arr.GetCount() == 3, "SetData should set count");
    TEST_ASSERT(arr.GetAllocCount() == 3, "SetData should set alloc_count");
    TEST_ASSERT(arr[0] == 100, "SetData should reference external data");
    TEST_ASSERT(arr[2] == 300, "SetData should reference external data");
    
    // Unlink - 解除关联
    arr.Unlink();
    TEST_ASSERT(arr.GetCount() == 0, "After Unlink, count should be 0");
    TEST_ASSERT(arr.GetAllocCount() == 0, "After Unlink, alloc_count should be 0");
    TEST_ASSERT(arr.GetData() == nullptr, "After Unlink, data pointer should be nullptr");
    
    // 原始数据不应被影响
    TEST_ASSERT(external_data[0] == 100, "External data should not be affected");
    
    return true;
}

// ============================================================================
// 测试用例 21: Zero函数
// ============================================================================

bool test_zero()
{
    DataArray<int> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = i * 100;
    
    arr.Zero();
    
    for (int i = 0; i < 5; i++)
        TEST_ASSERT(arr[i] == 0, "Zero should set all elements to 0");
    
    return true;
}

// ============================================================================
// 主测试函数
// ============================================================================

int main(int argc, char** argv)
{
    int passed = 0, failed = 0, total = 0;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  DataArray 综合测试套件" << std::endl;
    std::cout << "========================================" << std::endl;
    
    // 基本功能测试
    std::cout << "\n【基本功能】" << std::endl;
    RUN_TEST(test_basic_construction, "Test 1: 基本构造和初始化");
    RUN_TEST(test_reserve, "Test 2: 内存管理 - Reserve");
    RUN_TEST(test_resize, "Test 3: 内存管理 - Resize");
    RUN_TEST(test_expand, "Test 4: 内存管理 - Expand");
    
    // 数据操作测试
    std::cout << "\n【数据操作】" << std::endl;
    RUN_TEST(test_append, "Test 5: 数据添加 - Append");
    RUN_TEST(test_at, "Test 6: 数据访问 - At");
    RUN_TEST(test_read_write_at, "Test 7: 数据访问 - ReadAt/WriteAt");
    RUN_TEST(test_delete, "Test 8: 数据删除 - Delete");
    RUN_TEST(test_delete_shift, "Test 9: 数据删除 - DeleteShift");
    RUN_TEST(test_exchange, "Test 10: 数据交换 - Exchange");
    RUN_TEST(test_insert, "Test 11: 数据插入 - Insert");
    RUN_TEST(test_move, "Test 12: 数据移动 - Move");
    
    // 查询和比较测试
    std::cout << "\n【查询和比较】" << std::endl;
    RUN_TEST(test_find, "Test 13: 数据查找 - Find");
    RUN_TEST(test_compare, "Test 14: 数组比较 - Compare");
    
    // 高级功能测试
    std::cout << "\n【高级功能】" << std::endl;
    RUN_TEST(test_clear_and_free, "Test 15: 清空和释放 - Clear/Free");
    RUN_TEST(test_operators, "Test 16: 运算符重载");
    RUN_TEST(test_iterators, "Test 17: 迭代器接口");
    RUN_TEST(test_without_list, "Test 18: 集合操作 - WithoutList");
    RUN_TEST(test_byte_sizes, "Test 19: 字节大小查询");
    RUN_TEST(test_set_data_and_unlink, "Test 20: SetData和Unlink");
    RUN_TEST(test_zero, "Test 21: Zero函数");
    
    // 汇总测试结果
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试结果汇总" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  总测试数: " << total << std::endl;
    std::cout << "  通过: " << passed << std::endl;
    std::cout << "  失败: " << failed << std::endl;
    std::cout << "  成功率: " << (total > 0 ? (100.0 * passed / total) : 0) << "%" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
