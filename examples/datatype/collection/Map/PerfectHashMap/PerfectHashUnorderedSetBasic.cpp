/**
 * @file PerfectHashUnorderedSetBasic.cpp
 * @brief 测试 FlatPerfectHashUnorderedSet 基本功能
 */
#include <hgl/type/FlatPerfectHashUnorderedSet.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================================" << endl;
    cout << "TEST: FlatPerfectHashUnorderedSet Basic Operations" << endl;
    cout << "========================================================" << endl;

    FlatPerfectHashUnorderedSet<int> set;

    // Test 1: Empty set
    cout << "\n[1] Empty set checks:" << endl;
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    cout << "  ✓ Empty set verified" << endl;

    // Test 2: Add operations
    cout << "\n[2] Add operations:" << endl;
    assert(set.Add(5) >= 0);
    assert(set.Add(1) >= 0);
    assert(set.Add(3) >= 0);
    assert(set.GetCount() == 3);
    cout << "  ✓ Added 3 items" << endl;

    // Test 3: O(1) lookup
    cout << "\n[3] O(1) hash lookup:" << endl;
    assert(set.Contains(1));
    assert(set.Contains(3));
    assert(set.Contains(5));
    assert(!set.Contains(10));
    cout << "  ✓ Hash-based lookups work" << endl;

    // Test 4: Duplicate rejection
    cout << "\n[4] Duplicate rejection:" << endl;
    assert(set.Add(3) == -1);
    assert(set.GetCount() == 3);
    cout << "  ✓ Duplicates rejected" << endl;

    // Test 5: Find index
    cout << "\n[5] Find index:" << endl;
    int64 idx = set.Find(3);
    assert(idx >= 0 && idx < 3);
    cout << "  ✓ Find returns valid index" << endl;

    // Test 6: Delete (swap-and-pop)
    cout << "\n[6] Delete operations:" << endl;
    assert(set.Delete(3));
    assert(!set.Contains(3));
    assert(set.GetCount() == 2);
    cout << "  ✓ Delete works (swap-and-pop)" << endl;

    // Test 7: Iteration
    cout << "\n[7] Iteration:" << endl;
    int count = 0;
    for (auto val : set)
    {
        count++;
        cout << "  Value: " << val << endl;
    }
    assert(count == 2);
    cout << "  ✓ Iteration works" << endl;

    // Test 8: Clear
    cout << "\n[8] Clear:" << endl;
    set.Clear();
    assert(set.IsEmpty());
    cout << "  ✓ Clear empties the set" << endl;

    // Test 9: Large dataset
    cout << "\n[9] Large dataset (1000 elements):" << endl;
    for (int i = 0; i < 1000; ++i)
    {
        assert(set.Add(i) >= 0);
    }
    assert(set.GetCount() == 1000);
    
    // Verify O(1) lookups
    for (int i = 0; i < 1000; i += 100)
    {
        assert(set.Contains(i));
    }
    cout << "  ✓ Large dataset with O(1) lookups" << endl;

    cout << "\n✅ ALL TESTS PASSED" << endl;
    return 0;
}
