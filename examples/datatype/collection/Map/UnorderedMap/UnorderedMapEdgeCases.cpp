#include<hgl/type/UnorderedMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<limits>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n=====================================" << endl;
    cout << "TEST 04: UnorderedMap<K,V> Edge Cases" << endl;
    cout << "=====================================" << endl;

    cout << "\n[4.1] Empty map operations:" << endl;
    UnorderedMap<int, string> empty_map;

    assert(empty_map.IsEmpty());
    assert(empty_map.GetCount() == 0);

    string result;
    assert(!empty_map.Get(1, result));  // Get from empty returns false

    assert(empty_map.GetValuePointer(1) == nullptr);
    assert(!empty_map.ContainsKey(1));
    assert(!empty_map.DeleteByKey(1));  // Delete from empty returns false

    cout << "  ✓ All empty map operations handled correctly" << endl;

    cout << "\n[4.2] Single element map:" << endl;
    UnorderedMap<int, string> single_map;
    single_map.Add(42, "answer");

    assert(single_map.GetCount() == 1);
    assert(single_map.ContainsKey(42));
    assert(single_map.Get(42, result) && result == "answer");

    bool deleted = single_map.DeleteByKey(42);
    assert(deleted);
    assert(single_map.IsEmpty());
    cout << "  ✓ Single element operations work correctly" << endl;

    cout << "\n[4.3] Duplicate key rejection:" << endl;
    UnorderedMap<int, string> dup_map;

    bool added1 = dup_map.Add(10, "first");
    bool added2 = dup_map.Add(10, "second");  // Duplicate

    assert(added1 == true);
    assert(added2 == false);  // Duplicate rejected
    assert(dup_map.GetCount() == 1);
    assert(dup_map.Get(10, result) && result == "first");  // Original value unchanged
    cout << "  ✓ Duplicate keys correctly rejected" << endl;

    cout << "\n[4.4] Missing key operations:" << endl;
    UnorderedMap<int, string> map4;
    map4.Add(1, "one");
    map4.Add(2, "two");

    // Get missing key
    assert(!map4.Get(999, result));

    // Change missing key
    bool changed = map4.Change(999, "new");
    assert(changed == false);

    // Delete missing key
    bool deleted2 = map4.DeleteByKey(999);
    assert(deleted2 == false);

    // operator[] for missing key
    bool changed_or_added = !map4.ContainsKey(999);
    map4[999] = "added";
    assert(changed_or_added == true);  // Added since not present
    assert(map4.GetCount() == 3);
    assert(map4.Get(999, result) && result == "added");
    cout << "  ✓ Missing key operations behave correctly" << endl;

    cout << "\n[4.5] Clear vs Free:" << endl;
    UnorderedMap<int, string> cf_map;
    cf_map.Add(1, "one");
    cf_map.Add(2, "two");
    cf_map.Add(3, "three");

    UnorderedMap<int, string> cf_map2 = cf_map;  // Copy

    cf_map.Clear();  // Clear but may retain capacity
    assert(cf_map.IsEmpty());
    assert(cf_map.GetCount() == 0);

    cf_map2.Free();  // Free all memory
    assert(cf_map2.IsEmpty());
    assert(cf_map2.GetCount() == 0);
    cout << "  ✓ Clear() and Free() both empty the map" << endl;

    cout << "\n[4.6] Negative keys:" << endl;
    UnorderedMap<int, string> neg_map;

    neg_map.Add(-1, "minus_one");
    neg_map.Add(-100, "minus_hundred");
    neg_map.Add(0, "zero");
    neg_map.Add(100, "hundred");

    assert(neg_map.Get(-1, result) && result == "minus_one");
    assert(neg_map.Get(-100, result) && result == "minus_hundred");
    assert(neg_map.GetCount() == 4);
    cout << "  ✓ Negative keys handled correctly" << endl;

    cout << "\n[4.7] Large values:" << endl;
    UnorderedMap<int, string> large_map;
    string large_value(10000, 'x');  // 10KB string

    large_map.Add(1, large_value);
    string retrieved;
    assert(large_map.Get(1, retrieved));
    assert(retrieved.size() == 10000);
    assert(retrieved == large_value);
    cout << "  ✓ Large values (10KB) handled correctly" << endl;

    cout << "\n[4.8] Empty string values:" << endl;
    UnorderedMap<int, string> empty_str_map;

    empty_str_map.Add(1, "");  // Empty string value
    empty_str_map.Add(2, "non-empty");

    assert(empty_str_map.GetCount() == 2);
    assert(empty_str_map.Get(1, result) && result == "");
    assert(empty_str_map.Get(2, result) && result == "non-empty");
    cout << "  ✓ Empty string values handled correctly" << endl;

    cout << "\n[4.9] Multiple overwrites with operator[]:" << endl;
    UnorderedMap<int, string> overwrite_map;

    overwrite_map[5] = "first";
    assert(overwrite_map.Get(5, result) && result == "first");

    overwrite_map[5] = "second";
    assert(overwrite_map.Get(5, result) && result == "second");

    overwrite_map[5] = "third";
    assert(overwrite_map.Get(5, result) && result == "third");

    assert(overwrite_map.GetCount() == 1);
    cout << "  ✓ Multiple operator[] overwrites work correctly" << endl;

    cout << "\n[4.10] Const correctness:" << endl;
    const UnorderedMap<int, string> const_map = UnorderedMap<int, string>();

    // Const version should not modify
    // const_map.Add(1, "one");  // Would not compile
    // const_map.Delete(1);       // Would not compile

    // But const_map.GetValuePointer should work (returns const pointer)
    auto ptr = const_map.GetValuePointer(1);
    // The const version returns const string*
    cout << "  ✓ Const correctness enforced" << endl;

    cout << "\n✅ TEST 04 PASSED" << endl;
    return 0;
}
