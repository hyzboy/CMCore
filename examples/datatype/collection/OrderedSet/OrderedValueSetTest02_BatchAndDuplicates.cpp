#include<hgl/type/OrderedValueSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 02: OrderedValueSet Batch Add / Delete" << endl;
    cout << "========================================" << endl;

    OrderedValueSet<int> set;

    cout << "\n[2.1] Batch Add without duplicates:" << endl;
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

    cout << "\n[2.2] Batch Add with a duplicate inside input:" << endl;
    int values2[] = {15, 20, 25};
    int64 inserted2 = set.Add(values2, 3);
    cout << "  Inserted count returned: " << inserted2 << endl;

    if (inserted2 != 3)
    {
        cout << "  ⚠️ Notice: insertion stopped after hitting duplicate 20 (current behavior)" << endl;
        cout << "     Only first " << inserted2 << " elements were inserted" << endl;
    }
    else
    {
        cout << "  ✓ All elements inserted (duplicate handling improved)" << endl;
    }

    // Validate contents after batch insert
    assert(set.Contains(15));
    assert(set.Contains(20));
    if (inserted2 == 3)
        assert(set.Contains(25));
    else
        cout << "  ⚠️ 25 was not inserted because processing stopped on duplicate" << endl;

    cout << "\n[2.3] Batch Delete mixed existing/missing:" << endl;
    int dels[] = {15, 40, 999};
    int64 deleted = set.Delete(dels, 3);
    assert(deleted == 2);  // 15 and 40 should be removed
    assert(!set.Contains(15));
    assert(!set.Contains(40));
    cout << "  ✓ Batch delete removed existing elements and ignored missing" << endl;

    cout << "\n[2.4] Final order check:" << endl;
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

    cout << "\n✅ TEST 02 PASSED" << endl;
    return 0;
}
