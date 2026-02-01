#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [4] Random Operations" << endl;
    cout << "========================================" << endl;

    // [4] 随机操作压力测试
    cout << "\n[4] Random Operations (100000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        mt19937 rng(12345);  // 固定种子保证可复现
        uniform_int_distribution<int> op_dist(0, 2);  // 0:Add, 1:Delete, 2:Change
        uniform_int_distribution<int> key_dist(0, 9999);
        
        Timer timer;
        const int OPS = 100000;
        LogOpHeader("[4] Random Ops");
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            string value;
            bool result = false;
            const char* op_name = "";
            
            if (op_type == 0)  // Add
            {
                op_name = "Add";
                value = "random_" + to_string(key);
                result = bmap.Add(key, value);
                if (result)
                    expected[key] = value;
            }
            else if (op_type == 1)  // Delete
            {
                op_name = "Delete";
                value = "";
                result = bmap.DeleteByKey(key);
                if (result)
                    expected.erase(key);
            }
            else  // Change
            {
                op_name = "Change";
                value = "modified_" + to_string(key);
                result = bmap.Change(key, value);
                if (result)
                    expected[key] = value;
            }

            LogOp(op, op_name, key, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "RandomOps", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("RandomOps Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == (int)expected.size(), "Count matches expected");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final count: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
