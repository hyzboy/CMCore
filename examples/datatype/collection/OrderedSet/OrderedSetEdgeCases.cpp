#include<hgl/type/OrderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 08: OrderedSet<T> Edge Cases & Errors" << endl;
    cout << "========================================" << endl;

    OrderedSet<int> set;

    cout << "\n[8.1] Operations on empty set:" << endl;
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    int dummy;
    assert(!set.GetFirst(dummy));
    assert(!set.GetLast(dummy));
    cout << "  ✓ GetFirst/GetLast return false for empty set" << endl;

    cout << "\n[8.2] Get with invalid index:" << endl;
    assert(!set.Get(-1, dummy));
    assert(!set.Get(0, dummy));
    assert(!set.Get(100, dummy));
    cout << "  ✓ Invalid indices handled safely" << endl;

    cout << "\n[8.3] DeleteAt with invalid index:" << endl;
    assert(!set.DeleteAt(-1));
    assert(!set.DeleteAt(0));
    assert(!set.DeleteAt(100));
    cout << "  ✓ Invalid deleteAt indices handled safely" << endl;

    cout << "\n[8.4] Delete from empty set:" << endl;
    assert(!set.Delete(42));
    assert(set.GetCount() == 0);
    cout << "  ✓ Delete from empty set is safe" << endl;

    cout << "\n[8.5] Single element set:" << endl;
    set.Add(42);
    assert(!set.IsEmpty());
    assert(set.GetCount() == 1);
    assert(set.GetFirst(dummy) && dummy == 42);
    assert(set.GetLast(dummy) && dummy == 42);
    cout << "  ✓ Single element: GetFirst == GetLast" << endl;

    cout << "\n[8.6] Delete last remaining element:" << endl;
    assert(set.Delete(42));
    assert(set.IsEmpty());
    cout << "  ✓ Set empty after deleting only element" << endl;

    cout << "\n[8.7] Negative numbers:" << endl;
    set.Add(-100);
    set.Add(0);
    set.Add(-50);
    set.Add(100);
    assert(set.GetCount() == 4);
    assert(set.GetFirst(dummy) && dummy == -100);
    assert(set.GetLast(dummy) && dummy == 100);
    cout << "  ✓ Negative numbers handled correctly" << endl;

    cout << "\n[8.8] Large numbers:" << endl;
    set.Clear();
    set.Add(2147483647);  // INT_MAX
    set.Add(-2147483648); // INT_MIN (approximately)
    set.Add(0);
    assert(set.GetCount() == 3);
    cout << "  ✓ Large numbers (near INT_MAX) work" << endl;

    cout << "\n[8.9] Clear after operations:" << endl;
    set.Clear();
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    cout << "  ✓ Clear resets the set" << endl;

    cout << "\n[8.10] Free clears the set:" << endl;
    set.Add(1);
    set.Add(2);
    set.Add(3);
    assert(set.GetCount() == 3);
    set.Free();
    assert(set.GetCount() == 0);
    assert(set.IsEmpty());
    cout << "  ✓ Free empties the set" << endl;

    cout << "\n[8.11] FindIndex for missing and existing elements:" << endl;
    set.Clear();
    set.Add(10);
    set.Add(20);
    set.Add(30);
    assert(set.FindIndex(10) == 0);
    assert(set.FindIndex(20) == 1);
    assert(set.FindIndex(30) == 2);
    assert(set.FindIndex(15) == -1);
    assert(set.FindIndex(5) == -1);
    assert(set.FindIndex(40) == -1);
    cout << "  ✓ FindIndex returns correct positions" << endl;

    cout << "\n[8.12] Boundary: Add same value multiple times:" << endl;
    set.Clear();
    set.Add(5);
    int64 result2 = set.Add(5);
    int64 result3 = set.Add(5);
    assert(result2 == -1 && result3 == -1);
    assert(set.GetCount() == 1);
    cout << "  ✓ Duplicate adds correctly rejected" << endl;

    cout << "\n[8.13] DeleteAt boundary conditions:" << endl;
    set.Clear();
    set.Add(100);
    set.Add(200);
    set.Add(300);

    // Delete middle
    assert(set.DeleteAt(1));
    assert(set.GetCount() == 2);
    assert(set.Get(0, dummy) && dummy == 100);
    assert(set.Get(1, dummy) && dummy == 300);
    cout << "  ✓ DeleteAt(1) removed middle element" << endl;

    // Delete first
    assert(set.DeleteAt(0));
    assert(set.GetCount() == 1);
    assert(set.Get(0, dummy) && dummy == 300);
    cout << "  ✓ DeleteAt(0) removed first element" << endl;

    // Delete last
    assert(set.DeleteAt(0));
    assert(set.IsEmpty());
    cout << "  ✓ DeleteAt(0) on last element succeeded" << endl;

    cout << "\n[8.14] Batch operation with all duplicates:" << endl;
    set.Clear();
    set.Add(10);
    int values[] = {10, 10, 10};
    int64 added = set.Add(values, 3);
    assert(added == 0);  // No new elements added
    assert(set.GetCount() == 1);
    cout << "  ✓ Batch add with all duplicates skipped correctly" << endl;

    cout << "\n[8.15] Iterator on empty set:" << endl;
    set.Clear();
    assert(set.begin() == set.end());
    int count = 0;
    for (auto it = set.begin(); it != set.end(); ++it)
        ++count;
    assert(count == 0);
    cout << "  ✓ Empty set iterators work safely" << endl;

    cout << "\n✅ TEST 08 PASSED" << endl;
    return 0;
}
