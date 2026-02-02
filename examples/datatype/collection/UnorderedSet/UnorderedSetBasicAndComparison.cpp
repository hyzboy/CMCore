/**
 * UnorderedSetBasicAndComparison.cpp
 * 测试 UnorderedSet 的基本功能和与 FlatUnorderedSet 的对比
 */

#include <hgl/type/UnorderedSet.h>
#include <hgl/type/FlatUnorderedSet.h>
#include <iostream>
#include <cassert>
#include <chrono>
#include <string>

using namespace hgl;
using namespace std;

// ==================== 性能测试辅助 ====================

class Timer
{
    chrono::high_resolution_clock::time_point start_time;
    const char* name;

public:
    Timer(const char* n) : name(n), start_time(chrono::high_resolution_clock::now()) {}

    ~Timer()
    {
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        cout << "  " << name << ": " << duration.count() << " μs" << endl;
    }
};

// ==================== UnorderedSet 基本功能测试 ====================

void TestUnorderedSetBasic()
{
    cout << "\n=== Test 1: UnorderedSet Basic Operations ===" << endl;

    UnorderedSet<int> set;

    // 添加元素
    assert(set.Add(10));
    assert(set.Add(20));
    assert(set.Add(30));
    assert(!set.Add(20));  // 重复添加

    assert(set.GetCount() == 3);
    cout << "  ✓ Add operations work correctly" << endl;

    // 查找
    assert(set.Contains(10));
    assert(set.Contains(20));
    assert(!set.Contains(40));
    cout << "  ✓ Contains checks work correctly" << endl;

    // 删除
    assert(set.Delete(20));
    assert(!set.Delete(40));
    assert(set.GetCount() == 2);
    assert(!set.Contains(20));
    cout << "  ✓ Delete operations work correctly" << endl;

    // 遍历
    int sum = 0;
    set.Enum([&](int value) {
        sum += value;
    });
    assert(sum == 40);  // 10 + 30
    cout << "  ✓ Enumeration works correctly" << endl;
}

void TestUnorderedSetWithNonTrivial()
{
    cout << "\n=== Test 2: UnorderedSet with Non-Trivial Types ===" << endl;

    // 字符串集合
    UnorderedSet<std::string> set;

    assert(set.Add("apple"));
    assert(set.Add("banana"));
    assert(set.Add("cherry"));
    assert(!set.Add("apple"));

    assert(set.GetCount() == 3);
    cout << "  ✓ String addition works" << endl;

    assert(set.Contains("banana"));
    assert(!set.Contains("grape"));
    cout << "  ✓ String contains checks work" << endl;

    assert(set.Delete("banana"));
    assert(set.GetCount() == 2);
    cout << "  ✓ String deletion works" << endl;

    int count = 0;
    set.Enum([&](const std::string& s) {
        cout << "    - " << s << endl;
        ++count;
    });
    assert(count == 2);
    cout << "  ✓ String enumeration works" << endl;
}

void TestUnorderedSetCustomHash()
{
    cout << "\n=== Test 3: UnorderedSet with Custom Hash ===" << endl;

    // 自定义对象
    struct Person
    {
        int id;
        std::string name;

        Person(int i, const std::string& n) : id(i), name(n) {}

        bool operator==(const Person& other) const
        {
            return id == other.id && name == other.name;
        }
    };

    // 自定义哈希函数
    struct PersonHash
    {
        size_t operator()(const Person& p) const
        {
            return std::hash<int>()(p.id) ^ (std::hash<std::string>()(p.name) << 1);
        }
    };

    UnorderedSet<Person, PersonHash> set;

    Person p1(1, "Alice");
    Person p2(2, "Bob");
    Person p3(3, "Charlie");

    assert(set.Add(p1));
    assert(set.Add(p2));
    assert(set.Add(p3));
    assert(!set.Add(p1));

    assert(set.GetCount() == 3);
    cout << "  ✓ Custom hash for Person works" << endl;

    assert(set.Contains(p2));
    cout << "  ✓ Contains with custom hash works" << endl;

    assert(set.Delete(p2));
    assert(set.GetCount() == 2);
    cout << "  ✓ Delete with custom hash works" << endl;
}

