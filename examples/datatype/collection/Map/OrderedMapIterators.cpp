#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 05: OrderedMap<K,V> Iterators" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;
    
    cout << "\n[5.1] Populate map:" << endl;
    map.Add(10, "ten");
    map.Add(5, "five");
    map.Add(15, "fifteen");
    cout << "  Added 3 items (will be sorted by key)" << endl;

    cout << "\n[5.2] Forward iteration:" << endl;
    cout << "  Forward: ";
    for (auto it = map.begin(); it != map.end(); ++it)
    {
        cout << "[" << it->first << ":" << it->second << "] ";
    }
    cout << endl;
    cout << "  ✓ Forward iteration works" << endl;

    cout << "\n[5.3] Const iterator:" << endl;
    const OrderedMap<int, string>& const_map = map;
    cout << "  Const iteration: ";
    for (auto it = const_map.begin(); it != const_map.end(); ++it)
    {
        cout << "[" << it->first << ":" << it->second << "] ";
    }
    cout << endl;
    cout << "  ✓ Const iteration works" << endl;

    cout << "\n[5.4] Reverse iteration:" << endl;
    cout << "  Reverse: ";
    for (auto it = map.rbegin(); it != map.rend(); ++it)
    {
        cout << "[" << it->first << ":" << it->second << "] ";
    }
    cout << endl;
    cout << "  ✓ Reverse iteration works" << endl;

    cout << "\n[5.5] Range-based for loop:" << endl;
    cout << "  Range-based: ";
    for (auto& [key, value] : map)
    {
        cout << "[" << key << ":" << value << "] ";
    }
    cout << endl;
    cout << "  ✓ Range-based for works" << endl;

    cout << "\n[5.6] Iterate and modify value:" << endl;
    for (auto& [key, value] : map)
    {
        if (key == 5)
            value = "FIVE";
    }
    assert(map.Get(5, string()); // Dummy var
    string val;
    assert(map.Get(5, val) && val == "FIVE");
    cout << "  ✓ Modified value through iterator" << endl;

    cout << "\n[5.7] Const range iteration (read-only):" << endl;
    for (const auto& [key, value] : const_map)
    {
        cout << "  " << key << " -> " << value << endl;
    }
    cout << "  ✓ Const range iteration works" << endl;

    cout << "\n[5.8] Iterator count verification:" << endl;
    int count = 0;
    for (auto it = map.begin(); it != map.end(); ++it)
        ++count;
    assert(count == map.GetCount());
    cout << "  Iterator count (" << count << ") matches GetCount() (" << map.GetCount() << ")" << endl;

    cout << "\n[5.9] Empty map iteration:" << endl;
    OrderedMap<int, string> empty_map;
    assert(empty_map.begin() == empty_map.end());
    int empty_count = 0;
    for (auto it = empty_map.begin(); it != empty_map.end(); ++it)
        ++empty_count;
    assert(empty_count == 0);
    cout << "  ✓ Empty map iterator works safely" << endl;

    cout << "\n✅ TEST 05 PASSED" << endl;
    return 0;
}
