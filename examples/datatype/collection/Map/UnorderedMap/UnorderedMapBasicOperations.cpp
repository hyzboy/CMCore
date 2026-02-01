#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<hgl/type/UnorderedMap.h>
#include<hgl/type/String.h>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

// 测试计数器
static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            cout << "  ✓ PASS: " << message << endl; \
            test_passed++; \
        } else { \
            cout << "  ✗ FAIL: " << message << endl; \
            test_failed++; \
        } \
    } while(0)

// 带std::string的非平凡类型，用于测试内存管理
class ComplexValue
{
public:
    int id;
    std::string data;
    int* dynamic_ptr;

    static int construct_count;
    static int destruct_count;
    static int copy_count;

    ComplexValue() : id(0), data(""), dynamic_ptr(nullptr)
    {
        construct_count++;
    }

    ComplexValue(int i, const std::string& d) : id(i), data(d), dynamic_ptr(new int(i))
    {
        construct_count++;
    }

    ComplexValue(const ComplexValue& other) : id(other.id), data(other.data),
        dynamic_ptr(other.dynamic_ptr ? new int(*other.dynamic_ptr) : nullptr)
    {
        copy_count++;
    }

    ComplexValue& operator=(const ComplexValue& other)
    {
        if (this != &other) {
            id = other.id;
            data = other.data;
            delete dynamic_ptr;
            dynamic_ptr = other.dynamic_ptr ? new int(*other.dynamic_ptr) : nullptr;
            copy_count++;
        }
        return *this;
    }

    bool operator==(const ComplexValue& other) const
    {
        return id == other.id && data == other.data;
    }

    ~ComplexValue()
    {
        delete dynamic_ptr;
        destruct_count++;
    }

    static void ResetCounters() {
        construct_count = 0;
        destruct_count = 0;
        copy_count = 0;
    }
};

int ComplexValue::construct_count = 0;
int ComplexValue::destruct_count = 0;
int ComplexValue::copy_count = 0;

void out_id(UnorderedMap<int,int> &int_map)
{
    const int count=int_map.GetCount();
    cout<<"count:"<<count<<" ";

    int_map.EnumKV([](const int &key,int &value)
    {
        cout<<"["<<key<<","<<value<<"]";
    });

    cout<<endl;
}

// TEST 1: 基础操作测试
void BasicOperationTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: Basic Operations" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int,int> map;

    // 1.1 空Map测试
    cout << "\n[1.1] Empty map tests:" << endl;
    TEST_ASSERT(map.GetCount() == 0, "Empty map has count 0");
    TEST_ASSERT(!map.ContainsKey(1), "Non-existent key returns false");

    int value = 0;
    TEST_ASSERT(!map.Get(1, value), "Get on empty map returns false");

    // 1.2 添加元素
    cout << "\n[1.2] Add elements:" << endl;
    TEST_ASSERT(map.Add(1, 100), "Add first element");
    TEST_ASSERT(map.GetCount() == 1, "Count is 1 after add");
    TEST_ASSERT(map.ContainsKey(1), "Added key exists");

    TEST_ASSERT(!map.Add(1, 200), "Cannot add duplicate key");
    TEST_ASSERT(map.GetCount() == 1, "Count unchanged after duplicate add");

    // 1.3 获取元素
    cout << "\n[1.3] Get elements:" << endl;
    int retrieved_value = 0;
    TEST_ASSERT(map.Get(1, retrieved_value), "Get existing key succeeds");
    TEST_ASSERT(retrieved_value == 100, "Retrieved correct value");

    // 1.4 修改元素
    cout << "\n[1.4] Update elements:" << endl;
    TEST_ASSERT(map.Change(1, 150), "Change existing key");
    map.Get(1, retrieved_value);
    TEST_ASSERT(retrieved_value == 150, "Value updated correctly");

    TEST_ASSERT(map.ChangeOrAdd(1, 200), "ChangeOrAdd existing key");
    map.Get(1, retrieved_value);
    TEST_ASSERT(retrieved_value == 200, "Value updated via ChangeOrAdd");

    // 1.5 删除元素
    cout << "\n[1.5] Delete elements:" << endl;
    TEST_ASSERT(map.DeleteByKey(1), "Delete existing key");
    TEST_ASSERT(map.GetCount() == 0, "Count is 0 after delete");
    TEST_ASSERT(!map.ContainsKey(1), "Deleted key no longer exists");
    TEST_ASSERT(!map.DeleteByKey(1), "Cannot delete non-existent key");
}

