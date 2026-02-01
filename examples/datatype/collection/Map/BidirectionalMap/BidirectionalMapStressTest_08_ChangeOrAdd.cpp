#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [8] ChangeOrAdd" << endl;
    cout << "========================================" << endl;

    // [8] ChangeOrAdd压力测试
    cout << "\n[8] ChangeOrAdd Stress (50000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int OPS = 50000;
        mt19937 rng(54321);
        uniform_int_distribution<int> key_dist(0, 4999);  // 较小范围造成冲突
        LogOpHeader("[8] ChangeOrAdd Stress");
        
        for (int op = 0; op < OPS; op++)
        {
            int key = key_dist(rng);
            string value = "v" + to_string(key) + "_" + to_string(op);
            bmap.ChangeOrAdd(key, value);
            expected[key] = value;

            LogOp(op, "ChangeOrAdd", key, value, true, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "ChangeOrAdd", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("ChangeOrAdd Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() <= 5000, "Map size bounded by key range");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final unique keys: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
