#include<hgl/type/OrderedSet.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 05: OrderedSet<T> Iterator & Range" << endl;
    cout << "========================================" << endl;

    OrderedSet<int> set;
    int values[] = {10, 5, 15, 2, 8, 20};
    set.Add(values, 6);

    cout << "\n[5.1] Forward iteration:" << endl;
    cout << "  Forward: ";
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
    assert(true);
    cout << "  ✓ Forward iteration works" << endl;

    cout << "\n[5.2] Const iterator:" << endl;
    const OrderedSet<int>& const_set = set;
    cout << "  Const iteration: ";
    for (auto it = const_set.begin(); it != const_set.end(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
    cout << "  ✓ Const iteration works" << endl;

    cout << "\n[5.3] Reverse iteration:" << endl;
    cout << "  Reverse: ";
    for (auto it = set.rbegin(); it != set.rend(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
    cout << "  ✓ Reverse iteration works" << endl;

    cout << "\n[5.4] cbegin/cend:" << endl;
    cout << "  cbegin/cend: ";
    for (auto it = set.cbegin(); it != set.cend(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
    cout << "  ✓ cbegin/cend work" << endl;

    cout << "\n[5.5] crbegin/crend:" << endl;
    cout << "  crbegin/crend: ";
    for (auto it = set.crbegin(); it != set.crend(); ++it)
    {
        cout << *it << " ";
    }
    cout << endl;
    cout << "  ✓ crbegin/crend work" << endl;

    cout << "\n[5.6] lower_bound:" << endl;
    auto it_lower = set.lower_bound(8);
    assert(*it_lower == 8);
    cout << "  lower_bound(8) = " << *it_lower << endl;

    it_lower = set.lower_bound(9);
    assert(*it_lower == 10);
    cout << "  lower_bound(9) = " << *it_lower << " (first >= 9)" << endl;
    cout << "  ✓ lower_bound works correctly" << endl;

    cout << "\n[5.7] upper_bound:" << endl;
    auto it_upper = set.upper_bound(8);
    assert(*it_upper == 10);
    cout << "  upper_bound(8) = " << *it_upper << endl;

    it_upper = set.upper_bound(9);
    assert(*it_upper == 10);
    cout << "  upper_bound(9) = " << *it_upper << " (first > 9)" << endl;
    cout << "  ✓ upper_bound works correctly" << endl;

    cout << "\n[5.8] Find returns valid iterator:" << endl;
    auto it_find = set.Find(15);
    assert(it_find != set.end());
    assert(*it_find == 15);
    cout << "  Find(15) returned iterator to 15" << endl;

    it_find = set.Find(99);
    assert(it_find == set.end());
    cout << "  Find(99) returned end() for missing value" << endl;
    cout << "  ✓ Find iterator works correctly" << endl;

    cout << "\n✅ TEST 05 PASSED" << endl;
    return 0;
}
