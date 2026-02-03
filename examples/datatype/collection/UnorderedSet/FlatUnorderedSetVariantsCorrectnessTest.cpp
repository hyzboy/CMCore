/**
 * FlatUnorderedSetVariantsCorrectnessTest.cpp
 * 正确性测试：验证所有变体的行为一致性
 */

#include <hgl/type/FlatUnorderedSet.h>
#include <hgl/type/DualHashSet.h>
#include <hgl/type/LinearProbeSet.h>
#include <hgl/type/ShardedSet.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <random>
#include <string>
#include <vector>

using namespace hgl;
using namespace std;

// 底层数据一致性验证
template<typename SetType>
static bool VerifyDataConsistency(const SetType &set, const vector<int> &expected_values, const string &checkpoint)
{
    cout << "    [验证:" << checkpoint << "] ";

    int count = set.GetCount();
    int expected_count = (int)expected_values.size();

    if (count != expected_count)
    {
        cout << "❌ Count不匹配: set=" << count << ", expected=" << expected_count << endl;
        return false;
    }

    // 验证expected中的所有元素都在set中
    for (int val : expected_values)
    {
        if (!set.Contains(val))
        {
            cout << "❌ 元素" << val << "应该存在但不存在" << endl;
            return false;
        }
    }

    // 验证set中的所有元素都在expected中
    bool all_match = true;
    set.Enum([&](int val) {
        if (find(expected_values.begin(), expected_values.end(), val) == expected_values.end())
        {
            cout << "❌ 元素" << val << "不应该存在但存在" << endl;
            all_match = false;
        }
    });

    if (!all_match)
        return false;

    cout << "✓ (count=" << count << ")" << endl;
    return true;
}

template<typename SetType>
static void TestBasicOperations(const string &name)
{
    cout << "\n=== " << name << " - Basic Operations ===" << endl;

    SetType set;

    // 测试1: 添加和查找
    assert(set.Add(10));
    assert(set.Add(20));
    assert(set.Add(30));
    assert(!set.Add(20));  // 重复
    assert(set.GetCount() == 3);
    assert(set.Contains(10));
    assert(set.Contains(20));
    assert(set.Contains(30));
    assert(!set.Contains(40));
    cout << "  ✓ Add and Contains" << endl;

    // 测试2: 删除
    assert(set.Delete(20));
    assert(!set.Delete(40));
    assert(set.GetCount() == 2);
    assert(!set.Contains(20));
    assert(set.Contains(10));
    cout << "  ✓ Delete" << endl;

    // 测试3: 遍历
    int sum = 0;
    set.Enum([&](int v) { sum += v; });
    assert(sum == 40);  // 10 + 30
    cout << "  ✓ Enum" << endl;

    // 测试4: 清空
    set.Clear();
    assert(set.IsEmpty());
    assert(set.GetCount() == 0);
    cout << "  ✓ Clear" << endl;
}

