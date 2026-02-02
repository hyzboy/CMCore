#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [5] Alternating Add-Delete" << endl;
    cout << "========================================" << endl;

    // [5] 交替Add-Delete操作
    cout << "\n[5] Alternating Add-Delete (20000 rounds):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int ROUNDS = 20000;
        LogOpHeader("[5] Alternating Add-Delete");

        for (int round = 0; round < ROUNDS; round++)
        {
            // 添加
            const string value = "value_" + to_string(round);
            bool result = bmap.Add(round, value);
            if (result)
                expected[round] = value;

            LogOp(round * 2, "Add", round, value, result, bmap.GetCount(), (int)expected.size());

            bool ok = VerifyState(bmap, expected, "AltAdd", round * 2);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("AltAdd Integrity Failure");
                return 1;
            }

            // 立即删除
            result = bmap.DeleteByKey(round);
            if (result)
                expected.erase(round);

            LogOp(round * 2 + 1, "Delete", round, "", result, bmap.GetCount(), (int)expected.size());

            ok = VerifyState(bmap, expected, "AltDelete", round * 2 + 1);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("AltDelete Integrity Failure");
                return 1;
            }
        }

        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after all add-delete pairs");
        double elapsed = timer.elapsed_ms();
        cout << "  Total operations: " << (ROUNDS * 2) << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / (ROUNDS * 2) * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
