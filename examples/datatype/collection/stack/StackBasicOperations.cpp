#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║  Stack 基础操作测试                  ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // TEST 1: 边界条件测试
    cout << "\n[TEST 1] 边界条件测试" << endl;
    {
        Stack<int> stack;
        assert(stack.IsEmpty());
        assert(stack.GetCount() == 0);
        cout << "  ✓ 空栈检测正确" << endl;

        int value;
        assert(!stack.Pop(value));
        cout << "  ✓ 空栈弹出返回false" << endl;

        assert(!stack.Top(value));
        cout << "  ✓ 空栈Top返回false" << endl;

        assert(!stack.GetAt(0, value));
        cout << "  ✓ 空栈GetAt返回false" << endl;

        stack.Push(42);
        assert(stack.Top(value) && value == 42);
        cout << "  ✓ Push后Top成功返回值" << endl;

        assert(stack.Pop(value) && value == 42);
        assert(stack.IsEmpty());
        cout << "  ✓ Pop后栈为空" << endl;
    }

    // TEST 2: Peek/Top 函数测试
    cout << "\n[TEST 2] Peek/Top 函数测试" << endl;
    {
        Stack<int> stack;
        stack.Push(10);
        stack.Push(20);
        stack.Push(30);

        int value;
        assert(stack.Top(value) && value == 30);
        assert(stack.GetCount() == 3);
        cout << "  ✓ Top不改变栈大小" << endl;

        // 多次调用Top应返回相同值
        assert(stack.Top(value) && value == 30);
        cout << "  ✓ 多次Top返回一致的值" << endl;
    }

    // TEST 3: 批量操作测试
    cout << "\n[TEST 3] 批量操作测试" << endl;
    {
        Stack<int> stack;
        
        // 批量Push
        int data[] = {1, 2, 3, 4, 5};
        assert(stack.Push(data, 5));
        assert(stack.GetCount() == 5);
        cout << "  ✓ 批量Push成功" << endl;

        // 批量Pop
        int out[5] = {0};
        assert(stack.Pop(out, 5));
        // 批量Pop按原数组顺序返回（从栈底到栈顶）
        assert(out[0] == 1 && out[1] == 2 && out[2] == 3 && out[3] == 4 && out[4] == 5);
        cout << "  ✓ 批量Pop返回完整数据" << endl;
        assert(stack.IsEmpty());
        cout << "  ✓ 批量Pop后栈为空" << endl;
    }

    // TEST 4: 部分批量操作
    cout << "\n[TEST 4] 部分批量操作" << endl;
    {
        Stack<int> stack;
        int data[] = {10, 20, 30, 40, 50};
        stack.Push(data, 5);

        int out[3] = {0};
        assert(stack.Pop(out, 3));
        // 批量Pop从栈底到栈顶顺序返回
        assert(out[0] == 30 && out[1] == 40 && out[2] == 50);
        assert(stack.GetCount() == 2);
        cout << "  ✓ 部分批量Pop正确" << endl;

        int val;
        assert(stack.Pop(val) && val == 20);
        assert(stack.Pop(val) && val == 10);
        cout << "  ✓ 剩余元素弹出正确" << endl;
    }

    // TEST 5: GetAt 随机访问
    cout << "\n[TEST 5] GetAt 随机访问" << endl;
    {
        Stack<int> stack;
        for (int i = 0; i < 10; i++)
            stack.Push(i * 10);

        int value;
        assert(stack.GetAt(0, value) && value == 0);
        cout << "  ✓ GetAt(0)返回栈底元素" << endl;

        assert(stack.GetAt(9, value) && value == 90);
        cout << "  ✓ GetAt(9)返回栈顶元素" << endl;

        assert(!stack.GetAt(-1, value));
        assert(!stack.GetAt(10, value));
        cout << "  ✓ GetAt越界返回false" << endl;
    }

    cout << "\n✅ 基础操作测试全部通过" << endl;
    return 0;
}
