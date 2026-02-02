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
    cout << "TEST 04: OrderedMap<K,V> Find & Value Search" << endl;
    cout << "========================================" << endl;

    OrderedMap<string, int> map;

    cout << "\n[4.1] Populate map:" << endl;
    map.Add("apple", 1);
    map.Add("banana", 2);
    map.Add("cherry", 3);
    map.Add("date", 4);
    map.Add("elderberry", 5);
    assert(map.GetCount() == 5);
    cout << "  Added 5 fruit-count pairs" << endl;

    cout << "\n[4.2] ContainsKey:" << endl;
    assert(map.ContainsKey("apple"));
    assert(map.ContainsKey("cherry"));
    assert(!map.ContainsKey("orange"));
    cout << "  ✓ ContainsKey works correctly" << endl;

    cout << "\n[4.3] Find by key:" << endl;
    assert(map.Find("apple") == 0);
    assert(map.Find("cherry") == 2);
    assert(map.Find("elderberry") == 4);
    assert(map.Find("orange") == -1);
    cout << "  ✓ Find returns correct indices" << endl;

    cout << "\n[4.4] Get value by key:" << endl;
    int value;
    assert(map.Get("banana", value) && value == 2);
    cout << "  Key 'banana' -> " << value << endl;
    assert(!map.Get("orange", value));
    cout << "  ✓ Get returns false for missing key" << endl;

    cout << "\n[4.5] ContainsValue:" << endl;
    assert(map.ContainsValue(1));
    assert(map.ContainsValue(3));
    assert(map.ContainsValue(5));
    assert(!map.ContainsValue(10));
    cout << "  ✓ ContainsValue works correctly" << endl;

    cout << "\n[4.6] FindByValue:" << endl;
    assert(map.FindByValue(1) == 0);  // "apple" is at index 0
    assert(map.FindByValue(3) == 2);  // "cherry" is at index 2
    assert(map.FindByValue(5) == 4);  // "elderberry" is at index 4
    assert(map.FindByValue(10) == -1); // Not found
    cout << "  ✓ FindByValue returns correct indices" << endl;

    cout << "\n[4.7] Check key-value pair:" << endl;
    assert(map.Check("apple", 1));
    assert(map.Check("banana", 2));
    assert(!map.Check("apple", 2));  // Wrong value
    assert(!map.Check("orange", 1)); // Missing key
    cout << "  ✓ Check validates key-value pairs" << endl;

    cout << "\n[4.8] DeleteByValue:" << endl;
    assert(map.DeleteByValue(3));  // Delete entry with value 3
    assert(map.GetCount() == 4);
    assert(!map.ContainsValue(3));
    assert(!map.ContainsKey("cherry"));
    cout << "  ✓ DeleteByValue removes correct entry" << endl;

    cout << "\n[4.9] FindByValue after deletion:" << endl;
    assert(map.FindByValue(3) == -1);  // Value 3 no longer exists
    assert(map.FindByValue(4) == 2);   // Value 4 now at index 2 (was at index 3)
    cout << "  ✓ Indices updated after deletion" << endl;

    cout << "\n✅ TEST 04 PASSED" << endl;
    return 0;
}
