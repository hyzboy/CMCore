/**
 * DataArray 增强测试用例
 * 
 * 测试目标：
 * 1. 深度测试 mem_copy, mem_move, mem_fill_pattern 的正确性
 * 2. 测试 trivially copyable 和 non-trivial 类型
 * 3. 重点测试可能导致崩溃的操作：Delete, DeleteShift, Move, Insert
 * 4. 内存重叠情况测试
 */

#include<hgl/type/DataArray.h>
#include<iostream>
#include<iomanip>
#include<string>
#include<cstring>
#include<type_traits>

using namespace hgl;

// ============================================================================
// 测试类型定义
// ============================================================================

// 简单POD类型
struct Point2D
{
    int x;
    int y;

    Point2D() : x(0), y(0) {}
    Point2D(int _x, int _y) : x(_x), y(_y) {}

    bool operator==(const Point2D& other) const
    {
        return x == other.x && y == other.y;
    }
};

// ============================================================================
// 工具函数
// ============================================================================

template<typename Array>
void PrintDataArray(const Array& arr, const char* label)
{
    using Elem = std::remove_cv_t<std::remove_reference_t<decltype(arr[0])>>;

    std::cout << "  " << std::setw(30) << label << " [" << std::setw(2) << arr.GetCount() 
              << "/" << std::setw(2) << arr.GetAllocCount() << "]: ";

    if constexpr (std::is_same_v<Elem, int>)
    {
        for (int64 i = 0; i < arr.GetCount(); i++)
        {
            if (i > 0) std::cout << ",";
            std::cout << arr[i];
        }
    }
    else if constexpr (std::is_same_v<Elem, Point2D>)
    {
        for (int64 i = 0; i < arr.GetCount(); i++)
        {
            if (i > 0) std::cout << ",";
            std::cout << "(" << arr[i].x << "," << arr[i].y << ")";
        }
    }

    std::cout << std::endl;
}

// ============================================================================
// 测试用例
// ============================================================================

void TestBasicOperations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: Basic DataArray Operations (int)" << std::endl;
    std::cout << "========================================" << std::endl;

    DataArray<int> arr;

    // 测试Resize
    std::cout << "\n[1.1] Resize operations:" << std::endl;
    arr.Resize(10);
    for (int64 i = 0; i < 10; i++)
        arr[i] = i * 10;
    PrintDataArray(arr, "After Resize(10)");

    // 测试Expand
    std::cout << "\n[1.2] Expand operations:" << std::endl;
    arr.Expand(5);
    for (int64 i = 10; i < 15; i++)
        arr[i] = i * 10;
    PrintDataArray(arr, "After Expand(5)");

    // 测试WriteAt
    std::cout << "\n[1.3] WriteAt operations:" << std::endl;
    arr.WriteAt(999, 5);
    PrintDataArray(arr, "After WriteAt(999, 5)");

    int batch[] = {111, 222, 333};
    arr.WriteAt(batch, 0, 3);
    PrintDataArray(arr, "After WriteAt({111,222,333}, 0)");

    // 测试ReadAt
    std::cout << "\n[1.4] ReadAt operations:" << std::endl;
    int value;
    if (arr.ReadAt(value, 5))
        std::cout << "  ReadAt(5) = " << value << std::endl;

    // 测试Find
    std::cout << "\n[1.5] Find operations:" << std::endl;
    std::cout << "  Find(999) = " << arr.Find(999) << std::endl;
    std::cout << "  Find(12345) = " << arr.Find(12345) << std::endl;
}

void TestDeleteOperations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: Delete Operations (Critical!)" << std::endl;
    std::cout << "========================================" << std::endl;

    auto createArray = []() {
        DataArray<int> arr;
        arr.Resize(10);
        for (int64 i = 0; i < 10; i++)
            arr[i] = i;
        return arr;
    };

    // 测试Delete - 从头部删除
    std::cout << "\n[2.1] Delete from beginning:" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Delete(0, 3);
        PrintDataArray(arr, "After Delete(0,3)");
    }

    // 测试Delete - 从中间删除
    std::cout << "\n[2.2] Delete from middle:" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Delete(3, 4);
        PrintDataArray(arr, "After Delete(3,4)");
    }

    // 测试Delete - 从尾部删除
    std::cout << "\n[2.3] Delete from end:" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Delete(7, 3);
        PrintDataArray(arr, "After Delete(7,3)");
    }

    // 测试DeleteShift - 使用mem_move
    std::cout << "\n[2.4] DeleteShift (uses mem_move):" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.DeleteShift(2, 3);
        PrintDataArray(arr, "After DeleteShift(2,3)");
    }

    // 测试边界情况
    std::cout << "\n[2.5] Delete edge cases:" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        
        arr.Delete(-2, 5);  // 负索引
        PrintDataArray(arr, "After Delete(-2,5)");
        
        arr.Delete(5, 100);  // 超出范围
        PrintDataArray(arr, "After Delete(5,100)");
    }
}

