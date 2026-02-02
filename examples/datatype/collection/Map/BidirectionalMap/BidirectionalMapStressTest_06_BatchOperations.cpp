#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [6] Batch Operations" << endl;
    cout << "========================================" << endl;

    // [6] 批量操作：Add-Change-Delete循环
    cout << "\n[6] Batch Operations (Add-Change-Delete cycle, 10000 rounds):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int ROUNDS = 10000;
        LogOpHeader("[6] Batch Add-Change-Delete");

        for (int round = 0; round < ROUNDS; round++)
        {
            // Add
            string value = "v" + to_string(round);
            bool result = bmap.Add(round, value);
            if (result)
                expected[round] = value;
            LogOp(round * 3, "Add", round, value, result, bmap.GetCount(), (int)expected.size());
            bool ok = VerifyState(bmap, expected, "BatchAdd", round * 3);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BatchAdd Integrity Failure");
                return 1;
            }

            // Change
            value = "modified_v" + to_string(round);
            result = bmap.Change(round, value);
            if (result)
                expected[round] = value;
            LogOp(round * 3 + 1, "Change", round, value, result, bmap.GetCount(), (int)expected.size());
            ok = VerifyState(bmap, expected, "BatchChange", round * 3 + 1);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BatchChange Integrity Failure");
                return 1;
            }

            // Delete
            result = bmap.DeleteByKey(round);
            if (result)
                expected.erase(round);
            LogOp(round * 3 + 2, "Delete", round, "", result, bmap.GetCount(), (int)expected.size());
            ok = VerifyState(bmap, expected, "BatchDelete", round * 3 + 2);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BatchDelete Integrity Failure");
                return 1;
            }
        }

        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after batch operations");
        double elapsed = timer.elapsed_ms();
        cout << "  Total operations: " << (ROUNDS * 3) << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / (ROUNDS * 3) * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
