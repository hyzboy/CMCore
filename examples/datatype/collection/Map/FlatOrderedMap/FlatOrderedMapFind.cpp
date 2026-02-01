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

    FlatOrderedMap<int, string> map;

    cout << "\n[2.1] Populate map:" << endl;
    map.Add(10, "ten");
    map.Add(20, "twenty");
    map.Add(30, "thirty");
    assert(map.GetCount() == 3);
    cout << "  ✓ Populated with 3 items" << endl;

    cout << "\n[2.2] Find by key:" << endl;
    assert(map.Find(10) == 0);
    assert(map.Find(20) == 1);
    assert(map.Find(30) == 2);
    assert(map.Find(40) == -1);
    cout << "  ✓ Find by key works" << endl;

    cout << "\n[2.3] Get value:" << endl;
    string val;
    assert(map.Get(20, val) && val == "twenty");
    assert(!map.Get(99, val));
    cout << "  ✓ Get value works" << endl;

    cout << "\n[2.4] GetValuePointer:" << endl;
    const string* ptr = map.GetValuePointer(10);
    assert(ptr != nullptr && *ptr == "ten");
    ptr = map.GetValuePointer(99);
    assert(ptr == nullptr);
    cout << "  ✓ GetValuePointer works" << endl;

    cout << "\n[2.5] FindByValue:" << endl;
    assert(map.FindByValue("twenty") == 1);
    assert(map.FindByValue("thirty") == 2);
    assert(map.FindByValue("forty") == -1);
    cout << "  ✓ FindByValue works" << endl;

    cout << "\n[2.6] ContainsValue:" << endl;
    assert(map.ContainsValue("ten"));
    assert(map.ContainsValue("thirty"));
    assert(!map.ContainsValue("fifty"));
    cout << "  ✓ ContainsValue works" << endl;

    cout << "\n[2.7] DeleteByValue:" << endl;
    assert(map.DeleteByValue("twenty"));
    assert(map.GetCount() == 2);
    assert(!map.ContainsValue("twenty"));
    assert(map.ContainsKey(10));  // Key 10 still exists
    cout << "  ✓ DeleteByValue successful" << endl;

    cout << "\n[2.8] GetAndDelete:" << endl;
    string retrieved;
    assert(map.GetAndDelete(10, retrieved));
    assert(retrieved == "ten");
    assert(map.GetCount() == 1);
    assert(!map.ContainsKey(10));
    cout << "  ✓ GetAndDelete works" << endl;

    cout << "\n[2.9] GetAndDelete non-existent:" << endl;
    assert(!map.GetAndDelete(99, retrieved));
    cout << "  ✓ GetAndDelete on missing key returns false" << endl;

    cout << "\n✅ TEST 02 PASSED" << endl;
    return 0;
}
