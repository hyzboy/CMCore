/**
 * DataArray 非平凡类型测试
 * 
 * 本测试专门验证 DataArray 对非平凡类型（如包含动态内存的类）的正确处理：
 * 1. 构造和析构的调用
 * 2. 复制操作的管理
 * 3. 内存生命周期的正确性
 * 4. 删除和移动时的资源释放
 */

#include<hgl/type/DataArray.h>
#include<iostream>
#include<iomanip>
#include<string>
#include<cassert>

using namespace hgl;

// ============================================================================
// 被追踪的对象类 - 用于验证构造/析构调用
// ============================================================================

class TrackedObject
{
public:
    int id;
    std::string name;
    int* heap_ptr;
    
    // 静态追踪变量
    static int instance_count;
    static int constructor_count;
    static int destructor_count;
    static int copy_constructor_count;
    static int move_constructor_count;
    static int copy_assignment_count;
    static int move_assignment_count;
    
    // 构造函数
    TrackedObject(int _id = 0, const std::string& _name = "")
        : id(_id), name(_name), heap_ptr(new int(_id))
    {
        instance_count++;
        constructor_count++;
        std::cout << "    [Cons] T" << id << "(" << name << ") - instances: " << instance_count << std::endl;
    }
    
    // 复制构造函数
    TrackedObject(const TrackedObject& other)
        : id(other.id), name(other.name), heap_ptr(new int(*other.heap_ptr))
    {
        instance_count++;
        copy_constructor_count++;
        std::cout << "    [Copy] T" << id << "(" << name << ") <- T" << other.id << " - instances: " << instance_count << std::endl;
    }
    
    // 移动构造函数
    TrackedObject(TrackedObject&& other) noexcept
        : id(other.id), name(other.name), heap_ptr(other.heap_ptr)
    {
        instance_count++;
        move_constructor_count++;
        other.heap_ptr = nullptr;
        std::cout << "    [Move] T" << id << "(" << name << ") <- T" << other.id << " (move) - instances: " << instance_count << std::endl;
    }
    
    // 析构函数
    ~TrackedObject()
    {
        instance_count--;
        destructor_count++;
        std::cout << "    [Dest] T" << id << "(" << name << ") - instances: " << instance_count << std::endl;
        if (heap_ptr)
            delete heap_ptr;
    }
    
    // 复制赋值操作符
    TrackedObject& operator=(const TrackedObject& other)
    {
        if (this != &other)
        {
            copy_assignment_count++;
            id = other.id;
            name = other.name;
            if (heap_ptr)
                delete heap_ptr;
            heap_ptr = new int(*other.heap_ptr);
            std::cout << "    [Asgn] T" << id << "(" << name << ") <- T" << other.id << " - instances: " << instance_count << std::endl;
        }
        return *this;
    }
    
    // 移动赋值操作符
    TrackedObject& operator=(TrackedObject&& other) noexcept
    {
        if (this != &other)
        {
            move_assignment_count++;
            id = other.id;
            name = other.name;
            if (heap_ptr)
                delete heap_ptr;
            heap_ptr = other.heap_ptr;
            other.heap_ptr = nullptr;
            std::cout << "    [MovA] T" << id << "(" << name << ") <- T" << other.id << " (move) - instances: " << instance_count << std::endl;
        }
        return *this;
    }
    
    // 比较函数
    bool operator==(const TrackedObject& other) const
    {
        return id == other.id && name == other.name;
    }
    
    bool operator!=(const TrackedObject& other) const
    {
        return !(*this == other);
    }
};

// 初始化静态成员
int TrackedObject::instance_count = 0;
int TrackedObject::constructor_count = 0;
int TrackedObject::destructor_count = 0;
int TrackedObject::copy_constructor_count = 0;
int TrackedObject::move_constructor_count = 0;
int TrackedObject::copy_assignment_count = 0;
int TrackedObject::move_assignment_count = 0;

// ============================================================================
// 测试辅助工具
// ============================================================================

void reset_tracking()
{
    TrackedObject::constructor_count = 0;
    TrackedObject::destructor_count = 0;
    TrackedObject::copy_constructor_count = 0;
    TrackedObject::move_constructor_count = 0;
    TrackedObject::copy_assignment_count = 0;
    TrackedObject::move_assignment_count = 0;
}

void print_stats(const char* label)
{
    std::cout << "\n  " << label << " 统计:" << std::endl;
    std::cout << "    构造函数调用: " << TrackedObject::constructor_count << std::endl;
    std::cout << "    析构函数调用: " << TrackedObject::destructor_count << std::endl;
    std::cout << "    复制构造调用: " << TrackedObject::copy_constructor_count << std::endl;
    std::cout << "    移动构造调用: " << TrackedObject::move_constructor_count << std::endl;
    std::cout << "    复制赋值调用: " << TrackedObject::copy_assignment_count << std::endl;
    std::cout << "    移动赋值调用: " << TrackedObject::move_assignment_count << std::endl;
    std::cout << "    当前实例数: " << TrackedObject::instance_count << std::endl;
}

