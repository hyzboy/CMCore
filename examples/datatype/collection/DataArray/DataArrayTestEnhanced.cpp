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

// 带有动态内存的非平凡类
class TrackedObject
{
public:
    int id;
    std::string data;
    int* ref;

    static int aliveCount;
    static int totalCreated;
    static int totalDestroyed;
    static int copyCount;

    TrackedObject() : id(0), data(""), ref(new int(0))
    {
        aliveCount++;
        totalCreated++;
        std::cout << "    [T" << id << "] Constructed (default)" << std::endl;
    }

    TrackedObject(int _id, const std::string& _data) 
        : id(_id), data(_data), ref(new int(_id))
    {
        aliveCount++;
        totalCreated++;
        std::cout << "    [T" << id << "] Constructed (" << data << ")" << std::endl;
    }

    TrackedObject(const TrackedObject& other)
        : id(other.id), data(other.data), ref(new int(*other.ref))
    {
        aliveCount++;
        copyCount++;
        std::cout << "    [T" << id << "] Copy constructed from T" << other.id << std::endl;
    }

    TrackedObject& operator=(const TrackedObject& other)
    {
        if (this != &other)
        {
            id = other.id;
            data = other.data;
            *ref = *other.ref;
            copyCount++;
            std::cout << "    [T" << id << "] Copy assigned from T" << other.id << std::endl;
        }
        return *this;
    }

    ~TrackedObject()
    {
        aliveCount--;
        totalDestroyed++;
        std::cout << "    [T" << id << "] Destroyed (" << data << ") [alive=" << aliveCount << "]" << std::endl;
        delete ref;
    }

    bool operator==(const TrackedObject& other) const
    {
        return id == other.id && data == other.data;
    }
};

int TrackedObject::aliveCount = 0;
int TrackedObject::totalCreated = 0;
int TrackedObject::totalDestroyed = 0;
int TrackedObject::copyCount = 0;

// ============================================================================
// 工具函数
// ============================================================================

template<typename T>
void PrintDataArray(const DataArray<T>& arr, const char* label)
{
    std::cout << "  " << std::setw(30) << label << " [" << std::setw(2) << arr.GetCount() 
              << "/" << std::setw(2) << arr.GetAllocCount() << "]: ";

    if constexpr (std::is_same_v<T, int>)
    {
        for (int64 i = 0; i < arr.GetCount(); i++)
        {
            if (i > 0) std::cout << ",";
            std::cout << arr[i];
        }
    }
    else if constexpr (std::is_same_v<T, Point2D>)
    {
        for (int64 i = 0; i < arr.GetCount(); i++)
        {
            if (i > 0) std::cout << ",";
            std::cout << "(" << arr[i].x << "," << arr[i].y << ")";
        }
    }
    else if constexpr (std::is_same_v<T, TrackedObject>)
    {
        for (int64 i = 0; i < arr.GetCount(); i++)
        {
            if (i > 0) std::cout << ",";
            std::cout << "{" << arr[i].id << ":" << arr[i].data << "}";
        }
    }

    std::cout << std::endl;
}

void ResetTracking()
{
    TrackedObject::aliveCount = 0;
    TrackedObject::totalCreated = 0;
    TrackedObject::totalDestroyed = 0;
    TrackedObject::copyCount = 0;
}

void PrintTracking()
{
    std::cout << "    Tracking: Created=" << TrackedObject::totalCreated
              << ", Destroyed=" << TrackedObject::totalDestroyed
              << ", Alive=" << TrackedObject::aliveCount
              << ", Copies=" << TrackedObject::copyCount << std::endl;
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

void TestNonTrivialType()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: Non-Trivial Type (TrackedObject)" << std::endl;
    std::cout << "========================================" << std::endl;

    ResetTracking();

    // 测试基本操作
    std::cout << "\n[6.1] Create and fill array:" << std::endl;
    {
        DataArray<TrackedObject> arr;
        arr.Resize(5);
        
        for (int64 i = 0; i < 5; i++)
        {
            std::cout << "  Setting element " << i << ":" << std::endl;
            arr[i] = TrackedObject(i, "Data" + std::to_string(i));
        }
        
        PrintDataArray(arr, "Initial");
        PrintTracking();

        // 测试Delete - 关键！这会调用析构函数
        std::cout << "\n[6.2] Delete non-trivial objects:" << std::endl;
        arr.Delete(1, 2);
        PrintDataArray(arr, "After Delete(1,2)");
        PrintTracking();

        // 测试WriteAt - 测试赋值操作
        std::cout << "\n[6.3] WriteAt (copy assignment):" << std::endl;
        TrackedObject newObj(99, "NewData");
        arr.WriteAt(newObj, 1);
        PrintDataArray(arr, "After WriteAt");
        PrintTracking();

        std::cout << "\n[6.4] Scope exit (should destroy all):" << std::endl;
    }
    
    std::cout << "\nAfter scope:" << std::endl;
    PrintTracking();
    std::cout << "  Leak check: " << (TrackedObject::aliveCount == 0 ? "PASS" : "FAIL") << std::endl;
}