template<typename SetType>
static void TestLargeScale(const string &name)
{
    cout << "\n=== " << name << " - Large Scale Test ===" << endl;

    SetType set;
    const int N = 10000;
    vector<int> expected;

    // 插入
    int insert_success = 0;
    for (int i = 0; i < N; i++)
    {
        if (set.Add(i))
        {
            ++insert_success;
            expected.push_back(i);
        }
    }
    if (insert_success != N)
    {
        cout << "  ❌ Insert failed: expected " << N << ", got " << insert_success << endl;
        assert(false);
    }
    if (set.GetCount() != N)
    {
        cout << "  ❌ Count mismatch after insert: expected " << N << ", got " << set.GetCount() << endl;
        assert(false);
    }

    // 验证插入后数据一致性
    if (!VerifyDataConsistency(set, expected, "插入完成"))
    {
        assert(false);
    }

    cout << "  ✓ Insert " << N << " elements" << endl;

    // 查找
    for (int i = 0; i < N; i++)
    {
        if (!set.Contains(i))
        {
            cout << "  ❌ Element " << i << " not found after insert" << endl;
            assert(false);
        }
    }
    for (int i = N; i < N + 100; i++)
    {
        if (set.Contains(i))
        {
            cout << "  ❌ Element " << i << " should not exist" << endl;
            assert(false);
        }
    }
    cout << "  ✓ Lookup all elements" << endl;

    // 删除一半 - 逐步验证
    int delete_success = 0;
    const int verify_interval = 500;  // 每删除500个就验证一次

    for (int i = 0; i < N; i += 2)
    {
        // 删除前验证
        if (!set.Contains(i))
        {
            cout << "  ❌ Element " << i << " missing before delete!" << endl;
            cout << "     delete_success=" << delete_success << ", current_count=" << set.GetCount() << endl;

            // 打印周围元素的状态
            cout << "     Nearby elements: ";
            for (int j = max(0, i-5); j <= min(N-1, i+5); j++)
            {
                cout << j << ":" << (set.Contains(j) ? "Y" : "N") << " ";
            }
            cout << endl;

            assert(false);
        }

        if (set.Delete(i))
        {
            ++delete_success;

            // 从expected中移除
            auto it = find(expected.begin(), expected.end(), i);
            if (it != expected.end())
                expected.erase(it);

            // 定期验证
            if (delete_success % verify_interval == 0)
            {
                if (!VerifyDataConsistency(set, expected, "删除" + to_string(delete_success) + "个"))
                {
                    cout << "  ❌ 删除到第" << delete_success << "个时数据不一致！" << endl;
                    assert(false);
                }
            }
        }
        else
        {
            cout << "  ❌ Delete failed for element " << i << " (count=" << set.GetCount() << ")" << endl;
            assert(false);
        }
    }

    // 最终验证
    if (!VerifyDataConsistency(set, expected, "删除完成"))
    {
        assert(false);
    }

    if (delete_success != N / 2)
    {
        cout << "  ❌ Delete failed: expected " << (N/2) << ", got " << delete_success << endl;
        assert(false);
    }
    if (set.GetCount() != N / 2)
    {
        cout << "  ❌ Count after delete: expected " << (N/2) << ", got " << set.GetCount() << endl;
        assert(false);
    }
    cout << "  ✓ Delete half" << endl;

    // 验证
    for (int i = 0; i < N; i++)
    {
        bool should_exist = (i % 2 == 1);
        bool exists = set.Contains(i);
        if (exists != should_exist)
        {
            cout << "  ❌ Element " << i << " state incorrect: expected "
                 << (should_exist ? "exists" : "deleted")
                 << ", got " << (exists ? "exists" : "deleted") << endl;
            assert(false);
        }
    }
    cout << "  ✓ Verify correctness after delete" << endl;
}

template<typename SetType>
static void TestRandomOperations(const string &name)
{
    cout << "\n=== " << name << " - Random Operations ===" << endl;

    SetType set;
    vector<int> expected;
    mt19937 rng(12345);
    uniform_int_distribution<int> dist(0, 1000);

    const int ops = 5000;
    int add_count = 0, del_count = 0;

    for (int i = 0; i < ops; i++)
    {
        int val = dist(rng);
        int op = i % 3;

        if (op == 0)  // Add
        {
            bool added = set.Add(val);
            auto it = find(expected.begin(), expected.end(), val);
            if (it == expected.end())
            {
                assert(added);
                expected.push_back(val);
                ++add_count;
            }
            else
            {
                assert(!added);
            }
        }
        else if (op == 1)  // Delete
        {
            bool deleted = set.Delete(val);
            auto it = find(expected.begin(), expected.end(), val);
            if (it != expected.end())
            {
                assert(deleted);
                expected.erase(it);
                ++del_count;
            }
            else
            {
                assert(!deleted);
            }
        }
        else  // Contains
        {
            bool contains = set.Contains(val);
            auto it = find(expected.begin(), expected.end(), val);
            assert(contains == (it != expected.end()));
        }
    }

    // 验证最终状态
    assert(set.GetCount() == (int)expected.size());

    int enum_count = 0;
    set.Enum([&](int v) {
        assert(find(expected.begin(), expected.end(), v) != expected.end());
        ++enum_count;
    });
    assert(enum_count == (int)expected.size());

    cout << "  ✓ " << ops << " random operations (add=" << add_count
         << ", del=" << del_count << ", final=" << expected.size() << ")" << endl;
}

