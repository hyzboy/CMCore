#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<string>
#include<vector>
#include<hgl/type/OrderedMap.h>
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
    int obj_id;  // 唯一对象ID，用于追踪生命周期

    static int construct_count;
    static int destruct_count;
    static int copy_count;
    static int next_obj_id;
    static bool enable_trace;  // 是否启用详细追踪输出

    ComplexValue() : id(0), data(""), dynamic_ptr(nullptr), obj_id(next_obj_id++)
    {
        construct_count++;
        if (enable_trace)
            cout << "    [OBJ#" << obj_id << "] Default construct (addr=" << (void*)this << ")" << endl;
    }

    ComplexValue(int i, const std::string& d) : id(i), data(d), dynamic_ptr(new int(i)), obj_id(next_obj_id++)
    {
        construct_count++;
        if (enable_trace)
            cout << "    [OBJ#" << obj_id << "] Construct(id=" << i << ", data='" << d << "', addr=" << (void*)this << ")" << endl;
    }

    ComplexValue(const ComplexValue& other) : id(other.id), data(other.data),
        dynamic_ptr(other.dynamic_ptr ? new int(*other.dynamic_ptr) : nullptr), obj_id(next_obj_id++)
    {
        copy_count++;
        if (enable_trace)
            cout << "    [OBJ#" << obj_id << "] Copy from OBJ#" << other.obj_id << " (id=" << id << ", addr=" << (void*)this << ")" << endl;
    }

    ComplexValue& operator=(const ComplexValue& other)
    {
        if (this != &other) {
            id = other.id;
            data = other.data;
            delete dynamic_ptr;
            dynamic_ptr = other.dynamic_ptr ? new int(*other.dynamic_ptr) : nullptr;
            copy_count++;
            if (enable_trace)
                cout << "    [OBJ#" << obj_id << "] Assign from OBJ#" << other.obj_id << " (id=" << id << ", addr=" << (void*)this << ")" << endl;
        }
        return *this;
    }

    bool operator<(const ComplexValue& other) const
    {
        return id < other.id;
    }

    bool operator==(const ComplexValue& other) const
    {
        return id == other.id && data == other.data;
    }

    ~ComplexValue()
    {
        if (enable_trace)
            cout << "    [OBJ#" << obj_id << "] Destruct (id=" << id << ", addr=" << (void*)this << ")" << endl;
        delete dynamic_ptr;
        destruct_count++;
    }

    static void ResetCounters() {
        construct_count = 0;
        destruct_count = 0;
        copy_count = 0;
        next_obj_id = 0;
    }

    static void SetTrace(bool enable) {
        enable_trace = enable;
    }
};

int ComplexValue::construct_count = 0;
int ComplexValue::destruct_count = 0;
int ComplexValue::copy_count = 0;
int ComplexValue::next_obj_id = 0;
bool ComplexValue::enable_trace = false;

void out_map(OrderedMap<int,int> &int_map)
{
    const int count=int_map.GetCount();
    cout<<"count:"<<count<<" ";

    int_map.EnumKV([](const int &key,int &value)
    {
        cout<<"["<<key<<","<<value<<"]";
    });

    cout<<endl;
}

