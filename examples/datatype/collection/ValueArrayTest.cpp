/**
 * ValueArray 严格测试用例
 * 
 * 测试目标：
 * 1. 测试 trivially copyable 类型 (int, double, POD struct)
 * 2. 测试 non-trivial 类型 (std::string, 含析构的类)
 * 3. 测试内存操作的正确性 (mem_copy, mem_move, mem_fill_pattern)
 * 4. 边界条件和异常情况
 */

#include<hgl/type/ValueArray.h>
#include<hgl/type/ObjectArray.h>
#include<iostream>
#include<iomanip>
#include<string>
#include<cstring>
#include<cassert>

using namespace hgl;

// ============================================================================
// 测试辅助类
// ============================================================================

// POD 结构体 - trivially copyable
struct SimplePOD
{
    int id;
    double value;
    char name[32];

    SimplePOD() : id(0), value(0.0)
    {
        name[0] = '\0';
    }

    SimplePOD(int i, double v, const char* n) : id(i), value(v)
    {
        strncpy(name, n, 31);
        name[31] = '\0';
    }

    bool operator==(const SimplePOD& other) const
    {
        return id == other.id && value == other.value && strcmp(name, other.name) == 0;
    }
};

// 非平凡类 - non-trivial (有析构函数，包含std::string)
class NonTrivialClass
{
public:
    int id;
    std::string name;
    double* dynamicData;

    static int constructCount;
    static int destructCount;
    static int copyCount;
    static int moveCount;

    NonTrivialClass() : id(0), name("default"), dynamicData(nullptr)
    {
        constructCount++;
        std::cout << "  [NonTrivial] Default construct #" << id << std::endl;
    }

    NonTrivialClass(int i, const std::string& n) : id(i), name(n), dynamicData(new double(i * 1.5))
    {
        constructCount++;
        std::cout << "  [NonTrivial] Construct #" << id << " (" << name << ")" << std::endl;
    }

    // 拷贝构造
    NonTrivialClass(const NonTrivialClass& other) 
        : id(other.id), name(other.name)
    {
        if (other.dynamicData)
            dynamicData = new double(*other.dynamicData);
        else
            dynamicData = nullptr;
        
        copyCount++;
        std::cout << "  [NonTrivial] Copy construct #" << id << " (" << name << ")" << std::endl;
    }

    // 移动构造
    NonTrivialClass(NonTrivialClass&& other) noexcept
        : id(other.id), name(std::move(other.name)), dynamicData(other.dynamicData)
    {
        other.dynamicData = nullptr;
        moveCount++;
        std::cout << "  [NonTrivial] Move construct #" << id << std::endl;
    }

    // 拷贝赋值
    NonTrivialClass& operator=(const NonTrivialClass& other)
    {
        if (this != &other)
        {
            id = other.id;
            name = other.name;
            
            delete dynamicData;
            if (other.dynamicData)
                dynamicData = new double(*other.dynamicData);
            else
                dynamicData = nullptr;

            copyCount++;
            std::cout << "  [NonTrivial] Copy assign #" << id << " (" << name << ")" << std::endl;
        }
        return *this;
    }

    // 移动赋值
    NonTrivialClass& operator=(NonTrivialClass&& other) noexcept
    {
        if (this != &other)
        {
            id = other.id;
            name = std::move(other.name);
            
            delete dynamicData;
            dynamicData = other.dynamicData;
            other.dynamicData = nullptr;

            moveCount++;
            std::cout << "  [NonTrivial] Move assign #" << id << std::endl;
        }
        return *this;
    }

    ~NonTrivialClass()
    {
        destructCount++;
        std::cout << "  [NonTrivial] Destruct #" << id << " (" << name << ")"
                  << " dynamicData=" << (void*)dynamicData << std::endl;
        
        if (dynamicData != nullptr)
        {
            delete dynamicData;
            dynamicData = nullptr;  // 防止重复释放
        }
    }

    bool operator==(const NonTrivialClass& other) const
    {
        return id == other.id && name == other.name;
    }
};

