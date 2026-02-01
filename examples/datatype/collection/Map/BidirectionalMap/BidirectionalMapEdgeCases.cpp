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
    cout << "TEST: BidirectionalMap Edge Cases" << endl;
    cout << "========================================" << endl;

    // [1] 重复添加到空映射
    cout << "\n[1] Add to empty map multiple times:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        TEST_ASSERT(bmap.Add(1, "one"), "First add succeeds");
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1");
        TEST_ASSERT(!bmap.Add(1, "one_duplicate"), "Duplicate key rejected");
        TEST_ASSERT(bmap.GetCount() == 1, "Count still 1");
    }

    // [2] 空映射的查询
    cout << "\n[2] Query empty map:" << endl;
    {
        BidirectionalMap<int, string> empty_bmap;
        string result;
        int key_result;
        TEST_ASSERT(!empty_bmap.Get(1, result), "Get on empty map returns false");
        TEST_ASSERT(!empty_bmap.GetByValue("anything", key_result), 
                   "GetByValue on empty map returns false");
    }

    // [3] 删除后重新添加相同的 KEY
    cout << "\n[3] Delete and re-add same key:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        TEST_ASSERT(bmap.DeleteByKey(1), "Delete succeeds");
        TEST_ASSERT(bmap.Add(1, "ONE"), "Re-add with same key succeeds");
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1");
    }

    // [4] 删除后重新添加相同的 VALUE
    cout << "\n[4] Delete and re-add same value:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        TEST_ASSERT(bmap.DeleteByValue("one"), "Delete by value succeeds");
        TEST_ASSERT(bmap.Add(2, "one"), "Re-add with same value succeeds");
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1");
    }

    // [5] 大量 KEY-VALUE 对
    cout << "\n[5] Large dataset (10000 items):" << endl;
    {
        BidirectionalMap<int, string> large_bmap;
        const int LARGE_SIZE = 10000;
        
        // 添加大量数据
        for (int i = 0; i < LARGE_SIZE; i++)
        {
            TEST_ASSERT(large_bmap.Add(i, "value_" + to_string(i)), 
                       "Add item " + to_string(i));
            if (i != LARGE_SIZE - 1) test_passed--;  // 避免冗长输出
        }
        test_passed++;  // 恢复计数
        
        TEST_ASSERT(large_bmap.GetCount() == LARGE_SIZE, 
                   "Count is " + to_string(LARGE_SIZE));
        
        // 查询中间项
        string result;
        TEST_ASSERT(large_bmap.Get(5000, result) && result == "value_5000", 
                   "Query middle item succeeds");
    }

    // [6] 所有 KEY 都是同一类型的边界值
    cout << "\n[6] Boundary values for integer keys:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        TEST_ASSERT(bmap.Add(0, "zero"), "Add key 0");
        TEST_ASSERT(bmap.Add(-1, "negative_one"), "Add key -1");
        TEST_ASSERT(bmap.Add(INT_MAX, "max_int"), "Add INT_MAX");
        TEST_ASSERT(bmap.Add(INT_MIN, "min_int"), "Add INT_MIN");
        TEST_ASSERT(bmap.GetCount() == 4, "Count is 4");
    }

    // [7] 空字符串作为 VALUE
    cout << "\n[7] Empty string as value:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        TEST_ASSERT(bmap.Add(1, ""), "Add with empty string value");
        
        string result;
        TEST_ASSERT(bmap.Get(1, result) && result == "", "Retrieved empty string");
        TEST_ASSERT(bmap.ContainsValue(""), "ContainsValue for empty string works");
    }

    // [8] 删除所有项后再添加
    cout << "\n[8] Delete all then re-populate:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");
        bmap.Add(3, "three");
        
        bmap.DebugDump("After Add 1,2,3");
        
        bmap.DeleteByKey(1);
        bmap.DebugDump("After DeleteByKey(1)");
        
        bmap.DeleteByKey(2);
        bmap.DebugDump("After DeleteByKey(2)");
        
        bmap.DeleteByKey(3);
        bmap.DebugDump("After DeleteByKey(3)");
        
        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after deleting all");
        
        TEST_ASSERT(bmap.Add(10, "ten"), "Add to cleared map");
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1");
    }

    // [9] Clear 和 Free 的区别
    cout << "\n[9] Clear vs Free:" << endl;
    {
        BidirectionalMap<int, string> bmap1;
        bmap1.Add(1, "one");
        bmap1.Clear();
        TEST_ASSERT(bmap1.IsEmpty(), "After Clear, map is empty");
        TEST_ASSERT(bmap1.Add(2, "two"), "Can add after Clear");
        
        BidirectionalMap<int, string> bmap2;
        bmap2.Add(1, "one");
        bmap2.Free();
        TEST_ASSERT(bmap2.IsEmpty(), "After Free, map is empty");
        TEST_ASSERT(bmap2.Add(3, "three"), "Can add after Free");
    }

    // [10] 覆盖引用的迭代
    cout << "\n[10] Iteration after Clear:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");
        bmap.Add(3, "three");
        
        bmap.Clear();
        
        int count = 0;
        for (auto [k, v] : bmap)
        {
            count++;
        }
        TEST_ASSERT(count == 0, "Iteration count is 0 after Clear");
    }

    // [11] 连续的 Add-Delete 交替
    cout << "\n[11] Alternating Add and Delete:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        
        bmap.Add(1, "one");
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1");
        
        bmap.Add(2, "two");
        TEST_ASSERT(bmap.GetCount() == 2, "Count is 2");
        
        bmap.DeleteByKey(1);
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1 after delete");
        
        bmap.Add(3, "three");
        TEST_ASSERT(bmap.GetCount() == 2, "Count is 2 after re-add");
        
        bmap.DeleteByValue("two");
        TEST_ASSERT(bmap.GetCount() == 1, "Count is 1 after value delete");
    }

    // [12] GetKeys 和 GetValues 返回有效引用
    cout << "\n[12] GetKeys and GetValues references:" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "one");
        bmap.Add(2, "two");
        
        const auto& keys = bmap.GetKeys();
        const auto& values = bmap.GetValues();
        
        TEST_ASSERT(keys.size() == 2, "GetKeys returns size 2");
        TEST_ASSERT(values.size() == 2, "GetValues returns size 2");
        TEST_ASSERT(keys[0] == 1 && keys[1] == 2, "Keys are correct");
        TEST_ASSERT(values[0] == "one" && values[1] == "two", "Values are correct");
    }

    // [13] 针对纯 KEY 值的冲突测试
    cout << "\n[13] Key conflict handling (extensive):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "a");
        
        // 尝试多次添加同一 KEY
        TEST_ASSERT(!bmap.Add(1, "b"), "Cannot add duplicate key (attempt 1)");
        TEST_ASSERT(!bmap.Add(1, "c"), "Cannot add duplicate key (attempt 2)");
        TEST_ASSERT(bmap.GetCount() == 1, "Count remains 1");
        
        string result;
        TEST_ASSERT(bmap.Get(1, result) && result == "a", "Value unchanged");
    }

    // [14] 针对纯 VALUE 值的冲突测试
    cout << "\n[14] Value conflict handling (extensive):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        bmap.Add(1, "shared");
        
        // 尝试多次添加同一 VALUE
        TEST_ASSERT(!bmap.Add(2, "shared"), "Cannot add duplicate value (attempt 1)");
        TEST_ASSERT(!bmap.Add(3, "shared"), "Cannot add duplicate value (attempt 2)");
        TEST_ASSERT(bmap.GetCount() == 1, "Count remains 1");
        
        int result;
        TEST_ASSERT(bmap.GetByValue("shared", result) && result == 1, 
                   "Value still mapped to original key");
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