void TestNonTrivialDeleteShift()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 7: Non-Trivial DeleteShift (mem_move test)" << std::endl;
    std::cout << "========================================" << std::endl;

    ResetTracking();

    std::cout << "\n[7.1] Setup array:" << std::endl;
    {
        DataArray<TrackedObject> arr;
        arr.Resize(8);
        
        for (int64 i = 0; i < 8; i++)
        {
            arr[i] = TrackedObject(i, "Obj" + std::to_string(i));
        }
        
        PrintDataArray(arr, "Initial");
        PrintTracking();

        // 这个操作会使用mem_move，对于non-trivial类型需要特别注意
        std::cout << "\n[7.2] DeleteShift (critical for mem_move):" << std::endl;
        arr.DeleteShift(2, 3);
        PrintDataArray(arr, "After DeleteShift(2,3)");
        PrintTracking();

        std::cout << "\n[7.3] Cleanup:" << std::endl;
    }
    
    std::cout << "\nAfter scope:" << std::endl;
    PrintTracking();
    std::cout << "  Leak check: " << (TrackedObject::aliveCount == 0 ? "PASS" : "FAIL") << std::endl;
}

void TestNonTrivialMove()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 8: Non-Trivial Move (mem_move overlap test)" << std::endl;
    std::cout << "========================================" << std::endl;

    ResetTracking();

    std::cout << "\n[8.1] Setup array:" << std::endl;
    {
        DataArray<TrackedObject> arr;
        arr.Resize(10);
        
        for (int64 i = 0; i < 10; i++)
        {
            arr[i] = TrackedObject(i, "Item" + std::to_string(i));
        }
        
        PrintDataArray(arr, "Initial");
        PrintTracking();

        // Move操作可能触发mem_move的重叠区域处理
        std::cout << "\n[8.2] Move with potential overlap:" << std::endl;
        arr.Move(7, 3, 4);  // Move [3,4,5,6] to position 7
        PrintDataArray(arr, "After Move(7,3,4)");
        PrintTracking();

        std::cout << "\n[8.3] Move in opposite direction:" << std::endl;
        arr.Move(2, 7, 3);  // Move elements back
        PrintDataArray(arr, "After Move(2,7,3)");
        PrintTracking();

        std::cout << "\n[8.4] Cleanup:" << std::endl;
    }
    
    std::cout << "\nAfter scope:" << std::endl;
    PrintTracking();
    std::cout << "  Leak check: " << (TrackedObject::aliveCount == 0 ? "PASS" : "FAIL") << std::endl;
}

void TestCopyAssignment()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 9: Copy Assignment (mem_copy test)" << std::endl;
    std::cout << "========================================" << std::endl;

    ResetTracking();

    std::cout << "\n[9.1] Create source array:" << std::endl;
    DataArray<TrackedObject> arr1;
    arr1.Resize(5);
    for (int64 i = 0; i < 5; i++)
    {
        arr1[i] = TrackedObject(i, "Source" + std::to_string(i));
    }
    PrintDataArray(arr1, "Source");
    PrintTracking();

    std::cout << "\n[9.2] Copy to new array (operator=):" << std::endl;
    DataArray<TrackedObject> arr2;
    arr2 = arr1;
    PrintDataArray(arr2, "Destination");
    PrintTracking();

    std::cout << "\n[9.3] Modify source (should not affect copy):" << std::endl;
    arr1[0] = TrackedObject(100, "Modified");
    PrintDataArray(arr1, "Source modified");
    PrintDataArray(arr2, "Destination unchanged");

    std::cout << "\n[9.4] Cleanup:" << std::endl;
    arr1.Free();
    PrintDataArray(arr1, "Source freed");
    PrintTracking();
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
        TestNonTrivialType();
        TestNonTrivialDeleteShift();
        TestNonTrivialMove();
        TestCopyAssignment();

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
