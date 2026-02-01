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
    cout << "TEST 06: OrderedMap<K,V> Enum Functions" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;

    cout << "\n[6.1] Populate map:" << endl;
    map.Add(1, "one");
    map.Add(2, "two");
    map.Add(3, "three");
    map.Add(4, "four");
    assert(map.GetCount() == 4);
    cout << "  Added 4 items" << endl;

    cout << "\n[6.2] EnumKV - enumerate all key-value pairs:" << endl;
    cout << "  Enum: ";
    int enum_count = 0;
    map.EnumKV([&](const int& key, string& value) {
        cout << "[" << key << ":" << value << "] ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == map.GetCount());
    cout << "  ✓ EnumKV visited all " << enum_count << " pairs" << endl;

    cout << "\n[6.3] EnumKV const - read-only enumeration:" << endl;
    cout << "  Const enum: ";
    enum_count = 0;
    const OrderedMap<int, string>& const_map = map;
    const_map.EnumKV([&](const int& key, const string& value) {
        cout << "[" << key << ":" << value << "] ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == map.GetCount());
    cout << "  ✓ Const EnumKV works" << endl;

    cout << "\n[6.4] EnumKeys - enumerate all keys:" << endl;
    cout << "  Keys: ";
    enum_count = 0;
    map.EnumKeys([&](const int& key) {
        cout << key << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == map.GetCount());
    cout << "  ✓ EnumKeys enumerated all keys" << endl;

    cout << "\n[6.5] EnumKeys const - const version:" << endl;
    cout << "  Keys (const): ";
    enum_count = 0;
    const_map.EnumKeys([&](const int& key) {
        cout << key << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == map.GetCount());
    cout << "  ✓ Const EnumKeys works" << endl;

    cout << "\n[6.6] EnumValues - enumerate all values:" << endl;
    cout << "  Values: ";
    enum_count = 0;
    map.EnumValues([&](string& value) {
        cout << value << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == map.GetCount());
    cout << "  ✓ EnumValues enumerated all values" << endl;

    cout << "\n[6.7] EnumValues const - read-only:" << endl;
    cout << "  Const values: ";
    enum_count = 0;
    const_map.EnumValues([&](const string& value) {
        cout << value << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == map.GetCount());
    cout << "  ✓ Const EnumValues works" << endl;

    cout << "\n[6.8] EnumKV with modification:" << endl;
    map.EnumKV([](const int& key, string& value) {
        value = value + "_modified";
    });
    string v;
    assert(map.Get(1, v) && v == "one_modified");
    assert(map.Get(2, v) && v == "two_modified");
    cout << "  ✓ Values modified through EnumKV" << endl;

    cout << "\n[6.9] EnumKV with early termination check:" << endl;
    enum_count = 0;
    int target_count = 2;
    const_map.EnumKV([&](const int& key, const string& value) {
        if (enum_count < target_count) {
            cout << "  Item " << enum_count + 1 << ": " << key << " -> " << value << endl;
            ++enum_count;
        }
    });
    assert(enum_count == 2);
    cout << "  ✓ Can control enumeration flow" << endl;

    cout << "\n✅ TEST 06 PASSED" << endl;
    return 0;
}