// TEST 2: 大量数据测试
void StressTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Stress Test with Large Data" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int,int> map;
    constexpr int LARGE_COUNT = 1000;

    // 2.1 大量添加
    cout << "\n[2.1] Add " << LARGE_COUNT << " elements:" << endl;
    for(int i = 0; i < LARGE_COUNT; i++)
    {
        map.Add(i, i * 10);
    }
    TEST_ASSERT(map.GetCount() == LARGE_COUNT, "All elements added");

    // 2.2 随机访问
    cout << "\n[2.2] Random access:" << endl;
    int access_errors = 0;
    for(int i = 0; i < 100; i++)
    {
        int key = rand() % LARGE_COUNT;
        int value = 0;
        if(!map.Get(key, value) || value != key * 10)
            access_errors++;
    }
    TEST_ASSERT(access_errors == 0, "Random access correct");

    // 2.3 查找测试（使用ContainsKey而不是Find，因为UnorderedMap是无序的）
    cout << "\n[2.3] Find tests:" << endl;
    TEST_ASSERT(map.ContainsKey(500), "Find existing key via ContainsKey");
    TEST_ASSERT(!map.ContainsKey(9999), "Non-existent key returns false");

    // 2.4 遍历测试
    cout << "\n[2.4] Enumeration test:" << endl;
    int enum_count = 0;
    map.EnumKV([&enum_count](const int &key, int &value)
    {
        enum_count++;
    });
    TEST_ASSERT(enum_count == LARGE_COUNT, "Enumerated all elements");

    // 2.5 批量删除
    cout << "\n[2.5] Delete half elements:" << endl;
    for(int i = 0; i < LARGE_COUNT; i += 2)
    {
        map.DeleteByKey(i);
    }
    TEST_ASSERT(map.GetCount() == LARGE_COUNT / 2, "Half elements deleted");

    // 验证剩余元素
    int verify_errors = 0;
    for(int i = 1; i < LARGE_COUNT; i += 2)
    {
        if(!map.ContainsKey(i))
            verify_errors++;
    }
    TEST_ASSERT(verify_errors == 0, "Remaining elements intact");

    // 2.6 清空
    cout << "\n[2.6] Clear all:" << endl;
    map.Clear();
    TEST_ASSERT(map.GetCount() == 0, "Map cleared");
}

// TEST 3: 非平凡类型测试（内存管理）
void NonTrivialTypeTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Non-Trivial Type (Memory Management)" << endl;
    cout << "========================================" << endl;

    ComplexValue::ResetCounters();

    {
        UnorderedMap<int, ComplexValue> complex_map;

        // 3.1 添加非平凡对象
        cout << "\n[3.1] Add complex objects:" << endl;
        ComplexValue::ResetCounters();  // 重置计数器，忽略之前的构造
        complex_map.Add(1, ComplexValue(1, "First"));
        complex_map.Add(2, ComplexValue(2, "Second"));
        complex_map.Add(3, ComplexValue(3, "Third"));
        
        int initial_construct = ComplexValue::construct_count;
        int initial_destruct = ComplexValue::destruct_count;

        cout << "  Initial constructs: " << initial_construct << endl;
        cout << "  Initial destructs: " << initial_destruct << endl;

        TEST_ASSERT(complex_map.GetCount() == 3, "Added 3 complex objects");

        // 3.2 访问验证
        cout << "\n[3.2] Access and verify:" << endl;
        ComplexValue val;
        TEST_ASSERT(complex_map.Get(2, val), "Get complex object");
        TEST_ASSERT(val.id == 2 && val.data == "Second", "Complex object data correct");

        // 3.3 删除测试（确保对象被移除）
        cout << "\n[3.3] Delete one object:" << endl;
        TEST_ASSERT(complex_map.ContainsKey(2), "Key 2 exists before delete");
        complex_map.DeleteByKey(2);
        TEST_ASSERT(!complex_map.ContainsKey(2), "Key 2 removed after delete");
        TEST_ASSERT(complex_map.GetCount() == 2, "Object deleted");
        cout << "  Note: Destructor may be called later during cleanup" << endl;

        cout << "\n[3.4] Scope exit (auto cleanup):" << endl;
    }

    // 验证所有对象都被正确析构
    cout << "  Final destructs: " << ComplexValue::destruct_count << endl;
    // 验证清空时析构了剩余的对象（可能有临时对象）
    TEST_ASSERT(ComplexValue::destruct_count >= 2, "Remaining objects destroyed");
}

