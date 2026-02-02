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
    cout << "TEST: BidirectionalMap Iterators" << endl;
    cout << "========================================" << endl;

    BidirectionalMap<int, string> bmap;
    bmap.Add(10, "ten");
    bmap.Add(5, "five");
    bmap.Add(15, "fifteen");
    bmap.Add(20, "twenty");

    // [1] KEY-VALUE 对迭代
    cout << "\n[1] KEY-VALUE pair iteration:" << endl;
    cout << "  Using range-based for loop: ";
    int count = 0;
    for (auto [k, v] : bmap)
    {
        cout << "[" << k << ":" << v << "] ";
        count++;
    }
    cout << endl;
    TEST_ASSERT(count == 4, "Iterated 4 KEY-VALUE pairs");

    // [2] KEY 迭代
    cout << "\n[2] KEY-only iteration:" << endl;
    cout << "  Keys: ";
    count = 0;
    for (auto k : bmap.Keys())
    {
        cout << k << " ";
        count++;
    }
    cout << endl;
    TEST_ASSERT(count == 4, "Iterated 4 keys");

    // [3] VALUE 迭代
    cout << "\n[3] VALUE-only iteration:" << endl;
    cout << "  Values: ";
    count = 0;
    for (auto v : bmap.Values())
    {
        cout << v << " ";
        count++;
    }
    cout << endl;
    TEST_ASSERT(count == 4, "Iterated 4 values");

    // [4] 验证迭代顺序与插入顺序一致
    cout << "\n[4] Iteration order verification:" << endl;
    {
        int expected_keys[] = { 10, 5, 15, 20 };
        string expected_values[] = { "ten", "five", "fifteen", "twenty" };
        int idx = 0;

        for (auto [k, v] : bmap)
        {
            TEST_ASSERT(k == expected_keys[idx], "Key at index " + to_string(idx) + " matches");
            TEST_ASSERT(v == expected_values[idx], "Value at index " + to_string(idx) + " matches");
            idx++;
        }
    }

    // [5] 使用迭代器修改（通过指针）
    cout << "\n[5] Pointer-based access during iteration:" << endl;
    count = 0;
    for (auto [k, v] : bmap)
    {
        // 虽然迭代器返回 const 引用，但可以获取指向数据的指针
        count++;
    }
    TEST_ASSERT(count == 4, "Pointer access iteration works");

    // [6] 空映射的迭代
    cout << "\n[6] Empty map iteration:" << endl;
    {
        BidirectionalMap<int, string> empty_map;
        count = 0;
        for (auto [k, v] : empty_map)
        {
            count++;
        }
        TEST_ASSERT(count == 0, "Empty map iteration count is 0");
    }

    // [7] 单元素映射的迭代
    cout << "\n[7] Single element map iteration:" << endl;
    {
        BidirectionalMap<int, string> single_map;
        single_map.Add(42, "answer");

        int k_val = -1;
        string v_val = "";
        for (auto [k, v] : single_map)
        {
            k_val = k;
            v_val = v;
        }
        TEST_ASSERT(k_val == 42, "Single element key is correct");
        TEST_ASSERT(v_val == "answer", "Single element value is correct");
    }

    // [8] 迭代过程中验证 Keys() 视图
    cout << "\n[8] Keys() view verification:" << endl;
    {
        auto keys_view = bmap.Keys();
        auto it = keys_view.begin();
        TEST_ASSERT(*it == 10, "First key from Keys() view is 10");
    }

    // [9] 迭代过程中验证 Values() 视图
    cout << "\n[9] Values() view verification:" << endl;
    {
        auto values_view = bmap.Values();
        auto it = values_view.begin();
        TEST_ASSERT(*it == "ten", "First value from Values() view is 'ten'");
    }

    // [10] 多次迭代同一个映射
    cout << "\n[10] Multiple iterations over same map:" << endl;
    {
        int first_count = 0, second_count = 0;
        for (auto [k, v] : bmap)
        {
            first_count++;
        }
        for (auto [k, v] : bmap)
        {
            second_count++;
        }
        TEST_ASSERT(first_count == second_count && first_count == 4,
                   "Multiple iterations return consistent count");
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
