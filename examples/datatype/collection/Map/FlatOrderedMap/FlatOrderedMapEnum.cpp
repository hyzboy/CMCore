#include<hgl/type/FlatOrderedMap.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 03: FlatOrderedMap<K,V> Enum/Iterate" << endl;
    cout << "========================================" << endl;

    FlatOrderedMap<int, float> map;

    cout << "\n[3.1] Populate map:" << endl;
    map.Add(1, 1.5f);
    map.Add(2, 2.5f);
    map.Add(3, 3.5f);
    assert(map.GetCount() == 3);
    cout << "  ✓ Added 3 items" << endl;

    cout << "\n[3.2] EnumKV - enumerate all pairs:" << endl;
    cout << "  Pairs: ";
    int enum_count = 0;
    map.EnumKV([&](const int& key, const float& value) {
        cout << "[" << key << ":" << value << "] ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == 3);
    cout << "  ✓ Enumerated " << enum_count << " pairs" << endl;

    cout << "\n[3.3] EnumKV const:" << endl;
    enum_count = 0;
    const FlatOrderedMap<int, float>& const_map = map;
    const_map.EnumKV([&](const int& key, const float& value) {
        ++enum_count;
    });
    assert(enum_count == 3);
    cout << "  ✓ Const EnumKV works" << endl;

    cout << "\n[3.4] EnumKeys:" << endl;
    cout << "  Keys: ";
    enum_count = 0;
    map.EnumKeys([&](const int& key) {
        cout << key << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == 3);
    cout << "  ✓ EnumKeys visited " << enum_count << " keys" << endl;

    cout << "\n[3.5] EnumKeys const:" << endl;
    cout << "  Keys (const): ";
    enum_count = 0;
    const_map.EnumKeys([&](const int& key) {
        cout << key << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == 3);
    cout << "  ✓ Const EnumKeys works" << endl;

    cout << "\n[3.6] EnumValues:" << endl;
    cout << "  Values: ";
    enum_count = 0;
    map.EnumValues([&](float& value) {
        cout << value << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == 3);
    cout << "  ✓ EnumValues visited " << enum_count << " values" << endl;

    cout << "\n[3.7] EnumValues with modification:" << endl;
    map.EnumValues([](float& value) {
        value *= 10.0f;  // Multiply by 10
    });
    float v;
    assert(map.Get(1, v) && v == 15.0f);
    cout << "  ✓ Values modified through EnumValues" << endl;

    cout << "\n[3.8] EnumValues const:" << endl;
    cout << "  Values (const): ";
    enum_count = 0;
    const_map.EnumValues([&](const float& value) {
        cout << value << " ";
        ++enum_count;
    });
    cout << endl;
    assert(enum_count == 3);
    cout << "  ✓ Const EnumValues works" << endl;

    cout << "\n[3.9] Range-based for loop:" << endl;
    cout << "  Keys via range: ";
    enum_count = 0;
    for (auto key : map)
    {
        cout << key << " ";
        ++enum_count;
    }
    cout << endl;
    assert(enum_count == 3);
    cout << "  ✓ Range-based for works" << endl;

    cout << "\n[3.10] Empty map enumeration:" << endl;
    FlatOrderedMap<int, float> empty_map;
    enum_count = 0;
    empty_map.EnumKV([&](const int& key, const float& value) {
        ++enum_count;
    });
    assert(enum_count == 0);
    cout << "  ✓ Empty map enumeration safe" << endl;

    cout << "\n✅ TEST 03 PASSED" << endl;
    return 0;
}