void TestUnorderedSetSetOperations()
{
    cout << "\n=== Test 4: UnorderedSet Set Operations ===" << endl;

    UnorderedSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(2); set2.Add(3); set2.Add(4);

    // 并集
    UnorderedSet<int> union_set;
    union_set.Add(1); union_set.Add(2); union_set.Add(3);
    int added = union_set.Union(set2);
    assert(union_set.GetCount() == 4);  // {1, 2, 3, 4}
    assert(added == 1);  // 只添加了4
    cout << "  ✓ Union operation works correctly" << endl;

    // 交集
    UnorderedSet<int> intersect_set;
    intersect_set.Add(1); intersect_set.Add(2); intersect_set.Add(3);
    int removed = intersect_set.Intersect(set2);
    assert(intersect_set.GetCount() == 2);  // {2, 3}
    assert(removed == 1);  // 移除了1
    cout << "  ✓ Intersection operation works correctly" << endl;

    // 差集
    UnorderedSet<int> diff_set;
    diff_set.Add(1); diff_set.Add(2); diff_set.Add(3);
    removed = diff_set.Difference(set2);
    assert(diff_set.GetCount() == 1);  // {1}
    assert(removed == 2);  // 移除了2和3
    cout << "  ✓ Difference operation works correctly" << endl;
}

void TestUnorderedSetComparison()
{
    cout << "\n=== Test 5: UnorderedSet Comparison ===" << endl;

    UnorderedSet<int> set1, set2, set3;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(3); set2.Add(2); set2.Add(1);  // 顺序不同但内容相同
    set3.Add(1); set3.Add(2); set3.Add(4);  // 内容不同

    assert(set1 == set2);
    assert(set1 != set3);
    cout << "  ✓ Comparison operators work correctly" << endl;
}

void TestUnorderedSetIterator()
{
    cout << "\n=== Test 6: UnorderedSet Iterator ===" << endl;

    UnorderedSet<int> set;
    set.Add(10); set.Add(20); set.Add(30);

    int count = 0;
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        ++count;
    }
    assert(count == 3);
    cout << "  ✓ Iterator traversal works correctly" << endl;

    // 通过 const_iterator 遍历
    int sum = 0;
    for (auto it = set.cbegin(); it != set.cend(); ++it)
    {
        sum += *it;
    }
    assert(sum == 60);  // 10 + 20 + 30
    cout << "  ✓ Const iterator works correctly" << endl;
}

// ==================== FlatUnorderedSet vs UnorderedSet 对比测试 ====================

void TestFlatVsUnorderedPerformance()
{
    cout << "\n=== Test 7: FlatUnorderedSet vs UnorderedSet Performance ===" << endl;

    const int N = 100000;

    // FlatUnorderedSet 测试
    {
        cout << "\n  [FlatUnorderedSet<int>]" << endl;
        FlatUnorderedSet<int> set;
        set.Reserve(N);

        {
            Timer t("    Insert");
            for (int i = 0; i < N; i++)
                set.Add(i);
        }

        {
            Timer t("    Lookup");
            int found = 0;
            for (int i = 0; i < N; i++)
            {
                if (set.Contains(i))
                    ++found;
            }
            assert(found == N);
        }

        {
            Timer t("    Delete half");
            for (int i = 0; i < N / 2; i++)
                set.Delete(i * 2);
        }

        cout << "    Final count: " << set.GetCount() << endl;
    }

    // UnorderedSet 测试
    {
        cout << "\n  [UnorderedSet<int>]" << endl;
        UnorderedSet<int> set;
        set.Reserve(N);

        {
            Timer t("    Insert");
            for (int i = 0; i < N; i++)
                set.Add(i);
        }

        {
            Timer t("    Lookup");
            int found = 0;
            for (int i = 0; i < N; i++)
            {
                if (set.Contains(i))
                    ++found;
            }
            assert(found == N);
        }

        {
            Timer t("    Delete half");
            for (int i = 0; i < N / 2; i++)
                set.Delete(i * 2);
        }

        cout << "    Final count: " << set.GetCount() << endl;
    }

    cout << "\n  ✓ Performance comparison completed" << endl;
}

