#include<hgl/platform/Platform.h>
#include<hgl/type/OrderedMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

struct Person
{
    string name;
    int age;

    Person() : age(0) {}
    Person(const string& n, int a) : name(n), age(a) {}

    bool operator<(const Person& other) const
    {
        return age < other.age;
    }

    bool operator==(const Person& other) const
    {
        return age == other.age && name == other.name;
    }
};

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 07: OrderedMap<K,V> Edge Cases" << endl;
    cout << "========================================" << endl;

    OrderedMap<int, string> map;

    cout << "\n[7.1] Operations on empty map:" << endl;
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    string dummy;
    assert(!map.Get(1, dummy));
    assert(!map.ContainsKey(1));
    assert(map.Find(1) == -1);
    cout << "  ✓ Empty map operations safe" << endl;

    cout << "\n[7.2] Single element map:" << endl;
    map.Add(42, "answer");
    assert(!map.IsEmpty());
    assert(map.GetCount() == 1);
    assert(map.GetKeyAt(0) == 42);
    assert(map.GetValueAt(0) == "answer");
    cout << "  ✓ Single element map works" << endl;

    cout << "\n[7.3] Delete last remaining element:" << endl;
    assert(map.DeleteByKey(42));
    assert(map.IsEmpty());
    cout << "  ✓ Map empty after deleting only element" << endl;

    cout << "\n[7.4] Negative numbers as keys:" << endl;
    map.Add(-100, "minus_hundred");
    map.Add(-10, "minus_ten");
    map.Add(0, "zero");
    map.Add(10, "ten");
    assert(map.GetCount() == 4);
    assert(map.GetKeyAt(0) == -100);
    assert(map.GetKeyAt(3) == 10);
    cout << "  ✓ Negative keys handled correctly" << endl;

    cout << "\n[7.5] Large numbers as keys:" << endl;
    OrderedMap<long long, string> large_map;
    long long large_key = 9223372036854775807LL;  // Near LLONG_MAX
    large_map.Add(large_key, "very_large");
    large_map.Add(0LL, "zero");
    assert(large_map.GetKeyAt(0) == 0);
    assert(large_map.GetKeyAt(1) == large_key);
    cout << "  ✓ Large number keys work" << endl;

    cout << "\n[7.6] Empty string as value:" << endl;
    map.Clear();
    map.Add(1, "");
    map.Add(2, "nonempty");
    string v;
    assert(map.Get(1, v) && v == "");
    assert(map.Get(2, v) && v == "nonempty");
    cout << "  ✓ Empty string values work" << endl;

    cout << "\n[7.7] Get/Delete non-existent keys:" << endl;
    assert(!map.Get(999, v));
    assert(!map.ContainsKey(999));
    assert(!map.DeleteByKey(999));
    assert(map.GetCount() == 2);  // Unchanged
    cout << "  ✓ Non-existent key operations safe" << endl;

    cout << "\n[7.8] Duplicate key handling:" << endl;
    bool added = map.Add(1, "new_value_for_1");
    assert(!added);  // Add returns false for duplicate
    assert(map.GetCount() == 2);  // Count unchanged
    assert(map.Get(1, v) && v == "");  // Original value unchanged
    cout << "  ✓ Duplicate key correctly rejected" << endl;

    cout << "\n[7.9] GetValuePointer on missing key:" << endl;
    string* ptr = map.GetValuePointer(999);
    assert(ptr == nullptr);
    cout << "  ✓ GetValuePointer returns nullptr for missing key" << endl;

    cout << "\n[7.10] Clear empty map:" << endl;
    OrderedMap<int, string> empty_map;
    empty_map.Clear();
    assert(empty_map.IsEmpty());
    cout << "  ✓ Clearing already empty map is safe" << endl;

    cout << "\n[7.11] Complex value types:" << endl;
    OrderedMap<string, Person> person_map;
    person_map.Add("alice", Person("Alice", 30));
    person_map.Add("bob", Person("Bob", 25));

    Person p;
    assert(person_map.Get("alice", p) && p.age == 30);
    cout << "  ✓ Complex value types work" << endl;

    cout << "\n[7.12] All GetBySerial invalid cases:" << endl;
    map.Clear();
    map.Add(1, "one");
    int key;
    string val;
    assert(!map.GetBySerial(-1, key, val));
    assert(!map.GetBySerial(1, key, val));
    assert(!map.GetBySerial(100, key, val));
    cout << "  ✓ Invalid serial numbers handled safely" << endl;

    cout << "\n[7.13] Iterator on modified map:" << endl;
    map.Clear();
    map.Add(1, "one");
    map.Add(2, "two");
    map.Add(3, "three");

    // Iterate and collect
    int iter_count = 0;
    for (auto it = map.begin(); it != map.end(); ++it)
        ++iter_count;
    assert(iter_count == 3);
    cout << "  ✓ Iterator count matches map size" << endl;

    cout << "\n✅ TEST 07 PASSED" << endl;
    return 0;
}