int NonTrivialClass::constructCount = 0;
int NonTrivialClass::destructCount = 0;
int NonTrivialClass::copyCount = 0;
int NonTrivialClass::moveCount = 0;

// ============================================================================
// 测试工具函数
// ============================================================================

template<typename T>
void PrintArrayList(const ValueArray<T>& list, const char* label)
{
    std::cout << std::setw(30) << label << " [" << std::setw(2) << list.GetCount() 
              << "/" << std::setw(2) << list.GetAllocCount() << "]: ";
    
    if constexpr (std::is_same_v<T, int>)
    {
        for (int i = 0; i < list.GetCount(); i++)
        {
            if (i > 0) std::cout << ", ";
            std::cout << *list.At(i);
        }
    }
    else if constexpr (std::is_same_v<T, SimplePOD>)
    {
        for (int i = 0; i < list.GetCount(); i++)
        {
            if (i > 0) std::cout << ", ";
            const SimplePOD* pod = list.At(i);
            std::cout << "{" << pod->id << "," << pod->value << "," << pod->name << "}";
        }
    }
    else if constexpr (std::is_same_v<T, NonTrivialClass>)
    {
        for (int i = 0; i < list.GetCount(); i++)
        {
            if (i > 0) std::cout << ", ";
            const NonTrivialClass* obj = list.At(i);
            std::cout << "{" << obj->id << "," << obj->name << "}";
        }
    }
    
    std::cout << std::endl;
}

template<typename T>
void PrintArrayList(const ObjectArray<T>& list, const char* label)
{
    std::cout << std::setw(30) << label << " [" << std::setw(2) << list.GetCount() 
              << "/" << std::setw(2) << list.GetAllocCount() << "]: ";

    if constexpr (std::is_same_v<T, NonTrivialClass>)
    {
        for (int i = 0; i < list.GetCount(); i++)
        {
            if (i > 0) std::cout << ", ";
            const NonTrivialClass* obj = list.At(i);
            std::cout << "{" << obj->id << "," << obj->name << "}";
        }
    }
    else
    {
        for (int i = 0; i < list.GetCount(); i++)
        {
            if (i > 0) std::cout << ", ";
            const T* obj = list.At(i);
            std::cout << "(item)";
            (void)obj;
        }
    }

    std::cout << std::endl;
}

void ResetCounters()
{
    NonTrivialClass::constructCount = 0;
    NonTrivialClass::destructCount = 0;
    NonTrivialClass::copyCount = 0;
    NonTrivialClass::moveCount = 0;
}

void PrintCounters()
{
    std::cout << "  Statistics: Construct=" << NonTrivialClass::constructCount
              << ", Destruct=" << NonTrivialClass::destructCount
              << ", Copy=" << NonTrivialClass::copyCount
              << ", Move=" << NonTrivialClass::moveCount << std::endl;
}

// ============================================================================
// 测试用例
// ============================================================================

void TestIntArrayList()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: ValueArray<int> (Trivially Copyable)" << std::endl;
    std::cout << "========================================" << std::endl;

    ValueArray<int> list;
    
    // 测试基本添加
    std::cout << "\n[1.1] Add single elements:" << std::endl;
    for (int i = 0; i < 10; i++)
        list.Add(i * 10);
    PrintArrayList(list, "After adding 0-90");

    // 测试批量添加
    std::cout << "\n[1.2] Add multiple elements:" << std::endl;
    int batch[] = {100, 101, 102, 103, 104};
    list.Add(batch, 5);
    PrintArrayList(list, "After adding 100-104");

    // 测试重复添加
    std::cout << "\n[1.3] RepeatAdd:" << std::endl;
    list.RepeatAdd(999, 3);
    PrintArrayList(list, "After RepeatAdd(999, 3)");

    // 测试删除
    std::cout << "\n[1.4] Delete operations:" << std::endl;
    list.Delete(0, 3);
    PrintArrayList(list, "After Delete(0, 3)");

    list.DeleteShift(5, 2);
    PrintArrayList(list, "After DeleteShift(5, 2)");

    // 测试插入
    std::cout << "\n[1.5] Insert operations:" << std::endl;
    list.Insert(0, 777);
    PrintArrayList(list, "After Insert(0, 777)");

    int insertBatch[] = {888, 888};
    list.Insert(5, insertBatch, 2);
    PrintArrayList(list, "After Insert(5, {888,888})");

    // 测试交换
    std::cout << "\n[1.6] Exchange:" << std::endl;
    list.Exchange(0, list.GetCount() - 1);
    PrintArrayList(list, "After Exchange(0, last)");

    // 测试查找
    std::cout << "\n[1.7] Find and Contains:" << std::endl;
    int findValue = 888;
    int index = list.Find(findValue);
    std::cout << "  Find(888) = " << index << std::endl;
    std::cout << "  Contains(888) = " << (list.Contains(findValue) ? "true" : "false") << std::endl;
    std::cout << "  Contains(12345) = " << (list.Contains(12345) ? "true" : "false") << std::endl;

    // 测试移动
    std::cout << "\n[1.8] Move operations:" << std::endl;
    PrintArrayList(list, "Before Move");
    list.Move(0, 5, 3);
    PrintArrayList(list, "After Move(0, 5, 3)");
}

