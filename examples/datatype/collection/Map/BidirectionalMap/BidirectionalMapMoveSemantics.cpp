#include<hgl/platform/Platform.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Move Semantics" << endl;
    cout << "========================================" << endl;

    // 测试1: 基本移动语义
    cout << "\n[1] Test basic move semantics:" << endl;
    {
        BidirectionalMap<int, string> map;

        assert(map.Add(1, string("one")));
        assert(map.Add(2, string("two")));
        assert(map.Add(3, string("three")));

        string val;
        assert(map.Get(1, val) && val == "one");

        int key;
        assert(map.GetByValue("two", key) && key == 2);
        cout << "  ✓ Basic move semantics work" << endl;
    }

    // 测试2: 完全移动版本 (K&&, V&&)
    cout << "\n[2] Test full move semantics (K&&, V&&):" << endl;
    {
        BidirectionalMap<string, string> map;

        string k1 = "forward_key";
        string v1 = "forward_value";
        assert(map.Add(move(k1), move(v1)));

        string retrieved;
        assert(map.Get("forward_key", retrieved) && retrieved == "forward_value");
        cout << "  ✓ Full move semantics work" << endl;
    }

    // 测试3: 混合语义1 (K&&, const V&)
    cout << "\n[3] Test mixed semantics (K&&, const V&):" << endl;
    {
        BidirectionalMap<string, int> map;

        string key = "key1";
        int val = 100;
        assert(map.Add(move(key), val));

        string retrieved_key;
        assert(map.GetByValue(100, retrieved_key) && retrieved_key == "key1");
        cout << "  ✓ Mixed K&& const V& semantics work" << endl;
    }

    // 测试4: 混合语义2 (const K&, V&&)
    cout << "\n[4] Test mixed semantics (const K&, V&&):" << endl;
    {
        BidirectionalMap<int, string> map;

        int key = 42;
        assert(map.Add(key, string("value42")));

        string retrieved;
        assert(map.Get(42, retrieved) && retrieved == "value42");
        cout << "  ✓ Mixed const K& V&& semantics work" << endl;
    }

    // 测试5: 临时对象的自动移动
    cout << "\n[5] Test automatic move of temporaries:" << endl;
    {
        BidirectionalMap<int, string> map;

        assert(map.Add(1, string("temp1")));
        assert(map.Add(2, string("temp2")));
        assert(map.Add(3, string("temp3")));

        assert(map.GetCount() == 3);
        cout << "  ✓ Temporary objects automatically moved" << endl;
    }

    // 测试6: 双向查询的正确性
    cout << "\n[6] Test bidirectional query with move semantics:" << endl;
    {
        BidirectionalMap<string, string> map;

        assert(map.Add(string("key1"), string("value1")));
        assert(map.Add(string("key2"), string("value2")));

        // 正向查询
        string val;
        assert(map.Get("key1", val) && val == "value1");

        // 反向查询
        string key;
        assert(map.GetByValue("value2", key) && key == "key2");
        cout << "  ✓ Bidirectional queries work with move semantics" << endl;
    }

    // 测试7: 重复键/值的拒绝
    cout << "\n[7] Test duplicate key/value rejection:" << endl;
    {
        BidirectionalMap<int, string> map;

        assert(map.Add(1, string("one")));

        // 重复的键
        assert(!map.Add(1, string("ONE")));

        // 重复的值
        assert(!map.Add(2, string("one")));

        assert(map.GetCount() == 1);
        cout << "  ✓ Duplicates correctly rejected" << endl;
    }

    // 测试8: 大量元素的批量添加
    cout << "\n[8] Test batch add with move semantics:" << endl;
    {
        BidirectionalMap<int, string> map;

        for (int i = 0; i < 100; i++) {
            assert(map.Add(i, string("value_") + to_string(i)));
        }

        assert(map.GetCount() == 100);

        // 验证一些值
        string val;
        assert(map.Get(50, val) && val == "value_50");

        int key;
        assert(map.GetByValue("value_99", key) && key == 99);
        cout << "  ✓ Batch add with move semantics works" << endl;
    }

    // 测试9: 删除操作后的查询
    cout << "\n[9] Test delete operations with move semantics:" << endl;
    {
        BidirectionalMap<int, string> map;

        map.Add(1, string("one"));
        map.Add(2, string("two"));
        map.Add(3, string("three"));

        // 删除后验证
        assert(map.DeleteByKey(2));
        assert(map.GetCount() == 2);
        assert(!map.ContainsKey(2));

        // 添加新元素（使用移动语义）
        assert(map.Add(2, string("TWO")));
        assert(map.GetCount() == 3);
        cout << "  ✓ Delete and re-add work correctly" << endl;
    }

    // 测试10: 迭代器遍历
    cout << "\n[10] Test iteration with move-added elements:" << endl;
    {
        BidirectionalMap<string, int> map;

        // 使用移动语义添加
        for (int i = 0; i < 5; i++) {
            map.Add(string("key") + to_string(i), i * 10);
        }

        // 遍历验证
        int count = 0;
        for (auto [k, v] : map) {
            count++;
        }

        assert(count == 5);
        cout << "  ✓ Iteration over move-added elements works" << endl;
    }

    cout << "\n✅ ALL BIDIRECTIONALMAP MOVE SEMANTICS TESTS PASSED" << endl;
    return 0;
}