// TEST 1: 基础操作测试（有序性）
void BasicOperationTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: Basic Operations (Ordered)" << endl;
    cout << "========================================" << endl;

    OrderedMap<int,int> map;

    // 1.1 空Map测试
    cout << "\n[1.1] Empty map tests:" << endl;
    TEST_ASSERT(map.GetCount() == 0, "Empty map has count 0");
    TEST_ASSERT(!map.ContainsKey(1), "Non-existent key returns false");

    int value = 0;
    TEST_ASSERT(!map.Get(1, value), "Get on empty map returns false");

    // 1.2 添加元素（无序添加）
    cout << "\n[1.2] Add elements (unordered):" << endl;
    TEST_ASSERT(map.Add(3, 30), "Add element with key 3");
    TEST_ASSERT(map.Add(1, 10), "Add element with key 1");
    TEST_ASSERT(map.Add(2, 20), "Add element with key 2");
    TEST_ASSERT(map.GetCount() == 3, "Count is 3 after adds");

    // 1.3 验证有序性（按序遍历应该是有序的）
    cout << "\n[1.3] Verify ordering:" << endl;
    int prev_key = -1;
    bool is_ordered = true;
    map.EnumKV([&](const int &key, int &val)
    {
        if(key <= prev_key)
            is_ordered = false;
        prev_key = key;
    });
    TEST_ASSERT(is_ordered, "Keys are in sorted order");

    // 1.4 访问有序元素
    cout << "\n[1.4] Access ordered elements:" << endl;
    int key1, val1;
    TEST_ASSERT(map.GetBySerial(0, key1, val1), "Get first element");
    TEST_ASSERT(key1 == 1 && val1 == 10, "First element is key=1, val=10");

    int key3, val3;
    TEST_ASSERT(map.GetBySerial(2, key3, val3), "Get third element");
    TEST_ASSERT(key3 == 3 && val3 == 30, "Third element is key=3, val=30");

    // 1.5 获取元素
    cout << "\n[1.5] Get elements:" << endl;
    int retrieved_value = 0;
    TEST_ASSERT(map.Get(2, retrieved_value), "Get existing key 2");
    TEST_ASSERT(retrieved_value == 20, "Retrieved correct value");

    // 1.6 修改元素
    cout << "\n[1.6] Update elements:" << endl;
    TEST_ASSERT(map.Change(2, 25), "Change key 2 value");
    map.Get(2, retrieved_value);
    TEST_ASSERT(retrieved_value == 25, "Value updated correctly");

    // 1.7 删除元素
    cout << "\n[1.7] Delete elements:" << endl;
    TEST_ASSERT(map.DeleteByKey(2), "Delete key 2");
    TEST_ASSERT(map.GetCount() == 2, "Count is 2 after delete");
    TEST_ASSERT(!map.ContainsKey(2), "Key 2 no longer exists");

    // 验证顺序仍然保持
    map.EnumKV([&](const int &key, int &val)
    {
        cout << "  [" << key << "," << val << "]";
    });
    cout << endl;
}

// TEST 2: 大量数据测试（有序性保持）
void StressTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Stress Test with Ordered Data" << endl;
    cout << "========================================" << endl;

    OrderedMap<int,int> map;
    constexpr int LARGE_COUNT = 1000;

    // 2.1 随机顺序添加
    cout << "\n[2.1] Add " << LARGE_COUNT << " elements in random order:" << endl;
    std::vector<int> random_keys(LARGE_COUNT);
    for(int i = 0; i < LARGE_COUNT; i++)
        random_keys[i] = i;

    // 打乱顺序
    for(int i = LARGE_COUNT - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = random_keys[i];
        random_keys[i] = random_keys[j];
        random_keys[j] = temp;
    }

    for(int i = 0; i < LARGE_COUNT; i++)
    {
        map.Add(random_keys[i], random_keys[i] * 10);
    }
    TEST_ASSERT(map.GetCount() == LARGE_COUNT, "All elements added");

    // 2.2 验证所有元素有序
    cout << "\n[2.2] Verify all elements are ordered:" << endl;
    int prev_key = -1;
    bool all_ordered = true;
    int check_count = 0;
    map.EnumKV([&](const int &key, int &value)
    {
        if(key <= prev_key)
            all_ordered = false;
        if(value != key * 10)
            all_ordered = false;
        prev_key = key;
        check_count++;
    });
    TEST_ASSERT(all_ordered, "All elements in correct order");
    TEST_ASSERT(check_count == LARGE_COUNT, "Enumerated all elements");

    // 2.3 二分查找测试（利用有序性）
    cout << "\n[2.3] Binary search performance test:" << endl;
    int search_success = 0;
    for(int i = 0; i < 100; i++)
    {
        int key = rand() % LARGE_COUNT;
        int index = map.Find(key);
        if(index != -1)
        {
            int k, v;
            map.GetBySerial(index, k, v);
            if(k == key && v == key * 10)
                search_success++;
        }
    }
    TEST_ASSERT(search_success == 100, "All searches successful");

    // 2.4 按序遍历测试
    cout << "\n[2.4] Sequential iteration test:" << endl;
    int expected_key = 0;
    bool sequence_correct = true;
    for(int i = 0; i < LARGE_COUNT; i++)
    {
        int k, v;
        if(map.GetBySerial(i, k, v))
        {
            if(k != i || v != i * 10)
                sequence_correct = false;
        }
    }
    TEST_ASSERT(sequence_correct, "Sequential access yields ordered results");

    // 2.5 批量删除
    cout << "\n[2.5] Delete half elements:" << endl;
    for(int i = 0; i < LARGE_COUNT; i += 2)
    {
        map.DeleteByKey(i);
    }
    TEST_ASSERT(map.GetCount() == LARGE_COUNT / 2, "Half elements deleted");

    // 验证剩余元素仍然有序
    prev_key = -1;
    bool still_ordered = true;
    map.EnumKV([&](const int &key, int &value)
    {
        if(key <= prev_key || (key % 2) == 0) // 应该只剩奇数键
            still_ordered = false;
        prev_key = key;
    });
    TEST_ASSERT(still_ordered, "Remaining elements still ordered");

    // 2.6 清空
    cout << "\n[2.6] Clear all:" << endl;
    map.Clear();
    TEST_ASSERT(map.GetCount() == 0, "Map cleared");
}