#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "    ✗ 失败: " << message << std::endl; \
            return false; \
        } \
    } while(0)

// ============================================================================
// 测试用例 1: 构造和析构
// ============================================================================

bool test_construction_destruction()
{
    std::cout << "\n【测试 1: 构造和析构】" << std::endl;
    
    reset_tracking();
    
    {
        DataArray<TrackedObject> arr(5);
        std::cout << "  创建大小为5的数组" << std::endl;
        
        TEST_ASSERT(TrackedObject::instance_count == 5, "应有5个实例");
        TEST_ASSERT(TrackedObject::constructor_count == 5, "应调用5次构造函数");
    }
    
    print_stats("析构后");
    TEST_ASSERT(TrackedObject::instance_count == 0, "析构后实例数应为0");
    TEST_ASSERT(TrackedObject::destructor_count == 5, "应调用5次析构函数");
    
    return true;
}

// ============================================================================
// 测试用例 2: Resize和内存管理
// ============================================================================

bool test_resize_with_nontrivial()
{
    std::cout << "\n【测试 2: Resize和内存管理】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr;
    
    // Resize to 3
    arr.Resize(3);
    std::cout << "  Resize到3" << std::endl;
    TEST_ASSERT(arr.GetCount() == 3, "计数应为3");
    TEST_ASSERT(TrackedObject::instance_count == 3, "应有3个实例");
    
    // Resize to 5 (扩展)
    arr.Resize(5);
    std::cout << "  Resize到5 (扩展)" << std::endl;
    TEST_ASSERT(arr.GetCount() == 5, "计数应为5");
    // 可能有额外的构造调用用于新元素
    
    // Resize to 2 (缩小)
    int before = TrackedObject::instance_count;
    arr.Resize(2);
    std::cout << "  Resize到2 (缩小)" << std::endl;
    TEST_ASSERT(arr.GetCount() == 2, "计数应为2");
    TEST_ASSERT(TrackedObject::instance_count < before, "实例数应减少");
    
    return true;
}

// ============================================================================
// 测试用例 3: Append操作
// ============================================================================

bool test_append_nontrivial()
{
    std::cout << "\n【测试 3: Append操作】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr;
    
    {
        TrackedObject obj1(1, "first");
        arr.Append(obj1);
        std::cout << "  Append obj1" << std::endl;
        
        TrackedObject obj2(2, "second");
        arr.Append(obj2);
        std::cout << "  Append obj2" << std::endl;
    }
    
    print_stats("Append后");
    
    TEST_ASSERT(arr.GetCount() == 2, "应有2个元素");
    TEST_ASSERT(arr[0].id == 1, "第一个元素应是 id=1");
    TEST_ASSERT(arr[1].id == 2, "第二个元素应是 id=2");
    
    return true;
}

// ============================================================================
// 测试用例 4: Delete操作
// ============================================================================

bool test_delete_nontrivial()
{
    std::cout << "\n【测试 4: Delete操作】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = TrackedObject(i, std::string("obj_") + std::to_string(i));
    
    std::cout << "  删除索引2" << std::endl;
    arr.Delete(2);
    
    TEST_ASSERT(arr.GetCount() == 4, "删除后应有4个元素");
    
    return true;
}

// ============================================================================
// 测试用例 5: DeleteShift操作
// ============================================================================

bool test_delete_shift_nontrivial()
{
    std::cout << "\n【测试 5: DeleteShift操作】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr;
    arr.Resize(5);
    for (int i = 0; i < 5; i++)
        arr[i] = TrackedObject(i, std::string("obj_") + std::to_string(i));
    
    std::cout << "  DeleteShift 从索引1删除2个元素" << std::endl;
    arr.DeleteShift(1, 2);
    
    TEST_ASSERT(arr.GetCount() == 3, "删除后应有3个元素");
    TEST_ASSERT(arr[0].id == 0, "第一个元素应保持");
    TEST_ASSERT(arr[1].id == 3, "第二个元素应是原来的索引3");
    TEST_ASSERT(arr[2].id == 4, "第三个元素应是原来的索引4");
    
    return true;
}

// ============================================================================
// 测试用例 6: Insert操作
// ============================================================================

