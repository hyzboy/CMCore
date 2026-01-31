#include<hgl/type/OrderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 04: OrderedSet<T> Batch Operations" << endl;
    cout << "========================================" << endl;

    OrderedSet<int> set;

    cout << "\n[4.1] Batch Add without duplicates:" << endl;
    int values1[] = {40, 10, 30, 20};
    int64 inserted1 = set.Add(values1, 4);
    assert(inserted1 == 4);
    assert(set.GetCount() == 4);

    int v;
    assert(set.Get(0, v) && v == 10);
    assert(set.Get(1, v) && v == 20);
    assert(set.Get(2, v) && v == 30);
    assert(set.Get(3, v) && v == 40);
    cout << "  ✓ Batch insert kept order: [10,20,30,40]" << endl;

    cout << "\n[4.2] Batch Add with duplicates (should skip duplicates):" << endl;
    int values2[] = {15, 20, 25};
    int64 inserted2 = set.Add(values2, 3);
    cout << "  Inserted count returned: " << inserted2 << endl;
    cout << "  Expected: 2 (15 and 25, since 20 already exists)" << endl;
    
    assert(set.Contains(15));
    assert(set.Contains(20));
    assert(set.Contains(25));
    assert(inserted2 == 2);
    cout << "  ✓ Batch add correctly skipped duplicates" << endl;

    cout << "\n[4.3] Batch Delete mixed existing/missing:" << endl;
    int dels[] = {15, 40, 999};
    int64 deleted = set.Delete(dels, 3);
    assert(deleted == 2);  // 15 and 40 should be removed
    assert(!set.Contains(15));
    assert(!set.Contains(40));
    cout << "  ✓ Batch delete removed existing elements and ignored missing" << endl;

    cout << "\n[4.4] Final order check:" << endl;
    for (int i = 0; i < set.GetCount(); ++i)
    {
        int val;
        assert(set.Get(i, val));
        if (i > 0)
        {
            int prev;
            assert(set.Get(i - 1, prev));
            assert(prev < val);
        }
    }
    cout << "  ✓ Set remains sorted after mixed operations" << endl;

    cout << "\n[4.5] Edge case - Add from null pointer:" << endl;
    int64 added_null = set.Add(nullptr, 10);
    assert(added_null == 0);
    cout << "  ✓ Null pointer handled gracefully" << endl;

    cout << "\n[4.6] Edge case - Add zero count:" << endl;
    int dummy[] = {100, 200};
    int64 added_zero = set.Add(dummy, 0);
    assert(added_zero == 0);
    cout << "  ✓ Zero count handled gracefully" << endl;

    cout << "\n✅ TEST 04 PASSED" << endl;
    return 0;
}
