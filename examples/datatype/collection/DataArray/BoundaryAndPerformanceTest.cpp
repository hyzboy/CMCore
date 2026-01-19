/**
 * DataArray 边界案例和性能测试
 * 
 * 本测试覆盖边界条件和特殊情况：
 * 1. 空数组操作
 * 2. 单元素数组操作
 * 3. 大规模数据操作
 * 4. 边界索引操作
 * 5. 内存重叠情况
 * 6. 性能基准测试
 */

#include<hgl/type/DataArray.h>
#include<iostream>
#include<iomanip>
#include<chrono>
#include<vector>
#include<cstring>

using namespace hgl;

// ============================================================================
// 测试辅助工具
// ============================================================================

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "    ✗ " << message << std::endl; \
            return false; \
        } \
    } while(0)

template<typename T>
void print_array(const DataArray<T>& arr, const char* label, int max_print = 10)
{
    std::cout << "  " << label << " [" << arr.GetCount() << "]: ";
    int count = std::min((int)arr.GetCount(), max_print);
    for (int i = 0; i < count; i++)
    {
        if (i > 0) std::cout << ",";
        std::cout << arr[i];
    }
    if (arr.GetCount() > max_print)
        std::cout << ",...(" << arr.GetCount() - max_print << " more)";
    std::cout << std::endl;
}

// ============================================================================
// 测试用例 1: 空数组边界情况
// ============================================================================

bool test_empty_array_boundary()
{
    std::cout << "\n【测试 1: 空数组边界情况】" << std::endl;
    
    DataArray<int> arr;
    
    // 在空数组上执行操作
    TEST_ASSERT(arr.IsEmpty(), "新建数组应为空");
    TEST_ASSERT(arr.GetCount() == 0, "空数组计数为0");
    TEST_ASSERT(arr.last() == nullptr, "空数组last()应返回nullptr");
    TEST_ASSERT(arr.At(0) == nullptr, "At(0)在空数组上应返回nullptr");
    
    // 查找不应崩溃
    TEST_ASSERT(arr.Find(999) < 0, "在空数组中查找应返回-1");
    
    // 清空空数组
    arr.Clear();
    TEST_ASSERT(arr.GetCount() == 0, "清空空数组后应仍为空");
    
    // 释放空数组
    arr.Free();
    TEST_ASSERT(arr.GetCount() == 0, "释放空数组后应仍为空");
    
    // Zero空数组不应崩溃
    arr.Zero();
    
    return true;
}

// ============================================================================
// 测试用例 2: 单元素数组
// ============================================================================

bool test_single_element_array()
{
    std::cout << "\n【测试 2: 单元素数组】" << std::endl;
    
    DataArray<int> arr;
    arr.Append(42);
    
    TEST_ASSERT(arr.GetCount() == 1, "单元素数组计数为1");
    TEST_ASSERT(arr[0] == 42, "元素值应正确");
    TEST_ASSERT(arr.last() != nullptr, "last()应指向该元素");
    TEST_ASSERT(*arr.last() == 42, "last()值应正确");
    
    TEST_ASSERT(arr.Find(42) == 0, "Find应找到该元素");
    TEST_ASSERT(arr.At(0) != nullptr, "At(0)应返回有效指针");
    
    // 删除唯一的元素
    arr.Delete(0);
    TEST_ASSERT(arr.GetCount() == 0, "删除后应为空");
    
    return true;
}

// ============================================================================
// 测试用例 3: 边界索引操作
// ============================================================================

bool test_boundary_index_operations()
{
    std::cout << "\n【测试 3: 边界索引操作】" << std::endl;
    
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // 访问第一个元素
    TEST_ASSERT(arr.At(0) != nullptr, "At(0)应有效");
    TEST_ASSERT(*arr.At(0) == 0, "第一个元素应正确");
    
    // 访问最后一个元素
    TEST_ASSERT(arr.At(9) != nullptr, "At(9)应有效");
    TEST_ASSERT(*arr.At(9) == 9, "最后一个元素应正确");
    
    // 越界访问
    TEST_ASSERT(arr.At(-1) == nullptr, "At(-1)应返回nullptr");
    TEST_ASSERT(arr.At(10) == nullptr, "At(10)应返回nullptr");
    TEST_ASSERT(arr.At(1000) == nullptr, "At(1000)应返回nullptr");
    
    // 删除第一个元素
    arr.Delete(0);
    TEST_ASSERT(arr.GetCount() == 9, "删除第一个元素后计数为9");
    
    // 删除最后一个元素
    arr.Delete(8);
    TEST_ASSERT(arr.GetCount() == 8, "删除最后一个元素后计数为8");
    
    // 插入到边界
    int data[] = {99};
    arr.Insert(0, data, 1);
    TEST_ASSERT(arr[0] == 99, "在开始处插入应成功");
    
    return true;
}

// ============================================================================
// 测试用例 4: 大规模数据操作
// ============================================================================