template<typename SetType>
static void TestEdgeCases(const string &name)
{
    cout << "\n=== " << name << " - Edge Cases ===" << endl;

    SetType set;

    // 空集合操作
    assert(set.IsEmpty());
    assert(!set.Delete(1));
    int sum = 0;
    set.Enum([&](int) { ++sum; });
    assert(sum == 0);
    cout << "  ✓ Empty set operations" << endl;

    // 单元素
    assert(set.Add(42));
    assert(!set.IsEmpty());
    assert(set.Contains(42));
    assert(set.GetCount() == 1);
    cout << "  ✓ Single element" << endl;

    // 大量重复添加
    for (int i = 0; i < 100; i++)
        assert(!set.Add(42));
    assert(set.GetCount() == 1);
    cout << "  ✓ Duplicate additions" << endl;

    // 删除不存在
    for (int i = 0; i < 100; i++)
        assert(!set.Delete(999));
    cout << "  ✓ Delete non-existent" << endl;

    set.Clear();

    // 连续相同值
    for (int i = 0; i < 100; i++)
        set.Add(i % 10);
    assert(set.GetCount() == 10);
    cout << "  ✓ Sequential duplicates" << endl;
}

template<typename SetType>
static void TestDeleteReinsert(const string &name)
{
    cout << "\n=== " << name << " - Delete/Reinsert Cycles ===" << endl;

    SetType set;
    const int N = 1000;

    // 初始填充
    for (int i = 0; i < N; i++)
        set.Add(i);

    // 5个删除-重插入周期
    for (int cycle = 0; cycle < 5; cycle++)
    {
        // 删除所有偶数
        for (int i = 0; i < N; i += 2)
            assert(set.Delete(i));
        assert(set.GetCount() == N / 2);

        // 重新插入
        for (int i = 0; i < N; i += 2)
            assert(set.Add(i));
        assert(set.GetCount() == N);

        // 验证全部存在
        for (int i = 0; i < N; i++)
            assert(set.Contains(i));
    }

    cout << "  ✓ 5 delete/reinsert cycles" << endl;
}

template<typename SetType>
static void RunAllTests(const string &name)
{
    cout << "\n" << string(60, '=') << endl;
    cout << "  Testing: " << name << endl;
    cout << string(60, '=') << endl;

    TestBasicOperations<SetType>(name);
    TestLargeScale<SetType>(name);
    TestRandomOperations<SetType>(name);
    TestEdgeCases<SetType>(name);
    TestDeleteReinsert<SetType>(name);

    cout << "\n  ✅ All tests passed for " << name << endl;
}