bool test_insert_nontrivial()
{
    std::cout << "\n【测试 6: Insert操作】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr;
    arr.Resize(3);
    for (int i = 0; i < 3; i++)
        arr[i] = TrackedObject(i * 10, std::string("obj_") + std::to_string(i * 10));
    
    std::cout << "  在索引1处插入新元素" << std::endl;
    TrackedObject insert_data(99, "inserted");
    arr.Insert(1, &insert_data, 1);
    
    TEST_ASSERT(arr.GetCount() == 4, "插入后应有4个元素");
    TEST_ASSERT(arr[1].id == 99, "插入的元素应在索引1");
    
    return true;
}

// ============================================================================
// 测试用例 7: Move操作
// ============================================================================

bool test_move_nontrivial()
{
    std::cout << "\n【测试 7: Move操作】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr;
    arr.Resize(6);
    for (int i = 0; i < 6; i++)
        arr[i] = TrackedObject(i, std::string("obj_") + std::to_string(i));
    
    std::cout << "  移动索引0-1的元素到索引4" << std::endl;
    arr.Move(4, 0, 2);
    
    TEST_ASSERT(arr.GetCount() == 6, "移动后元素数应保持");
    
    return true;
}

// ============================================================================
// 测试用例 8: Copy赋值
// ============================================================================

bool test_copy_assignment()
{
    std::cout << "\n【测试 8: Copy赋值】" << std::endl;
    
    reset_tracking();
    
    DataArray<TrackedObject> arr1;
    arr1.Resize(3);
    for (int i = 0; i < 3; i++)
        arr1[i] = TrackedObject(i + 100, std::string("src_") + std::to_string(i));
    
    std::cout << "  复制整个数组" << std::endl;
    DataArray<TrackedObject> arr2 = arr1;
    
    print_stats("复制后");
    
    TEST_ASSERT(arr2.GetCount() == 3, "复制后计数应为3");
    TEST_ASSERT(arr2[0].id == 100, "复制的数据应相同");
    TEST_ASSERT(arr2[1].id == 101, "复制的数据应相同");
    
    return true;
}

// ============================================================================
// 测试用例 9: Clear和Free
// ============================================================================

bool test_clear_and_free_nontrivial()
{
    std::cout << "\n【测试 9: Clear和Free】" << std::endl;
    
    reset_tracking();
    
    {
        DataArray<TrackedObject> arr;
        arr.Resize(5);
        for (int i = 0; i < 5; i++)
            arr[i] = TrackedObject(i, std::string("obj_") + std::to_string(i));
        
        std::cout << "  Clear数组" << std::endl;
        arr.Clear();
        TEST_ASSERT(arr.GetCount() == 0, "Clear后计数应为0");
        // 注意: Clear不调用析构函数，只重置计数
        
        std::cout << "  Free数组" << std::endl;
        arr.Free();
        TEST_ASSERT(arr.GetCount() == 0, "Free后计数应为0");
        TEST_ASSERT(arr.GetAllocCount() == 0, "Free后分配数应为0");
    }
    
    print_stats("释放后");
    
    return true;
}

// ============================================================================
// 测试用例 10: 内存泄漏检查
// ============================================================================

bool test_memory_leak_check()
{
    std::cout << "\n【测试 10: 内存泄漏检查】" << std::endl;
    
    reset_tracking();
    int initial_count = TrackedObject::instance_count;
    
    {
        DataArray<TrackedObject> arr;
        arr.Resize(10);
        for (int i = 0; i < 10; i++)
            arr[i] = TrackedObject(i, std::string("leak_test_") + std::to_string(i));
        
        std::cout << "  创建10个对象，执行各种操作" << std::endl;
        
        arr.Resize(15);
        arr.Delete(3);
        arr.Insert(5, &arr[0], 1);
        arr.DeleteShift(1, 2);
    }
    
    print_stats("清理后");
    
    // 所有对象应被销毁
    TEST_ASSERT(TrackedObject::instance_count == initial_count, "所有对象应被销毁，无内存泄漏");
    
    return true;
}

// ============================================================================
// 主测试函数
// ============================================================================

int main(int argc, char** argv)
{
    int passed = 0, failed = 0;
    
    std::cout << "========================================" << std::endl;
    std::cout << "  DataArray 非平凡类型测试" << std::endl;
    std::cout << "========================================" << std::endl;
    
    if (test_construction_destruction()) { passed++; } else { failed++; }
    if (test_resize_with_nontrivial()) { passed++; } else { failed++; }
    if (test_append_nontrivial()) { passed++; } else { failed++; }
    if (test_delete_nontrivial()) { passed++; } else { failed++; }
    if (test_delete_shift_nontrivial()) { passed++; } else { failed++; }
    if (test_insert_nontrivial()) { passed++; } else { failed++; }
    if (test_move_nontrivial()) { passed++; } else { failed++; }
    if (test_copy_assignment()) { passed++; } else { failed++; }
    if (test_clear_and_free_nontrivial()) { passed++; } else { failed++; }
    if (test_memory_leak_check()) { passed++; } else { failed++; }
    
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
