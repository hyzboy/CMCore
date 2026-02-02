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
    cout << "TEST: BidirectionalMap Basic Operations" << endl;
    cout << "========================================" << endl;

    BidirectionalMap<int, string> bmap;

    // [1] 空映射检查
    cout << "\n[1] Empty map checks:" << endl;
    TEST_ASSERT(bmap.IsEmpty(), "Initial map is empty");
    TEST_ASSERT(bmap.GetCount() == 0, "Initial count is 0");

    // [2] 添加操作
    cout << "\n[2] Add operations:" << endl;
    TEST_ASSERT(bmap.Add(1, "one"), "Add(1, 'one') succeeds");
    TEST_ASSERT(bmap.Add(2, "two"), "Add(2, 'two') succeeds");
    TEST_ASSERT(bmap.Add(3, "three"), "Add(3, 'three') succeeds");
    TEST_ASSERT(bmap.GetCount() == 3, "Count is 3 after adding 3 items");
    TEST_ASSERT(!bmap.IsEmpty(), "Map is not empty after adding");

    // [3] 重复 KEY 检查（应该失败）
    cout << "\n[3] Duplicate key detection:" << endl;
    TEST_ASSERT(!bmap.Add(1, "ONE"), "Add with duplicate key fails");
    TEST_ASSERT(bmap.GetCount() == 3, "Count remains 3 after failed add");

    // [4] 重复 VALUE 检查（应该失败）
    cout << "\n[4] Duplicate value detection:" << endl;
    TEST_ASSERT(!bmap.Add(4, "one"), "Add with duplicate value fails");
    TEST_ASSERT(bmap.GetCount() == 3, "Count remains 3 after duplicate value add");

    // [5] 向前查找（KEY -> VALUE）
    cout << "\n[5] Forward lookup (KEY -> VALUE):" << endl;
    string value;
    TEST_ASSERT(bmap.Get(1, value) && value == "one", "Get(1) returns 'one'");
    TEST_ASSERT(bmap.Get(2, value) && value == "two", "Get(2) returns 'two'");
    TEST_ASSERT(bmap.Get(3, value) && value == "three", "Get(3) returns 'three'");
    TEST_ASSERT(!bmap.Get(999, value), "Get(999) returns false");

    // [6] 反向查找（VALUE -> KEY）
    cout << "\n[6] Reverse lookup (VALUE -> KEY):" << endl;
    int key;
    TEST_ASSERT(bmap.GetByValue("one", key) && key == 1, "GetByValue('one') returns 1");
    TEST_ASSERT(bmap.GetByValue("two", key) && key == 2, "GetByValue('two') returns 2");
    TEST_ASSERT(bmap.GetByValue("three", key) && key == 3, "GetByValue('three') returns 3");
    TEST_ASSERT(!bmap.GetByValue("nonexistent", key), "GetByValue('nonexistent') returns false");

    // [7] 指针查询
    cout << "\n[7] Pointer queries:" << endl;
    const string* pval = bmap.GetValuePointer(1);
    TEST_ASSERT(pval != nullptr && *pval == "one", "GetValuePointer(1) returns valid pointer");

    const int* pkey = bmap.GetKeyPointer("two");
    TEST_ASSERT(pkey != nullptr && *pkey == 2, "GetKeyPointer('two') returns valid pointer");

    // [8] 包含检查
    cout << "\n[8] Contains checks:" << endl;
    TEST_ASSERT(bmap.ContainsKey(1), "ContainsKey(1) returns true");
    TEST_ASSERT(!bmap.ContainsKey(999), "ContainsKey(999) returns false");
    TEST_ASSERT(bmap.ContainsValue("one"), "ContainsValue('one') returns true");
    TEST_ASSERT(!bmap.ContainsValue("nonexistent"), "ContainsValue('nonexistent') returns false");

    // [9] 根据 KEY 删除
    cout << "\n[9] Delete by key:" << endl;
    TEST_ASSERT(bmap.DeleteByKey(1), "DeleteByKey(1) succeeds");
    TEST_ASSERT(bmap.GetCount() == 2, "Count becomes 2 after delete");
    TEST_ASSERT(!bmap.ContainsKey(1), "Key 1 no longer exists");
    TEST_ASSERT(!bmap.ContainsValue("one"), "Value 'one' no longer exists");
    TEST_ASSERT(!bmap.DeleteByKey(1), "DeleteByKey(1) again returns false");

    // [10] 根据 VALUE 删除
    cout << "\n[10] Delete by value:" << endl;
    TEST_ASSERT(bmap.DeleteByValue("two"), "DeleteByValue('two') succeeds");
    TEST_ASSERT(bmap.GetCount() == 1, "Count becomes 1 after delete");
    TEST_ASSERT(!bmap.ContainsValue("two"), "Value 'two' no longer exists");
    TEST_ASSERT(!bmap.ContainsKey(2), "Key 2 no longer exists");
    TEST_ASSERT(!bmap.DeleteByValue("two"), "DeleteByValue('two') again returns false");

    // [11] 清空操作
    cout << "\n[11] Clear operations:" << endl;
    bmap.Add(10, "ten");
    TEST_ASSERT(bmap.GetCount() == 2, "Map has 2 items before clear");
    bmap.Clear();
    TEST_ASSERT(bmap.IsEmpty(), "Map is empty after clear");
    TEST_ASSERT(bmap.GetCount() == 0, "Count is 0 after clear");

    // [12] 重新使用已清空的映射
    cout << "\n[12] Reuse after clear:" << endl;
    TEST_ASSERT(bmap.Add(100, "hundred"), "Add to cleared map succeeds");
    TEST_ASSERT(bmap.GetCount() == 1, "Count is 1 after re-adding");
    TEST_ASSERT(bmap.Get(100, value) && value == "hundred", "Can retrieve re-added item");

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
