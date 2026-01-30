#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 12: Implicit Conversion to ValueBuffer" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);

    cout << "\n[12.1] Convert to const std::vector:" << endl;
    const std::vector<int>& arr_const = stack;
    assert((int)arr_const.size() == 5);
    assert(arr_const[0] == 0);
    assert(arr_const[4] == 40);
    cout << "  ✓ Implicit conversion to const std::vector&" << endl;

    cout << "\n[12.2] Convert to mutable std::vector:" << endl;
    std::vector<int>& arr_mut = stack;

    // 警告：这会破坏栈的LIFO语义！
    arr_mut[0] = 888;
    int result;

    assert(stack.GetAt(0,result));
    assert(result == 888);
    cout << "  ⚠️  Mutable conversion allows bypassing stack semantics" << endl;
    cout << "  Note: This is a design concern mentioned in detection report" << endl;

    cout << "\n✅ TEST 12 PASSED" << endl;
    return 0;
}
