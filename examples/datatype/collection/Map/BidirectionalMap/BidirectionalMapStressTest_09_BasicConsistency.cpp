#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [9] Basic Consistency" << endl;
    cout << "========================================" << endl;

    // [9] 简化的一致性测试
    cout << "\n[9] Basic Consistency Check (50000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        const int OPS = 50000;
        int add_count = 0, delete_count = 0;

        mt19937 rng(88888);
        uniform_int_distribution<int> op_dist(0, 2);
        uniform_int_distribution<int> key_dist(0, 4999);

        Timer timer;
        LogOpHeader("[9] Basic Consistency");

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
                value = "v" + to_string(key);
                result = bmap.Add(key, value);
                if (result)
                {
                    expected[key] = value;
                    add_count++;
                }
            }
            else if (op_type == 1)  // Delete
            {
                op_name = "Delete";
                value = "";
                result = bmap.DeleteByKey(key);
                if (result)
                {
                    expected.erase(key);
                    delete_count++;
                }
            }
            else  // Query
            {
                op_name = "Get";
                value = "v" + to_string(key);
                string dummy;
                result = bmap.Get(key, dummy);
            }

            LogOp(op, op_name, key, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "BasicConsistency", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BasicConsistency Integrity Failure");
                return 1;
            }
        }

        TEST_ASSERT(bmap.GetCount() == add_count - delete_count, "Count = adds - deletes");
        double elapsed = timer.elapsed_ms();
        cout << "  Adds: " << add_count << ", Deletes: " << delete_count << endl;
        cout << "  Final size: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