int main()
{
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║      FlatUnorderedSet Variants - Correctness Tests        ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;

    try
    {
        RunAllTests<FlatUnorderedSet<int>>("FlatUnorderedSet (Original)");
        RunAllTests<DualHashSet<int>>("DualHashSet");
        RunAllTests<LinearProbeSet<int>>("LinearProbeSet");
        RunAllTests<ShardedSet<int>>("ShardedSet");

        cout << "\n" << string(60, '=') << endl;
        cout << "  🎉 All Variants Passed All Tests!" << endl;
        cout << string(60, '=') << endl;

        // 正确性测试汇总表
        cout << "\n╔════════════════════════════════════════════════════════════════════════════╗" << endl;
        cout << "║                    Correctness Test Summary                               ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════════════════════╝" << endl;

        cout << "\n┌─ BASIC FUNCTIONALITY TESTS" << endl;
        cout << "│" << endl;
        cout << "│  Test Case                       │  Original  │ DualHash │ OpenAddr │ Sharded" << endl;
        cout << "├──────────────────────────────────┼────────────┼──────────┼──────────┼─────────" << endl;
        cout << "│  Add and Contains                │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Delete Operation                │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Enumeration                     │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Clear Operation                 │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "└──────────────────────────────────┴────────────┴──────────┴──────────┴─────────" << endl;

        cout << "\n┌─ SCALE TESTS (10,000 elements)" << endl;
        cout << "│" << endl;
        cout << "│  Operation                       │  Original  │ DualHash │ OpenAddr │ Sharded" << endl;
        cout << "├──────────────────────────────────┼────────────┼──────────┼──────────┼─────────" << endl;
        cout << "│  Insert All                      │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Lookup All                      │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Delete Half                     │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Verify Final State              │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "└──────────────────────────────────┴────────────┴──────────┴──────────┴─────────" << endl;

        cout << "\n┌─ RANDOM OPERATION TESTS (5,000 ops)" << endl;
        cout << "│" << endl;
        cout << "│  Stress Test                     │  Original  │ DualHash │ OpenAddr │ Sharded" << endl;
        cout << "├──────────────────────────────────┼────────────┼──────────┼──────────┼─────────" << endl;
        cout << "│  Random Add/Delete/Contains      │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Final State Consistency         │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Enumeration Integrity          │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "└──────────────────────────────────┴────────────┴──────────┴──────────┴─────────" << endl;

        cout << "\n┌─ EDGE CASE TESTS" << endl;
        cout << "│" << endl;
        cout << "│  Edge Case                       │  Original  │ DualHash │ OpenAddr │ Sharded" << endl;
        cout << "├──────────────────────────────────┼────────────┼──────────┼──────────┼─────────" << endl;
        cout << "│  Empty Set Operations            │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Single Element                  │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Duplicate Additions             │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Delete Non-Existent             │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Sequential Duplicates           │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "└──────────────────────────────────┴────────────┴──────────┴──────────┴─────────" << endl;

        cout << "\n┌─ REAL-WORLD USAGE PATTERNS" << endl;
        cout << "│" << endl;
        cout << "│  Pattern                         │  Original  │ DualHash │ OpenAddr │ Sharded" << endl;
        cout << "├──────────────────────────────────┼────────────┼──────────┼──────────┼─────────" << endl;
        cout << "│  5 Delete/Reinsert Cycles (1k)   │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "│  Data Consistency Through Cycles │    ✅      │    ✅    │    ✅    │   ✅" << endl;
        cout << "└──────────────────────────────────┴────────────┴──────────┴──────────┴─────────" << endl;

        cout << "\n【测试覆盖统计】" << endl;
        cout << "  ✓ 基本操作:       4项测试  (Add, Delete, Enum, Clear)" << endl;
        cout << "  ✓ 大规模测试:     4项测试  (10k元素插入、查找、删除、验证)" << endl;
        cout << "  ✓ 随机操作:       3项测试  (5k随机操作、状态一致性、枚举完整性)" << endl;
        cout << "  ✓ 边界条件:       5项测试  (空集、单元素、重复、不存在、顺序重复)" << endl;
        cout << "  ✓ 真实场景:       2项测试  (5轮删除-重插入循环、数据一致性)" << endl;
        cout << "  =" << endl;
        cout << "  总计:            18项测试 × 4个变体 = 72项测试用例 ✅ 全部通过" << endl;
        cout << endl;

        return 0;
    }
    catch (const exception &e)
    {
        cerr << "\n❌ Test failed with exception: " << e.what() << endl;
        return 1;
    }
}
