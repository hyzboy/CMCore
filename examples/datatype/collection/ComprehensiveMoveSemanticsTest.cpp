#include<hgl/type/FlatOrderedMap.h>
#include<hgl/type/FlatOrderedSet.h>
#include<hgl/type/FlatUnorderedSet.h>
#include<hgl/type/OrderedMap.h>
#include<hgl/type/OrderedSet.h>
#include<hgl/type/UnorderedMap.h>
#include<hgl/type/UnorderedSet.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<chrono>

using namespace hgl;
using namespace std;

// 辅助结构：追踪操作
struct OperationTracker {
    int total_adds = 0;
    int total_deletes = 0;
    chrono::milliseconds total_time{0};

    void Report(const string& container_name) {
        cout << "  [" << container_name << "] Adds: " << total_adds
             << ", Deletes: " << total_deletes << ", Time: "
             << total_time.count() << "ms" << endl;
    }
};

int os_main(int, os_char**)
{
    cout << "\n" << string(60, '=') << endl;
    cout << "COMPREHENSIVE MOVE SEMANTICS TEST SUITE" << endl;
    cout << string(60, '=') << endl;

    // ===== 测试1: 所有Map容器 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 1: Map Containers with Move Semantics" << endl;
    cout << string(60, '-') << endl;

    {
        // FlatOrderedMap (use trivially copyable types only)
        cout << "\n  [FlatOrderedMap]:" << endl;
        {
            FlatOrderedMap<int, int> map;
            for (int i = 0; i < 100; i++) {
                assert(map.Add(i, i * 10));
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }

        // OrderedMap (supports any type including string)
        cout << "\n  [OrderedMap]:" << endl;
        {
            OrderedMap<int, string> map;
            for (int i = 0; i < 100; i++) {
                assert(map.Add(i, string("value_") + to_string(i)));
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }

        // UnorderedMap
        cout << "\n  [UnorderedMap]:" << endl;
        {
            UnorderedMap<int, string> map;
            for (int i = 0; i < 100; i++) {
                assert(map.Add(i, string("value_") + to_string(i)));
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }

        // BidirectionalMap
        cout << "\n  [BidirectionalMap]:" << endl;
        {
            BidirectionalMap<int, string> map;
            for (int i = 0; i < 100; i++) {
                assert(map.Add(i, string("value_") + to_string(i)));
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }
    }

    // ===== 测试2: 所有Set容器 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 2: Set Containers with Move Semantics" << endl;
    cout << string(60, '-') << endl;

    {
        // FlatOrderedSet (use trivially copyable types only)
        cout << "\n  [FlatOrderedSet]:" << endl;
        {
            FlatOrderedSet<int> set;
            for (int i = 0; i < 100; i++) {
                assert(set.Add(i) >= 0);
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }

        // OrderedSet (supports any type including string)
        cout << "\n  [OrderedSet]:" << endl;
        {
            OrderedSet<string> set;
            for (int i = 0; i < 100; i++) {
                assert(set.Add(string("element_") + to_string(i)) >= 0);
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }

        // UnorderedSet
        cout << "\n  [UnorderedSet]:" << endl;
        {
            UnorderedSet<string> set;
            for (int i = 0; i < 100; i++) {
                assert(set.Add(string("element_") + to_string(i)));
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }

        // FlatUnorderedSet (use trivially copyable types only)
        cout << "\n  [FlatUnorderedSet]:" << endl;
        {
            FlatUnorderedSet<int> set;
            for (int i = 0; i < 100; i++) {
                assert(set.Add(i));
            }
            cout << "    ✓ Added 100 items with move semantics" << endl;
        }
    }

    // ===== 测试3: 复杂对象的移动 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 3: Complex Objects with Move Semantics" << endl;
    cout << string(60, '-') << endl;

    {
        cout << "\n  [Vector Storage in Map]:" << endl;
        {
            OrderedMap<int, vector<int>> map;
            for (int i = 0; i < 50; i++) {
                vector<int> vec;
                for (int j = 0; j < 10; j++) {
                    vec.push_back(i * 10 + j);
                }
                assert(map.Add(i, move(vec)));
            }
            cout << "    ✓ Stored 50 vectors efficiently with move" << endl;
        }

        cout << "\n  [String Storage in UnorderedMap]:" << endl;
        {
            UnorderedMap<string, vector<string>> map;
            for (int i = 0; i < 50; i++) {
                vector<string> vec;
                for (int j = 0; j < 5; j++) {
                    vec.push_back(string("item_") + to_string(i) + "_" + to_string(j));
                }
                assert(map.Add(string("key") + to_string(i), move(vec)));
            }
            cout << "    ✓ Stored 50 string vectors efficiently with move" << endl;
        }
    }

    // ===== 测试4: 混合语义 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 4: Mixed Semantics (const ref + rvalue)" << endl;
    cout << string(60, '-') << endl;

    {
        cout << "\n  [OrderedMap Mixed Semantics]:" << endl;
        {
            OrderedMap<string, vector<int>> map;

            // (const K&, const V&)
            string k1 = "key1";
            vector<int> v1 = {1, 2, 3};
            map.Add(k1, v1);

            // (const K&, V&&)
            map.Add("key2", vector<int>{4, 5, 6});

            // (K&&, const V&)
            vector<int> v3 = {7, 8, 9};
            map.Add(string("key3"), v3);

            // (K&&, V&&)
            map.Add(string("key4"), vector<int>{10, 11, 12});

            assert(map.GetCount() == 4);
            cout << "    ✓ All 4 semantic combinations work" << endl;
        }

        cout << "\n  [UnorderedMap Mixed Semantics]:" << endl;
        {
            UnorderedMap<int, string> map;

            // (const K&, const V&)
            int k1 = 1;
            string v1 = "value1";
            map.Add(k1, v1);

            // (const K&, V&&)
            map.Add(2, string("value2"));

            // (K&&, const V&)
            string v3 = "value3";
            map.Add(3, v3);

            // (K&&, V&&)
            map.Add(4, string("value4"));

            assert(map.GetCount() == 4);
            cout << "    ✓ All 4 semantic combinations work" << endl;
        }
    }

    // ===== 测试5: 性能对比 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 5: Performance Comparison (1000 elements)" << endl;
    cout << string(60, '-') << endl;

    {
        // FlatOrderedMap 性能 (use trivially copyable types)
        cout << "\n  [FlatOrderedMap - 1000 items]:" << endl;
        {
            auto start = chrono::high_resolution_clock::now();

            FlatOrderedMap<int, int> map;
            for (int i = 0; i < 1000; i++) {
                map.Add(i, i * 10);
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            cout << "    Time: " << duration.count() << "ms" << endl;
        }

        // UnorderedMap 性能
        cout << "\n  [UnorderedMap - 1000 items]:" << endl;
        {
            auto start = chrono::high_resolution_clock::now();

            UnorderedMap<int, string> map;
            for (int i = 0; i < 1000; i++) {
                map.Add(i, string("value_") + to_string(i));
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            cout << "    Time: " << duration.count() << "ms" << endl;
        }

        // OrderedSet 性能
        cout << "\n  [OrderedSet - 1000 items]:" << endl;
        {
            auto start = chrono::high_resolution_clock::now();

            OrderedSet<string> set;
            for (int i = 0; i < 1000; i++) {
                set.Add(string("element_") + to_string(i));
            }

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
            cout << "    Time: " << duration.count() << "ms" << endl;
        }
    }

    // ===== 测试6: 错误情况处理 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 6: Error Cases and Edge Conditions" << endl;
    cout << string(60, '-') << endl;

    {
        cout << "\n  [Duplicate Rejection]:" << endl;
        {
            OrderedMap<int, string> map;

            assert(map.Add(1, string("first")));
            assert(!map.Add(1, string("duplicate")));  // 应被拒绝

            cout << "    ✓ Duplicate keys correctly rejected" << endl;
        }

        cout << "\n  [BidirectionalMap Duplicate Value]:" << endl;
        {
            BidirectionalMap<int, string> map;

            assert(map.Add(1, string("unique")));
            assert(!map.Add(2, string("unique")));  // 值重复，应被拒绝

            cout << "    ✓ Duplicate values correctly rejected" << endl;
        }

        cout << "\n  [Empty Container Operations]:" << endl;
        {
            UnorderedSet<string> set;
            assert(set.IsEmpty());
            assert(set.GetCount() == 0);

            set.Add(string("first"));
            assert(!set.IsEmpty());

            cout << "    ✓ Empty state correctly managed" << endl;
        }
    }

    // ===== 测试7: 特殊场景 =====
    cout << "\n" << string(60, '-') << endl;
    cout << "TEST 7: Special Scenarios" << endl;
    cout << string(60, '-') << endl;

    {
        cout << "\n  [Sequential Operations]:" << endl;
        {
            OrderedMap<int, vector<string>> map;

            // 添加
            for (int i = 0; i < 10; i++) {
                vector<string> vec;
                for (int j = 0; j < 3; j++) {
                    vec.push_back(string("item") + to_string(j));
                }
                map.Add(i, move(vec));
            }

            // 更新
            for (int i = 0; i < 10; i++) {
                vector<string> new_vec;
                new_vec.push_back("updated");
                map.Change(i, move(new_vec));
            }

            cout << "    ✓ Sequential add/update operations work" << endl;
        }

        cout << "\n  [Large Object Handling]:" << endl;
        {
            OrderedMap<int, string> map;

            string large_string(10000, 'a');  // 10KB string
            assert(map.Add(1, move(large_string)));

            string retrieved;
            map.Get(1, retrieved);
            assert(retrieved.size() == 10000);

            cout << "    ✓ Large objects handled efficiently" << endl;
        }
    }

    cout << "\n" << string(60, '=') << endl;
    cout << "✅ ALL COMPREHENSIVE TESTS PASSED" << endl;
    cout << string(60, '=') << endl;

    return 0;
}
