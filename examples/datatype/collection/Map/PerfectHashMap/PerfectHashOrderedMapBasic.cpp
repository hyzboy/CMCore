/**
 * @file PerfectHashOrderedMapBasic.cpp
 * @brief 测试 FlatPerfectHashOrderedMap 基本功能
 */
#include <hgl/type/FlatPerfectHashOrderedMap.h>
#include <hgl/platform/Platform.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================================" << endl;
    cout << "TEST: FlatPerfectHashOrderedMap Basic Operations" << endl;
    cout << "========================================================" << endl;

    FlatPerfectHashOrderedMap<int, float> map;

    // Test 1: Empty map
    cout << "\n[1] Empty map checks:" << endl;
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  ✓ Empty map verified" << endl;

    // Test 2: Add operations
    cout << "\n[2] Add operations:" << endl;
    assert(map.Add(5, 5.5f));
    assert(map.Add(1, 1.1f));
    assert(map.Add(3, 3.3f));
    assert(map.Add(2, 2.2f));
    assert(map.Add(4, 4.4f));
    assert(map.GetCount() == 5);
    cout << "  ✓ Added 5 items in random order" << endl;

    // Test 3: Verify sorting (keys should be ordered)
    cout << "\n[3] Verify sorting:" << endl;
    assert(map.GetKeyAt(0) == 1);
    assert(map.GetKeyAt(1) == 2);
    assert(map.GetKeyAt(2) == 3);
    assert(map.GetKeyAt(3) == 4);
    assert(map.GetKeyAt(4) == 5);
    cout << "  ✓ Keys sorted: [1, 2, 3, 4, 5]" << endl;

    // Test 4: O(1) lookup with hash
    cout << "\n[4] O(1) hash lookup:" << endl;
    float value;
    assert(map.Get(1, value) && value == 1.1f);
    assert(map.Get(3, value) && value == 3.3f);
    assert(map.Get(5, value) && value == 5.5f);
    assert(!map.Get(99, value));
    cout << "  ✓ Hash-based lookups work correctly" << endl;

    // Test 5: Contains check
    cout << "\n[5] ContainsKey:" << endl;
    assert(map.ContainsKey(1));
    assert(map.ContainsKey(3));
    assert(map.ContainsKey(5));
    assert(!map.ContainsKey(10));
    assert(!map.ContainsKey(0));
    cout << "  ✓ ContainsKey checks work" << endl;

    // Test 6: Value pointers
    cout << "\n[6] Value pointers:" << endl;
    float* ptr = map.GetValuePointer(3);
    assert(ptr != nullptr && *ptr == 3.3f);
    *ptr = 3.9f;  // Modify through pointer
    assert(map.Get(3, value) && value == 3.9f);
    cout << "  ✓ Value pointers work and allow modification" << endl;

    // Test 7: AddOrUpdate
    cout << "\n[7] AddOrUpdate:" << endl;
    assert(map.AddOrUpdate(6, 6.6f));  // New key
    assert(!map.AddOrUpdate(3, 3.0f)); // Existing key, should update
    assert(map.Get(3, value) && value == 3.0f);
    assert(map.GetCount() == 6);
    cout << "  ✓ AddOrUpdate works for both new and existing keys" << endl;

    // Test 8: Change
    cout << "\n[8] Change:" << endl;
    assert(map.Change(2, 2.8f));
    assert(map.Get(2, value) && value == 2.8f);
    assert(!map.Change(99, 1.0f));
    cout << "  ✓ Change modifies existing values" << endl;

    // Test 9: Delete operations
    cout << "\n[9] Delete operations:" << endl;
    assert(map.DeleteByKey(3));
    assert(!map.ContainsKey(3));
    assert(map.GetCount() == 5);
    assert(!map.DeleteByKey(99));
    cout << "  ✓ DeleteByKey works correctly" << endl;

    // Test 10: Enum operations
    cout << "\n[10] Enumeration:" << endl;
    int count = 0;
    map.EnumKV([&count](const int& k, const float& v) {
        count++;
        cout << "  Key=" << k << ", Value=" << v << endl;
    });
    assert(count == 5);
    cout << "  ✓ Enumeration visited all " << count << " items" << endl;

    // Test 11: Clear
    cout << "\n[11] Clear:" << endl;
    map.Clear();
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  ✓ Clear empties the map" << endl;

    // Test 12: Large dataset performance
    cout << "\n[12] Large dataset (1000 elements):" << endl;
    for (int i = 0; i < 1000; ++i)
    {
        assert(map.Add(i, static_cast<float>(i) * 1.5f));
    }
    assert(map.GetCount() == 1000);
    
    // Verify O(1) lookups work
    for (int i = 0; i < 1000; i += 100)
    {
        assert(map.Get(i, value) && value == static_cast<float>(i) * 1.5f);
    }
    cout << "  ✓ Large dataset handled correctly with O(1) lookups" << endl;

    cout << "\n✅ ALL TESTS PASSED" << endl;
    return 0;
}