void TestMoveOperations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Move Operations (Critical!)" << std::endl;
    std::cout << "========================================" << std::endl;

    auto createArray = []() {
        DataArray<int> arr;
        arr.Resize(10);
        for (int64 i = 0; i < 10; i++)
            arr[i] = i;
        return arr;
    };

    // 测试Move - 向后移动
    std::cout << "\n[3.1] Move forward (may use mem_move with overlap):" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Move(7, 2, 3);  // Move [2,3,4] to position 7
        PrintDataArray(arr, "After Move(7,2,3)");
    }

    // 测试Move - 向前移动
    std::cout << "\n[3.2] Move backward (may use mem_move with overlap):" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Move(2, 7, 3);  // Move [7,8,9] to position 2
        PrintDataArray(arr, "After Move(2,7,3)");
    }

    // 测试Move - 到开始
    std::cout << "\n[3.3] Move to beginning:" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Move(0, 5, 3);
        PrintDataArray(arr, "After Move(0,5,3)");
    }

    // 测试Move - 到结尾
    std::cout << "\n[3.4] Move to end:" << std::endl;
    {
        DataArray<int> arr = createArray();
        PrintDataArray(arr, "Before");
        arr.Move(7, 0, 3);
        PrintDataArray(arr, "After Move(7,0,3)");
    }
}

void TestInsertOperations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Insert Operations" << std::endl;
    std::cout << "========================================" << std::endl;

    DataArray<int> arr;
    arr.Resize(5);
    for (int64 i = 0; i < 5; i++)
        arr[i] = i * 10;

    // 测试Insert - 在开始处插入
    std::cout << "\n[4.1] Insert at beginning:" << std::endl;
    PrintDataArray(arr, "Before");
    int insert1 = 999;
    arr.Insert(0, &insert1, 1);
    PrintDataArray(arr, "After Insert(0, 999)");

    // 测试Insert - 在中间插入
    std::cout << "\n[4.2] Insert in middle:" << std::endl;
    int insert2[] = {111, 222};
    arr.Insert(3, insert2, 2);
    PrintDataArray(arr, "After Insert(3, {111,222})");

    // 测试Insert - 在结尾插入
    std::cout << "\n[4.3] Insert at end:" << std::endl;
    int insert3 = 888;
    arr.Insert(arr.GetCount(), &insert3, 1);
    PrintDataArray(arr, "After Insert(end, 888)");

    // 测试Insert - 触发扩容并保持顺序
    std::cout << "\n[4.4] Insert with grow (keep order):" << std::endl;
    int insert4[] = {777, 666, 555};
    arr.Insert(2, insert4, 3); // 中间插入，触发扩容与后移
    PrintDataArray(arr, "After Insert(2, {777,666,555})");
}

void TestPODType()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: POD Type (Point2D)" << std::endl;
    std::cout << "========================================" << std::endl;

    DataArray<Point2D> arr;

    // 测试基本操作
    std::cout << "\n[5.1] Basic operations:" << std::endl;
    arr.Resize(5);
    for (int64 i = 0; i < 5; i++)
        arr[i] = Point2D(i, i * 10);
    PrintDataArray(arr, "Initial");

    // 测试Delete
    std::cout << "\n[5.2] Delete POD:" << std::endl;
    arr.Delete(1, 2);
    PrintDataArray(arr, "After Delete(1,2)");

    // 测试Move
    std::cout << "\n[5.3] Move POD:" << std::endl;
    arr.Resize(8);
    for (int64 i = 3; i < 8; i++)
        arr[i] = Point2D(i, i * 10);
    PrintDataArray(arr, "Before Move");
    arr.Move(0, 5, 2);
    PrintDataArray(arr, "After Move(0,5,2)");
}


// ============================================================================
// Main
// ============================================================================

int main(int, char**)
{
    std::cout << "========================================" << std::endl;
    std::cout << "DataArray Enhanced Test Suite" << std::endl;
    std::cout << "Testing mem_copy, mem_move, mem_fill_pattern" << std::endl;
    std::cout << "========================================" << std::endl;

    try
    {
        TestBasicOperations();
        TestDeleteOperations();
        TestMoveOperations();
        TestInsertOperations();
        TestPODType();

        std::cout << "\n========================================" << std::endl;
        std::cout << "ALL TESTS COMPLETED" << std::endl;
        std::cout << "Check output for any memory leaks or incorrect behavior" << std::endl;
        std::cout << "========================================" << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "\n!!! EXCEPTION CAUGHT !!!" << std::endl;
        std::cerr << "What: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
