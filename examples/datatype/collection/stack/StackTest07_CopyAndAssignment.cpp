#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 7: Copy and Assignment" << endl;
    cout << "========================================" << endl;

    Stack<int> stack1;
    for (int i = 0; i < 5; i++)
        stack1.Push(i * 10);

    cout << "\n[7.1] Copy assignment:" << endl;
    Stack<int> stack2;
    stack2 = stack1;

    assert(stack2.GetCount() == stack1.GetCount());
    assert(stack2.GetCount() == 5);
    cout << "  ✓ Copied " << stack2.GetCount() << " elements" << endl;

    // 验证深拷贝
    int val1, val2;
    stack1.Pop(val1);
    stack2.Pop(val2);

    assert(val1 == val2);
    assert(stack1.GetCount() == 4);
    assert(stack2.GetCount() == 4);
    cout << "  ✓ Deep copy verified (modifications independent)" << endl;

    cout << "\n✅ TEST 7 PASSED" << endl;
    return 0;
}
