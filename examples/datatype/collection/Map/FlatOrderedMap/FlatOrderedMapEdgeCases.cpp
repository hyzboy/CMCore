#include<hgl/platform/Platform.h>
#include<hgl/type/FlatOrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 06: FlatOrderedMap<K,V> Edge Cases" << endl;
    cout << "========================================" << endl;

    FlatOrderedMap<int, int> map;

    cout << "\n[6.1] Empty map operations:" << endl;
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    assert(map.Find(1) == -1);
    int dummy;
    assert(!map.Get(1, dummy));
    cout << "  ✓ Empty map operations safe" << endl;

    cout << "\n[6.2] Single element:" << endl;
    assert(map.Add(42, 420));
    assert(!map.IsEmpty());
    assert(map.GetCount() == 1);
    assert(map.GetKeyAt(0) == 42);
    cout << "  ✓ Single element operations work" << endl;

    cout << "\n[6.3] Delete to empty:" << endl;
    assert(map.DeleteByKey(42));
    assert(map.IsEmpty());
    cout << "  ✓ Can delete back to empty" << endl;

    cout << "\n[6.4] Negative keys:" << endl;
    map.Add(-100, -1000);
    map.Add(-10, -100);
    map.Add(0, 0);
    map.Add(10, 100);
    assert(map.GetKeyAt(0) == -100);
    assert(map.GetKeyAt(3) == 10);
    cout << "  ✓ Negative keys sorted correctly" << endl;

    cout << "\n[6.5] Zero values:" << endl;
    FlatOrderedMap<int, int> val_map;
    val_map.Add(1, 0);
    val_map.Add(2, 100);
    int v;
    assert(val_map.Get(1, v) && v == 0);
    assert(val_map.Get(2, v) && v == 100);
    cout << "  ✓ Zero values handled correctly" << endl;

    cout << "\n[6.6] Large keys:" << endl;
    FlatOrderedMap<long long, int> large_map;
    long long large_key = 9223372036854775800LL;
    large_map.Add(large_key, 1);
    large_map.Add(0LL, 2);
    assert(large_map.GetKeyAt(0) == 0);
    assert(large_map.GetKeyAt(1) == large_key);
    cout << "  ✓ Large keys work" << endl;

    cout << "\n[6.7] Duplicate key handling:" << endl;
    map.Clear();
    map.Add(1, 100);
    bool added = map.Add(1, 999);
    assert(!added);  // false = not added
    assert(map.GetCount() == 1);
    int val;
    assert(map.Get(1, val) && val == 100);  // Original value unchanged
    cout << "  ✓ Duplicate keys rejected" << endl;

    cout << "\n[6.8] Non-existent operations:" << endl;
    assert(!map.Get(999, v));
    assert(!map.ContainsKey(999));
    assert(map.Find(999) == -1);
    assert(!map.DeleteByKey(999));
    assert(!map.DeleteByValue(9999));
    assert(map.GetValuePointer(999) == nullptr);
    cout << "  ✓ Non-existent key/value operations safe" << endl;

    cout << "\n[6.9] Clear on empty:" << endl;
    FlatOrderedMap<int, int> empty_map;
    empty_map.Clear();
    assert(empty_map.IsEmpty());
    cout << "  ✓ Clear on empty map safe" << endl;

    cout << "\n[6.10] Iterator on empty:" << endl;
    assert(empty_map.begin() == empty_map.end());
    int count = 0;
    for (auto key : empty_map)
        ++count;
    assert(count == 0);
    cout << "  ✓ Empty map iterator safe" << endl;

    cout << "\n[6.11] GetAllocCount:" << endl;
    FlatOrderedMap<int, int> cap_map;
    cap_map.Reserve(50);
    assert(cap_map.GetAllocCount() >= 50);
    cap_map.Add(1, 1);
    assert(cap_map.GetCount() == 1);
    assert(cap_map.GetAllocCount() >= 50);  // Capacity preserved
    cout << "  ✓ Capacity management works" << endl;

    cout << "\n[6.12] Const correctness:" << endl;
    map.Clear();
    map.Add(1, 100);
    const FlatOrderedMap<int, int>& const_ref = map;
    assert(const_ref.Find(1) == 0);
    assert(const_ref.ContainsKey(1));
    const int* const_ptr = const_ref.GetValuePointer(1);
    assert(const_ptr != nullptr && *const_ptr == 100);
    cout << "  ✓ Const operations work" << endl;

    cout << "\n✅ TEST 06 PASSED" << endl;
    return 0;
}
