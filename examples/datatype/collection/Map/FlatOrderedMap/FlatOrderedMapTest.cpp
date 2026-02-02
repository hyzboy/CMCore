#include <iostream>
#include <cassert>
#include <hgl/type/FlatOrderedMap.h>

using namespace hgl;
using namespace std;

void TestBasicOperations()
{
    cout << "Testing basic operations..." << endl;

    FlatOrderedMap<int, float> map;

    // Test empty
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);

    // Test Add
    assert(map.Add(1, 1.5f) == true);
    assert(map.Add(3, 3.5f) == true);
    assert(map.Add(2, 2.5f) == true);
    assert(map.GetCount() == 3);

    // Test duplicate key
    assert(map.Add(2, 2.5f) == false);
    assert(map.GetCount() == 3);

    cout << "  ✓ Basic operations passed" << endl;
}

void TestOrdering()
{
    cout << "Testing ordering..." << endl;

    FlatOrderedMap<int, float> map;

    // Insert in random order
    map.Add(5, 5.5f);
    map.Add(2, 2.5f);
    map.Add(8, 8.5f);
    map.Add(1, 1.5f);
    map.Add(9, 9.5f);

    // Verify keys are in ascending order
    int prev_key = -1;
    map.EnumKeys([&](const int& key) {
        assert(key > prev_key);
        prev_key = key;
    });

    cout << "  ✓ Ordering test passed" << endl;
}

void TestFind()
{
    cout << "Testing find operations..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(10, 10.5f);
    map.Add(20, 20.5f);
    map.Add(30, 30.5f);

    // Test Find by key
    assert(map.Find(10) == 0);
    assert(map.Find(20) == 1);
    assert(map.Find(30) == 2);
    assert(map.Find(40) == -1);

    // Test ContainsKey
    assert(map.ContainsKey(10) == true);
    assert(map.ContainsKey(40) == false);

    // Test Get
    float value;
    assert(map.Get(20, value) == true);
    assert(value == 20.5f);

    assert(map.Get(99, value) == false);

    cout << "  ✓ Find operations passed" << endl;
}

void TestPointerAccess()
{
    cout << "Testing pointer access..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);

    // Test GetValuePointer
    float* ptr = map.GetValuePointer(1);
    assert(ptr != nullptr);
    assert(*ptr == 1.5f);

    ptr = map.GetValuePointer(99);
    assert(ptr == nullptr);

    // Test GetValuePointerAt
    ptr = map.GetValuePointerAt(0);
    assert(ptr != nullptr);
    assert(*ptr == 1.5f);

    ptr = map.GetValuePointerAt(10);
    assert(ptr == nullptr);

    cout << "  ✓ Pointer access passed" << endl;
}

void TestIndexAccess()
{
    cout << "Testing index access..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(100, 100.5f);
    map.Add(200, 200.5f);
    map.Add(300, 300.5f);

    // Test GetKeyAt
    assert(map.GetKeyAt(0) == 100);
    assert(map.GetKeyAt(1) == 200);
    assert(map.GetKeyAt(2) == 300);
    assert(map.GetKeyAt(10) == 0);  // Out of range

    // Test GetValueAt
    assert(map.GetValueAt(0) == 100.5f);
    assert(map.GetValueAt(1) == 200.5f);
    assert(map.GetValueAt(2) == 300.5f);

    cout << "  ✓ Index access passed" << endl;
}

void TestDelete()
{
    cout << "Testing delete operations..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);
    map.Add(3, 3.5f);
    map.Add(4, 4.5f);

    // Test DeleteByKey
    assert(map.DeleteByKey(2) == true);
    assert(map.GetCount() == 3);
    assert(map.ContainsKey(2) == false);
    assert(map.ContainsKey(1) == true);

    // Test DeleteAt
    assert(map.DeleteAt(0) == true);  // Delete key 1
    assert(map.GetCount() == 2);
    assert(map.GetKeyAt(0) == 3);

    // Test DeleteAt range
    map.Add(1, 1.5f);
    assert(map.GetCount() == 3);
    assert(map.DeleteAt(0, 2) == true);
    assert(map.GetCount() == 1);
    assert(map.GetKeyAt(0) == 4);

    cout << "  ✓ Delete operations passed" << endl;
}

