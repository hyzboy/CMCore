#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [12] Dense Random Operations" << endl;
    cout << "========================================" << endl;

    // [12] 随机Key范围的密集操作
    cout << "\n[12] Dense Random Operations in Limited Range (20000 ops, 1000 keys):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        
        mt19937 rng(777);
        uniform_int_distribution<int> op_dist(0, 2);
        uniform_int_distribution<int> key_dist(0, 999);  // 仅1000个KEY
        
        Timer timer;
        const int OPS = 20000;
        LogOpHeader("[12] Dense Random Ops");
        
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
            else  // Query
            {
                op_name = "Get";
                value = "v" + to_string(key);
                string dummy;
                result = bmap.Get(key, dummy);
            }

            LogOp(op, op_name, key, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "DenseRandom", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("DenseRandom Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == (int)expected.size(), "Consistency maintained");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final size: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
