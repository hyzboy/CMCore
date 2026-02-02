#include<hgl/type/OrderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 03: OrderedSet<T> Basic Operations" << endl;
    cout << "========================================" << endl;

    OrderedSet<int> set;
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);

    cout << "\n[3.1] Add unsorted elements (should keep sorted and unique):" << endl;
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

    cout << "\n[3.2] Duplicate insert should be rejected:" << endl;
    int64 dup_pos = set.Add(3);
    assert(dup_pos == -1);
    assert(set.GetCount() == 3);
    cout << "  ✓ Duplicate not inserted" << endl;

    cout << "\n[3.3] Contains / Find:" << endl;
    assert(set.Contains(1));
    assert(set.Contains(3));
    assert(!set.Contains(4));
    assert(set.FindIndex(1) == 0);
    assert(set.FindIndex(5) == 2);
    assert(set.FindIndex(4) == -1);
    cout << "  ✓ Contains/Find behave correctly" << endl;

    cout << "\n[3.4] GetFirst / GetLast:" << endl;
    int first = 0, last = 0;
    assert(set.GetFirst(first) && first == 1);
    assert(set.GetLast(last) && last == 5);
    cout << "  ✓ GetFirst=1, GetLast=5" << endl;

    cout << "\n[3.5] Delete existing and missing elements:" << endl;
    assert(set.Delete(3));
    assert(!set.Contains(3));
    assert(!set.Delete(42));
    cout << "  ✓ Delete works and ignores missing values" << endl;

    cout << "\n[3.6] DeleteAt by index:" << endl;
    assert(set.DeleteAt(0));  // remove 1
    assert(set.GetCount() == 1);
    assert(set.GetFirst(first) && first == 5);
    cout << "  ✓ DeleteAt removed by position" << endl;

    cout << "\n[3.7] Clear and Free:" << endl;
    set.Clear();
    assert(set.GetCount() == 0);
    cout << "  ✓ Clear() empties the set" << endl;

    cout << "\n✅ TEST 03 PASSED" << endl;
    return 0;
}