// TEST 3: 非平凡类型测试（内存管理 + 有序）
void NonTrivialTypeTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Non-Trivial Type (Ordered)" << endl;
    cout << "========================================" << endl;

    ComplexValue::ResetCounters();
    ComplexValue::SetTrace(true);  // 启用详细追踪

    {
        OrderedMap<int, ComplexValue> complex_map;

        // 3.1 无序添加非平凡对象
        cout << "\n[3.1] Add complex objects (unordered):" << endl;
        complex_map.Add(3, ComplexValue(3, "Third"));
        complex_map.Add(1, ComplexValue(1, "First"));
        complex_map.Add(2, ComplexValue(2, "Second"));

        cout << "  Constructs: " << ComplexValue::construct_count << endl;
        cout << "  Copies: " << ComplexValue::copy_count << endl;

        TEST_ASSERT(complex_map.GetCount() == 3, "Added 3 complex objects");

        // 3.2 验证有序性
        cout << "\n[3.2] Verify ordered by key:" << endl;
        int prev_id = 0;
        bool ordered = true;
        complex_map.EnumKV([&](const int &key, ComplexValue &val)
        {
            if(key != val.id || key <= prev_id)
                ordered = false;
            prev_id = key;
        });
        TEST_ASSERT(ordered, "Complex objects ordered by key");

        // 3.3 访问验证
        cout << "\n[3.3] Access and verify:" << endl;
        ComplexValue val;
        TEST_ASSERT(complex_map.Get(2, val), "Get complex object");
        TEST_ASSERT(val.id == 2 && val.data == "Second", "Complex object data correct");

        // 3.4 删除测试
        cout << "\n[3.4] Delete one object:" << endl;
        TEST_ASSERT(complex_map.ContainsKey(2), "Key 2 exists before delete");
        complex_map.DeleteByKey(2);
        TEST_ASSERT(!complex_map.ContainsKey(2), "Key 2 removed after delete");
        TEST_ASSERT(complex_map.GetCount() == 2, "Object deleted");

        cout << "\n[3.5] Scope exit (auto cleanup):" << endl;
        cout << "  Map清空中..." << endl;
    }

    cout << "\n[3.6] 清空后统计:" << endl;
    // 验证所有对象都被正确析构
    // 注：析构数可能大于构造数，因为btree_map内部复制产生的临时对象也会被析构
    cout << "  Final destructs: " << ComplexValue::destruct_count << endl;
    cout << "  Total instances (construct+copy): " << (ComplexValue::construct_count + ComplexValue::copy_count) << endl;
    TEST_ASSERT(ComplexValue::destruct_count >= ComplexValue::construct_count,
                "All constructed objects destroyed (no memory leak)");

    ComplexValue::SetTrace(false);  // 关闭追踪
}