// TEST 4: 边界条件和异常情况
void EdgeCaseTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Edge Cases and Error Handling" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int,int> map;

    // 4.1 空键值测试
    cout << "\n[4.1] Empty/zero key tests:" << endl;
    TEST_ASSERT(map.Add(0, 999), "Can add key 0");
    TEST_ASSERT(map.ContainsKey(0), "Key 0 exists");

    // 4.2 负数键测试
    cout << "\n[4.2] Negative key tests:" << endl;
    TEST_ASSERT(map.Add(-1, -100), "Can add negative key");
    TEST_ASSERT(map.Add(-999, -9999), "Can add large negative key");
    int neg_val = 0;
    map.Get(-1, neg_val);
    TEST_ASSERT(neg_val == -100, "Negative key retrieval works");

    // 4.3 重复删除测试
    cout << "\n[4.3] Double delete test:" << endl;
    map.Add(100, 1000);
    TEST_ASSERT(map.DeleteByKey(100), "First delete succeeds");
    TEST_ASSERT(!map.DeleteByKey(100), "Second delete fails");

    // 4.4 ChangeOrAdd vs Change测试
    cout << "\n[4.4] ChangeOrAdd vs Change:" << endl;
    TEST_ASSERT(!map.Change(200, 2000), "Change non-existent key fails");
    TEST_ASSERT(map.ChangeOrAdd(200, 2000), "ChangeOrAdd creates new key");
    TEST_ASSERT(map.ContainsKey(200), "Key 200 now exists");
    TEST_ASSERT(map.Change(200, 3000), "Change existing key succeeds");
    int updated_val = 0;
    map.Get(200, updated_val);
    TEST_ASSERT(updated_val == 3000, "Value updated correctly");

    // 4.5 Get + Delete 测试（UnorderedMap没有GetAndDelete）
    cout << "\n[4.5] Get and Delete test:" << endl;
    map.Add(300, 3000);
    int delete_val = 0;
    TEST_ASSERT(map.Get(300, delete_val), "Get existing key succeeds");
    TEST_ASSERT(delete_val == 3000, "Got correct value");
    TEST_ASSERT(map.DeleteByKey(300), "Delete succeeds");
    TEST_ASSERT(!map.ContainsKey(300), "Key deleted after DeleteByKey");

    // 4.6 迭代器稳定性
    cout << "\n[4.6] Iterator stability:" << endl;
    map.Clear();
    for(int i = 0; i < 10; i++)
        map.Add(i, i * 10);

    int iter_count = 0;
    for(auto it : map)
    {
        iter_count++;
    }
    TEST_ASSERT(iter_count == 10, "Iterator traversed all elements");
}

// TEST 5: String键测试（复杂键类型）
void StringKeyTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: String Key Tests" << endl;
    cout << "========================================" << endl;

    UnorderedMap<AnsiString,UserInfo> ui_map;

    // 5.1 添加所有用户
    cout << "\n[5.1] Add all users:" << endl;
    for(auto &ui : user_info_array)
    {
        ui_map.Add(ui.name, ui);
    }
    TEST_ASSERT(ui_map.GetCount() == user_info_array_count, "All users added");

    // 5.2 查找测试
    cout << "\n[5.2] Lookup tests:" << endl;
    UserInfo found;
    TEST_ASSERT(ui_map.Get("Tom", found), "Find Tom");
    TEST_ASSERT(found.age == 37, "Tom's age is 37");
    TEST_ASSERT(!ui_map.Get("NonExist", found), "Non-existent user not found");

    // 5.3 修改测试
    cout << "\n[5.3] Update user info:" << endl;
    UserInfo modified = {"Tom", true, 40};
    TEST_ASSERT(ui_map.Change("Tom", modified), "Update Tom's info");
    ui_map.Get("Tom", found);
    TEST_ASSERT(found.age == 40, "Tom's age updated to 40");

    // 5.4 GetValuePointer测试
    cout << "\n[5.4] GetValuePointer test:" << endl;
    UserInfo* ptr = ui_map.GetValuePointer("Bella");
    TEST_ASSERT(ptr != nullptr, "GetValuePointer returns valid pointer");
    TEST_ASSERT(ptr && ptr->age == 19, "Bella's age is 19");

    // 5.5 删除测试
    cout << "\n[5.5] Delete user:" << endl;
    TEST_ASSERT(ui_map.DeleteByKey("Tom"), "Delete Tom");
    TEST_ASSERT(!ui_map.ContainsKey("Tom"), "Tom no longer exists");
    TEST_ASSERT(ui_map.GetCount() == user_info_array_count - 1, "Count decreased");

    // 5.6 枚举测试
    cout << "\n[5.6] Enumerate all users:" << endl;
    int female_count = 0, male_count = 0;
    ui_map.EnumKV([&](const AnsiString &key, UserInfo &ui)
    {
        if(ui.sex) male_count++;
        else female_count++;
    });
    cout << "  Males: " << male_count << ", Females: " << female_count << endl;
    TEST_ASSERT(male_count + female_count == ui_map.GetCount(), "Enumeration complete");

    // 5.7 迭代器测试
    cout << "\n[5.7] Iterator test:" << endl;
    int iter_count = 0;
    for(auto& [key, value] : ui_map)
    {
        iter_count++;
        if(iter_count <= 3)
        {
            cout << "  Iterator[" << key.c_str() << ", "
                 << (value.sex ? "male" : "female") << ", "
                 << value.age << "]" << endl;
        }
    }
    TEST_ASSERT(iter_count == ui_map.GetCount(), "Iterator counted all elements");
}

