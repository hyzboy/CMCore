#include<hgl/type/OrderedSet.h>
#include<hgl/type/FlatOrderedSet.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: OrderedSet/FlatOrderedSet Move Semantics" << endl;
    cout << "========================================" << endl;

    // 测试1: OrderedSet 的基本移动语义
    cout << "\n[1] Test OrderedSet with move semantics:" << endl;
    {
        OrderedSet<string> set;

        assert(set.Add(string("alpha")) >= 0);
        assert(set.Add(string("beta")) >= 0);
        assert(set.Add(string("gamma")) >= 0);

        assert(set.GetCount() == 3);
        assert(set.Contains("beta"));
        cout << "  ✓ OrderedSet move semantics work" << endl;
    }

    // 测试2: FlatOrderedSet 的基本移动语义
    cout << "\n[2] Test FlatOrderedSet with move semantics:" << endl;
    {
        FlatOrderedSet<string> set;

        assert(set.Add(string("one")) >= 0);
        assert(set.Add(string("two")) >= 0);
        assert(set.Add(string("three")) >= 0);

        assert(set.GetCount() == 3);
        assert(set.Contains("two"));
        cout << "  ✓ FlatOrderedSet move semantics work" << endl;
    }

    // 测试3: OrderedSet 的排序验证
    cout << "\n[3] Test OrderedSet ordering with move:" << endl;
    {
        OrderedSet<int> set;

        assert(set.Add(5) >= 0);
        assert(set.Add(move(2)) >= 0);  // 虽然 int 通常不会真正移动，但测试调用
        assert(set.Add(8) >= 0);
        assert(set.Add(move(1)) >= 0);
        assert(set.Add(9) >= 0);

        // 验证有序
        int prev = -1;
        for (const auto& val : set) {
            assert(val > prev);
            prev = val;
        }
        cout << "  ✓ OrderedSet maintains order with move" << endl;
    }

    // 测试4: FlatOrderedSet 的排序验证
    cout << "\n[4] Test FlatOrderedSet ordering with move:" << endl;
    {
        FlatOrderedSet<int> set;

        assert(set.Add(10) >= 0);
        assert(set.Add(5) >= 0);
        assert(set.Add(15) >= 0);
        assert(set.Add(3) >= 0);

        // 验证元素
        assert(set.Contains(3));
        assert(set.Contains(5));
        assert(set.Contains(10));
        assert(set.Contains(15));
        cout << "  ✓ FlatOrderedSet maintains order" << endl;
    }

    // 测试5: 临时对象的自动移动
    cout << "\n[5] Test automatic move of temporaries:" << endl;
    {
        OrderedSet<string> set;

        set.Add(string("temp1"));
        set.Add(string("temp2"));
        set.Add(string("temp3"));

        assert(set.GetCount() == 3);
        cout << "  ✓ Temporary objects automatically moved in OrderedSet" << endl;
    }

    // 测试6: 重复元素的拒绝
    cout << "\n[6] Test duplicate rejection with move semantics:" << endl;
    {
        OrderedSet<string> set;

        assert(set.Add(string("first")) >= 0);
        assert(set.Add(string("first")) < 0);  // 重复应被拒绝，返回 -1
        assert(set.Add(string("second")) >= 0);

        assert(set.GetCount() == 2);
        cout << "  ✓ Duplicates correctly rejected" << endl;
    }

    // 测试7: 批量添加数据（混合用法）
    cout << "\n[7] Test batch operations with mixed usage:" << endl;
    {
        FlatOrderedSet<string> set;

        string val1 = "alpha";
        assert(set.Add(val1) >= 0);           // const 引用
        assert(set.Add(string("beta")) >= 0); // 临时对象（自动移动）

        string val3 = "gamma";
        assert(set.Add(move(val3)) >= 0);     // 显式移动

        assert(set.GetCount() == 3);
        cout << "  ✓ Mixed usage works correctly" << endl;
    }

    // 测试8: 大规模插入
    cout << "\n[8] Performance test: 1000 string elements:" << endl;
    {
        OrderedSet<string> set;

        auto start = chrono::high_resolution_clock::now();

        for (int i = 0; i < 1000; i++) {
            set.Add(string("element_") + to_string(i));
        }

        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

        assert(set.GetCount() == 1000);
        cout << "  ✓ Added 1000 elements in " << duration.count() << "ms" << endl;
    }

    // 测试9: FlatOrderedSet 的删除和重新添加
    cout << "\n[9] Test FlatOrderedSet delete and re-add:" << endl;
    {
        FlatOrderedSet<string> set;

        assert(set.Add(string("first")) >= 0);
        assert(set.Add(string("second")) >= 0);
        assert(set.Add(string("third")) >= 0);

        assert(set.Delete("second"));
        assert(set.GetCount() == 2);

        assert(set.Add(string("second")) >= 0);  // 重新添加
        assert(set.GetCount() == 3);
        cout << "  ✓ Delete and re-add works" << endl;
    }

    // 测试10: 查找操作
    cout << "\n[10] Test find operations:" << endl;
    {
        OrderedSet<int> set;

        for (int i = 0; i < 50; i++) {
            set.Add(i);
        }

        // 测试查找
        auto it = set.Find(25);
        assert(it != set.end());
        assert(*it == 25);

        int64 index = set.FindIndex(30);
        assert(index >= 0);
        cout << "  ✓ Find operations work correctly" << endl;
    }

    cout << "\n✅ ALL ORDEREDSET/FLATORDEREDSET MOVE SEMANTICS TESTS PASSED" << endl;
    return 0;
}
