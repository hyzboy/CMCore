#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 9: Top() and GetAt() Methods" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;
    
    cout << "\n[9.1] Top() on non-empty stack:" << endl;
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);
    
    int top_val;
    assert(stack.Top(top_val) && top_val == 40);  // 最后压入的是4*10
    cout << "  ✓ Top() returned: " << top_val << endl;
    
    cout << "\n[9.2] GetAt() random access:" << endl;
    stack.Clear();
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);
    
    // GetAt(0) = 底部, GetAt(4) = 顶部
    int val0, val2, val4;
    assert(stack.GetAt(0, val0) && val0 == 0);
    assert(stack.GetAt(2, val2) && val2 == 20);
    assert(stack.GetAt(4, val4) && val4 == 40);
    cout << "  ✓ GetAt() bottom to top: " << val0 << ", " 
         << val2 << ", " << val4 << endl;
    
    cout << "\n[9.3] GetAt() out of bounds:" << endl;
    int dummy;
    assert(!stack.GetAt(-1, dummy));
    assert(!stack.GetAt(100, dummy));
    cout << "  ✓ GetAt() handles invalid indices (returns false)" << endl;
    
    cout << "\n[9.4] Top() on empty stack:" << endl;
    Stack<int> empty_stack;
    int empty_val;
    assert(!empty_stack.Top(empty_val));
    cout << "  ✓ Top() on empty returns false" << endl;

    cout << "\n[9.5] Bottom() access:" << endl;
    stack.Clear();
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);
    
    int bottom_val;
    assert(stack.Bottom(bottom_val) && bottom_val == 0);
    cout << "  ✓ Bottom() returned: " << bottom_val << endl;

    cout << "\n✅ TEST 9 PASSED" << endl;
    return 0;
}
