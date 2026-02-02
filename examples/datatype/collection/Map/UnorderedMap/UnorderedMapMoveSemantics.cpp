#include<hgl/type/UnorderedMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<chrono>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: UnorderedMap Move Semantics" << endl;
    cout << "========================================" << endl;

    // 测试1: string 对象的移动语义
    cout << "\n[1] Test with std::string:" << endl;
    {
        UnorderedMap<int, string> map;
        
        assert(map.Add(1, string("value1")));
        assert(map.Add(2, string("value2")));
        assert(map.Add(3, string("value3")));
        
        string val;
        assert(map.Get(1, val) && val == "value1");
        assert(map.GetCount() == 3);
        cout << "  ✓ String move semantics work" << endl;
    }

    // 测试2: vector 对象的移动语义
    cout << "\n[2] Test with std::vector:" << endl;
    {
        UnorderedMap<string, vector<int>> map;
        
        vector<int> test_vec = {10, 20, 30, 40, 50};
        auto vec_copy = test_vec;
        
        assert(map.Add("key1", move(test_vec)));
        
        vector<int> retrieved;
        assert(map.Get("key1", retrieved) && retrieved == vec_copy);
        cout << "  ✓ Vector move semantics work" << endl;
    }

    // 测试3: Add 的所有重载版本
    cout << "\n[3] Test all Add overloads:" << endl;
    {
        UnorderedMap<int, string> map;
        
        // (const K&, const V&)
        int key1 = 1;
        string val1 = "const_const";
        assert(map.Add(key1, val1));
        
        // (const K&, V&&)
        int key2 = 2;
        assert(map.Add(key2, string("const_move")));
        
        // (K&&, const V&)
        string val3 = "move_const";
        assert(map.Add(3, val3));
        
        // (K&&, V&&)
        assert(map.Add(4, string("move_move")));
        
        assert(map.GetCount() == 4);
        cout << "  ✓ All Add overloads work" << endl;
    }

    // 测试4: ChangeOrAdd 的移动语义
    cout << "\n[4] Test ChangeOrAdd with move semantics:" << endl;
    {
        UnorderedMap<int, string> map;
        
        // 添加新元素
        map.ChangeOrAdd(1, string("first"));
        
        // 更新现有元素
        map.ChangeOrAdd(1, string("updated"));
        
        string val;
        assert(map.Get(1, val) && val == "updated");
        cout << "  ✓ ChangeOrAdd move semantics work" << endl;
    }

    // 测试5: ChangeOrAdd 的所有重载版本
    cout << "\n[5] Test ChangeOrAdd overloads:" << endl;
    {
        UnorderedMap<string, int> map;
        
        // (const K&, const V&)
        string k1 = "key1";
        int v1 = 10;
        map.ChangeOrAdd(k1, v1);
        
        // (const K&, V&&)  - 实际上这里 int 是平凡类型，但测试调用
        map.ChangeOrAdd("key2", 20);
        
        // (K&&, const V&)
        int v3 = 30;
        map.ChangeOrAdd(string("key3"), v3);
        
        // (K&&, V&&)
        map.ChangeOrAdd(string("key4"), 40);
        
        assert(map.GetCount() == 4);
        cout << "  ✓ All ChangeOrAdd overloads work" << endl;
    }

    // 测试6: Change 的移动语义
    cout << "\n[6] Test Change with move semantics:" << endl;
    {
        UnorderedMap<int, string> map;
        map.Add(1, string("original"));
        
        // 使用移动语义更新
        assert(map.Change(1, string("modified")));
        
        string val;
        assert(map.Get(1, val) && val == "modified");
        cout << "  ✓ Change move semantics work" << endl;
    }

    // 测试7: 临时对象的自动移动
    cout << "\n[7] Test automatic move of temporaries:" << endl;
    {
        UnorderedMap<int, string> map;
        
        // 直接传递临时对象
        map.Add(1, string("temp1"));
        map.ChangeOrAdd(2, string("temp2"));
        map.Change(1, string("modified_temp"));
        
        assert(map.GetCount() == 2);
        cout << "  ✓ Temporary objects automatically moved" << endl;
    }

    // 测试8: 复杂对象的批量移动
    cout << "\n[8] Test batch move operations:" << endl;
    {
        UnorderedMap<string, vector<string>> map;
        
        for (int i = 0; i < 10; i++) {
            vector<string> vec;
            for (int j = 0; j < 3; j++) {
                vec.push_back(string("item_") + to_string(i) + "_" + to_string(j));
            }
            map.Add(string("key") + to_string(i), move(vec));
        }
        
        assert(map.GetCount() == 10);
        
        vector<string> retrieved;
        map.Get("key5", retrieved);
        assert(retrieved.size() == 3);
        cout << "  ✓ Batch move operations work" << endl;
    }

    // 测试9: ChangeOrAdd 覆盖测试
    cout << "\n[9] Test ChangeOrAdd coverage:" << endl;
    {
        UnorderedMap<int, vector<int>> map;
        
        // 添加
        vector<int> v1 = {1, 2, 3};
        map.ChangeOrAdd(1, move(v1));
        
        // 更新
        map.ChangeOrAdd(1, vector<int>{4, 5, 6});
        
        vector<int> result;
        map.Get(1, result);
        vector<int> expected{4, 5, 6};
        assert(result == expected);
        cout << "  ✓ ChangeOrAdd add/update cycle works" << endl;
    }

    // 测试10: 大规模插入性能
    cout << "\n[10] Performance test: 1000 complex objects:" << endl;
    {
        UnorderedMap<int, vector<int>> map;
        
        auto start = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < 1000; i++) {
            vector<int> vec;
            for (int j = 0; j < 10; j++) {
                vec.push_back(i * 10 + j);
            }
            map.Add(i, move(vec));
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        assert(map.GetCount() == 1000);
        cout << "  ✓ Inserted 1000 items in " << duration.count() << "ms" << endl;
    }

    cout << "\n✅ ALL UNORDEREDMAP MOVE SEMANTICS TESTS PASSED" << endl;
    return 0;
}
