#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 02: OrderedMap<K,V> Add/Update/Change" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;

    cout << "\n[2.1] AddOrUpdate - add new key:" << endl;
    map.AddOrUpdate(1, "one");
    assert(map.GetCount() == 1);
    string v;
    assert(map.Get(1, v) && v == "one");
    cout << "  Added (1, 'one')" << endl;

    cout << "\n[2.2] AddOrUpdate - update existing key:" << endl;
    map.AddOrUpdate(1, "ONE");
    assert(map.GetCount() == 1);  // Count unchanged
    assert(map.Get(1, v) && v == "ONE");
    cout << "  Updated key 1 to 'ONE'" << endl;

    cout << "\n[2.3] Change - modify existing key:" << endl;
    assert(map.Change(1, "one_modified"));
    assert(map.Get(1, v) && v == "one_modified");
    cout << "  Changed key 1 to 'one_modified'" << endl;

    cout << "\n[2.4] Change - non-existent key returns false:" << endl;
    assert(!map.Change(99, "value"));
    cout << "  ✓ Change on non-existent key returns false" << endl;

    cout << "\n[2.5] ChangeOrAdd - change existing:" << endl;
    bool is_new = map.ChangeOrAdd(1, "ONE_AGAIN");
    assert(!is_new);  // false means changed
    assert(map.Get(1, v) && v == "ONE_AGAIN");
    cout << "  Changed key 1, is_new=false" << endl;

    cout << "\n[2.6] ChangeOrAdd - add new:" << endl;
    is_new = map.ChangeOrAdd(2, "two");
    assert(is_new);   // true means added
    assert(map.GetCount() == 2);
    assert(map.Get(2, v) && v == "two");
    cout << "  Added key 2, is_new=true" << endl;

    cout << "\n[2.7] Multiple ChangeOrAdd:" << endl;
    map.ChangeOrAdd(3, "three");
    map.ChangeOrAdd(4, "four");
    map.ChangeOrAdd(5, "five");
    assert(map.GetCount() == 5);
    cout << "  ✓ Multiple operations maintained consistency" << endl;

    cout << "\n[2.8] GetAndDelete:" << endl;
    string retrieved;
    assert(map.GetAndDelete(3, retrieved));
    assert(retrieved == "three");
    assert(map.GetCount() == 4);
    assert(!map.ContainsKey(3));
    cout << "  Retrieved and deleted key 3: '" << retrieved << "'" << endl;

    cout << "\n[2.9] GetAndDelete - non-existent key:" << endl;
    assert(!map.GetAndDelete(99, retrieved));
    cout << "  ✓ GetAndDelete on non-existent key returns false" << endl;

    cout << "\n✅ TEST 02 PASSED" << endl;
    return 0;
}
