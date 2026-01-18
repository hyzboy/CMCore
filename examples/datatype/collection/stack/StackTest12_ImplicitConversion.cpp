#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 12: Implicit Conversion to DataArray" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);
    
    cout << "\n[12.1] Convert to const DataArray:" << endl;
    const DataArray<int>& arr_const = stack;
    assert(arr_const.GetCount() == 5);
    assert(arr_const[0] == 0);
    assert(arr_const[4] == 40);
    cout << "  ✓ Implicit conversion to const DataArray&" << endl;
    
    cout << "\n[12.2] Convert to mutable DataArray:" << endl;
    DataArray<int>& arr_mut = stack;
    
    // 警告：这会破坏栈的LIFO语义！
    arr_mut[0] = 888;
    assert(stack.GetAt(0) == 888);
    cout << "  ⚠️  Mutable conversion allows bypassing stack semantics" << endl;
    cout << "  Note: This is a design concern mentioned in detection report" << endl;

    cout << "\n✅ TEST 12 PASSED" << endl;
    return 0;
}