bool test_large_scale_operations()
{
    std::cout << "\n【测试 4: 大规模数据操作 (10000 elements)】" << std::endl;
    
    DataArray<int> arr;
    
    // 添加大量元素
    std::cout << "  添加10000个元素..." << std::endl;
    for (int i = 0; i < 10000; i++)
        arr.Append(i);
    
    TEST_ASSERT(arr.GetCount() == 10000, "应有10000个元素");
    
    // 验证数据完整性
    std::cout << "  验证数据完整性..." << std::endl;
    bool data_valid = true;
    for (int i = 0; i < 10000; i++)
    {
        if (arr[i] != i)
        {
            data_valid = false;
            break;
        }
    }
    TEST_ASSERT(data_valid, "所有数据应正确");
    
    // 删除一半
    std::cout << "  删除5000个元素..." << std::endl;
    arr.Delete(0, 5000);
    TEST_ASSERT(arr.GetCount() == 5000, "应有5000个元素");
    
    // 验证删除后的数据
    std::cout << "  验证删除后的数据..." << std::endl;
    for (int i = 0; i < (int)arr.GetCount(); i++)
    {
        if (arr[i] >= 5000)
            data_valid = false;
    }
    
    return true;
}

// ============================================================================
// 测试用例 5: Reserve的多次操作
// ============================================================================

bool test_multiple_reserve_operations()
{
    std::cout << "\n【测试 5: 多次Reserve操作】" << std::endl;
    
    DataArray<int> arr;
    
    // 首次预留
    arr.Reserve(100);
    int64 alloc1 = arr.GetAllocCount();
    TEST_ASSERT(alloc1 >= 100, "首次预留应>=100");
    
    // 多次扩展
    for (int i = 0; i < 5; i++)
    {
        arr.Reserve(alloc1 * 2);
        int64 alloc_new = arr.GetAllocCount();
        TEST_ASSERT(alloc_new >= alloc1, "预留应单调递增");
        alloc1 = alloc_new;
    }
    
    // 数据应仍为空（Reserve不改变count）
    TEST_ASSERT(arr.GetCount() == 0, "Reserve不应改变count");
    
    // 现在添加数据应快速
    for (int i = 0; i < 100; i++)
        arr.Append(i);
    
    TEST_ASSERT(arr.GetCount() == 100, "应成功添加100个元素");
    
    return true;
}

// ============================================================================
// 测试用例 6: 连续Delete操作
// ============================================================================

bool test_consecutive_deletes()
{
    std::cout << "\n【测试 6: 连续Delete操作】" << std::endl;
    
    DataArray<int> arr;
    arr.Resize(20);
    for (int i = 0; i < 20; i++)
        arr[i] = i;
    
    // 多次删除同一位置
    while (arr.GetCount() > 0)
    {
        arr.Delete(0);
        // 验证剩余数据有效
        for (int64 i = 0; i < arr.GetCount(); i++)
            if (arr[i] < 0 || arr[i] >= 20)
                return false;
    }
    
    TEST_ASSERT(arr.GetCount() == 0, "所有元素应被删除");
    
    return true;
}

// ============================================================================
// 测试用例 7: 连续DeleteShift操作
// ============================================================================

bool test_consecutive_delete_shifts()
{
    std::cout << "\n【测试 7: 连续DeleteShift操作】" << std::endl;
    
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // 从后向前删除（避免索引变化问题）
    while (arr.GetCount() > 0)
    {
        arr.DeleteShift(0);
    }
    
    TEST_ASSERT(arr.GetCount() == 0, "所有元素应被删除");
    
    return true;
}

// ============================================================================
// 测试用例 8: 随机访问模式
// ============================================================================

bool test_random_access_pattern()
{
    std::cout << "\n【测试 8: 随机访问模式】" << std::endl;
    
    DataArray<int> arr;
    
    // 创建数组
    for (int i = 0; i < 100; i++)
        arr.Append(i * 10);
    
    // 随机访问验证
    TEST_ASSERT(*arr.At(0) == 0, "At(0)应正确");
    TEST_ASSERT(*arr.At(50) == 500, "At(50)应正确");
    TEST_ASSERT(*arr.At(99) == 990, "At(99)应正确");
    
    // 使用[]访问
    TEST_ASSERT(arr[0] == 0, "operator[0]应正确");
    TEST_ASSERT(arr[50] == 500, "operator[50]应正确");
    TEST_ASSERT(arr[99] == 990, "operator[99]应正确");
    
    return true;
}

// ============================================================================
// 测试用例 9: 内存重叠情况 (Move)
// ============================================================================

bool test_memory_overlap_scenarios()
{
    std::cout << "\n【测试 9: 内存重叠情况 (Move)】" << std::endl;
    
    DataArray<int> arr;
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    // 源区间和目标区间重叠（向后移动）
    std::cout << "  重叠移动 (向后): Move(6, 2, 3)" << std::endl;
    arr.Move(6, 2, 3);
    // 结果：0,1,4,5,6,7,2,3,8,9
    TEST_ASSERT(arr.GetCount() == 10, "元素数应保持");
    
    // 源区间和目标区间重叠（向前移动）
    arr.Resize(10);
    for (int i = 0; i < 10; i++)
        arr[i] = i;
    
    std::cout << "  重叠移动 (向前): Move(2, 6, 3)" << std::endl;
    arr.Move(2, 6, 3);
    TEST_ASSERT(arr.GetCount() == 10, "元素数应保持");
    
    return true;
}

