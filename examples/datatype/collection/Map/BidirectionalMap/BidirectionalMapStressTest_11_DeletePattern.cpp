#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [11] Delete Pattern" << endl;
    cout << "========================================" << endl;

    // [11] 删除模式压力测试
    cout << "\n[11] Delete Pattern Stress (5 add, 4 delete pattern):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        int next_id = 0;
        Timer timer;
        const int ROUNDS = 10000;
        LogOpHeader("[11] Delete Pattern Stress");
        int op_index = 0;

        for (int round = 0; round < ROUNDS; round++)
        {
            // Add 5
            for (int i = 0; i < 5; i++)
            {
                const int key = next_id;
                const string value = "v" + to_string(next_id);
                const bool result = bmap.Add(key, value);
                if (result)
                    expected[key] = value;

                LogOp(op_index++, "Add", key, value, result, bmap.GetCount(), (int)expected.size());
                const bool ok = VerifyState(bmap, expected, "DeletePatternAdd", op_index);
                if (!ok)
                {
                    if (kDumpOnFailure)
                        bmap.DebugDump("DeletePattern Add Integrity Failure");
                    return 1;
                }

                next_id++;
            }

            // Delete 4 (oldest)
            for (int i = 0; i < 4; i++)
            {
                int del_id = (round * 5) + i;
                if (del_id < next_id)
                {
                    const bool result = bmap.DeleteByKey(del_id);
                    if (result)
                        expected.erase(del_id);

                    LogOp(op_index++, "Delete", del_id, "", result, bmap.GetCount(), (int)expected.size());
                    const bool ok = VerifyState(bmap, expected, "DeletePatternDelete", op_index);
                    if (!ok)
                    {
                        if (kDumpOnFailure)
                            bmap.DebugDump("DeletePattern Delete Integrity Failure");
                        return 1;
                    }
                }
            }
        }

        int expected_count = ROUNDS * 5 - ROUNDS * 4;
        TEST_ASSERT(bmap.GetCount() == expected_count, "Count matches pattern");

        double elapsed = timer.elapsed_ms();
        cout << "  Rounds: " << ROUNDS << endl;
        cout << "  Final count: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
