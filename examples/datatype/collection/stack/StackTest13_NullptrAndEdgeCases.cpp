#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 13: Nullptr and Edge Cases" << endl;
    cout << "========================================" << endl;

    cout << "\n[13.1] Push nullptr array:" << endl;
    Stack<int> stack;
    assert(!stack.Push(nullptr, 5));  // 应该失败
    assert(stack.GetCount() == 0);
    cout << "  ✓ Push(nullptr, count) correctly rejected" << endl;
    
    cout << "\n[13.2] Push with zero/negative count:" << endl;
    int data[] = {1, 2, 3};
    assert(!stack.Push(data, 0));
    assert(!stack.Push(data, -1));
    assert(stack.GetCount() == 0);
    cout << "  ✓ Push with invalid count rejected" << endl;
    
    cout << "\n[13.3] Pop more elements than available:" << endl;
    stack.Push(10);
    stack.Push(20);
    
    int buffer[10];
    assert(!stack.Pop(buffer, 10));  // 只有2个元素，不能pop 10个
    assert(stack.GetCount() == 2);  // 失败后不应改变状态
    cout << "  ✓ Pop with insufficient elements rejected" << endl;
    
    cout << "\n[13.4] GetTotalBytes():" << endl;
    Stack<int> byte_stack;
    byte_stack.Push(1);
    byte_stack.Push(2);
    byte_stack.Push(3);
    
    int total_bytes = byte_stack.GetTotalBytes();
    assert(total_bytes == 3 * sizeof(int));
    cout << "  ✓ GetTotalBytes() = " << total_bytes << " (3 × " << sizeof(int) << ")" << endl;

    cout << "\n✅ TEST 13 PASSED" << endl;
    return 0;
}
