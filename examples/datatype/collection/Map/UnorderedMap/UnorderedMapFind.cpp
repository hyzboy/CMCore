#include<hgl/type/UnorderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 01: UnorderedMap<K,V> Find & Query" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int, string> map;

    cout << "\n[1.1] Populate map:" << endl;
    assert(map.Add(10, "ten"));
    assert(map.Add(20, "twenty"));
    assert(map.Add(30, "thirty"));
    assert(map.Add(15, "fifteen"));
    assert(map.GetCount() == 4);
    cout << "  ✓ Added 4 items" << endl;

    cout << "\n[1.2] Get value by key:" << endl;
    string value;
    assert(map.Get(10, value) && value == "ten");
    assert(map.Get(20, value) && value == "twenty");
    assert(!map.Get(99, value));
    cout << "  ✓ Get works correctly" << endl;

    cout << "\n[1.3] GetValuePointer:" << endl;
    const string* ptr = map.GetValuePointer(10);
    assert(ptr != nullptr && *ptr == "ten");

    string* ptr_write = map.GetValuePointer(20);
    assert(ptr_write != nullptr && *ptr_write == "twenty");

    ptr = map.GetValuePointer(99);
    assert(ptr == nullptr);
    cout << "  ✓ GetValuePointer works" << endl;

    cout << "\n[1.4] ContainsKey:" << endl;
    assert(map.ContainsKey(10));
    assert(map.ContainsKey(15));
    assert(map.ContainsKey(20));
    assert(!map.ContainsKey(99));
    assert(!map.ContainsKey(0));
    cout << "  ✓ ContainsKey works correctly" << endl;

    cout << "\n[1.5] Change existing value:" << endl;
    assert(map.Change(10, "TEN"));
    assert(map.Get(10, value) && value == "TEN");
    cout << "  Changed key 10 to 'TEN'" << endl;

    cout << "\n[1.6] Change non-existent key:" << endl;
    assert(!map.Change(99, "ninety-nine"));
    cout << "  ✓ Change on missing key returns false" << endl;

    cout << "\n[1.7] ChangeOrAdd - change existing:" << endl;
    bool result = map.ChangeOrAdd(20, "TWENTY");
    assert(result);  // ChangeOrAdd returns true for any operation
    assert(map.Get(20, value) && value == "TWENTY");
    cout << "  ✓ ChangeOrAdd changed existing key" << endl;

    cout << "\n[1.8] ChangeOrAdd - add new:" << endl;
    result = map.ChangeOrAdd(40, "forty");
    assert(result);
    assert(map.GetCount() == 5);
    assert(map.Get(40, value) && value == "forty");
    cout << "  ✓ ChangeOrAdd added new key" << endl;

    cout << "\n[1.9] DeleteByKey:" << endl;
    assert(map.DeleteByKey(15));
    assert(map.GetCount() == 4);
    assert(!map.ContainsKey(15));
    assert(!map.DeleteByKey(99));
    cout << "  ✓ DeleteByKey works correctly" << endl;

    cout << "\n[1.10] Clear and Free:" << endl;
    map.Clear();
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  ✓ Clear empties the map" << endl;

    cout << "\n✅ TEST 01 PASSED" << endl;
    return 0;
}
