#include<hgl/type/FlatOrderedMap.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 01: FlatOrderedMap<K,V> Basic Ops" << endl;
    cout << "========================================" << endl;

    FlatOrderedMap<int, float> map;
    
    cout << "\n[1.1] Empty map checks:" << endl;
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  ✓ Empty map verified" << endl;

    cout << "\n[1.2] Add operations:" << endl;
    assert(map.Add(1, 1.5f));
    assert(map.Add(3, 3.5f));
    assert(map.Add(2, 2.5f));
    assert(map.GetCount() == 3);
    cout << "  ✓ Added 3 items" << endl;

    cout << "\n[1.3] Verify sorting:" << endl;
    assert(map.GetKeyAt(0) == 1);
    assert(map.GetKeyAt(1) == 2);
    assert(map.GetKeyAt(2) == 3);
    cout << "  ✓ Keys sorted: [1, 2, 3]" << endl;

    cout << "\n[1.4] Duplicate key rejection:" << endl;
    assert(!map.Add(2, 2.5f));  // Try to add duplicate
    assert(map.GetCount() == 3);  // Count unchanged
    cout << "  ✓ Duplicate key correctly rejected" << endl;

    cout << "\n[1.5] ContainsKey:" << endl;
    assert(map.ContainsKey(1));
    assert(map.ContainsKey(2));
    assert(!map.ContainsKey(10));
    cout << "  ✓ ContainsKey works correctly" << endl;

    cout << "\n[1.6] Find by key:" << endl;
    assert(map.Find(1) == 0);
    assert(map.Find(2) == 1);
    assert(map.Find(3) == 2);
    assert(map.Find(10) == -1);
    cout << "  ✓ Find returns correct indices" << endl;

    cout << "\n[1.7] Get value:" << endl;
    float value;
    assert(map.Get(2, value) && value == 2.5f);
    assert(!map.Get(99, value));
    cout << "  ✓ Get works correctly" << endl;

    cout << "\n[1.8] DeleteByKey:" << endl;
    assert(map.DeleteByKey(2));
    assert(map.GetCount() == 2);
    assert(!map.ContainsKey(2));
    cout << "  ✓ DeleteByKey successful" << endl;

    cout << "\n[1.9] Clear:" << endl;
    map.Clear();
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  ✓ Clear empties the map" << endl;

    cout << "\n✅ TEST 01 PASSED" << endl;
    return 0;
}
