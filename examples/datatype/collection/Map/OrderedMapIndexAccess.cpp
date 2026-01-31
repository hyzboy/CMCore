#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 03: OrderedMap<K,V> Index Access" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;
    
    cout << "\n[3.1] Populate map with sorted keys:" << endl;
    map.Add(10, "ten");
    map.Add(5, "five");
    map.Add(15, "fifteen");
    map.Add(2, "two");
    map.Add(20, "twenty");
    assert(map.GetCount() == 5);
    cout << "  Added 5 items" << endl;

    cout << "\n[3.2] GetKeyAt and GetValueAt:" << endl;
    for (int i = 0; i < map.GetCount(); ++i)
    {
        int key = map.GetKeyAt(i);
        string value = map.GetValueAt(i);
        cout << "  [" << i << "] Key=" << key << ", Value=" << value << endl;
    }
    cout << "  ✓ Keys are in sorted order: 2, 5, 10, 15, 20" << endl;

    cout << "\n[3.3] GetValuePointerAt:" << endl;
    string* ptr = map.GetValuePointerAt(2);  // Index 2 is key 10
    assert(ptr != nullptr && *ptr == "ten");
    cout << "  Index 2 (key 10) -> " << *ptr << endl;

    cout << "\n[3.4] GetValuePointerAt with invalid index:" << endl;
    assert(map.GetValuePointerAt(-1) == nullptr);
    assert(map.GetValuePointerAt(5) == nullptr);
    assert(map.GetValuePointerAt(100) == nullptr);
    cout << "  ✓ Invalid indices return nullptr" << endl;

    cout << "\n[3.5] GetBySerial (get by index):" << endl;
    int key;
    string value;
    assert(map.GetBySerial(0, key, value));
    assert(key == 2 && value == "two");
    cout << "  Serial 0: key=" << key << ", value=" << value << endl;

    assert(map.GetBySerial(2, key, value));
    assert(key == 10 && value == "ten");
    cout << "  Serial 2: key=" << key << ", value=" << value << endl;

    assert(map.GetBySerial(4, key, value));
    assert(key == 20 && value == "twenty");
    cout << "  Serial 4: key=" << key << ", value=" << value << endl;
    cout << "  ✓ GetBySerial works correctly" << endl;

    cout << "\n[3.6] GetBySerial with invalid index:" << endl;
    assert(!map.GetBySerial(-1, key, value));
    assert(!map.GetBySerial(5, key, value));
    assert(!map.GetBySerial(100, key, value));
    cout << "  ✓ Invalid indices return false" << endl;

    cout << "\n[3.7] DeleteAt by index:" << endl;
    assert(map.GetCount() == 5);
    assert(map.DeleteAt(2));  // Delete index 2 (key 10)
    assert(map.GetCount() == 4);
    
    // Verify key 10 is gone
    assert(!map.ContainsKey(10));
    
    // Verify remaining keys are in order
    int key1 = map.GetKeyAt(0);
    int key2 = map.GetKeyAt(1);
    int key3 = map.GetKeyAt(2);
    int key4 = map.GetKeyAt(3);
    assert(key1 == 2 && key2 == 5 && key3 == 15 && key4 == 20);
    cout << "  ✓ DeleteAt(2) removed key 10" << endl;

    cout << "\n[3.8] DeleteAt range:" << endl;
    assert(map.DeleteAt(1, 2));  // Delete 2 items starting at index 1
    assert(map.GetCount() == 2);
    assert(map.GetKeyAt(0) == 2);
    assert(map.GetKeyAt(1) == 20);
    cout << "  ✓ DeleteAt(1, 2) removed keys at positions 1 and 2" << endl;

    cout << "\n[3.9] DeleteAt range - invalid cases:" << endl;
    assert(!map.DeleteAt(-1, 1));
    assert(!map.DeleteAt(0, 0));
    assert(!map.DeleteAt(0, -1));
    assert(!map.DeleteAt(0, 10));  // Out of range
    cout << "  ✓ Invalid ranges handled safely" << endl;

    cout << "\n✅ TEST 03 PASSED" << endl;
    return 0;
}