void TestChange()
{
    cout << "Testing change operations..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);

    // Test Change
    assert(map.Change(1, 1.9f) == true);
    float value;
    map.Get(1, value);
    assert(value == 1.9f);

    assert(map.Change(99, 99.9f) == false);

    // Test AddOrUpdate
    bool is_new = map.AddOrUpdate(2, 2.9f);
    assert(is_new == false);  // Updated
    map.Get(2, value);
    assert(value == 2.9f);

    is_new = map.AddOrUpdate(3, 3.5f);
    assert(is_new == true);  // Added
    assert(map.GetCount() == 3);

    cout << "  ✓ Change operations passed" << endl;
}

void TestGetAndDelete()
{
    cout << "Testing GetAndDelete..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);

    float value;
    assert(map.GetAndDelete(1, value) == true);
    assert(value == 1.5f);
    assert(map.GetCount() == 1);
    assert(map.ContainsKey(1) == false);

    assert(map.GetAndDelete(99, value) == false);

    cout << "  ✓ GetAndDelete passed" << endl;
}

void TestByValue()
{
    cout << "Testing find by value..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(10, 1.5f);
    map.Add(20, 2.5f);
    map.Add(30, 3.5f);

    // Test FindByValue
    assert(map.FindByValue(2.5f) == 1);
    assert(map.FindByValue(99.9f) == -1);

    // Test ContainsValue
    assert(map.ContainsValue(1.5f) == true);
    assert(map.ContainsValue(99.9f) == false);

    // Test DeleteByValue
    assert(map.DeleteByValue(2.5f) == true);
    assert(map.GetCount() == 2);
    assert(map.ContainsValue(2.5f) == false);

    cout << "  ✓ Find by value passed" << endl;
}

void TestSerialization()
{
    cout << "Testing serialization..." << endl;

    FlatOrderedMap<int, float> map1;
    map1.Add(1, 1.5f);
    map1.Add(2, 2.5f);
    map1.Add(3, 3.5f);

    // Get raw data
    const int* keys = map1.GetKeyData();
    const float* values = map1.GetValueData();
    int count = static_cast<int>(map1.GetCount());

    assert(keys != nullptr);
    assert(values != nullptr);
    assert(count == 3);

    // Verify data
    assert(keys[0] == 1 && values[0] == 1.5f);
    assert(keys[1] == 2 && values[1] == 2.5f);
    assert(keys[2] == 3 && values[2] == 3.5f);

    // Load from buffers
    FlatOrderedMap<int, float> map2;
    map2.LoadFromBuffers(keys, values, count, true);

    assert(map2.GetCount() == 3);
    assert(map2.GetKeyAt(0) == 1);
    assert(map2.GetValueAt(0) == 1.5f);

    cout << "  ✓ Serialization passed" << endl;
}

void TestSerializationUnsorted()
{
    cout << "Testing serialization with unsorted data..." << endl;

    int unsorted_keys[] = {3, 1, 2};
    float unsorted_values[] = {3.5f, 1.5f, 2.5f};

    FlatOrderedMap<int, float> map;
    map.LoadFromBuffers(unsorted_keys, unsorted_values, 3, false);

    // Should be sorted now
    assert(map.GetKeyAt(0) == 1);
    assert(map.GetKeyAt(1) == 2);
    assert(map.GetKeyAt(2) == 3);

    assert(map.GetValueAt(0) == 1.5f);
    assert(map.GetValueAt(1) == 2.5f);
    assert(map.GetValueAt(2) == 3.5f);

    cout << "  ✓ Unsorted serialization passed" << endl;
}

void TestEnumeration()
{
    cout << "Testing enumeration..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);
    map.Add(3, 3.5f);

    int key_sum = 0;
    float value_sum = 0.0f;

    // Test EnumKV
    map.EnumKV([&](const int& k, const float& v) {
        key_sum += k;
        value_sum += v;
    });

    assert(key_sum == 6);   // 1+2+3
    assert(value_sum == 7.5f);  // 1.5+2.5+3.5

    // Test EnumKeys
    int key_count = 0;
    map.EnumKeys([&](const int& k) {
        key_count++;
    });
    assert(key_count == 3);

    // Test EnumValues
    int value_count = 0;
    map.EnumValues([&](float& v) {
        value_count++;
    });
    assert(value_count == 3);

    cout << "  ✓ Enumeration passed" << endl;
}