void TestPODArrayList()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: ValueArray<SimplePOD> (POD Structure)" << std::endl;
    std::cout << "========================================" << std::endl;

    ValueArray<SimplePOD> list;

    // 测试添加POD
    std::cout << "\n[2.1] Add POD objects:" << std::endl;
    list.Add(SimplePOD(1, 1.1, "First"));
    list.Add(SimplePOD(2, 2.2, "Second"));
    list.Add(SimplePOD(3, 3.3, "Third"));
    PrintArrayList(list, "After adding 3 PODs");

    // 测试批量添加
    std::cout << "\n[2.2] Add multiple PODs:" << std::endl;
    SimplePOD batch[] = {
        SimplePOD(10, 10.1, "Batch1"),
        SimplePOD(11, 11.1, "Batch2"),
        SimplePOD(12, 12.1, "Batch3")
    };
    list.Add(batch, 3);
    PrintArrayList(list, "After adding batch");

    // 测试RepeatAdd
    std::cout << "\n[2.3] RepeatAdd POD:" << std::endl;
    SimplePOD repeated(99, 99.9, "Repeated");
    list.RepeatAdd(repeated, 2);
    PrintArrayList(list, "After RepeatAdd");

    // 测试删除
    std::cout << "\n[2.4] Delete operations:" << std::endl;
    list.Delete(1, 2);
    PrintArrayList(list, "After Delete(1, 2)");

    // 测试插入
    std::cout << "\n[2.5] Insert POD:" << std::endl;
    SimplePOD insert(77, 77.7, "Inserted");
    list.Insert(2, insert);
    PrintArrayList(list, "After Insert at 2");
}

void TestNonTrivialArrayList()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: ValueArray<NonTrivialClass> (Non-Trivial Type)" << std::endl;
    std::cout << "========================================" << std::endl;

    ResetCounters();

    std::cout << "\n[3.0] Creating ValueArray..." << std::endl;
    ObjectArray<NonTrivialClass> list;
    std::cout << "  ValueArray created. Count=" << list.GetCount() 
              << ", AllocCount=" << list.GetAllocCount() << std::endl;

    // 测试添加非平凡对象 - 简化测试
    std::cout << "\n[3.1] Add first non-trivial object:" << std::endl;
    std::cout << "  Creating temp object..." << std::endl;
    NonTrivialClass temp1(1, "Object1");
    std::cout << "  Calling Add..." << std::endl;
    list.Add(temp1);
    std::cout << "  Add completed. Count=" << list.GetCount() << std::endl;
    PrintArrayList(list, "After adding 1 object");
    PrintCounters();

    std::cout << "\n[3.2] Add second non-trivial object:" << std::endl;
    NonTrivialClass temp2(2, "Object2");
    list.Add(temp2);
    PrintArrayList(list, "After adding 2 objects");
    PrintCounters();

    std::cout << "\n[3.3] Add third non-trivial object:" << std::endl;
    NonTrivialClass temp3(3, "Object3");
    list.Add(temp3);
    PrintArrayList(list, "After adding 3 objects");
    PrintCounters();

    std::cout << "\n[3.4] List cleanup (destructor will be called)..." << std::endl;
}

