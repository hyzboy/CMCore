#include<hgl/platform/Platform.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<cassert>
#include<string>

using namespace hgl;
using namespace std;

// 测试计数器
static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            cout << "  ✓ PASS: " << message << endl; \
            test_passed++; \
        } else { \
            cout << "  ✗ FAIL: " << message << endl; \
            test_failed++; \
        } \
    } while(0)

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Modify Operations" << endl;
    cout << "========================================" << endl;

    // [1] Change - 修改现有 KEY 的 VALUE
    cout << "\n[1] Change existing mapping:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");
        bmap.Add(3, "three");

        TEST_ASSERT(bmap.Change(1, "ONE"), "Change(1, 'ONE') succeeds");

        string result;
        TEST_ASSERT(bmap.Get(1, result) && result == "ONE", "Changed value is 'ONE'");
        TEST_ASSERT(!bmap.ContainsValue("one"), "Old value 'one' no longer exists");
        TEST_ASSERT(bmap.ContainsValue("ONE"), "New value 'ONE' exists");
    }

    // [2] Change - 尝试修改为已存在的 VALUE（应失败）
    cout << "\n[2] Change to existing value (conflict):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");

        TEST_ASSERT(!bmap.Change(1, "two"), "Change(1, 'two') fails (value already used)");
        string dummy;
        TEST_ASSERT(bmap.Get(1, dummy) && bmap.ContainsValue("one"),
                   "Mapping unchanged after failed change");
    }

    // [3] Change - 非存在的 KEY（应失败）
    cout << "\n[3] Change non-existent key:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");

        TEST_ASSERT(!bmap.Change(999, "new"), "Change(999, 'new') fails");
        TEST_ASSERT(bmap.GetCount() == 1, "Map remains unchanged");
    }

    // [4] ChangeOrAdd - KEY 存在，更新 VALUE
    cout << "\n[4] ChangeOrAdd with existing key:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");

        TEST_ASSERT(bmap.ChangeOrAdd(1, "ONE"), "ChangeOrAdd(1, 'ONE') succeeds");
        TEST_ASSERT(bmap.GetCount() == 2, "Count remains 2");

        string result;
        TEST_ASSERT(bmap.Get(1, result) && result == "ONE", "Value updated to 'ONE'");
    }

    // [5] ChangeOrAdd - KEY 不存在，添加新映射
    cout << "\n[5] ChangeOrAdd with new key:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");

        TEST_ASSERT(bmap.ChangeOrAdd(2, "two"), "ChangeOrAdd(2, 'two') succeeds");
        TEST_ASSERT(bmap.GetCount() == 2, "Count becomes 2");

        string result;
        TEST_ASSERT(bmap.Get(2, result) && result == "two", "New mapping added");
    }

    // [6] ChangeOrAdd - VALUE 被占用，自动删除旧映射
    cout << "\n[6] ChangeOrAdd with value conflict (auto-replace):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");

        // 尝试将 KEY 1 的值改为 "two"，应该删除 KEY 2 -> "two" 的映射
        TEST_ASSERT(bmap.ChangeOrAdd(1, "two"), "ChangeOrAdd(1, 'two') succeeds");
        TEST_ASSERT(bmap.GetCount() == 1, "Count becomes 1 (old mapping removed)");

        TEST_ASSERT(!bmap.ContainsKey(2), "Old key 2 no longer exists");
        TEST_ASSERT(!bmap.ContainsValue("one"), "Old value 'one' no longer exists");
    }

    // [7] ChangeOrAdd - 值相同不修改
    cout << "\n[7] ChangeOrAdd with same value (no change):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");

        TEST_ASSERT(bmap.ChangeOrAdd(1, "one"), "ChangeOrAdd(1, 'one') succeeds");
        TEST_ASSERT(bmap.GetCount() == 1, "Count remains 1");
        TEST_ASSERT(bmap.ContainsValue("one"), "Value still exists");
    }

    // [8] 多次 Change 操作
    cout << "\n[8] Multiple Change operations:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");
        bmap.Add(3, "three");

        TEST_ASSERT(bmap.Change(1, "ONE"), "First change succeeds");
        TEST_ASSERT(bmap.Change(2, "TWO"), "Second change succeeds");
        TEST_ASSERT(bmap.Change(3, "THREE"), "Third change succeeds");

        TEST_ASSERT(bmap.GetCount() == 3, "Count remains 3");
        TEST_ASSERT(bmap.ContainsValue("ONE"), "New value 'ONE' exists");
        TEST_ASSERT(bmap.ContainsValue("TWO"), "New value 'TWO' exists");
        TEST_ASSERT(bmap.ContainsValue("THREE"), "New value 'THREE' exists");
    }

    // [9] Change 后的反向查找
    cout << "\n[9] Reverse lookup after Change:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");

        bmap.Change(1, "ONE");

        int key;
        TEST_ASSERT(bmap.GetByValue("ONE", key) && key == 1,
                   "Reverse lookup works after change");
        TEST_ASSERT(!bmap.GetByValue("one", key),
                   "Old value cannot be found after change");
    }

    // [10] ChangeOrAdd 复杂场景
    cout << "\n[10] ChangeOrAdd complex scenario:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(10, "apple");
        bmap.Add(20, "banana");
        bmap.Add(30, "cherry");

        // 修改 KEY 10 为新值
        bmap.ChangeOrAdd(10, "apricot");
        TEST_ASSERT(bmap.GetCount() == 3, "Count remains 3");

        // 添加新 KEY
        bmap.ChangeOrAdd(40, "date");
        TEST_ASSERT(bmap.GetCount() == 4, "Count becomes 4");

        // 修改 KEY 20 为已存在的值"cherry"，应删除 KEY 30
        bmap.ChangeOrAdd(20, "cherry");
        TEST_ASSERT(bmap.GetCount() == 3, "Count becomes 3 after conflict resolution");
        TEST_ASSERT(!bmap.ContainsKey(30), "Old key 30 removed due to conflict");
    }

    // [11] 空映射的 Change（应失败）
    cout << "\n[11] Change on empty map:" << endl;
    {
        BidirectionalMap<int, string> empty_bmap;
        TEST_ASSERT(!empty_bmap.Change(1, "one"), "Change on empty map fails");
    }

    // [12] ChangeOrAdd 链式操作
    cout << "\n[12] ChangeOrAdd chain operations:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.ChangeOrAdd(1, "one");
        bmap.ChangeOrAdd(2, "two");
        bmap.ChangeOrAdd(3, "three");
        bmap.ChangeOrAdd(1, "ONE");  // 更新第一个
        bmap.ChangeOrAdd(2, "TWO");  // 更新第二个

        TEST_ASSERT(bmap.GetCount() == 3, "Count is 3 after chain operations");
        TEST_ASSERT(bmap.ContainsValue("ONE"), "Updated value 'ONE' exists");
        TEST_ASSERT(bmap.ContainsValue("TWO"), "Updated value 'TWO' exists");
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
