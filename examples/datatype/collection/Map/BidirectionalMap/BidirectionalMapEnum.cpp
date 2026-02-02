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
    cout << "TEST: BidirectionalMap Enum Operations" << endl;
    cout << "========================================" << endl;

    BidirectionalMap<int, string> bmap;
    bmap.Add(1, "one");
    bmap.Add(2, "two");
    bmap.Add(3, "three");
    bmap.Add(4, "four");
    bmap.Add(5, "five");

    // [1] EnumKV 基本功能
    cout << "\n[1] EnumKV (KEY-VALUE pairs):" << endl;
    {
        int count = 0;
        int sum_keys = 0;
        cout << "  Enumerated: ";
        bmap.EnumKV([&](const int& k, const string& v) {
            cout << "[" << k << ":" << v << "] ";
            count++;
            sum_keys += k;
        });
        cout << endl;
        TEST_ASSERT(count == 5, "EnumKV called 5 times");
        TEST_ASSERT(sum_keys == 1+2+3+4+5, "Sum of enumerated keys is 15");
    }

    // [2] EnumKeys 基本功能
    cout << "\n[2] EnumKeys:" << endl;
    {
        int count = 0;
        int sum_keys = 0;
        cout << "  Keys: ";
        bmap.EnumKeys([&](const int& k) {
            cout << k << " ";
            count++;
            sum_keys += k;
        });
        cout << endl;
        TEST_ASSERT(count == 5, "EnumKeys called 5 times");
        TEST_ASSERT(sum_keys == 15, "Sum of enumerated keys is 15");
    }

    // [3] EnumValues 基本功能
    cout << "\n[3] EnumValues:" << endl;
    {
        int count = 0;
        cout << "  Values: ";
        bmap.EnumValues([&](const string& v) {
            cout << v << " ";
            count++;
        });
        cout << endl;
        TEST_ASSERT(count == 5, "EnumValues called 5 times");
    }

    // [4] EnumKV 中修改外部状态
    cout << "\n[4] EnumKV with state modification:" << endl;
    {
        vector<pair<int, string>> collected;
        bmap.EnumKV([&](const int& k, const string& v) {
            collected.push_back({k, v});
        });
        TEST_ASSERT(collected.size() == 5, "Collected 5 pairs");
        TEST_ASSERT(collected[0].first == 1 && collected[0].second == "one",
                   "First collected pair is (1, 'one')");
    }

    // [5] EnumKeys 收集所有 KEY
    cout << "\n[5] EnumKeys with collection:" << endl;
    {
        vector<int> keys;
        bmap.EnumKeys([&](const int& k) {
            keys.push_back(k);
        });
        TEST_ASSERT(keys.size() == 5, "Collected 5 keys");
        TEST_ASSERT(keys[0] == 1 && keys[4] == 5, "Keys are in insertion order");
    }

    // [6] EnumValues 收集所有 VALUE
    cout << "\n[6] EnumValues with collection:" << endl;
    {
        vector<string> values;
        bmap.EnumValues([&](const string& v) {
            values.push_back(v);
        });
        TEST_ASSERT(values.size() == 5, "Collected 5 values");
        TEST_ASSERT(values[0] == "one" && values[4] == "five",
                   "Values are in insertion order");
    }

    // [7] EnumKV 条件提前终止（通过 lambda 捕获）
    cout << "\n[7] EnumKV with conditional processing:" << endl;
    {
        int count = 0;
        bmap.EnumKV([&](const int& k, const string& v) {
            if (k <= 3)  // 仅处理前3个
            {
                count++;
            }
        });
        TEST_ASSERT(count == 3, "Processed 3 items conditionally");
    }

    // [8] EnumKeys 字符串连接
    cout << "\n[8] EnumKeys with string concatenation:" << endl;
    {
        string key_str;
        bmap.EnumKeys([&](const int& k) {
            if (!key_str.empty()) key_str += ",";
            key_str += to_string(k);
        });
        TEST_ASSERT(key_str == "1,2,3,4,5", "Keys concatenated correctly: " + key_str);
    }

    // [9] EnumValues 字符串连接
    cout << "\n[9] EnumValues with string concatenation:" << endl;
    {
        string val_str;
        bmap.EnumValues([&](const string& v) {
            if (!val_str.empty()) val_str += ",";
            val_str += v;
        });
        TEST_ASSERT(val_str == "one,two,three,four,five",
                   "Values concatenated correctly: " + val_str);
    }

    // [10] 空映射的枚举
    cout << "\n[10] Enum on empty map:" << endl;
    {
        BidirectionalMap<int, string> empty_bmap;
        int count = 0;
        empty_bmap.EnumKV([&](const int& k, const string& v) {
            count++;
        });
        TEST_ASSERT(count == 0, "EnumKV on empty map not called");
    }

    // [11] 单元素映射的枚举
    cout << "\n[11] Enum on single-element map:" << endl;
    {
        BidirectionalMap<int, string> single_bmap;
        single_bmap.Add(42, "answer");

        int k_result = -1;
        string v_result = "";
        single_bmap.EnumKV([&](const int& k, const string& v) {
            k_result = k;
            v_result = v;
        });
        TEST_ASSERT(k_result == 42 && v_result == "answer",
                   "Single element enumeration works");
    }

    // [12] Lambda 捕获验证
    cout << "\n[12] Lambda capture verification:" << endl;
    {
        int outer_sum = 100;
        bmap.EnumKV([&](const int& k, const string& v) {
            outer_sum += k;
        });
        TEST_ASSERT(outer_sum == 100 + 15,
                   "Lambda correctly captured and modified outer variable");
    }

    // [13] EnumKV 中的异常安全性（虚拟）
    cout << "\n[13] Enum robustness with complex logic:" << endl;
    {
        vector<pair<int, string>> results;
        bmap.EnumKV([&](const int& k, const string& v) {
            // 复杂的处理逻辑
            if (k % 2 == 0)
            {
                results.push_back({k, v + "_EVEN"});
            }
            else
            {
                results.push_back({k, v + "_ODD"});
            }
        });
        TEST_ASSERT(results.size() == 5, "Processed all 5 items");
        TEST_ASSERT(results[0].second == "one_ODD", "First item correctly processed");
        TEST_ASSERT(results[1].second == "two_EVEN", "Second item correctly processed");
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
