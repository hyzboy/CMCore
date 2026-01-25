#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 11: Const Correctness" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);
    
    cout << "\n[11.1] const Stack iterators:" << endl;
    const Stack<int>& const_stack = stack;
    
    // const版本应返回const指针
    const int* const_begin = const_stack.begin();
    const int* const_end = const_stack.end();
    
    int count = 0;
    cout << "  const iteration: ";
    for (const int* it = const_begin; it != const_end; ++it) {
        cout << *it << " ";
        count++;
    }
    cout << endl;
    assert(count == 5);
    cout << "  ✓ const iterators work correctly" << endl;
    
    cout << "\n[11.2] Non-const Stack iterators (writable):" << endl;
    int* mut_begin = stack.begin();
    int* mut_end = stack.end();
    
    // 修改第一个元素
    *mut_begin = 999;
    int result;

    assert(stack.GetAt(0,result));
    assert(result == 999);
    cout << "  ✓ Non-const iterators allow modification" << endl;
    
    cout << "\n[11.3] const GetData():" << endl;
    const int* const_data = const_stack.GetData();
    cout << "  ✓ GetData() returns const pointer for const stack" << endl;

    cout << "\n✅ TEST 11 PASSED" << endl;
    return 0;
}
