#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 8: Capacity Management" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;

    cout << "\n[8.1] Reserve capacity:" << endl;
    assert(stack.Reserve(1000));
    assert(stack.GetAllocCount() >= 1000);
    assert(stack.GetCount() == 0);  // 不影响元素数量
    cout << "  ✓ Reserved capacity: " << stack.GetAllocCount() << endl;

    cout << "\n[8.2] Push without reallocation:" << endl;
    int alloc_before = stack.GetAllocCount();

    for (int i = 0; i < 500; i++)
        stack.Push(i);

    int alloc_after = stack.GetAllocCount();
    assert(alloc_before == alloc_after);  // 不应重新分配
    cout << "  ✓ Pushed 500 elements without reallocation" << endl;

    cout << "\n[8.3] Clear and verify capacity retained:" << endl;
    stack.Clear();
    assert(stack.GetCount() == 0);
    assert(stack.GetAllocCount() == alloc_after);  // 容量保留
    cout << "  ✓ Capacity retained after Clear()" << endl;

    cout << "\n[8.4] Free and verify capacity released:" << endl;
    stack.Free();
    assert(stack.GetCount() == 0);
    assert(stack.GetAllocCount() == 0);
    cout << "  ✓ Capacity released after Free()" << endl;

    cout << "\n✅ TEST 8 PASSED" << endl;
    return 0;
}
