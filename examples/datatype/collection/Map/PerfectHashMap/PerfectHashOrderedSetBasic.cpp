/**
 * @file PerfectHashOrderedSetBasic.cpp
 * @brief 测试 FlatPerfectHashOrderedSet 基本功能
 */
#include <hgl/type/FlatPerfectHashOrderedSet.h>
#include <hgl/platform/Platform.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================================" << endl;
    cout << "TEST: FlatPerfectHashOrderedSet Basic Operations" << endl;
    cout << "========================================================" << endl;

    FlatPerfectHashOrderedSet<int> set;

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
    assert(set.Add(2) >= 0);
    assert(set.Add(4) >= 0);
    assert(set.GetCount() == 5);
    cout << "  ✓ Added 5 items" << endl;

    // Test 3: Verify sorting
    cout << "\n[3] Verify sorting:" << endl;
    assert(set[0] == 1);
    assert(set[1] == 2);
    assert(set[2] == 3);
    assert(set[3] == 4);
    assert(set[4] == 5);
    cout << "  ✓ Elements sorted: [1, 2, 3, 4, 5]" << endl;

    // Test 4: O(1) lookup
    cout << "\n[4] O(1) hash lookup:" << endl;
    assert(set.Contains(1));
    assert(set.Contains(3));
    assert(set.Contains(5));
    assert(!set.Contains(10));
    cout << "  ✓ Hash-based lookups work" << endl;

    // Test 5: Duplicate rejection
    cout << "\n[5] Duplicate rejection:" << endl;
    assert(set.Add(3) == -1);  // Already exists
    assert(set.GetCount() == 5);
    cout << "  ✓ Duplicates rejected" << endl;

    // Test 6: Find index
    cout << "\n[6] Find index:" << endl;
    assert(set.Find(1) == 0);
    assert(set.Find(3) == 2);
    assert(set.Find(5) == 4);
    assert(set.Find(99) == -1);
    cout << "  ✓ Find returns correct indices" << endl;

    // Test 7: Delete operations
    cout << "\n[7] Delete operations:" << endl;
    assert(set.Delete(3));
    assert(!set.Contains(3));
    assert(set.GetCount() == 4);
    assert(!set.Delete(99));
    cout << "  ✓ Delete works correctly" << endl;

    // Test 8: Iteration
    cout << "\n[8] Iteration:" << endl;
    int count = 0;
    for (auto val : set)
    {
        count++;
        cout << "  Value: " << val << endl;
    }
    assert(count == 4);
    cout << "  ✓ Iteration works (visited " << count << " items)" << endl;

    // Test 9: Range queries
    cout << "\n[9] Range queries:" << endl;
    auto it_lb = set.lower_bound(2);
    auto it_ub = set.upper_bound(4);
    assert(it_lb != set.end() && *it_lb == 2);
    assert(it_ub != set.end() && *it_ub == 5);
    cout << "  ✓ Range queries work" << endl;

    // Test 10: Clear
    cout << "\n[10] Clear:" << endl;
    set.Clear();
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    cout << "  ✓ Clear empties the set" << endl;

    // Test 11: Large dataset
    cout << "\n[11] Large dataset (1000 elements):" << endl;
    for (int i = 0; i < 1000; ++i)
    {
        assert(set.Add(i) >= 0);
    }
    assert(set.GetCount() == 1000);
    
    // Verify lookups
    for (int i = 0; i < 1000; i += 100)
    {
        assert(set.Contains(i));
    }
    cout << "  ✓ Large dataset handled with O(1) lookups" << endl;

    cout << "\n✅ ALL TESTS PASSED" << endl;
    return 0;
}
