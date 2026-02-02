#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [7] Query-Heavy Operations" << endl;
    cout << "========================================" << endl;

    // [7] 查询密集型操作
    cout << "\n[7] Query-Heavy Operations (100000 queries on 10000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        const int ITEMS = 10000;

        // 添加10000个元素
        for (int i = 0; i < ITEMS; i++)
        {
            const string value = "value_" + to_string(i);
            bmap.Add(i, value);
            expected[i] = value;
        }

        Timer timer;
        const int QUERIES = 100000;
        string dummy;
        int key_dummy;
        LogOpHeader("[7] Query-Heavy Ops");

        for (int q = 0; q < QUERIES; q++)
        {
            int key = q % ITEMS;
            bool result = false;
            string value = "value_" + to_string(key);

            // 交替进行正向和反向查询
            if (q % 2 == 0)
            {
                result = bmap.Get(key, dummy);
                LogOp(q, "Get", key, value, result, bmap.GetCount(), (int)expected.size());
            }
            else
            {
                result = bmap.GetByValue(value, key_dummy);
                LogOp(q, "GetByValue", key, value, result, bmap.GetCount(), (int)expected.size());
            }

            const bool ok = VerifyState(bmap, expected, "QueryHeavy", q);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("QueryHeavy Integrity Failure");
                return 1;
            }
        }

        double elapsed = timer.elapsed_ms();
        cout << "  Total queries: " << QUERIES << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / QUERIES * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