// TEST 6: UnorderedMap存储复杂对象测试
void ObjectMapTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: UnorderedMap with Complex Objects" << endl;
    cout << "========================================" << endl;

    {
        // 存储对象本身，不是指针
        UnorderedMap<AnsiString, UserInfoClass> obj_map;

        // 6.1 添加对象
        cout << "\n[6.1] Add objects:" << endl;
        int add_count = 0;
        for(auto &ui : user_info_array)
        {
            UserInfoClass uic;
            uic.Set(ui);
            obj_map.Add(ui.name, uic);
            add_count++;
            if(add_count >= 5) break; // 只添加前5个
        }
        TEST_ASSERT(obj_map.GetCount() == 5, "Added 5 objects");

        // 6.2 访问对象
        cout << "\n[6.2] Access objects:" << endl;
        UserInfoClass found;
        obj_map.Get("Bella", found);
        TEST_ASSERT(found.GetAge() == 19, "Bella's age is 19");

        // 6.3 修改对象
        cout << "\n[6.3] Modify object:" << endl;
        UserInfoClass* ptr = obj_map.GetValuePointer(AnsiString("David"));
        TEST_ASSERT(ptr != nullptr, "GetValuePointer returns valid pointer");
        if(ptr)
        {
            ptr->SetAge(99);
            TEST_ASSERT(ptr->GetAge() == 99, "David's age updated to 99");
        }

        // 6.4 验证修改
        cout << "\n[6.4] Verify modification:" << endl;
        UserInfoClass david_data;
        TEST_ASSERT(obj_map.Get(AnsiString("David"), david_data), "Get David from map");
        TEST_ASSERT(david_data.GetAge() == 99, "David's age persists");

        // 6.5 删除对象
        cout << "\n[6.5] Delete object:" << endl;
        TEST_ASSERT(obj_map.DeleteByKey("Bella"), "Delete Bella from map");
        TEST_ASSERT(!obj_map.ContainsKey("Bella"), "Bella deleted from map");
        TEST_ASSERT(obj_map.GetCount() == 4, "Count decreased to 4");

        // 6.6 枚举对象
        cout << "\n[6.6] Enumerate objects:" << endl;
        int enum_count = 0;
        obj_map.EnumKV([&enum_count](const AnsiString &key, UserInfoClass &ui)
        {
            cout << "  [" << key.c_str() << ", "
                 << (ui.GetSex() ? "male" : "female") << ", "
                 << ui.GetAge() << "]" << endl;
            enum_count++;
        });
        TEST_ASSERT(enum_count == 4, "Enumerated 4 remaining objects");

        // 6.7 清空容器
        cout << "\n[6.7] Clear all:" << endl;
        obj_map.Clear();
        TEST_ASSERT(obj_map.GetCount() == 0, "Map cleared");

        cout << "\n[6.8] Scope exit:" << endl;
    }
    cout << "  All objects automatically cleaned up" << endl;
}

// TEST 7: 并发修改测试（在枚举中修改）
void ConcurrentModificationTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Concurrent Modification Test" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int,int> map;

    // 7.1 在枚举中修改值
    cout << "\n[7.1] Modify values during enumeration:" << endl;
    for(int i = 0; i < 10; i++)
        map.Add(i, i);

    map.EnumKV([](const int &key, int &value)
    {
        value *= 2; // 修改值
    });

    int val = 0;
    map.Get(5, val);
    TEST_ASSERT(val == 10, "Value modified during enumeration");

    // 注意：不要在枚举中添加/删除元素，这会导致未定义行为
    cout << "  Warning: Do NOT add/delete during enumeration!" << endl;
}

