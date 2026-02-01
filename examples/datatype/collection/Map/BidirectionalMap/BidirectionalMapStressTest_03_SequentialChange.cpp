#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [3] Sequential Change" << endl;
    cout << "========================================" << endl;

    // [3] 顺序修改 50000 个元素
    cout << "\n[3] Sequential Change (50000 items):" << endl;
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
        LogOpHeader("[3] Sequential Change");
        // 顺序修改
        for (int i = 0; i < COUNT; i++)
        {
            const string new_value = "changed_" + to_string(i);
            const bool result = bmap.Change(i, new_value);
            if (result)
                expected[i] = new_value;

            LogOp(i, "Change", i, new_value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "SeqChange", i);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("SeqChange Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == COUNT, "Count remains 50000");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
