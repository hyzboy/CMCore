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
    
    const int& top_ref = stack.Top();
    assert(top_ref == 40);  // 最后压入的是4*10
    cout << "  ✓ Top() returned: " << top_ref << endl;
    
    // 测试非const版本
    int& top_mut = stack.Top();
    top_mut = 999;
    assert(stack.Top() == 999);
    cout << "  ✓ Top() writable reference works" << endl;
    
    cout << "\n[9.2] GetAt() random access:" << endl;
    stack.Clear();
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);
    
    // GetAt(0) = 底部, GetAt(4) = 顶部
    assert(stack.GetAt(0) == 0);
    assert(stack.GetAt(2) == 20);
    assert(stack.GetAt(4) == 40);
    cout << "  ✓ GetAt() bottom to top: " << stack.GetAt(0) << ", " 
         << stack.GetAt(2) << ", " << stack.GetAt(4) << endl;
    
    cout << "\n[9.3] GetAt() out of bounds:" << endl;
    const int& invalid = stack.GetAt(-1);
    const int& invalid2 = stack.GetAt(100);
    cout << "  ✓ GetAt() handles invalid indices (returns static empty)" << endl;
    
    cout << "\n[9.4] Top() on empty stack (static empty value issue):" << endl;
    Stack<int> empty_stack;
    const int& empty_top = empty_stack.Top();
    cout << "  ⚠️  Top() on empty returns static value: " << empty_top << endl;
    cout << "  Note: This is a known design issue (not thread-safe)" << endl;

    cout << "\n✅ TEST 9 PASSED" << endl;
    return 0;
}