void TestClear()
{
    cout << "Testing clear..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);

    assert(map.GetCount() == 2);

    map.Clear();

    assert(map.IsEmpty() == true);
    assert(map.GetCount() == 0);
    assert(map.ContainsKey(1) == false);

    // Can add again after clear
    assert(map.Add(3, 3.5f) == true);
    assert(map.GetCount() == 1);

    cout << "  ✓ Clear passed" << endl;
}

void TestGetBySerial()
{
    cout << "Testing GetBySerial..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(10, 10.5f);
    map.Add(20, 20.5f);
    map.Add(30, 30.5f);

    int key;
    float value;

    assert(map.GetBySerial(0, key, value) == true);
    assert(key == 10 && value == 10.5f);

    assert(map.GetBySerial(1, key, value) == true);
    assert(key == 20 && value == 20.5f);

    assert(map.GetBySerial(10, key, value) == false);  // Out of range

    cout << "  ✓ GetBySerial passed" << endl;
}

void TestReserve()
{
    cout << "Testing Reserve..." << endl;

    FlatOrderedMap<int, float> map;

    map.Reserve(100);
    assert(map.GetAllocCount() >= 100);

    // Add some elements
    map.Add(1, 1.5f);
    assert(map.GetCount() == 1);
    assert(map.GetAllocCount() >= 100);  // Should still have reserved capacity

    cout << "  ✓ Reserve passed" << endl;
}

void TestDirectVectorAccess()
{
    cout << "Testing direct vector access..." << endl;

    FlatOrderedMap<int, float> map;

    map.Add(1, 1.5f);
    map.Add(2, 2.5f);

    auto& key_vec = map.GetKeyVector();
    auto& value_vec = map.GetValueVector();

    assert(key_vec.size() == 2);
    assert(value_vec.size() == 2);
    assert(key_vec[0] == 1);
    assert(value_vec[0] == 1.5f);

    cout << "  ✓ Direct vector access passed" << endl;
}

void TestLargeDataset()
{
    cout << "Testing with large dataset..." << endl;

    FlatOrderedMap<int, int> map;
    const int count = 1000;

    // Add elements in random-like order
    for (int i = 0; i < count; ++i)
    {
        int key = (i * 17 + 5) % count;  // Pseudo-random sequence
        map.Add(key, key * 2);
    }

    // Verify all are added (or duplicates rejected)
    assert(map.GetCount() <= count);

    // Verify ordering
    int prev = -1;
    map.EnumKeys([&](const int& k) {
        assert(k > prev);
        prev = k;
    });

    // Verify we can find them
    for (int i = 0; i < count; ++i)
    {
        if (map.ContainsKey(i))
        {
            int value;
            map.Get(i, value);
            assert(value == i * 2);
        }
    }

    cout << "  ✓ Large dataset passed" << endl;
}

int main()
{
    cout << "================================================" << endl;
    cout << "FlatOrderedMap Test Suite" << endl;
    cout << "================================================" << endl;
    cout << endl;

    try
    {
        TestBasicOperations();
        TestOrdering();
        TestFind();
        TestPointerAccess();
        TestIndexAccess();
        TestDelete();
        TestChange();
        TestGetAndDelete();
        TestByValue();
        TestSerialization();
        TestSerializationUnsorted();
        TestEnumeration();
        TestClear();
        TestGetBySerial();
        TestReserve();
        TestDirectVectorAccess();
        TestLargeDataset();

        cout << endl;
        cout << "================================================" << endl;
        cout << "✓ ALL TESTS PASSED!" << endl;
        cout << "================================================" << endl;

        return 0;
    }
    catch (const exception& e)
    {
        cout << endl;
        cout << "✗ TEST FAILED: " << e.what() << endl;
        return 1;
    }
}
