#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 10: Resize Functionality" << endl;
    cout << "========================================" << endl;

    cout << "\n[10.1] Resize to larger size:" << endl;
    {
        Stack<int> stack;
        stack.Push(100);
        stack.Push(200);

        stack.Resize(5);  // 扩大到5个元素
        assert(stack.GetCount() == 5);
        cout << "  ✓ Resized to 5 elements" << endl;
    }

    cout << "\n[10.2] Resize to smaller size:" << endl;
    {
        Stack<int> stack;
        for (int i = 0; i < 10; i++)
            stack.Push(i);

        stack.Resize(5);
        assert(stack.GetCount() == 5);

        int result;
        // 验证保留了前5个元素
        for (int i = 0; i < 5; i++)
        {
            assert(stack.GetAt(i,result));
            assert(result == i);
        }

        cout << "  ✓ Resized down to 5 elements, data preserved" << endl;
    }

    cout << "\n✅ TEST 10 PASSED" << endl;
    return 0;
}
