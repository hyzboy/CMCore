#include<hgl/platform/Platform.h>
#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cstdlib>
#include<string>

using namespace hgl;
using namespace std;

// CHECK() is stripped under NDEBUG (Release), which silently drops any
// side-effecting call written inside it (e.g. map.Add(...)).
// Use an always-evaluated check instead.
static void CheckFailed(const char *expr)
{
    cout << "  [FAIL] " << expr << endl;
    exit(1);
}
#define CHECK(expr) do { if(!(expr)) CheckFailed(#expr); } while(0)

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 01: OrderedMap<K,V> Basic Operations" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;
    CHECK(map.IsEmpty());
    CHECK(map.GetCount() == 0);

    cout << "\n[1.1] Add key-value pairs:" << endl;
    CHECK(map.Add(5, "five"));
    CHECK(map.Add(1, "one"));
    CHECK(map.Add(3, "three"));
    CHECK(map.GetCount() == 3);
    cout << "  Added 3 items, map size: " << map.GetCount() << endl;

    cout << "\n[1.2] Verify sorted order (by key):" << endl;
    string v;
    CHECK(map.Get(1, v) && v == "one");
    cout << "  Key 1 -> " << v << endl;
    CHECK(map.Get(3, v) && v == "three");
    cout << "  Key 3 -> " << v << endl;
    CHECK(map.Get(5, v) && v == "five");
    cout << "  Key 5 -> " << v << endl;
    cout << "  ✓ Map sorted by key" << endl;

    cout << "\n[1.3] Duplicate key insert should fail:" << endl;
    CHECK(!map.Add(3, "THREE"));  // Try to add duplicate key
    CHECK(map.GetCount() == 3);   // Count unchanged
    cout << "  ✓ Duplicate key correctly rejected" << endl;

    cout << "\n[1.4] ContainsKey / Find:" << endl;
    CHECK(map.ContainsKey(1));
    CHECK(map.ContainsKey(3));
    CHECK(!map.ContainsKey(4));
    CHECK(map.Find(1) == 0);
    CHECK(map.Find(3) == 1);
    CHECK(map.Find(5) == 2);
    CHECK(map.Find(4) == -1);
    cout << "  ✓ ContainsKey/Find work correctly" << endl;

    cout << "\n[1.5] GetValuePointer:" << endl;
    const string* ptr = map.GetValuePointer(3);
    CHECK(ptr != nullptr && *ptr == "three");
    cout << "  Key 3 -> " << *ptr << endl;
    ptr = map.GetValuePointer(99);
    CHECK(ptr == nullptr);
    cout << "  ✓ GetValuePointer works" << endl;

    cout << "\n[1.6] Delete by key:" << endl;
    CHECK(map.DeleteByKey(3));
    CHECK(map.GetCount() == 2);
    CHECK(!map.ContainsKey(3));
    CHECK(!map.DeleteByKey(99));  // Try to delete non-existent key
    cout << "  ✓ Delete by key works" << endl;

    cout << "\n[1.7] Clear:" << endl;
    map.Clear();
    CHECK(map.IsEmpty());
    CHECK(map.GetCount() == 0);
    cout << "  ✓ Clear empties the map" << endl;

    cout << "\n✅ TEST 01 PASSED" << endl;
    return 0;
}