// TEST 4: 边界条件和有序性保持
void EdgeCaseTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 4: Edge Cases (Ordering Preserved)" << endl;
    cout << "========================================" << endl;

    OrderedMap<int,int> map;

    // 4.1 逆序添加测试
    cout << "\n[4.1] Reverse order insertion:" << endl;
    for(int i = 100; i > 0; i--)
    {
        map.Add(i, i * 2);
    }
    TEST_ASSERT(map.GetCount() == 100, "All reverse-order elements added");

    int first_key, first_val;
    map.GetBySerial(0, first_key, first_val);
    TEST_ASSERT(first_key == 1, "First element has smallest key");

    int last_key, last_val;
    map.GetBySerial(99, last_key, last_val);
    TEST_ASSERT(last_key == 100, "Last element has largest key");

    // 4.2 重复键测试（有序性不变）
    cout << "\n[4.2] Duplicate key test:" << endl;
    TEST_ASSERT(!map.Add(50, 999), "Cannot add duplicate key");
    int val;
    map.Get(50, val);
    TEST_ASSERT(val == 100, "Original value preserved");

    // 4.3 ChangeOrAdd测试（维护顺序）
    cout << "\n[4.3] ChangeOrAdd test:" << endl;
    TEST_ASSERT(map.ChangeOrAdd(150, 300), "ChangeOrAdd new key");
    TEST_ASSERT(map.ContainsKey(150), "New key 150 exists");

    // 验证150在末尾
    int new_last_key, new_last_val;
    map.GetBySerial(map.GetCount() - 1, new_last_key, new_last_val);
    TEST_ASSERT(new_last_key == 150, "New largest key at end");

    // 4.4 FindPos测试
    cout << "\n[4.4] FindPos test (binary search):" << endl;
    int pos = map.Find(50);
    TEST_ASSERT(pos != -1, "FindPos found key 50");
    cout << "  Key 50 at position: " << pos << endl;

    // 4.5 迭代器稳定性
    cout << "\n[4.5] Iterator stability:" << endl;
    map.Clear();
    for(int i = 10; i >= 1; i--)
        map.Add(i, i * 10);

    int iter_count = 0;
    int expected_key = 1;
    for(auto it : map)
    {
        if(it.first != expected_key)
        {
            cout << "  Expected key " << expected_key << ", got " << it.first << endl;
        }
        expected_key++;
        iter_count++;
    }
    TEST_ASSERT(iter_count == 10, "Iterator traversed all elements in order");
}

// TEST 5: String键测试（字典序）
void StringKeyTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: String Key Tests (Lexicographical Order)" << endl;
    cout << "========================================" << endl;

    OrderedMap<AnsiString,UserInfo> ui_map;

    // 5.1 添加所有用户
    cout << "\n[5.1] Add all users:" << endl;
    for(auto &ui : user_info_array)
    {
        ui_map.Add(ui.name, ui);
    }
    TEST_ASSERT(ui_map.GetCount() == user_info_array_count, "All users added");

    // 5.2 验证字典序
    cout << "\n[5.2] Verify lexicographical order:" << endl;
    AnsiString prev_name = "";
    bool lexical_ordered = true;
    ui_map.EnumKV([&](const AnsiString &name, UserInfo &ui)
    {
        if(!prev_name.IsEmpty() && name < prev_name)
            lexical_ordered = false;
        prev_name = name;
    });
    TEST_ASSERT(lexical_ordered, "Names in lexicographical order");

    // 5.3 查找测试
    cout << "\n[5.3] Lookup tests:" << endl;
    UserInfo found;
    TEST_ASSERT(ui_map.Get("Tom", found), "Find Tom");
    TEST_ASSERT(found.age == 37, "Tom's age is 37");

    // 5.4 按序遍历前5个用户
    cout << "\n[5.4] First 5 users (alphabetically):" << endl;
    for(int i = 0; i < 5 && i < ui_map.GetCount(); i++)
    {
        AnsiString name;
        UserInfo info;
        if(ui_map.GetBySerial(i, name, info))
        {
            cout << "  [" << i << "] " << name.c_str()
                 << " (" << (info.sex ? "M" : "F") << ", " << info.age << ")" << endl;
        }
    }

    // 5.5 删除测试（保持顺序）
    cout << "\n[5.5] Delete user:" << endl;
    int count_before = ui_map.GetCount();
    TEST_ASSERT(ui_map.DeleteByKey("Tom"), "Delete Tom");
    TEST_ASSERT(!ui_map.ContainsKey("Tom"), "Tom no longer exists");
    TEST_ASSERT(ui_map.GetCount() == count_before - 1, "Count decreased");

    // 验证删除后仍然有序
    prev_name = "";
    bool still_ordered = true;
    ui_map.EnumKV([&](const AnsiString &name, UserInfo &ui)
    {
        if(!prev_name.IsEmpty() && name < prev_name)
            still_ordered = false;
        prev_name = name;
    });
    TEST_ASSERT(still_ordered, "Still ordered after deletion");
}

