#include<hgl/type/FlatOrderedValueSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 03: FlatOrderedValueSet Comparison & Copy" << endl;
    cout << "========================================" << endl;

    cout << "\n[3.1] Equality for identical sets:" << endl;
    FlatOrderedValueSet<int> a, b;
    int base[] = {1, 3, 5, 7};
    a.Add(base, 4);
    b.Add(base, 4);
    assert(a == b);
    assert(!(a != b));
    cout << "  ✓ a == b" << endl;

    cout << "\n[3.2] Inequality when lengths differ:" << endl;
    b.Delete(7);
    assert(a != b);
    assert(!(a == b));
    cout << "  ✓ a != b when one element removed" << endl;

    cout << "\n[3.3] Inequality with same length but different content:" << endl;
    b.Add(9);
    assert(a != b);
    cout << "  ✓ a != b with different tail element" << endl;

    cout << "\n[3.4] Copy assignment and self-assignment:" << endl;
    FlatOrderedValueSet<int> c;
    c = a;
    assert(c == a);
    cout << "  ✓ Copy assignment produced equal set" << endl;

    c = c;  // self assignment should be harmless
    assert(c == a);
    cout << "  ✓ Self assignment keeps content" << endl;

    cout << "\n[3.5] Mutation after copy keeps originals independent:" << endl;
    c.Delete(5);
    assert(c != a);
    assert(a.Contains(5));
    cout << "  ✓ Copies are independent after mutation" << endl;

    cout << "\n✅ TEST 03 PASSED" << endl;
    return 0;
}
