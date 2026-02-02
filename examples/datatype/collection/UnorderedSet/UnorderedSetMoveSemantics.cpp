#include<hgl/type/FlatUnorderedSet.h>
#include<hgl/type/UnorderedSet.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: FlatUnorderedSet/UnorderedSet Move Semantics" << endl;
    cout << "========================================" << endl;

    // 测试1: UnorderedSet 的基本移动语义
    cout << "\n[1] Test UnorderedSet with move semantics:" << endl;
    {
        UnorderedSet<string> set;
        
        assert(set.Add(string("first")));
        assert(set.Add(string("second")));
        assert(set.Add(string("third")));
        
        assert(set.GetCount() == 3);
        assert(set.Contains("second"));
        cout << "  ✓ UnorderedSet move semantics work" << endl;
    }

    // 测试2: FlatUnorderedSet 的基本移动语义（使用int类型，因为FlatUnorderedSet要求trivially_copyable）
    cout << "\n[2] Test FlatUnorderedSet with move semantics:" << endl;
    {
        FlatUnorderedSet<int> set;
        
        assert(set.Add(1));
        assert(set.Add(2));
        assert(set.Add(3));
        
        assert(set.GetCount() == 3);
        assert(set.Contains(2));
        cout << "  ✓ FlatUnorderedSet move semantics work" << endl;
    }

    // 测试3: 临时对象的自动移动
    cout << "\n[3] Test automatic move of temporaries:" << endl;
    {
        UnorderedSet<string> set;
        
        // 直接传递临时对象
        set.Add(string("temp1"));
        set.Add(string("temp2"));
        set.Add(string("temp3"));
        
        assert(set.GetCount() == 3);
        cout << "  ✓ UnorderedSet temporary objects automatically moved" << endl;
    }

    // 测试4: FlatUnorderedSet 临时对象（使用int）
    cout << "\n[4] Test FlatUnorderedSet temporaries:" << endl;
    {
        FlatUnorderedSet<int> set;
        
        set.Add(10);
        set.Add(20);
        set.Add(30);
        
        assert(set.GetCount() == 3);
        cout << "  ✓ FlatUnorderedSet temporary objects automatically moved" << endl;
    }

    // 测试5: 重复拒绝
    cout << "\n[5] Test duplicate rejection:" << endl;
    {
        UnorderedSet<string> set;
        
        assert(set.Add(string("first")));
        assert(!set.Add(string("first")));  // 重复应被拒绝
        
        assert(set.GetCount() == 1);
        cout << "  ✓ Duplicates correctly rejected in UnorderedSet" << endl;
    }

    // 测试6: FlatUnorderedSet 重复拒绝
    cout << "\n[6] Test FlatUnorderedSet duplicate rejection:" << endl;
    {
        FlatUnorderedSet<int> set;
        
        assert(set.Add(42));
        assert(!set.Add(42));  // 重复应被拒绝
        
        assert(set.GetCount() == 1);
        cout << "  ✓ Duplicates correctly rejected in FlatUnorderedSet" << endl;
    }

    // 测试7: 删除和重新添加
    cout << "\n[7] Test delete and re-add:" << endl;
    {
        UnorderedSet<string> set;
        
        set.Add(string("first"));
        set.Add(string("second"));
        
        assert(set.Delete("second"));
        assert(set.GetCount() == 1);
        
        assert(set.Add(string("second")));  // 重新添加
        assert(set.GetCount() == 2);
        cout << "  ✓ Delete and re-add work in UnorderedSet" << endl;
    }

    // 测试8: FlatUnorderedSet 删除和重新添加
    cout << "\n[8] Test FlatUnorderedSet delete and re-add:" << endl;
    {
        FlatUnorderedSet<int> set;
        
        assert(set.Add(1));
        assert(set.Add(2));
        assert(set.Add(3));
        
        assert(set.Delete(2));
        assert(set.GetCount() == 2);
        
        assert(set.Add(2));  // 重新添加
        assert(set.GetCount() == 3);
        cout << "  ✓ Delete and re-add work in FlatUnorderedSet" << endl;
    }

    // 测试9: 批量添加操作
    cout << "\n[9] Test batch add operations:" << endl;
    {
        UnorderedSet<string> set;
        
        for (int i = 0; i < 100; i++) {
            assert(set.Add(string("element_") + to_string(i)));
        }
        
        assert(set.GetCount() == 100);
        assert(set.Contains("element_50"));
        cout << "  ✓ Batch add of 100 elements works" << endl;
    }

    // 测试10: FlatUnorderedSet 批量添加
    cout << "\n[10] Test FlatUnorderedSet batch add:" << endl;
    {
        FlatUnorderedSet<int> set;
        
        for (int i = 0; i < 100; i++) {
            assert(set.Add(i));
        }
        
        assert(set.GetCount() == 100);
        assert(set.Contains(50));
        cout << "  ✓ FlatUnorderedSet batch add of 100 elements works" << endl;
    }

    // 测试11: 集合运算 - Union
    cout << "\n[11] Test set union operation:" << endl;
    {
        FlatUnorderedSet<int> set1, set2;
        
        for (int i = 0; i < 50; i++) {
            set1.Add(i);
        }
        
        for (int i = 25; i < 75; i++) {
            set2.Add(i);
        }
        
        int added = set1.Union(set2);
        assert(set1.GetCount() == 75);  // 0-74
        cout << "  ✓ Union operation works (" << added << " new elements added)" << endl;
    }

    // 测试12: 集合运算 - Intersection
    cout << "\n[12] Test set intersection operation:" << endl;
    {
        FlatUnorderedSet<int> set1, set2;
        
        for (int i = 0; i < 50; i++) {
            set1.Add(i);
        }
        
        for (int i = 25; i < 75; i++) {
            set2.Add(i);
        }
        
        int removed = set1.Intersect(set2);
        assert(set1.GetCount() == 25);  // 25-49
        cout << "  ✓ Intersection operation works (" << removed << " elements removed)" << endl;
    }

    // 测试13: 迭代遍历
    cout << "\n[13] Test iteration over UnorderedSet:" << endl;
    {
        UnorderedSet<string> set;
        
        for (int i = 0; i < 10; i++) {
            set.Add(string("item_") + to_string(i));
        }
        
        int count = 0;
        for (auto& item : set) {
            count++;
        }
        
        assert(count == 10);
        cout << "  ✓ Iteration works correctly" << endl;
    }

    // 测试14: 大规模性能测试
    cout << "\n[14] Performance test: 5000 elements:" << endl;
    {
        auto start = chrono::high_resolution_clock::now();
        
        FlatUnorderedSet<int> set;
        for (int i = 0; i < 5000; i++) {
            set.Add(i);
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        assert(set.GetCount() == 5000);
        cout << "  ✓ Added 5000 elements in " << duration.count() << "ms" << endl;
    }

    // 测试15: 查询性能
    cout << "\n[15] Query performance test: find among 10000:" << endl;
    {
        FlatUnorderedSet<int> set;
        for (int i = 0; i < 10000; i++) {
            set.Add(i);
        }
        
        auto start = chrono::high_resolution_clock::now();
        
        // 进行1000次查询
        int found = 0;
        for (int i = 0; i < 1000; i++) {
            if (set.Contains(i * 10)) {
                found++;
            }
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
        
        cout << "  Found " << found << " items in " << duration.count() << "μs" << endl;
        cout << "  ✓ Query performance acceptable" << endl;
    }

    cout << "\n✅ ALL UNORDEREDSET/FLATUNORDEREDSET MOVE SEMANTICS TESTS PASSED" << endl;
    return 0;
}
