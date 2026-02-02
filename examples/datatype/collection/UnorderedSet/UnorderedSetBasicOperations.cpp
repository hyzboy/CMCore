/**
 * UnorderedSetBasicOperations.cpp
 * UnorderedSet 基本操作测试
 */

#include <hgl/type/UnorderedSet.h>
#include <iostream>
#include <cassert>

using namespace hgl;
using namespace std;

void TestAdd()
{
    cout << "\n=== Test 1: Add Operations ===" << endl;

    UnorderedSet<int> set;

    // 单个添加
    assert(set.Add(10) == true);
    assert(set.Add(20) == true);
    assert(set.Add(30) == true);
    assert(set.Add(10) == false);  // 重复

    assert(set.GetCount() == 3);
    cout << "  ✓ Single add works" << endl;

    // 批量添加
    int values[] = {40, 50, 60, 10, 20};
    int64 added = set.Add(values, 5);
    assert(added == 3);  // 只添加了 40, 50, 60
    assert(set.GetCount() == 6);
    cout << "  ✓ Batch add with duplicates works" << endl;
}

void TestDelete()
{
    cout << "\n=== Test 2: Delete Operations ===" << endl;

    UnorderedSet<int> set;

    set.Add(10); set.Add(20); set.Add(30); set.Add(40);

    // 单个删除
    assert(set.Delete(20) == true);
    assert(set.Delete(20) == false);  // 已删除
    assert(set.Delete(99) == false);  // 不存在
    assert(set.GetCount() == 3);
    cout << "  ✓ Single delete works" << endl;

    // 批量删除
    int values[] = {10, 30, 50};
    int64 deleted = set.Delete(values, 3);
    assert(deleted == 2);  // 删除了 10 和 30（50 不存在）
    assert(set.GetCount() == 1);
    cout << "  ✓ Batch delete works" << endl;

    // 清空
    set.Clear();
    assert(set.IsEmpty());
    cout << "  ✓ Clear works" << endl;
}

void TestContains()
{
    cout << "\n=== Test 3: Contains Operations ===" << endl;

    UnorderedSet<int> set;

    set.Add(10);
    set.Add(20);
    set.Add(30);

    assert(set.Contains(10) == true);
    assert(set.Contains(20) == true);
    assert(set.Contains(40) == false);
    cout << "  ✓ Contains check works" << endl;

    // 删除后再检查
    set.Delete(20);
    assert(set.Contains(20) == false);
    cout << "  ✓ Contains after delete works" << endl;
}

void TestFind()
{
    cout << "\n=== Test 4: Find Operations ===" << endl;

    UnorderedSet<int> set;

    set.Add(10);
    set.Add(20);
    set.Add(30);

    // Find 返回迭代器
    auto it = set.Find(20);
    assert(it != set.end());
    assert(*it == 20);
    cout << "  ✓ Find existing element works" << endl;

    auto it_not = set.Find(99);
    assert(it_not == set.end());
    cout << "  ✓ Find non-existing element works" << endl;
}

void TestCapacity()
{
    cout << "\n=== Test 5: Capacity Operations ===" << endl;

    UnorderedSet<int> set;

    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    cout << "  ✓ Empty set check works" << endl;

    set.Add(10);
    assert(!set.IsEmpty());
    assert(set.GetCount() == 1);
    cout << "  ✓ Count update works" << endl;

    // 预分配
    UnorderedSet<int> set2;
    set2.Reserve(1000);
    for (int i = 0; i < 100; i++)
        set2.Add(i);
    assert(set2.GetCount() == 100);
    cout << "  ✓ Reserve works" << endl;
}

void TestEnumeration()
{
    cout << "\n=== Test 6: Enumeration ===" << endl;

    UnorderedSet<int> set;

    set.Add(10); set.Add(20); set.Add(30);

    // 统计元素
    int count = 0;
    set.Enum([&](int value) {
        ++count;
    });
    assert(count == 3);
    cout << "  ✓ Enum count works" << endl;

    // 求和
    int sum = 0;
    set.Enum([&](int value) {
        sum += value;
    });
    assert(sum == 60);  // 10 + 20 + 30
    cout << "  ✓ Enum sum calculation works" << endl;
}

void TestIterator()
{
    cout << "\n=== Test 7: Iterator ===" << endl;

    UnorderedSet<int> set;

    set.Add(10); set.Add(20); set.Add(30);

    // begin/end
    int count = 0;
    for (auto it = set.begin(); it != set.end(); ++it)
    {
        ++count;
        assert(*it != 0);  // 应该是有效的值
    }
    assert(count == 3);
    cout << "  ✓ begin/end works" << endl;

    // cbegin/cend
    count = 0;
    for (auto it = set.cbegin(); it != set.cend(); ++it)
    {
        ++count;
    }
    assert(count == 3);
    cout << "  ✓ cbegin/cend works" << endl;

    // Range-based for
    count = 0;
    for (int value : set)
    {
        ++count;
        assert(value != 0);
    }
    assert(count == 3);
    cout << "  ✓ Range-based for works" << endl;
}

void TestWithStrings()
{
    cout << "\n=== Test 8: String Set ===" << endl;

    UnorderedSet<std::string> words;

    words.Add("hello");
    words.Add("world");
    words.Add("unordered");
    words.Add("hello");  // 重复

    assert(words.GetCount() == 3);
    cout << "  ✓ String add works" << endl;

    assert(words.Contains("world"));
    assert(!words.Contains("test"));
    cout << "  ✓ String contains works" << endl;

    words.Delete("world");
    assert(!words.Contains("world"));
    cout << "  ✓ String delete works" << endl;

    int count = 0;
    words.Enum([&](const std::string& s) {
        ++count;
    });
    assert(count == 2);
    cout << "  ✓ String enumeration works" << endl;
}

void TestCopyAndAssign()
{
    cout << "\n=== Test 9: Copy and Assignment ===" << endl;

    UnorderedSet<int> set1;
    set1.Add(10);
    set1.Add(20);

    // 复制构造
    UnorderedSet<int> set2 = set1;
    assert(set2.GetCount() == 2);
    assert(set2.Contains(10));
    assert(set2.Contains(20));
    cout << "  ✓ Copy construction works" << endl;

    // 修改副本不影响原件
    set2.Add(30);
    assert(set1.GetCount() == 2);
    assert(set2.GetCount() == 3);
    cout << "  ✓ Independent copies work" << endl;
}

void TestEdgeCases()
{
    cout << "\n=== Test 10: Edge Cases ===" << endl;

    UnorderedSet<int> set;

    // 空集合删除
    assert(!set.Delete(10));
    cout << "  ✓ Delete from empty set works" << endl;

    // 空集合查询
    assert(!set.Contains(10));
    cout << "  ✓ Contains on empty set works" << endl;

    // 添加并立即删除
    set.Add(10);
    assert(set.Delete(10));
    assert(set.IsEmpty());
    cout << "  ✓ Add and immediate delete works" << endl;

    // 添加零个元素
    int64 added = set.Add(nullptr, 0);
    assert(added == 0);
    cout << "  ✓ Add zero elements works" << endl;
}

int main()
{
    cout << "=== UnorderedSet Basic Operations Test Suite ===" << endl;

    try {
        TestAdd();
        TestDelete();
        TestContains();
        TestFind();
        TestCapacity();
        TestEnumeration();
        TestIterator();
        TestWithStrings();
        TestCopyAndAssign();
        TestEdgeCases();

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