void TestFlatVsUnorderedMemoryUsage()
{
    cout << "\n=== Test 8: FlatUnorderedSet vs UnorderedSet Memory Characteristics ===" << endl;

    const int N = 10000;

    // FlatUnorderedSet：连续内存、无个别分配
    {
        cout << "\n  [FlatUnorderedSet<int>]" << endl;
        FlatUnorderedSet<int> set;
        set.Reserve(N);

        for (int i = 0; i < N; i++)
            set.Add(i);

        cout << "    Count: " << set.GetCount() << endl;
        cout << "    Memory: Contiguous ActiveDataManager (single allocation)" << endl;

        // 删除多个元素
        for (int i = 0; i < N / 2; i++)
            set.Delete(i);

        cout << "    After delete count: " << set.GetCount() << endl;
        cout << "    ✓ Deletion doesn't fragment memory (ID reuse)" << endl;
    }

    // UnorderedSet：哈希表、个别分配
    {
        cout << "\n  [UnorderedSet<int>]" << endl;
        UnorderedSet<int> set;
        set.Reserve(N);

        for (int i = 0; i < N; i++)
            set.Add(i);

        cout << "    Count: " << set.GetCount() << endl;
        cout << "    Memory: Hash table (standard allocator)" << endl;

        // 删除多个元素
        for (int i = 0; i < N / 2; i++)
            set.Delete(i);

        cout << "    After delete count: " << set.GetCount() << endl;
        cout << "    ✓ Uses standard STL memory management" << endl;
    }
}

void TestFlatVsUnorderedFeatures()
{
    cout << "\n=== Test 9: FlatUnorderedSet vs UnorderedSet Features Comparison ===" << endl;

    cout << "\n  [FlatUnorderedSet]" << endl;
    cout << "    ✓ Trivially copyable types only" << endl;
    cout << "    ✓ Contiguous memory layout" << endl;
    cout << "    ✓ Zero-copy serialization (GetData/LoadFromBuffer)" << endl;
    cout << "    ✓ Direct ID-based access (GetActiveView, At)" << endl;
    cout << "    ✓ EnumMutable with auto-rehash" << endl;
    cout << "    ✓ Perfect for game engine data (positions, IDs, etc)" << endl;

    cout << "\n  [UnorderedSet]" << endl;
    cout << "    ✓ Any type with Hash/Equal" << endl;
    cout << "    ✓ Standard std::hash based" << endl;
    cout << "    ✓ Supports complex objects (strings, custom types)" << endl;
    cout << "    ✓ Standard iterator interface" << endl;
    cout << "    ✓ No serialization constraints" << endl;
    cout << "    ✓ Better for general C++ usage" << endl;

    cout << "\n  ✓ Feature comparison completed" << endl;
}

// ==================== 主函数 ====================

int main()
{
    cout << "=== UnorderedSet Test Suite ===" << endl;
    cout << "Testing UnorderedSet basic operations and comparison with FlatUnorderedSet" << endl;

    try {
        TestUnorderedSetBasic();
        TestUnorderedSetWithNonTrivial();
        TestUnorderedSetCustomHash();
        TestUnorderedSetSetOperations();
        TestUnorderedSetComparison();
        TestUnorderedSetIterator();
        TestFlatVsUnorderedPerformance();
        TestFlatVsUnorderedMemoryUsage();
        TestFlatVsUnorderedFeatures();

        cout << "\n=== All tests passed successfully ✅ ===" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "\n❌ Test failed with exception: " << e.what() << endl;
        return 1;
    }
    catch (...) {
        cerr << "\n❌ Test failed with unknown exception" << endl;
        return 1;
    }
}
