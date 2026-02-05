/**
 * @file PerfectHashUnorderedMapBasic.cpp
 * @brief 测试 FlatPerfectHashUnorderedMap 基本功能
 */
#include <hgl/type/FlatPerfectHashUnorderedMap.h>
#include <hgl/platform/Platform.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================================" << endl;
    cout << "TEST: FlatPerfectHashUnorderedMap Basic Operations" << endl;
    cout << "========================================================" << endl;

    FlatPerfectHashUnorderedMap<int, float> map;

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
    assert(map.GetCount() == 3);
    cout << "  ✓ Added 3 items" << endl;

    // Test 3: O(1) lookup
    cout << "\n[3] O(1) hash lookup:" << endl;
    float value;
    assert(map.Get(1, value) && value == 1.1f);
    assert(map.Get(3, value) && value == 3.3f);
    assert(map.Get(5, value) && value == 5.5f);
    assert(!map.Get(99, value));
    cout << "  ✓ Hash-based lookups work" << endl;

    // Test 4: Contains check
    cout << "\n[4] ContainsKey:" << endl;
    assert(map.ContainsKey(1));
    assert(map.ContainsKey(3));
    assert(map.ContainsKey(5));
    assert(!map.ContainsKey(10));
    cout << "  ✓ ContainsKey checks work" << endl;

    // Test 5: Duplicate rejection
    cout << "\n[5] Duplicate rejection:" << endl;
    assert(!map.Add(3, 3.9f));
    assert(map.GetCount() == 3);
    cout << "  ✓ Duplicates rejected" << endl;

    // Test 6: AddOrUpdate
    cout << "\n[6] AddOrUpdate:" << endl;
    assert(map.AddOrUpdate(6, 6.6f));  // New
    assert(!map.AddOrUpdate(3, 3.0f)); // Update
    assert(map.Get(3, value) && value == 3.0f);
    assert(map.GetCount() == 4);
    cout << "  ✓ AddOrUpdate works" << endl;

    // Test 7: Delete (swap-and-pop)
    cout << "\n[7] Delete operations:" << endl;
    int old_count = map.GetCount();
    assert(map.DeleteByKey(3));
    assert(!map.ContainsKey(3));
    assert(map.GetCount() == old_count - 1);
    cout << "  ✓ DeleteByKey works (swap-and-pop)" << endl;

    // Test 8: Enumeration
    cout << "\n[8] Enumeration:" << endl;
    int count = 0;
    map.EnumKV([&count](const int& k, const float& v) {
        count++;
        cout << "  Key=" << k << ", Value=" << v << endl;
    });
    assert(count == map.GetCount());
    cout << "  ✓ Enumerated " << count << " items" << endl;

    // Test 9: Clear
    cout << "\n[9] Clear:" << endl;
    map.Clear();
    assert(map.IsEmpty());
    cout << "  ✓ Clear empties the map" << endl;

    // Test 10: Large dataset
    cout << "\n[10] Large dataset (1000 elements):" << endl;
    for (int i = 0; i < 1000; ++i)
    {
        assert(map.Add(i, static_cast<float>(i) * 2.0f));
    }
    assert(map.GetCount() == 1000);
    
    // Verify O(1) lookups
    for (int i = 0; i < 1000; i += 100)
    {
        assert(map.Get(i, value) && value == static_cast<float>(i) * 2.0f);
    }
    cout << "  ✓ Large dataset with O(1) lookups" << endl;

    cout << "\n✅ ALL TESTS PASSED" << endl;
    return 0;
}
