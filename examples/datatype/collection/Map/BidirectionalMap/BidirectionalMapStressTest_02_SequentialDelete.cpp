#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [2] Sequential Delete" << endl;
    cout << "========================================" << endl;

    // [2] 顺序删除 50000 个元素
    cout << "\n[2] Sequential Delete (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        const int COUNT = 50000;

        // 先添加
        for (int i = 0; i < COUNT; i++)
        {
            const string value = "value_" + to_string(i);
            bmap.Add(i, value);
            expected[i] = value;
        }

        Timer timer;
        LogOpHeader("[2] Sequential Delete");
        // 顺序删除
        for (int i = 0; i < COUNT; i++)
        {
            const bool result = bmap.DeleteByKey(i);
            if (result)
                expected.erase(i);

            LogOp(i, "Delete", i, "", result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "SeqDelete", i);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("SeqDelete Integrity Failure");
                return 1;
            }
        }

        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after all deletes");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