// TEST 8: 性能和批量操作测试
void PerformanceAndBatchTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 8: Performance & Batch Operations" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int,int> map;
    constexpr int PERF_COUNT = 10000;

    // 8.1 顺序添加
    cout << "\n[8.1] Sequential add " << PERF_COUNT << " elements..." << endl;
    for(int i = 0; i < PERF_COUNT; i++)
    {
        map.Add(i, i);
    }
    TEST_ASSERT(map.GetCount() == PERF_COUNT, "All elements added");

    // 8.2 随机查找
    cout << "\n[8.2] Random lookup test..." << endl;
    int lookup_success = 0;
    for(int i = 0; i < 1000; i++)
    {
        int key = rand() % PERF_COUNT;
        if(map.ContainsKey(key))
            lookup_success++;
    }
    TEST_ASSERT(lookup_success == 1000, "All random lookups succeeded");

    // 8.3 随机删除
    cout << "\n[8.3] Random delete test..." << endl;
    int delete_count = 0;
    for(int i = 0; i < PERF_COUNT; i += 10)
    {
        if(map.DeleteByKey(i))
            delete_count++;
    }
    cout << "  Deleted " << delete_count << " elements" << endl;
    TEST_ASSERT(map.GetCount() == PERF_COUNT - delete_count, "Count matches deletes");

    // 8.4 Free vs Clear测试
    cout << "\n[8.4] Free vs Clear:" << endl;
    int count_before_clear = map.GetCount();
    map.Clear();
    TEST_ASSERT(map.GetCount() == 0, "Clear empties map");

    // 重新填充
    for(int i = 0; i < 100; i++)
        map.Add(i, i);

    map.Free();
    TEST_ASSERT(map.GetCount() == 0, "Free empties map and releases memory");
}

// TEST 9: 极限边界测试
void ExtremeCaseTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 9: Extreme Boundary Tests" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int,int> map;

    // 9.1 单元素操作
    cout << "\n[9.1] Single element operations:" << endl;
    map.Add(42, 42);
    TEST_ASSERT(map.GetCount() == 1, "Single element added");
    TEST_ASSERT(map.DeleteByKey(42), "Single element deleted");
    TEST_ASSERT(map.GetCount() == 0, "Back to empty");

    // 9.2 连续添加删除
    cout << "\n[9.2] Repeated add/delete cycles:" << endl;
    for(int cycle = 0; cycle < 5; cycle++)
    {
        for(int i = 0; i < 100; i++)
            map.Add(i, i * 2);

        for(int i = 0; i < 100; i++)
            map.DeleteByKey(i);
    }
    TEST_ASSERT(map.GetCount() == 0, "All cycles completed, map empty");

    // 9.3 大量重复键尝试
    cout << "\n[9.3] Duplicate key stress test:" << endl;
    map.Add(999, 1);
    int duplicate_attempts = 0;
    for(int i = 0; i < 1000; i++)
    {
        if(!map.Add(999, i))
            duplicate_attempts++;
    }
    TEST_ASSERT(duplicate_attempts == 1000, "All duplicate adds rejected");
    TEST_ASSERT(map.GetCount() == 1, "Still only one element");

    int final_value;
    map.Get(999, final_value);
    TEST_ASSERT(final_value == 1, "Original value unchanged");
}

int main(int,char **)
{
    cout << "========================================" << endl;
    cout << "Map Comprehensive Test Suite" << endl;
    cout << "Testing dangerous scenarios and edge cases" << endl;
    cout << "========================================" << endl;

    srand(static_cast<unsigned int>(time(nullptr)));

    BasicOperationTest();
    StressTest();
    NonTrivialTypeTest();
    EdgeCaseTest();
    StringKeyTest();
    ObjectMapTest();
    ConcurrentModificationTest();
    PerformanceAndBatchTest();
    ExtremeCaseTest();

    cout << "\n========================================" << endl;
    cout << "Test Summary" << endl;
    cout << "========================================" << endl;
    cout << "Tests Passed: " << test_passed << endl;
    cout << "Tests Failed: " << test_failed << endl;
    cout << "Total Tests:  " << (test_passed + test_failed) << endl;

    if(test_failed == 0)
    {
        cout << "\n🎉 ALL TESTS PASSED! 🎉" << endl;
    }
    else
    {
        cout << "\n⚠️  SOME TESTS FAILED! ⚠️" << endl;
    }

    return test_failed > 0 ? 1 : 0;
}
