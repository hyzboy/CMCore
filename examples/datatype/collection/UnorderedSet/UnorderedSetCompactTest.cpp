/**
 * UnorderedSetCompactTest.cpp
 * 测试新版高性能无序集合的功能和性能
 */

#include <hgl/type/UnorderedValueSet.h>
#include <hgl/type/UnorderedManagedSet.h>
#include <iostream>
#include <cassert>
#include <chrono>

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

// ==================== 基本功能测试 ====================

void TestBasicOperations()
{
    cout << "\n=== Test 1: Basic Operations ===" << endl;

    UnorderedValueSet<int> set;

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

void TestBatchOperations()
{
    cout << "\n=== Test 2: Batch Operations ===" << endl;

    UnorderedValueSet<int> set;

    int values[] = {1, 2, 3, 4, 5, 2, 3};  // 包含重复
    int added = set.Add(values, 7);
    assert(added == 5);  // 只添加了5个唯一值
    assert(set.GetCount() == 5);
    cout << "  ✓ Batch add with duplicates handled correctly" << endl;

    int to_delete[] = {2, 4, 6};
    int deleted = set.Delete(to_delete, 3);
    assert(deleted == 2);  // 删除了2和4（6不存在）
    assert(set.GetCount() == 3);
    cout << "  ✓ Batch delete works correctly" << endl;
}

void TestSetOperations()
{
    cout << "\n=== Test 3: Set Operations ===" << endl;

    UnorderedValueSet<int> set1, set2;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(2); set2.Add(3); set2.Add(4);

    // 并集
    UnorderedValueSet<int> union_set;
    union_set.Add(1); union_set.Add(2); union_set.Add(3);  // 复制 set1 的内容
    int added = union_set.Union(set2);
    assert(union_set.GetCount() == 4);  // {1, 2, 3, 4}
    assert(added == 1);  // 只添加了4
    cout << "  ✓ Union operation works correctly" << endl;

    // 交集
    UnorderedValueSet<int> intersect_set;
    intersect_set.Add(1); intersect_set.Add(2); intersect_set.Add(3);  // 复制 set1 的内容
    int removed = intersect_set.Intersect(set2);
    assert(intersect_set.GetCount() == 2);  // {2, 3}
    assert(removed == 1);  // 移除了1
    cout << "  ✓ Intersection operation works correctly" << endl;

    // 差集
    UnorderedValueSet<int> diff_set;
    diff_set.Add(1); diff_set.Add(2); diff_set.Add(3);  // 复制 set1 的内容
    removed = diff_set.Difference(set2);
    assert(diff_set.GetCount() == 1);  // {1}
    assert(removed == 2);  // 移除了2和3
    cout << "  ✓ Difference operation works correctly" << endl;
}

void TestComparison()
{
    cout << "\n=== Test 4: Comparison Operators ===" << endl;

    UnorderedValueSet<int> set1, set2, set3;

    set1.Add(1); set1.Add(2); set1.Add(3);
    set2.Add(3); set2.Add(2); set2.Add(1);  // 顺序不同但内容相同
    set3.Add(1); set3.Add(2); set3.Add(4);  // 内容不同

    assert(set1 == set2);
    assert(set1 != set3);
    cout << "  ✓ Comparison operators work correctly" << endl;
}

void TestMutableEnum()
{
    cout << "\n=== Test 5: Mutable Enumeration ===" << endl;

    UnorderedValueSet<int> set;
    auto dump = [&](const char* tag)
    {
        cout << "  [" << tag << "] count=" << set.GetCount() << " values={";

        bool first = true;
        set.Enum([&](int v){ if(!first) cout << ", "; cout << v; first=false; });
        cout << "}" << endl;
    };

    cout << "  Adding 10: " << set.Add(10) << endl;
    cout << "  Adding 20: " << set.Add(20) << endl;
    cout << "  Adding 30: " << set.Add(30) << endl;
    dump("after add");

    cout << "  Contains before mutate -> 10:" << set.Contains(10)
         << " 20:" << set.Contains(20)
         << " 30:" << set.Contains(30) << endl;

    // 修改所有元素
    set.EnumMutable([](int& value) {
        value *= 2;
    });
    cout << "  EnumMutable done" << endl;
    dump("after mutate");

    cout << "  Contains after mutate -> 10:" << set.Contains(10)
         << " 20:" << set.Contains(20)
         << " 40:" << set.Contains(40)
         << " 60:" << set.Contains(60) << endl;

    int count = 0; int sum = 0;
    set.Enum([&](int v){ ++count; sum += v; });
    cout << "  Enum after mutate count=" << count << " sum=" << sum << endl;

    assert(set.Contains(20));
    assert(set.Contains(40));
    assert(set.Contains(60));
    assert(!set.Contains(10));
    cout << "  ✓ Mutable enumeration works correctly" << endl;
}

// ==================== 性能测试 ====================

void TestPerformance()
{
    cout << "\n=== Test 6: Performance Test ===" << endl;

    const int N = 10000;

    UnorderedValueSet<int> set;
    set.Reserve(N);

    {
        Timer t("  Insert");
        for (int i = 0; i < N; i++)
            set.Add(i);
    }

    {
        Timer t("  Lookup");
        int found = 0;
        for (int i = 0; i < N; i++)
        {
            if (set.Contains(i))
                ++found;
        }
        assert(found == N);
    }

    {
        Timer t("  Iteration");
        int sum = 0;
        set.Enum([&](int value) {
            sum += value;
        });
    }

    {
        Timer t("  Delete");
        for (int i = 0; i < N / 2; i++)
            set.Delete(i * 2);
    }

    cout << "  Final count: " << set.GetCount() << " (expected: " << N / 2 << ")" << endl;
}

// ==================== 边界测试 ====================

void TestBoundaryConditions()
{
    cout << "\n=== Test 7: Boundary Conditions ===" << endl;

    UnorderedValueSet<int> set;

    // 空集合操作
    assert(set.IsEmpty());
    assert(!set.Contains(1));
    assert(!set.Delete(1));
    
    int value;
    assert(!set.GetFirst(value));
    assert(!set.GetLast(value));
    cout << "  ✓ Empty set operations handled correctly" << endl;

    // 单元素
    set.Add(42);
    assert(!set.IsEmpty());
    assert(set.GetFirst(value) && value == 42);
    assert(set.GetLast(value) && value == 42);
    cout << "  ✓ Single element operations work correctly" << endl;

    // 清空
    set.Clear();
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    cout << "  ✓ Clear operation works correctly" << endl;
}

void TestDirectAccess()
{
    cout << "\n=== Test 8: Direct Data Access (High Performance) ===" << endl;

    UnorderedValueSet<int> set;
    set.Add(10); set.Add(20); set.Add(30);

    auto dump = [&](const char* tag)
    {
        cout << "  [" << tag << "] count=" << set.GetCount() << " values={";

        bool first = true;
        set.Enum([&](int v){ if(!first) cout << ", "; cout << v; first=false; });
        cout << "}" << endl;
    };

    // 通过 ID 直接访问
    const ValueBuffer<int>& ids = set.GetActiveView();
    cout << "  Active IDs: [";
    for (int i = 0; i < ids.GetCount(); i++)
    {
        int id = ids[i];
        int* ptr = set.At(id);
        if (ptr)
            cout << *ptr << (i < ids.GetCount() - 1 ? ", " : "");
    }
    cout << "]" << endl;
    dump("before modify");

    // 修改数据
    int* ptr = set.At(ids[0]);
    if (ptr)
    {
        int old_value = *ptr;
        *ptr = 999;
        cout << "  Modified id=" << ids[0] << " from " << old_value << " to " << *ptr << endl;
        set.RefreshHashMap();
        cout << "  HashMap refreshed" << endl;
        dump("after modify");

        cout << "  Contains 999:" << set.Contains(999)
             << " old(" << old_value << "):" << set.Contains(old_value) << endl;

        assert(set.Contains(999));
        assert(!set.Contains(old_value));
        cout << "  ✓ Direct modification works correctly" << endl;
    }
}

// ==================== 测试 UnorderedManagedSet ====================

class TestObject
{
private:
    int id;
    int value;

public:
    TestObject() : id(0), value(0) {}
    TestObject(int i, int v) : id(i), value(v) {}

    int GetId() const { return id; }
    int GetValue() const { return value; }
    void SetValue(int v) { value = v; }

    bool operator==(const TestObject& other) const
    {
        return id == other.id && value == other.value;
    }
};

// 非平凡类型，需使用 UnorderedManagedSet
class ManagedObject
{
private:
    int id;
    int value;
    std::string name;    // 使其非平凡

public:
    ManagedObject() : id(0), value(0), name() {}
    ManagedObject(int i, int v, std::string n) : id(i), value(v), name(std::move(n)) {}

    int GetId() const { return id; }
    int GetValue() const { return value; }
    void SetValue(int v) { value = v; }
    const std::string& GetName() const { return name; }

    bool operator==(const ManagedObject& other) const
    {
        return id == other.id && value == other.value && name == other.name;
    }
};

void TestManagedSet()
{
    cout << "\n=== Test 9: UnorderedManagedSet (Non-trivial Objects) ===" << endl;

    // 对于平凡类型，使用 UnorderedValueSet
    {
        UnorderedValueSet<TestObject> value_set;

        value_set.Add(TestObject(1, 100));
        value_set.Add(TestObject(2, 200));
        value_set.Add(TestObject(3, 300));

        assert(value_set.GetCount() == 3);
        cout << "  ✓ ValueSet added objects successfully" << endl;

        assert(value_set.Contains(TestObject(2, 200)));
        assert(!value_set.Contains(TestObject(4, 400)));
        cout << "  ✓ ValueSet contains checks work" << endl;

        int sum = 0;
        value_set.Enum([&](const TestObject& obj) { sum += obj.GetValue(); });
        assert(sum == 600);
        cout << "  ✓ ValueSet enumeration sum=" << sum << endl;

        value_set.EnumMutable([](TestObject& obj) { obj.SetValue(obj.GetValue() * 2); });
        sum = 0;
        value_set.Enum([&](const TestObject& obj) { sum += obj.GetValue(); });
        assert(sum == 1200);
        cout << "  ✓ ValueSet mutable enumeration sum=" << sum << endl;

        assert(value_set.Delete(TestObject(2, 400)));
        assert(value_set.GetCount() == 2);
        cout << "  ✓ ValueSet delete works" << endl;

        value_set.Clear();
        assert(value_set.IsEmpty());
        cout << "  ✓ ValueSet clear works" << endl;
    }

    // 对于非平凡类型，使用 UnorderedManagedSet（指针接口）
    {
        UnorderedManagedSet<ManagedObject> managed_set;

        ManagedObject* a = new ManagedObject(1, 100, "a");
        ManagedObject* b = new ManagedObject(2, 200, "b");
        ManagedObject* c = new ManagedObject(3, 300, "c");

        managed_set.Add(a);
        managed_set.Add(b);
        managed_set.Add(c);

        assert(managed_set.GetCount() == 3);
        cout << "  ✓ ManagedSet added objects successfully" << endl;

        assert(managed_set.Contains(b));
        ManagedObject* fake = new ManagedObject(4, 400, "d");
        assert(!managed_set.Contains(fake));
        delete fake;
        cout << "  ✓ ManagedSet contains checks work" << endl;

        int sum = 0;
        managed_set.Enum([&](const ManagedObject& obj) { sum += obj.GetValue(); });
        assert(sum == 600);
        cout << "  ✓ ManagedSet enumeration sum=" << sum << endl;

        managed_set.EnumMutable([](ManagedObject& obj) { obj.SetValue(obj.GetValue() * 2); });
        sum = 0;
        managed_set.Enum([&](const ManagedObject& obj) { sum += obj.GetValue(); });
        assert(sum == 1200);
        cout << "  ✓ ManagedSet mutable enumeration sum=" << sum << endl;

        assert(managed_set.Delete(b));
        assert(managed_set.GetCount() == 2);
        cout << "  ✓ ManagedSet delete works" << endl;

        managed_set.Free();
        assert(managed_set.IsEmpty());
        cout << "  ✓ ManagedSet free works (objects deleted)" << endl;
    }
}

// ==================== 主函数 ====================

int main()
{
    cout << "=== New UnorderedSet Test Suite ===" << endl;
    cout << "Testing high-performance optimized implementation" << endl;

    try {
        TestBasicOperations();
        TestBatchOperations();
        TestSetOperations();
        TestComparison();
        TestMutableEnum();
        TestPerformance();
        TestBoundaryConditions();
        TestDirectAccess();
        TestManagedSet();

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
