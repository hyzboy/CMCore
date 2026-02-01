#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [1] Sequential Add" << endl;
    cout << "========================================" << endl;

    // [1] 顺序添加 50000 个元素
    cout << "\n[1] Sequential Add (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int COUNT = 50000;
        LogOpHeader("[1] Sequential Add");
        
        for (int i = 0; i < COUNT; i++)
        {
            const string value = "value_" + to_string(i);
            const bool result = bmap.Add(i, value);
            if (result)
                expected[i] = value;

            LogOp(i, "Add", i, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "SeqAdd", i);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("SeqAdd Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == COUNT, "Count is 50000");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
