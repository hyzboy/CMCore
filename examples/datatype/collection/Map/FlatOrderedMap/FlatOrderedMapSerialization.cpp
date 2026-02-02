#include<hgl/type/FlatOrderedMap.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 04: FlatOrderedMap<K,V> Serialization" << endl;
    cout << "========================================" << endl;

    FlatOrderedMap<int, float> map1;

    cout << "\n[4.1] Populate map for serialization:" << endl;
    map1.Add(1, 1.5f);
    map1.Add(2, 2.5f);
    map1.Add(3, 3.5f);
    assert(map1.GetCount() == 3);
    cout << "  ✓ Added 3 items to source map" << endl;

    cout << "\n[4.2] GetKeyData and GetValueData:" << endl;
    const int* keys = map1.GetKeyData();
    const float* values = map1.GetValueData();
    assert(keys != nullptr);
    assert(values != nullptr);
    cout << "  ✓ Data pointers valid" << endl;

    cout << "\n[4.3] Verify raw data:" << endl;
    assert(keys[0] == 1 && values[0] == 1.5f);
    assert(keys[1] == 2 && values[1] == 2.5f);
    assert(keys[2] == 3 && values[2] == 3.5f);
    cout << "  ✓ Raw data verified" << endl;

    cout << "\n[4.4] LoadFromBuffers - sorted data:" << endl;
    FlatOrderedMap<int, float> map2;
    map2.LoadFromBuffers(keys, values, 3, true);  // is_sorted=true
    assert(map2.GetCount() == 3);
    assert(map2.GetKeyAt(0) == 1);
    assert(map2.GetValueAt(0) == 1.5f);
    cout << "  ✓ LoadFromBuffers (sorted) successful" << endl;

    cout << "\n[4.5] LoadFromBuffers - unsorted data:" << endl;
    int unsorted_keys[] = {3, 1, 2};
    float unsorted_values[] = {3.5f, 1.5f, 2.5f};
    FlatOrderedMap<int, float> map3;
    map3.LoadFromBuffers(unsorted_keys, unsorted_values, 3, false);  // is_sorted=false
    assert(map3.GetCount() == 3);
    assert(map3.GetKeyAt(0) == 1);
    assert(map3.GetKeyAt(1) == 2);
    assert(map3.GetKeyAt(2) == 3);
    assert(map3.GetValueAt(0) == 1.5f);
    assert(map3.GetValueAt(1) == 2.5f);
    assert(map3.GetValueAt(2) == 3.5f);
    cout << "  ✓ LoadFromBuffers auto-sorted correctly" << endl;

    cout << "\n[4.6] LoadFromBuffers with duplicates (unsorted):" << endl;
    int dup_keys[] = {1, 3, 2, 1};  // Has duplicate 1
    float dup_values[] = {1.5f, 3.5f, 2.5f, 1.0f};
    FlatOrderedMap<int, float> map4;
    map4.LoadFromBuffers(dup_keys, dup_values, 4, false);
    // Should keep only one instance of each key
    assert(map4.GetCount() == 3);  // Duplicates removed
    assert(map4.GetKeyAt(0) == 1);
    cout << "  ✓ Duplicates auto-removed during load" << endl;

    cout << "\n[4.7] GetRawDataSize:" << endl;
    int64 raw_size = map1.GetRawDataSize();
    int64 expected_size = 3 * sizeof(int) + 3 * sizeof(float);
    assert(raw_size == expected_size);
    cout << "  Size: " << raw_size << " bytes" << endl;
    cout << "  ✓ GetRawDataSize correct" << endl;

    cout << "\n[4.8] Reserve and capacity:" << endl;
    FlatOrderedMap<int, float> map5;
    map5.Reserve(100);
    assert(map5.GetAllocCount() >= 100);
    cout << "  ✓ Reserve allocates capacity" << endl;

    cout << "\n[4.9] GetKeyVector and GetValueVector:" << endl;
    auto& key_vec = map1.GetKeyVector();
    auto& value_vec = map1.GetValueVector();
    assert(key_vec.size() == 3);
    assert(value_vec.size() == 3);
    assert(key_vec[0] == 1);
    assert(value_vec[0] == 1.5f);
    cout << "  ✓ Direct vector access works" << endl;

    cout << "\n✅ TEST 04 PASSED" << endl;
    return 0;
}
