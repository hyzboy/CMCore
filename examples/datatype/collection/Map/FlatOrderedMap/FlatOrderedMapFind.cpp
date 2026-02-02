#include<hgl/platform/Platform.h>
#include<hgl/type/FlatOrderedMap.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 02: FlatOrderedMap<K,V> Find & Value" << endl;
    cout << "========================================" << endl;

    FlatOrderedMap<int, int> map;

    cout << "\n[2.1] Populate map:" << endl;
    map.Add(10, 100);
    map.Add(20, 200);
    map.Add(30, 300);
    assert(map.GetCount() == 3);
    cout << "  ✓ Populated with 3 items" << endl;

    cout << "\n[2.2] Find by key:" << endl;
    assert(map.Find(10) == 0);
    assert(map.Find(20) == 1);
    assert(map.Find(30) == 2);
    assert(map.Find(40) == -1);
    cout << "  ✓ Find by key works" << endl;

    cout << "\n[2.3] Get value:" << endl;
    int val;
    assert(map.Get(20, val) && val == 200);
    assert(!map.Get(99, val));
    cout << "  ✓ Get value works" << endl;

    cout << "\n[2.4] GetValuePointer:" << endl;
    const int* ptr = map.GetValuePointer(10);
    assert(ptr != nullptr && *ptr == 100);
    ptr = map.GetValuePointer(99);
    assert(ptr == nullptr);
    cout << "  ✓ GetValuePointer works" << endl;

    cout << "\n[2.5] FindByValue:" << endl;
    assert(map.FindByValue(200) == 1);
    assert(map.FindByValue(300) == 2);
    assert(map.FindByValue(400) == -1);
    cout << "  ✓ FindByValue works" << endl;

    cout << "\n[2.6] ContainsValue:" << endl;
    assert(map.ContainsValue(100));
    assert(map.ContainsValue(300));
    assert(!map.ContainsValue(500));
    cout << "  ✓ ContainsValue works" << endl;

    cout << "\n[2.7] DeleteByValue:" << endl;
    assert(map.DeleteByValue(200));
    assert(map.GetCount() == 2);
    assert(!map.ContainsValue(200));
    assert(map.ContainsKey(10));  // Key 10 still exists
    cout << "  ✓ DeleteByValue successful" << endl;

    cout << "\n[2.8] GetAndDelete:" << endl;
    int retrieved;
    assert(map.GetAndDelete(10, retrieved));
    assert(retrieved == 100);
    assert(map.GetCount() == 1);
    assert(!map.ContainsKey(10));
    cout << "  ✓ GetAndDelete works" << endl;

    cout << "\n[2.9] GetAndDelete non-existent:" << endl;
    assert(!map.GetAndDelete(99, retrieved));
    cout << "  ✓ GetAndDelete on missing key returns false" << endl;

    cout << "\n✅ TEST 02 PASSED" << endl;
    return 0;
}
