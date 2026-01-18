#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 2: Peek Function" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;

    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);

    const int* peek_ptr;
    assert(stack.Peek(peek_ptr));
    assert(*peek_ptr == 40);  // 最后压入的是4*10
    cout << "  ✓ Peeked top element: " << *peek_ptr << endl;

    assert(stack.GetCount() == 5);  // Peek不应改变栈
    cout << "  ✓ Stack count unchanged after Peek" << endl;

    int popped;
    stack.Pop(popped);
    assert(popped == 40);

    assert(stack.Peek(peek_ptr));
    assert(*peek_ptr == 30);
    cout << "  ✓ Peeked new top element: " << *peek_ptr << endl;

    cout << "\n✅ TEST 2 PASSED" << endl;
    return 0;
}