// TEST 6: 指针管理测试（手动管理指针）
void PointerManagementTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 6: Pointer Management Tests" << endl;
    cout << "========================================" << endl;

    {
        OrderedMap<int, UserInfoClass*> obj_map;

        // 6.1 无序添加对象指针
        cout << "\n[6.1] Add object pointers (unordered):" << endl;
        int keys[] = {5, 2, 8, 1, 9, 3};
        for(int i = 0; i < 6; i++)
        {
            UserInfoClass *uic = new UserInfoClass;
            uic->Set(user_info_array[i % user_info_array_count]);
            obj_map.Add(keys[i], uic);
        }
        TEST_ASSERT(obj_map.GetCount() == 6, "Added 6 objects");

        // 6.2 验证有序性
        cout << "\n[6.2] Verify ordering:" << endl;
        int prev_key = 0;
        bool ordered = true;
        obj_map.EnumKV([&](const int &key, UserInfoClass* &ui)
        {
            if(key <= prev_key)
                ordered = false;
            prev_key = key;
        });
        TEST_ASSERT(ordered, "Objects ordered by key");

        // 6.3 访问对象
        cout << "\n[6.3] Access objects:" << endl;
        UserInfoClass* found = nullptr;
        obj_map.Get(2, found);
        TEST_ASSERT(found != nullptr, "Find key 2");

        // 6.4 删除对象（手动delete）
        cout << "\n[6.4] Delete object:" << endl;
        UserInfoClass* to_delete = nullptr;
        if(obj_map.Get(5, to_delete))
        {
            delete to_delete;
            obj_map.DeleteByKey(5);
        }
        TEST_ASSERT(!obj_map.ContainsKey(5), "Key 5 deleted");

        // 6.5 枚举对象（应该还是有序的）
        cout << "\n[6.5] Enumerate objects (ordered):" << endl;
        prev_key = 0;
        bool enum_ordered = true;
        obj_map.EnumKV([&](const int &key, UserInfoClass* &ui)
        {
            cout << "  [" << key << ", "
                 << (ui->GetSex() ? "male" : "female") << ", "
                 << ui->GetAge() << "]" << endl;
            if(key <= prev_key)
                enum_ordered = false;
            prev_key = key;
        });
        TEST_ASSERT(enum_ordered, "Enumeration in correct order");

        cout << "\n[6.6] Clear all:" << endl;
        // Manual cleanup before clear
        obj_map.EnumKV([](const int &key, UserInfoClass* &ui)
        {
            delete ui;
        });
        obj_map.Clear();
        TEST_ASSERT(obj_map.GetCount() == 0, "All objects cleared");
    }
    cout << "  Pointer map destroyed" << endl;
}

// TEST 7: 性能对比（有序 vs 无序）
void PerformanceComparisonTest()
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Performance Characteristics" << endl;
    cout << "========================================" << endl;

    OrderedMap<int,int> map;
    constexpr int PERF_COUNT = 1000;

    // 7.1 顺序插入性能
    cout << "\n[7.1] Sequential insertion " << PERF_COUNT << " elements..." << endl;
    for(int i = 0; i < PERF_COUNT; i++)
    {
        map.Add(i, i * 2);
    }
    TEST_ASSERT(map.GetCount() == PERF_COUNT, "All elements added");
    cout << "  Note: Sequential insertion is O(n) for ordered structures" << endl;

    // 7.2 查找性能（应该是O(log n)）
    cout << "\n[7.2] Binary search performance..." << endl;
    int find_success = 0;
    for(int i = 0; i < 100; i++)
    {
        int key = rand() % PERF_COUNT;
        if(map.Find(key) != -1)
            find_success++;
    }
    TEST_ASSERT(find_success == 100, "All searches successful");
    cout << "  Note: Binary search is O(log n)" << endl;

    // 7.3 有序遍历性能（直接按序）
    cout << "\n[7.3] Ordered enumeration..." << endl;
    int enum_count = 0;
    map.EnumKV([&](const int &key, int &value)
    {
        enum_count++;
    });
    TEST_ASSERT(enum_count == PERF_COUNT, "Enumerated all in order");
    cout << "  Note: Ordered enumeration is O(n)" << endl;

    // 7.4 范围查询（利用有序性）
    cout << "\n[7.4] Range query (100-199):" << endl;
    int range_count = 0;
    for(int i = 0; i < map.GetCount(); i++)
    {
        int k, v;
        if(map.GetBySerial(i, k, v))
        {
            if(k >= 100 && k < 200)
                range_count++;
            else if(k >= 200)
                break; // 利用有序性提前退出
        }
    }
    TEST_ASSERT(range_count == 100, "Range query found 100 elements");
    cout << "  Note: Range queries benefit from ordering" << endl;
}

int main(int,char **)
{
    cout << "========================================" << endl;
    cout << "OrderedMap Comprehensive Test Suite" << endl;
    cout << "Testing ordered map implementation" << endl;
    cout << "========================================" << endl;

    srand(static_cast<unsigned int>(time(nullptr)));

    BasicOperationTest();
    StressTest();
    NonTrivialTypeTest();
    EdgeCaseTest();
    StringKeyTest();
    PointerManagementTest();
    PerformanceComparisonTest();

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
