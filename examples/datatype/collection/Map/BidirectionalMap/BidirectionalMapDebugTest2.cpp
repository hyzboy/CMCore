#include<hgl/platform/Platform.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<random>
#include<set>
#include<string>

using namespace hgl;
using namespace std;

#define TEST_ASSERT(condition, msg) \
    if (!(condition)) { \
        cout << "❌ ASSERTION FAILED: " << msg << endl; \
        cout << "   at line " << __LINE__ << endl; \
        bmap.DebugDump("State at failure"); \
        return 1; \
    }

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "BidirectionalMap Debug Test (INT ↔ STRING)" << endl;
    cout << "========================================\n" << endl;

    BidirectionalMap<int, string> bmap;
    set<int> present_keys;

    mt19937 rng(12345);  // 固定种子
    uniform_int_distribution<int> op_dist(0, 2);  // 0:Add, 1:Delete, 2:Change
    uniform_int_distribution<int> key_dist(0, 9999);

    int op_count = 0;

    for (int i = 0; i < 50000; i++)
    {
        int op_type = op_dist(rng);
        int key = key_dist(rng);
        string value = "v" + to_string(key);

        // 进度显示
        if (i % 500 == 0)
            cout << "." << flush;
        if (i % 5000 == 0 && i > 0)
            cout << " [" << i << " ops, size=" << bmap.GetCount() << "]" << endl;

        if (op_type == 0)  // Add
        {
            if (present_keys.find(key) == present_keys.end())
            {
                bool result = bmap.Add(key, value);
                if (result)
                {
                    present_keys.insert(key);
                    op_count++;

                    // 立即验证
                    TEST_ASSERT(bmap.GetCount() == (int)present_keys.size(),
                               "Size mismatch after Add");
                    TEST_ASSERT(bmap.ContainsKey(key),
                               "Key " + to_string(key) + " not found after Add");
                    TEST_ASSERT(bmap.ContainsValue(value),
                               "Value " + value + " not found after Add");

                    string check_val;
                    TEST_ASSERT(bmap.Get(key, check_val) && check_val == value,
                               "Get failed after Add");
                }
            }
        }
        else if (op_type == 1)  // Delete
        {
            if (present_keys.find(key) != present_keys.end())
            {
                bool result = bmap.DeleteByKey(key);
                if (result)
                {
                    present_keys.erase(key);
                    op_count++;

                    // 立即验证
                    TEST_ASSERT(bmap.GetCount() == (int)present_keys.size(),
                               "Size mismatch after Delete");
                    TEST_ASSERT(!bmap.ContainsKey(key),
                               "Key " + to_string(key) + " still exists after Delete");

                    // 验证所有剩余的键都能找到
                    for (int k : present_keys)
                    {
                        TEST_ASSERT(bmap.ContainsKey(k),
                                   "Key " + to_string(k) + " lost after deleting " + to_string(key));
                    }
                }
            }
        }
        else  // Change
        {
            if (present_keys.find(key) != present_keys.end())
            {
                string new_value = "modified_" + to_string(key);
                bool result = bmap.Change(key, new_value);
                if (result)
                {
                    op_count++;

                    // 立即验证
                    TEST_ASSERT(bmap.GetCount() == (int)present_keys.size(),
                               "Size changed after Change");
                    TEST_ASSERT(bmap.ContainsKey(key),
                               "Key lost after Change");
                    TEST_ASSERT(bmap.ContainsValue(new_value),
                               "New value not found after Change");

                    string check_val;
                    TEST_ASSERT(bmap.Get(key, check_val) && check_val == new_value,
                               "Get returned wrong value after Change");
                }
            }
        }
    }

    cout << "\n\n========================================" << endl;
    cout << "✅ ALL OPERATIONS COMPLETED!" << endl;
    cout << "========================================" << endl;
    cout << "Total successful operations: " << op_count << endl;
    cout << "Final size: " << bmap.GetCount() << endl;
    cout << "Reference size: " << present_keys.size() << endl;

    TEST_ASSERT(bmap.GetCount() == (int)present_keys.size(), "Final size mismatch");

    cout << "\n✅ All checks passed!" << endl;

    return 0;
}
