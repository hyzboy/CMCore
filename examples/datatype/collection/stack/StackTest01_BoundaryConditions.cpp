#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 1: Boundary Conditions" << endl;
    cout << "========================================" << endl;

    cout << "\n[1.1] Empty stack operations:" << endl;
    Stack<int> empty_stack;

    assert(empty_stack.IsEmpty());
    assert(empty_stack.GetCount() == 0);

    int val;
    assert(!empty_stack.Pop(val));  // 应该失败
    cout << "  ✓ Pop from empty stack failed as expected" << endl;

    const int* peek_ptr;
    assert(!empty_stack.Peek(peek_ptr));  // 应该失败
    cout << "  ✓ Peek from empty stack failed as expected" << endl;

    cout << "\n[1.2] Large stack test (10000 elements):" << endl;
    Stack<int> large_stack;
    large_stack.Reserve(10000);  // 预分配

    for (int i = 0; i < 10000; i++)
        large_stack.Push(i);

    assert(large_stack.GetCount() == 10000);
    cout << "  ✓ Pushed 10000 elements" << endl;

    for (int i = 9999; i >= 0; i--)
    {
        int popped;
        assert(large_stack.Pop(popped));
        assert(popped == i);
    }

    assert(large_stack.IsEmpty());
    cout << "  ✓ Popped all 10000 elements in correct order" << endl;

    cout << "\n✅ TEST 1 PASSED" << endl;
    return 0;
}