// ============================================================================
// 性能基准测试
// ============================================================================

bool test_performance_benchmarks()
{
    std::cout << "\n【性能基准测试】" << std::endl;
    
    // 基准 1: Append性能
    {
        std::cout << "  基准 1: Append 100000个元素..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        DataArray<int> arr;
        for (int i = 0; i < 100000; i++)
            arr.Append(i);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    耗时: " << duration.count() << " ms" << std::endl;
        std::cout << "    速率: " << (100000.0 / duration.count() * 1000) << " ops/sec" << std::endl;
        
        TEST_ASSERT(arr.GetCount() == 100000, "应添加100000个元素");
    }
    
    // 基准 2: Reserve + Append性能
    {
        std::cout << "  基准 2: Reserve + Append 100000个元素..." << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        
        DataArray<int> arr;
        arr.Reserve(100000);
        for (int i = 0; i < 100000; i++)
            arr.Append(i);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    耗时: " << duration.count() << " ms" << std::endl;
        std::cout << "    速率: " << (100000.0 / duration.count() * 1000) << " ops/sec" << std::endl;
    }
    
    // 基准 3: 随机访问性能
    {
        std::cout << "  基准 3: 随机访问 1000000次..." << std::endl;
        
        DataArray<int> arr;
        for (int i = 0; i < 1000; i++)
            arr.Append(i);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        int sum = 0;
        for (int i = 0; i < 1000000; i++)
            sum += arr[i % arr.GetCount()];
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    耗时: " << duration.count() << " ms" << std::endl;
        std::cout << "    速率: " << (1000000.0 / duration.count() * 1000) << " ops/sec" << std::endl;
        std::cout << "    (sum=" << sum << " - 防止编译器优化)" << std::endl;
    }
    
    // 基准 4: Delete性能
    {
        std::cout << "  基准 4: Delete 操作..." << std::endl;
        
        DataArray<int> arr;
        for (int i = 0; i < 10000; i++)
            arr.Append(i);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        while (arr.GetCount() > 0)
            arr.Delete(0);
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        std::cout << "    删除10000个元素耗时: " << duration.count() << " ms" << std::endl;
    }
    
    return true;
}

// ============================================================================
// 测试用例 10: 比较不同大小的数组
// ============================================================================

bool test_array_comparisons()
{
    std::cout << "\n【测试 10: 数组比较】" << std::endl;
    
    DataArray<int> arr1, arr2, arr3;
    
    // 相同内容不同对象
    for (int i = 0; i < 5; i++)
    {
        arr1.Append(i * 10);
        arr2.Append(i * 10);
    }
    TEST_ASSERT(arr1.compare(arr2) == 0, "内容相同的数组应相等");
    
    // 不同大小
    arr3.Append(100);
    int cmp = arr1.compare(arr3);
    TEST_ASSERT(cmp > 0, "较大的数组应比较大");
    
    // 相同大小不同内容
    DataArray<int> arr4, arr5;
    arr4.Append(1);
    arr4.Append(2);
    arr5.Append(1);
    arr5.Append(3);
    int cmp2 = arr4.compare(arr5);
    TEST_ASSERT(cmp2 != 0, "不同内容应不相等");
    
    return true;
}

// ============================================================================
// 主测试函数
// ============================================================================

int main(int argc, char** argv)
{
    int passed = 0, failed = 0;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  DataArray 边界和性能测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (test_empty_array_boundary()) { passed++; } else { failed++; }
    if (test_single_element_array()) { passed++; } else { failed++; }
    if (test_boundary_index_operations()) { passed++; } else { failed++; }
    if (test_large_scale_operations()) { passed++; } else { failed++; }
    if (test_multiple_reserve_operations()) { passed++; } else { failed++; }
    if (test_consecutive_deletes()) { passed++; } else { failed++; }
    if (test_consecutive_delete_shifts()) { passed++; } else { failed++; }
    if (test_random_access_pattern()) { passed++; } else { failed++; }
    if (test_memory_overlap_scenarios()) { passed++; } else { failed++; }
    if (test_performance_benchmarks()) { passed++; } else { failed++; }
    if (test_array_comparisons()) { passed++; } else { failed++; }
    
    // 汇总测试结果
    std::cout << "\n========================================" << std::endl;
    std::cout << "  测试结果汇总" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  总测试数: " << (passed + failed) << std::endl;
    std::cout << "  通过: " << passed << std::endl;
    std::cout << "  失败: " << failed << std::endl;
    std::cout << "========================================" << std::endl;
    
    return (failed == 0) ? 0 : 1;
}
