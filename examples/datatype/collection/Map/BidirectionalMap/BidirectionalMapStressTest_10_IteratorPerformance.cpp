#include "StressTestCommon.h"

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress [10] Iterator Performance" << endl;
    cout << "========================================" << endl;

    // [10] 迭代器性能测试
    cout << "\n[10] Iterator Performance (50000 items, 1000 iterations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        const int ITEMS = 50000;

        for (int i = 0; i < ITEMS; i++)
            bmap.Add(i, "value_" + to_string(i));

        Timer timer;
        const int ITERATIONS = 1000;
        long long sum = 0;

        for (int iter = 0; iter < ITERATIONS; iter++)
        {
            for (auto [k, v] : bmap)
                sum += k;
        }

        double elapsed = timer.elapsed_ms();
        long long total_items = (long long)ITEMS * ITERATIONS;
        cout << "  Total iterations: " << total_items << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / total_items * 1000) << " us/item)" << endl;
        TEST_ASSERT(sum > 0, "Iterator traversal completed");
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