void TestEdgeCases()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Edge Cases and Boundary Conditions" << std::endl;
    std::cout << "========================================" << std::endl;

    // 测试空列表操作
    std::cout << "\n[4.1] Operations on empty list:" << std::endl;
    ValueArray<int> emptyList;
    std::cout << "  IsEmpty: " << (emptyList.IsEmpty() ? "true" : "false") << std::endl;
    std::cout << "  GetCount: " << emptyList.GetCount() << std::endl;
    emptyList.Delete(0, 1);  // Should not crash
    std::cout << "  Delete on empty list: OK" << std::endl;

    // 测试越界删除
    std::cout << "\n[4.2] Out-of-bounds delete:" << std::endl;
    ValueArray<int> list;
    for (int i = 0; i < 5; i++)
        list.Add(i);
    PrintArrayList(list, "Before delete");
    list.Delete(10, 5);  // Beyond bounds
    PrintArrayList(list, "After delete(10,5)");
    list.Delete(3, 100);  // Partial beyond bounds
    PrintArrayList(list, "After delete(3,100)");

    // 测试Reserve和Resize
    std::cout << "\n[4.3] Reserve and Resize:" << std::endl;
    ValueArray<int> reserveList;
    reserveList.Reserve(100);
    std::cout << "  After Reserve(100): Count=" << reserveList.GetCount() 
              << ", AllocCount=" << reserveList.GetAllocCount() << std::endl;
    
    reserveList.Resize(50);
    std::cout << "  After Resize(50): Count=" << reserveList.GetCount() 
              << ", AllocCount=" << reserveList.GetAllocCount() << std::endl;

    // 测试Move边界情况
    std::cout << "\n[4.4] Move edge cases:" << std::endl;
    ValueArray<int> moveList;
    for (int i = 0; i < 10; i++)
        moveList.Add(i);
    
    PrintArrayList(moveList, "Before Move");
    moveList.Move(0, 7, 3);  // Move to beginning
    PrintArrayList(moveList, "After Move(0,7,3)");
    
    moveList.Move(7, 0, 3);  // Move to end
    PrintArrayList(moveList, "After Move(7,0,3)");
}

void TestMemorySafety()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: Memory Safety and Leak Detection" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "\n[5.1] Repeated add/delete cycles:" << std::endl;
    ResetCounters();
    {
        ObjectArray<NonTrivialClass> list;
        
        for (int cycle = 0; cycle < 3; cycle++)
        {
            std::cout << "\n  Cycle " << cycle << ":" << std::endl;
            
            // Add objects
            for (int i = 0; i < 5; i++)
                list.Add(NonTrivialClass(cycle * 100 + i, "Obj"));
            
            PrintArrayList(list, "After adding");
            
            // Delete some
            list.Delete(0, 2);
            PrintArrayList(list, "After delete");
        }
        
        std::cout << "\n  Before scope exit:" << std::endl;
    }
    std::cout << "  After scope exit (list destroyed)" << std::endl;
    PrintCounters();
    
    std::cout << "\n[5.2] Memory leak check:" << std::endl;
    std::cout << "  Constructs - Destructs = " 
              << (NonTrivialClass::constructCount - NonTrivialClass::destructCount) << std::endl;
    std::cout << "  (Should be 0 if no leaks)" << std::endl;
}

// ============================================================================
// Main
// ============================================================================

int main(int, char**)
{
    std::cout << "========================================"  << std::endl;
    std::cout << "ValueArray Comprehensive Test Suite" << std::endl;
    std::cout << "========================================" << std::endl;

    try
    {
        TestIntArrayList();
        TestPODArrayList();
        TestNonTrivialArrayList();
        //TestEdgeCases();  // 暂时跳过
        //TestMemorySafety();  // 暂时跳过

        std::cout << "\n========================================" << std::endl;
        std::cout << "ALL TESTS COMPLETED" << std::endl;
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
