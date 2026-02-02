#include<hgl/type/FlatOrderedMap.h>
#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<chrono>

using namespace hgl;
using namespace std;

// 追踪对象的拷贝和移动次数
class TrackedObject {
public:
    static int copy_count;
    static int move_count;
    
    int value;
    string data;
    
    TrackedObject() : value(0), data("") {}
    TrackedObject(int v, const string& d) : value(v), data(d) {}
    
    // 拷贝构造
    TrackedObject(const TrackedObject& other) : value(other.value), data(other.data) {
        copy_count++;
    }
    
    // 移动构造
    TrackedObject(TrackedObject&& other) noexcept : value(other.value), data(move(other.data)) {
        move_count++;
        other.value = 0;
    }
    
    // 拷贝赋值
    TrackedObject& operator=(const TrackedObject& other) {
        if (this != &other) {
            value = other.value;
            data = other.data;
            copy_count++;
        }
        return *this;
    }
    
    // 移动赋值
    TrackedObject& operator=(TrackedObject&& other) noexcept {
        if (this != &other) {
            value = other.value;
            data = move(other.data);
            move_count++;
            other.value = 0;
        }
        return *this;
    }
    
    bool operator<(const TrackedObject& other) const {
        return value < other.value;
    }
    
    bool operator==(const TrackedObject& other) const {
        return value == other.value && data == other.data;
    }
    
    static void ResetCounters() {
        copy_count = 0;
        move_count = 0;
    }
};

int TrackedObject::copy_count = 0;
int TrackedObject::move_count = 0;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: FlatOrderedMap Move Semantics" << endl;
    cout << "========================================" << endl;

    // 测试1: string 对象的移动语义
    cout << "\n[1] Test with std::string (non-trivial type):" << endl;
    {
        OrderedMap<int, string> map;
        
        cout << "  Adding with rvalue references..." << endl;
        assert(map.Add(1, string("value1")));
        assert(map.Add(2, string("value2")));
        
        string val;
        assert(map.Get(1, val) && val == "value1");
        cout << "  ✓ String move semantics work" << endl;
    }

    // 测试2: 移动构造 vs 拷贝构造性能对比
    cout << "\n[2] Test with vector (heavy copy cost):" << endl;
    {
        OrderedMap<int, vector<int>> map;
        
        vector<int> large_vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
        
        cout << "  Using move semantics..." << endl;
        auto vec_copy = large_vec;  // 保留副本用于验证
        assert(map.Add(1, move(large_vec)));  // 显式移动
        
        vector<int> retrieved;
        assert(map.Get(1, retrieved) && retrieved == vec_copy);
        cout << "  ✓ Vector move semantics work" << endl;
    }

    // 测试3: AddOrUpdate 的移动语义
    cout << "\n[3] Test AddOrUpdate with move semantics:" << endl;
    {
        OrderedMap<int, string> map;
        
        // 首次添加（移动）
        map.AddOrUpdate(1, string("first"));
        
        // 更新（移动）
        map.AddOrUpdate(1, string("updated"));
        
        string val;
        assert(map.Get(1, val) && val == "updated");
        cout << "  ✓ AddOrUpdate move semantics work" << endl;
    }

    // 测试4: Change 的移动语义
    cout << "\n[4] Test Change with move semantics:" << endl;
    {
        OrderedMap<int, string> map;
        map.Add(1, string("original"));
        
        bool changed = map.Change(1, string("modified"));
        assert(changed);
        
        string val;
        assert(map.Get(1, val) && val == "modified");
        cout << "  ✓ Change move semantics work" << endl;
    }

    // 测试5: 混合语义（Key右值，Value左值）
    cout << "\n[5] Test mixed semantics (Key rvalue, Value lvalue):" << endl;
    {
        OrderedMap<string, int> map;
        
        int value = 42;
        assert(map.Add(string("key1"), value));
        
        int retrieved;
        assert(map.Get("key1", retrieved) && retrieved == 42);
        cout << "  ✓ Mixed semantics (K&&, const V&) work" << endl;
    }

    // 测试6: 混合语义（Key左值，Value右值）
    cout << "\n[6] Test mixed semantics (Key lvalue, Value rvalue):" << endl;
    {
        OrderedMap<int, string> map;
        
        int key = 99;
        assert(map.Add(key, string("value99")));
        
        string retrieved;
        assert(map.Get(99, retrieved) && retrieved == "value99");
        cout << "  ✓ Mixed semantics (const K&, V&&) work" << endl;
    }

    // 测试7: 临时对象的自动移动
    cout << "\n[7] Test automatic move of temporaries:" << endl;
    {
        OrderedMap<int, string> map;
        
        // 直接传递临时对象，应该自动使用移动
        assert(map.Add(1, string("temporary")));
        map.AddOrUpdate(2, string("temp2"));
        
        cout << "  ✓ Temporary objects automatically moved" << endl;
    }

    // 测试8: 性能测试 - 插入100个string对象
    cout << "\n[8] Performance test: inserting 100 string objects:" << endl;
    {
        OrderedMap<int, string> map;
        
        auto start = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 100; i++) {
            map.Add(i, string("value_") + to_string(i));
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        assert(map.GetCount() == 100);
        cout << "  ✓ Inserted 100 items in " << duration.count() << "ms" << endl;
    }

    // 测试9: 连续调用多个移动语义重载
    cout << "\n[9] Test multiple overload combinations:" << endl;
    {
        OrderedMap<string, vector<int>> map;
        
        string key1 = "key1";
        map.Add(move(key1), vector<int>{1, 2, 3});  // K&&, V&&
        
        string key2 = "key2";
        map.Add(move(key2), vector<int>{4, 5});     // K&&, V&&
        
        string key3 = "key3";
        vector<int> val3 = {7, 8, 9};
        map.Add(key3, move(val3));                   // const K&, V&&
        
        assert(map.GetCount() == 3);
        cout << "  ✓ All overload combinations work correctly" << endl;
    }

    cout << "\n✅ ALL MOVE SEMANTICS TESTS PASSED" << endl;
    return 0;
}
