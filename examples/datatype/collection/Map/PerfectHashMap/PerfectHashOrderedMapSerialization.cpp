/**
 * @file PerfectHashOrderedMapSerialization.cpp
 * @brief 测试 FlatPerfectHashOrderedMap 序列化功能
 */
#include <hgl/type/FlatPerfectHashOrderedMap.h>
#include <iostream>
#include <cassert>
#include <cstring>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================================" << endl;
    cout << "TEST: FlatPerfectHashOrderedMap Serialization" << endl;
    cout << "========================================================" << endl;

    // Test 1: Serialize and deserialize
    cout << "\n[1] Serialize map with data:" << endl;
    
    FlatPerfectHashOrderedMap<int, double> map1;
    map1.Add(10, 10.5);
    map1.Add(5, 5.25);
    map1.Add(15, 15.75);
    map1.Add(3, 3.125);
    map1.Add(20, 20.875);
    
    cout << "  Original map: " << map1.GetCount() << " elements" << endl;
    
    // Get data for serialization
    const int* keys = map1.GetKeyData();
    const double* values = map1.GetValueData();
    const int32_t* hash_table = map1.GetHashTable();
    int64 count = map1.GetCount();
    int32_t hash_size = map1.GetHashTableSize();
    
    cout << "  Serialization data:" << endl;
    cout << "    - Element count: " << count << endl;
    cout << "    - Hash table size: " << hash_size << endl;
    cout << "    - Total raw size: " << map1.GetRawDataSize() << " bytes" << endl;
    
    // Simulate file save/load by copying to buffers
    int* key_buffer = new int[count];
    double* value_buffer = new double[count];
    int32_t* hash_buffer = new int32_t[hash_size];
    
    memcpy(key_buffer, keys, count * sizeof(int));
    memcpy(value_buffer, values, count * sizeof(double));
    memcpy(hash_buffer, hash_table, hash_size * sizeof(int32_t));
    
    cout << "  ✓ Data copied to buffers (simulating file save)" << endl;
    
    // Test 2: Load into new map
    cout << "\n[2] Load into new map:" << endl;
    FlatPerfectHashOrderedMap<int, double> map2;
    map2.LoadFromBuffers(key_buffer, value_buffer, hash_buffer, count, hash_size, true);
    
    assert(map2.GetCount() == count);
    assert(map2.GetHashTableSize() == hash_size);
    cout << "  ✓ Loaded " << map2.GetCount() << " elements" << endl;
    
    // Test 3: Verify all data matches
    cout << "\n[3] Verify loaded data:" << endl;
    double val1, val2;
    for (int64 i = 0; i < count; ++i)
    {
        int key = map1.GetKeyAt(i);
        assert(map1.Get(key, val1));
        assert(map2.Get(key, val2));
        assert(val1 == val2);
    }
    cout << "  ✓ All values match original" << endl;
    
    // Test 4: Verify hash lookups work
    cout << "\n[4] Verify hash lookups:" << endl;
    assert(map2.ContainsKey(10));
    assert(map2.ContainsKey(5));
    assert(map2.ContainsKey(15));
    assert(map2.Get(10, val2) && val2 == 10.5);
    assert(map2.Get(3, val2) && val2 == 3.125);
    cout << "  ✓ Hash-based lookups work on loaded map" << endl;
    
    // Test 5: Verify ordering maintained
    cout << "\n[5] Verify ordering:" << endl;
    assert(map2.GetKeyAt(0) == 3);
    assert(map2.GetKeyAt(1) == 5);
    assert(map2.GetKeyAt(2) == 10);
    assert(map2.GetKeyAt(3) == 15);
    assert(map2.GetKeyAt(4) == 20);
    cout << "  ✓ Keys remain sorted after load" << endl;
    
    // Test 6: Load unsorted data
    cout << "\n[6] Load unsorted data:" << endl;
    int unsorted_keys[] = {50, 10, 30, 20, 40};
    double unsorted_values[] = {50.0, 10.0, 30.0, 20.0, 40.0};
    int32_t dummy_hash[16] = {-1};  // Will be rebuilt
    
    FlatPerfectHashOrderedMap<int, double> map3;
    map3.LoadFromBuffers(unsorted_keys, unsorted_values, dummy_hash, 5, 16, false);
    
    assert(map3.GetCount() == 5);
    assert(map3.GetKeyAt(0) == 10);
    assert(map3.GetKeyAt(1) == 20);
    assert(map3.GetKeyAt(2) == 30);
    assert(map3.GetKeyAt(3) == 40);
    assert(map3.GetKeyAt(4) == 50);
    cout << "  ✓ Unsorted data automatically sorted during load" << endl;
    
    // Test 7: Load with duplicates (should be removed)
    cout << "\n[7] Load with duplicates:" << endl;
    int dup_keys[] = {1, 2, 2, 3, 3, 3, 4};
    double dup_values[] = {1.0, 2.0, 2.1, 3.0, 3.1, 3.2, 4.0};
    
    FlatPerfectHashOrderedMap<int, double> map4;
    map4.LoadFromBuffers(dup_keys, dup_values, dummy_hash, 7, 16, false);
    
    assert(map4.GetCount() == 4);  // Duplicates removed
    assert(map4.ContainsKey(1));
    assert(map4.ContainsKey(2));
    assert(map4.ContainsKey(3));
    assert(map4.ContainsKey(4));
    cout << "  ✓ Duplicates removed during unsorted load" << endl;
    
    // Cleanup
    delete[] key_buffer;
    delete[] value_buffer;
    delete[] hash_buffer;
    
    cout << "\n✅ ALL SERIALIZATION TESTS PASSED" << endl;
    return 0;
}
