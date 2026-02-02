#include<hgl/platform/Platform.h>
#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 01: OrderedMap<K,V> Basic Operations" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);

    cout << "\n[1.1] Add key-value pairs:" << endl;
    assert(map.Add(5, "five"));
    assert(map.Add(1, "one"));
    assert(map.Add(3, "three"));
    assert(map.GetCount() == 3);
    cout << "  Added 3 items, map size: " << map.GetCount() << endl;

    cout << "\n[1.2] Verify sorted order (by key):" << endl;
    string v;
    assert(map.Get(1, v) && v == "one");
    cout << "  Key 1 -> " << v << endl;
    assert(map.Get(3, v) && v == "three");
    cout << "  Key 3 -> " << v << endl;
    assert(map.Get(5, v) && v == "five");
    cout << "  Key 5 -> " << v << endl;
    cout << "  ✓ Map sorted by key" << endl;

    cout << "\n[1.3] Duplicate key insert should fail:" << endl;
    assert(!map.Add(3, "THREE"));  // Try to add duplicate key
    assert(map.GetCount() == 3);   // Count unchanged
    cout << "  ✓ Duplicate key correctly rejected" << endl;

    cout << "\n[1.4] ContainsKey / Find:" << endl;
    assert(map.ContainsKey(1));
    assert(map.ContainsKey(3));
    assert(!map.ContainsKey(4));
    assert(map.Find(1) == 0);
    assert(map.Find(3) == 1);
    assert(map.Find(5) == 2);
    assert(map.Find(4) == -1);
    cout << "  ✓ ContainsKey/Find work correctly" << endl;

    cout << "\n[1.5] GetValuePointer:" << endl;
    const string* ptr = map.GetValuePointer(3);
    assert(ptr != nullptr && *ptr == "three");
    cout << "  Key 3 -> " << *ptr << endl;
    ptr = map.GetValuePointer(99);
    assert(ptr == nullptr);
    cout << "  ✓ GetValuePointer works" << endl;

    cout << "\n[1.6] Delete by key:" << endl;
    assert(map.DeleteByKey(3));
    assert(map.GetCount() == 2);
    assert(!map.ContainsKey(3));
    assert(!map.DeleteByKey(99));  // Try to delete non-existent key
    cout << "  ✓ Delete by key works" << endl;

    cout << "\n[1.7] Clear:" << endl;
    map.Clear();
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  ✓ Clear empties the map" << endl;

    cout << "\n✅ TEST 01 PASSED" << endl;
    return 0;
}
