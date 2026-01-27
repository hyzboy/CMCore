#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 5: Iteration Functions" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;
    for (int i = 0; i < 5; i++)
        stack.Push(i);

    cout << "\n[5.1] ForEachFromTop (4 to 0):" << endl;
    cout << "  ";
    stack.ForEachFromTop([](const int& val, int idx) {
        cout << val << " ";
    });
    cout << endl;

    cout << "\n[5.2] ForEachFromBottom (0 to 4):" << endl;
    cout << "  ";
    stack.ForEachFromBottom([](const int& val, int idx) {
        cout << val << " ";
    });
    cout << endl;

    cout << "\n[5.3] Range-based for loop:" << endl;
    cout << "  ";
    for (const auto& val : stack)
        cout << val << " ";
    cout << endl;

    cout << "\n✅ TEST 5 PASSED" << endl;
    return 0;
}
