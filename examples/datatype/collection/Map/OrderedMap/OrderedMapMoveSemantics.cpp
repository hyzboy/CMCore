#include<hgl/platform/Platform.h>
#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: OrderedMap Move Semantics" << endl;
    cout << "========================================" << endl;

    // 测试1: string 类型的移动语义
    cout << "\n[1] Test with std::string:" << endl;
    {
        OrderedMap<int, string> map;
        
        // 使用右值引用添加
        assert(map.Add(1, string("value1")));
        assert(map.Add(3, string("value3")));
        assert(map.Add(2, string("value2")));
        
        string val;
        assert(map.Get(1, val) && val == "value1");
        assert(map.GetCount() == 3);
        cout << "  ✓ String move semantics work" << endl;
    }

    // 测试2: vector 对象的移动语义
    cout << "\n[2] Test with std::vector:" << endl;
    {
        OrderedMap<int, vector<int>> map;
        
        vector<int> test_vec = {1, 2, 3, 4, 5};
        auto vec_copy = test_vec;
        
        // 使用移动语义
        assert(map.Add(1, move(test_vec)));
        
        vector<int> retrieved;
        assert(map.Get(1, retrieved) && retrieved == vec_copy);
        cout << "  ✓ Vector move semantics work" << endl;
    }

    // 测试3: AddOrUpdate 的移动语义
    cout << "\n[3] Test AddOrUpdate with move semantics:" << endl;
    {
        OrderedMap<int, string> map;
        
        // 完全移动版本
        map.AddOrUpdate(1, string("first"));
        map.AddOrUpdate(1, string("updated"));
        
        string val;
        assert(map.Get(1, val) && val == "updated");
        cout << "  ✓ AddOrUpdate move semantics work" << endl;
    }

    // 测试4: 混合语义 - Key右值，Value左值
    cout << "\n[4] Test mixed semantics (K&&, const V&):" << endl;
    {
        OrderedMap<string, int> map;
        
        int val = 42;
        map.Add(string("key1"), val);
        map.Add(string("key2"), val);
        
        int result;
        assert(map.Get("key1", result) && result == 42);
        cout << "  ✓ Mixed K&& const V& semantics work" << endl;
    }

    // 测试5: 混合语义 - Key左值，Value右值
    cout << "\n[5] Test mixed semantics (const K&, V&&):" << endl;
    {
        OrderedMap<int, string> map;
        
        int key = 99;
        map.Add(key, string("value99"));
        
        string result;
        assert(map.Get(99, result) && result == "value99");
        cout << "  ✓ Mixed const K& V&& semantics work" << endl;
    }

    // 测试6: 临时对象的自动移动
    cout << "\n[6] Test automatic move of temporaries:" << endl;
    {
        OrderedMap<int, string> map;
        
        // 直接使用临时对象
        map.Add(1, string("temp1"));
        map.AddOrUpdate(2, string("temp2"));
        
        assert(map.GetCount() == 2);
        cout << "  ✓ Temporary objects automatically moved" << endl;
    }

    // 测试7: 排序验证（混合操作）
    cout << "\n[7] Test ordering with mixed semantics:" << endl;
    {
        OrderedMap<int, string> map;
        
        // 乱序添加
        map.Add(5, string("five"));
        map.Add(2, move(string("two")));
        map.Add(8, string("eight"));
        map.Add(1, move(string("one")));
        
        assert(map.GetKeyAt(0) == 1);
        assert(map.GetKeyAt(1) == 2);
        assert(map.GetKeyAt(2) == 5);
        assert(map.GetKeyAt(3) == 8);
        cout << "  ✓ Keys maintained in sorted order" << endl;
    }

    // 测试8: 批量移动操作
    cout << "\n[8] Test batch operations with move semantics:" << endl;
    {
        OrderedMap<string, vector<int>> map;
        
        for (int i = 0; i < 10; i++) {
            vector<int> vec;
            for (int j = 0; j < 5; j++) {
                vec.push_back(i * 10 + j);
            }
            map.Add(string("key") + to_string(i), move(vec));
        }
        
        assert(map.GetCount() == 10);
        
        vector<int> retrieved;
        map.Get("key5", retrieved);
        assert(retrieved.size() == 5);
        cout << "  ✓ Batch move operations work" << endl;
    }

    // 测试9: AddOrUpdate 的混合语义
    cout << "\n[9] Test AddOrUpdate mixed semantics:" << endl;
    {
        OrderedMap<int, string> map;
        
        // K&&, const V&
        string val = "shared";
        map.AddOrUpdate(1, val);
        
        // const K&, V&&
        int key = 2;
        map.AddOrUpdate(key, string("moved"));
        
        assert(map.GetCount() == 2);
        cout << "  ✓ AddOrUpdate mixed semantics work" << endl;
    }

    // 测试10: 性能对比 - 100个元素
    cout << "\n[10] Performance test: 100 elements:" << endl;
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

    cout << "\n✅ ALL ORDEREDMAP MOVE SEMANTICS TESTS PASSED" << endl;
    return 0;
}
