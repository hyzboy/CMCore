#include<hgl/type/UnorderedMap.h>
#include<iostream>
#include<cassert>
#include<string>
#include<vector>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 03: UnorderedMap<K,V> Serialization" << endl;
    cout << "========================================" << endl;

    UnorderedMap<int, string> map;

    cout << "\n[3.1] Populate source map:" << endl;
    map.Add(1, "one");
    map.Add(2, "two");
    map.Add(3, "three");
    assert(map.GetCount() == 3);
    cout << "  ✓ Added 3 items" << endl;

    cout << "\n[3.2] GetKeyArray and GetValueArray:" << endl;
    vector<int> keys;
    vector<string> values;

    int key_count = map.GetKeyArray(keys);
    int value_count = map.GetValueArray(values);

    assert(key_count == 3);
    assert(value_count == 3);
    assert(keys.size() == 3);
    assert(values.size() == 3);
    cout << "  ✓ GetKeyArray returned " << key_count << " keys" << endl;
    cout << "  ✓ GetValueArray returned " << value_count << " values" << endl;

    cout << "\n[3.3] Verify extracted data:" << endl;
    cout << "  Keys: ";
    for (int k : keys) cout << k << " ";
    cout << endl;
    cout << "  Values: ";
    for (const auto& v : values) cout << v << " ";
    cout << endl;
    cout << "  ✓ Data extraction successful" << endl;

    cout << "\n[3.4] GetKeyValueArrays (combined):" << endl;
    vector<int> combined_keys;
    vector<string> combined_values;
    int count = map.GetKeyValueArrays(combined_keys, combined_values);
    assert(count == 3);
    assert(combined_keys.size() == combined_values.size());
    cout << "  ✓ GetKeyValueArrays returned " << count << " pairs" << endl;

    cout << "\n[3.5] RebuildFromArrays - clear and rebuild:" << endl;
    UnorderedMap<int, string> map2;
    map2.Add(99, "should_be_removed");

    int rebuilt_count = map2.RebuildFromArrays(keys, values);
    assert(rebuilt_count == 3);
    assert(map2.GetCount() == 3);
    assert(!map2.ContainsKey(99));  // Old data cleared

    string val;
    assert(map2.Get(1, val) && val == "one");
    assert(map2.Get(2, val) && val == "two");
    assert(map2.Get(3, val) && val == "three");
    cout << "  ✓ RebuildFromArrays successful" << endl;

    cout << "\n[3.6] AddFromArrays - add without clearing:" << endl;
    UnorderedMap<int, string> map3;
    map3.Add(0, "zero");  // Pre-existing data

    int added_count = map3.AddFromArrays(keys, values);
    assert(added_count == 3);
    assert(map3.GetCount() == 4);  // 1 existing + 3 added
    assert(map3.Get(0, val) && val == "zero");
    assert(map3.Get(1, val) && val == "one");
    cout << "  ✓ AddFromArrays added " << added_count << " items" << endl;

    cout << "\n[3.7] AddFromArrays with duplicates:" << endl;
    UnorderedMap<int, string> map4;
    map4.Add(1, "existing_one");

    vector<int> dup_keys = {1, 2, 3};  // 1 is duplicate
    vector<string> dup_values = {"new_one", "new_two", "new_three"};

    int dup_added = map4.AddFromArrays(dup_keys, dup_values);
    assert(dup_added == 2);  // Only 2 and 3 added
    assert(map4.GetCount() == 3);
    assert(map4.Get(1, val) && val == "existing_one");  // Unchanged
    assert(map4.Get(2, val) && val == "new_two");
    cout << "  ✓ AddFromArrays correctly rejected duplicate key 1" << endl;

    cout << "\n[3.8] Empty array handling:" << endl;
    vector<int> empty_keys;
    vector<string> empty_values;

    int empty_count = map.RebuildFromArrays(empty_keys, empty_values);
    assert(empty_count == 0);
    cout << "  ✓ Empty arrays handled safely" << endl;

    cout << "\n[3.9] Mismatched array sizes:" << endl;
    vector<int> short_keys = {1, 2};
    vector<string> long_values = {"one", "two", "three", "four"};

    UnorderedMap<int, string> map5;
    int mismatched_count = map5.RebuildFromArrays(short_keys, long_values);
    assert(mismatched_count == 2);  // Uses minimum size
    assert(map5.GetCount() == 2);
    cout << "  ✓ Mismatched sizes handled correctly" << endl;

    cout << "\n[3.10] Free memory:" << endl;
    UnorderedMap<int, string> map6;
    map6.Add(1, "one");
    map6.Add(2, "two");
    assert(map6.GetCount() == 2);

    map6.Free();
    assert(map6.IsEmpty());
    assert(map6.GetCount() == 0);
    cout << "  ✓ Free() clears and releases memory" << endl;

    cout << "\n✅ TEST 03 PASSED" << endl;
    return 0;
}
