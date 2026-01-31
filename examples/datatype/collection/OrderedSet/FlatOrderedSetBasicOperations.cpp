#include<hgl/type/FlatOrderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 01: FlatOrderedValueSet Basic Operations" << endl;
    cout << "========================================" << endl;

    FlatOrderedSet<int> set;
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);

    cout << "\n[1.1] Add unsorted elements (should keep sorted and unique):" << endl;
    int64 p1 = set.Add(5);
    int64 p2 = set.Add(1);
    int64 p3 = set.Add(3);
    cout << "  positions: " << p1 << ", " << p2 << ", " << p3 << endl;
    assert(set.GetCount() == 3);

    int v0, v1, v2;
    assert(set.Get(0, v0));
    assert(set.Get(1, v1));
    assert(set.Get(2, v2));
    assert(v0 == 1 && v1 == 3 && v2 == 5);
    cout << "  ✓ Sorted order is [1,3,5]" << endl;

    cout << "\n[1.2] Duplicate insert should be rejected:" << endl;
    int64 dup_pos = set.Add(3);
    assert(dup_pos == -1);
    assert(set.GetCount() == 3);
    cout << "  ✓ Duplicate not inserted" << endl;

    cout << "\n[1.3] Contains / Find:" << endl;
    assert(set.Contains(1));
    assert(set.Contains(3));
    assert(!set.Contains(4));
    assert(set.FindIndex(1) == 0);
    assert(set.FindIndex(5) == 2);
    assert(set.FindIndex(4) == -1);
    cout << "  ✓ Contains/Find behave correctly" << endl;

    cout << "\n[1.4] GetFirst / GetLast:" << endl;
    int first = 0, last = 0;
    assert(set.GetFirst(first) && first == 1);
    assert(set.GetLast(last) && last == 5);
    cout << "  ✓ GetFirst=1, GetLast=5" << endl;

    cout << "\n[1.5] Delete existing and missing elements:" << endl;
    assert(set.Delete(3));
    assert(!set.Contains(3));
    assert(!set.Delete(42));
    cout << "  ✓ Delete works and ignores missing values" << endl;

    cout << "\n[1.6] DeleteAt by index:" << endl;
    assert(set.DeleteAt(0));  // remove 1
    assert(set.GetCount() == 1);
    assert(set.GetFirst(first) && first == 5);
    cout << "  ✓ DeleteAt removed by position" << endl;

    cout << "\n[1.7] Clear vs Free (capacity behavior):" << endl;
    set.Clear();
    assert(set.GetCount() == 0);
    int64 alloc_after_clear = set.GetAllocCount();
    cout << "  Capacity after Clear(): " << alloc_after_clear << endl;

    set.Reserve(10);
    int64 alloc_reserved = set.GetAllocCount();
    assert(alloc_reserved >= 10);
    cout << "  Reserved capacity: " << alloc_reserved << endl;

    set.Free();
    assert(set.GetCount() == 0);
    int64 alloc_after_free = set.GetAllocCount();
    if (alloc_after_free != 0)
        cout << "  ⚠️ Free() did not release capacity (alloc=" << alloc_after_free << ")" << endl;
    else
        cout << "  ✓ Free() released capacity" << endl;

    cout << "\n✅ TEST 01 PASSED" << endl;
    return 0;
}
