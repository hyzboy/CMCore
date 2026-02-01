#include<hgl/type/UnorderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 02: UnorderedMap<K,V> Enum & Iterate" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int, string> map;

    cout << "\n[2.1] Populate map:" << endl;
    map.Add(1, "one");
    map.Add(2, "two");
    map.Add(3, "three");
    map.Add(4, "four");
    assert(map.GetCount() == 4);
    cout << "  ✓ Added 4 items" << endl;

    cout << "\n[2.2] EnumKV - enumerate all pairs:" << endl;
    int count = 0;
    cout << "  Pairs: ";
    map.EnumKV([&](const int& key, string& value) {
        cout << "[" << key << ":" << value << "] ";
        ++count;
    });
    cout << endl;
    assert(count == 4);
    cout << "  ✓ EnumKV visited " << count << " pairs" << endl;

    cout << "\n[2.3] EnumKV const version:" << endl;
    count = 0;
    const UnorderedMap<int, string>& const_map = map;
    const_map.EnumKV([&](const int& key, const string& value) {
        ++count;
    });
    assert(count == 4);
    cout << "  ✓ Const EnumKV works" << endl;

    cout << "\n[2.4] EnumKeys:" << endl;
    cout << "  Keys: ";
    count = 0;
    map.EnumKeys([&](const int& key) {
        cout << key << " ";
        ++count;
    });
    cout << endl;
    assert(count == 4);
    cout << "  ✓ EnumKeys visited " << count << " keys" << endl;

    cout << "\n[2.5] EnumKeys const:" << endl;
    cout << "  Keys (const): ";
    count = 0;
    const_map.EnumKeys([&](const int& key) {
        cout << key << " ";
        ++count;
    });
    cout << endl;
    assert(count == 4);
    cout << "  ✓ Const EnumKeys works" << endl;

    cout << "\n[2.6] EnumValues:" << endl;
    cout << "  Values: ";
    count = 0;
    map.EnumValues([&](string& value) {
        cout << value << " ";
        ++count;
    });
    cout << endl;
    assert(count == 4);
    cout << "  ✓ EnumValues visited " << count << " values" << endl;

    cout << "\n[2.7] EnumValues const:" << endl;
    cout << "  Values (const): ";
    count = 0;
    const_map.EnumValues([&](const string& value) {
        cout << value << " ";
        ++count;
    });
    cout << endl;
    assert(count == 4);
    cout << "  ✓ Const EnumValues works" << endl;

    cout << "\n[2.8] EnumValues with modification:" << endl;
    map.EnumValues([](string& value) {
        value = value + "_modified";
    });
    string v;
    assert(map.Get(1, v) && v == "one_modified");
    cout << "  ✓ Values modified through EnumValues" << endl;

    cout << "\n[2.9] Range-based for loop (begin/end):" << endl;
    cout << "  Iteration: ";
    count = 0;
    for (auto& [key, value] : map)
    {
        cout << "[" << key << "] ";
        ++count;
    }
    cout << endl;
    assert(count == 4);
    cout << "  ✓ Range-based for works" << endl;

    cout << "\n[2.10] Empty map enumeration:" << endl;
    UnorderedMap<int, string> empty_map;
    count = 0;
    empty_map.EnumKV([&](const int& key, const string& value) {
        ++count;
    });
    assert(count == 0);
    cout << "  ✓ Empty map enumeration safe" << endl;

    cout << "\n✅ TEST 02 PASSED" << endl;
    return 0;
}
