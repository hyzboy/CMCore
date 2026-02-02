#include <iostream>
#include <chrono>
#include "hgl/type/BidirectionalMap.h"

using namespace std;
using namespace hgl;

void TestSequentialDeleteLarge()
{
    cout << "=== Test: Sequential Delete (10,000 items) ===\n";
    cout.flush();

    BidirectionalMap<int, string> bmap;

    // Add items
    cout << "Adding items...";
    cout.flush();
    for (int i = 0; i < 10000; i++)
    {
        string val = "val_" + to_string(i);
        if (!bmap.Insert(i, val))
        {
            cerr << "\nERROR: Insert failed at i=" << i << "\n";
            return;
        }
    }
    cout << " OK (" << bmap.GetSize() << " items)\n";
    cout.flush();

    // Delete sequentially
    cout << "Deleting items in sequence...\n";
    cout.flush();
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < 10000; i++)
    {
        if (!bmap.DeleteByKey(i))
        {
            cerr << "ERROR: DeleteByKey(" << i << ") failed. Size: " << bmap.GetSize() << "\n";
            return;
        }
        if ((i + 1) % 1000 == 0)
        {
            cout << "  Deleted " << (i + 1) << " items, remaining: " << bmap.GetSize() << "\n";
            cout.flush();
        }
    }

    auto end = chrono::high_resolution_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(end - start).count();

    cout << "RESULT: All 10,000 items deleted successfully in " << elapsed << " ms\n";
    cout << "Final size: " << bmap.GetSize() << " (expected: 0)\n";

    if (bmap.GetSize() == 0)
        cout << "✓ PASS: Sequential delete test successful!\n";
    else
        cout << "✗ FAIL: Final size not zero\n";

    cout.flush();
}

int main()
{
    TestSequentialDeleteLarge();
    return 0;
}
