#include<hgl/type/OrderedSet.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 06: OrderedSet<T> Comparison & Copy" << endl;
    cout << "========================================" << endl;

    OrderedSet<int> set1, set2, set3;

    cout << "\n[6.1] Equality comparison - empty sets:" << endl;
    assert(set1 == set2);
    assert(!(set1 != set2));
    cout << "  ✓ Empty sets are equal" << endl;

    cout << "\n[6.2] Populate set1:" << endl;
    int values1[] = {1, 2, 3, 4, 5};
    set1.Add(values1, 5);
    assert(set1.GetCount() == 5);
    cout << "  ✓ set1 = [1,2,3,4,5]" << endl;

    cout << "\n[6.3] Inequality - set1 vs empty set2:" << endl;
    assert(set1 != set2);
    assert(!(set1 == set2));
    cout << "  ✓ Non-empty and empty sets are not equal" << endl;

    cout << "\n[6.4] Assignment operator:" << endl;
    set2 = set1;
    assert(set2 == set1);
    assert(set2.GetCount() == 5);
    int v;
    for (int i = 1; i <= 5; ++i)
    {
        assert(set2.Contains(i));
    }
    cout << "  ✓ After assignment, set2 equals set1" << endl;

    cout << "\n[6.5] Assignment - modify copy without affecting original:" << endl;
    set2.Delete(3);
    assert(set2.GetCount() == 4);
    assert(set1.GetCount() == 5);
    assert(set1.Contains(3));
    assert(!set2.Contains(3));
    cout << "  ✓ Modifying set2 doesn't affect set1 (deep copy)" << endl;

    cout << "\n[6.6] Modify original after assignment copy:" << endl;
    set1.Add(6);
    assert(set1.GetCount() == 6);
    assert(set2.GetCount() == 4);
    assert(set1.Contains(6));
    assert(!set2.Contains(6));
    cout << "  ✓ Modifying set1 doesn't affect set2" << endl;

    cout << "\n[6.7] Test with different element sets:" << endl;
    set1.Clear();
    set2.Clear();
    
    int vals_a[] = {10, 20, 30};
    int vals_b[] = {10, 20, 30};
    int vals_c[] = {10, 20, 40};
    
    set1.Add(vals_a, 3);
    set2.Add(vals_b, 3);
    set3.Add(vals_c, 3);
    
    assert(set1 == set2);
    assert(set1 != set3);
    cout << "  ✓ Sets with same content are equal, different content are not equal" << endl;

    cout << "\n[6.8] Self-assignment:" << endl;
    set1 = set1;
    assert(set1.GetCount() == 3);
    assert(set1.Contains(10) && set1.Contains(20) && set1.Contains(30));
    cout << "  ✓ Self-assignment works correctly" << endl;

    cout << "\n✅ TEST 06 PASSED" << endl;
    return 0;
}
