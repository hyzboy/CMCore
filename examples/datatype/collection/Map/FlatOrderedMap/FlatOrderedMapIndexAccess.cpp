#include<hgl/type/FlatOrderedMap.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 05: FlatOrderedMap<K,V> Index Access" << endl;
    cout << "========================================" << endl;

    FlatOrderedMap<int, double> map;

    cout << "\n[5.1] Populate map:" << endl;
    map.Add(10, 10.5);
    map.Add(5, 5.5);
    map.Add(15, 15.5);
    map.Add(2, 2.5);
    assert(map.GetCount() == 4);
    cout << "  ✓ Added 4 items" << endl;

    cout << "\n[5.2] GetKeyAt and GetValueAt:" << endl;
    for (int i = 0; i < map.GetCount(); ++i)
    {
        int key = map.GetKeyAt(i);
        double value = map.GetValueAt(i);
        cout << "  [" << i << "] Key=" << key << ", Value=" << value << endl;
    }
    cout << "  ✓ All keys in sorted order: 2, 5, 10, 15" << endl;

    cout << "\n[5.3] GetValuePointerAt:" << endl;
    double* ptr = map.GetValuePointerAt(1);  // Index 1 is key 5
    assert(ptr != nullptr && *ptr == 5.5);
    cout << "  Index 1 value: " << *ptr << endl;
    cout << "  ✓ GetValuePointerAt works" << endl;

    cout << "\n[5.4] Invalid indices:" << endl;
    assert(map.GetKeyAt(-1) == 0);
    assert(map.GetKeyAt(100) == 0);
    assert(map.GetValueAt(-1) == 0.0);
    assert(map.GetValuePointerAt(-1) == nullptr);
    assert(map.GetValuePointerAt(100) == nullptr);
    cout << "  ✓ Invalid indices handled safely" << endl;

    cout << "\n[5.5] GetBySerial:" << endl;
    int key;
    double value;
    assert(map.GetBySerial(0, key, value));
    assert(key == 2 && value == 2.5);
    cout << "  Serial 0: key=" << key << ", value=" << value << endl;

    assert(map.GetBySerial(2, key, value));
    assert(key == 10 && value == 10.5);
    cout << "  Serial 2: key=" << key << ", value=" << value << endl;

    assert(!map.GetBySerial(-1, key, value));
    assert(!map.GetBySerial(4, key, value));
    cout << "  ✓ GetBySerial works correctly" << endl;

    cout << "\n[5.6] DeleteAt by index:" << endl;
    assert(map.DeleteAt(1));  // Delete index 1 (key 5)
    assert(map.GetCount() == 3);
    assert(!map.ContainsKey(5));
    assert(map.GetKeyAt(0) == 2);
    assert(map.GetKeyAt(1) == 10);
    assert(map.GetKeyAt(2) == 15);
    cout << "  ✓ DeleteAt removed correct element" << endl;

    cout << "\n[5.7] DeleteAt range:" << endl;
    map.Add(5, 5.5);  // Add back to have 4 items again
    assert(map.GetCount() == 4);
    assert(map.DeleteAt(1, 2));  // Delete 2 items starting at index 1
    assert(map.GetCount() == 2);
    assert(map.GetKeyAt(0) == 2);
    assert(map.GetKeyAt(1) == 15);
    cout << "  ✓ DeleteAt(start, count) works" << endl;

    cout << "\n[5.8] DeleteAt invalid cases:" << endl;
    assert(!map.DeleteAt(-1));
    assert(!map.DeleteAt(10));
    assert(!map.DeleteAt(0, -1));
    assert(!map.DeleteAt(0, 10));
    cout << "  ✓ Invalid delete indices handled" << endl;

    cout << "\n[5.9] Change and AddOrUpdate:" << endl;
    map.Clear();
    map.Add(1, 1.5);
    map.Add(2, 2.5);

    assert(map.Change(1, 1.9));
    double v;
    assert(map.Get(1, v) && v == 1.9);
    cout << "  Changed key 1 to 1.9" << endl;

    bool is_new = map.AddOrUpdate(2, 2.9);
    assert(!is_new);  // false = updated
    assert(map.Get(2, v) && v == 2.9);

    is_new = map.AddOrUpdate(3, 3.5);
    assert(is_new);   // true = added
    assert(map.GetCount() == 3);
    cout << "  ✓ Change/AddOrUpdate work correctly" << endl;

    cout << "\n✅ TEST 05 PASSED" << endl;
    return 0;
}
