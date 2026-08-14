#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║  Stack Advanced Features Test        ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // TEST 1: 移动语义测试
    cout << "\n[TEST 1] Move semantics" << endl;
    {
        Stack<int> stack1;
        stack1.Push(1);
        stack1.Push(2);
        stack1.Push(3);

        Stack<int> stack2 = move(stack1);
        assert(stack2.GetCount() == 3);
        assert(stack1.GetCount() == 0);
        cout << "  ✓ Source stack empty after move construction" << endl;

        int value;
        assert(stack2.Pop(value) && value == 3);
        cout << "  ✓ Moved stack data usable" << endl;
    }

    // TEST 2: 拷贝赋值测试
    cout << "\n[TEST 2] Copy assignment" << endl;
    {
        Stack<int> stack1, stack2;
        stack1.Push(10);
        stack1.Push(20);

        stack2 = stack1;
        assert(stack2.GetCount() == 2);
        assert(stack1.GetCount() == 2);
        cout << "  ✓ Stacks independent after assignment" << endl;

        int v1, v2;
        assert(stack1.Pop(v1) && v1 == 20);
        assert(stack2.Pop(v2) && v2 == 20);
        cout << "  ✓ Data consistent after copy" << endl;
    }

    // TEST 3: 遍历操作测试
    cout << "\n[TEST 3] Traversal" << endl;
    {
        Stack<int> stack;
        for (int i = 1; i <= 5; i++)
            stack.Push(i);

        cout << "  Stack content (bottom to top): ";
        int count = 0;
        stack.ForEachFromBottom([&](const int& val, int idx) {
            cout << val << " ";
            count++;
        });
        cout << endl;
        assert(count == 5);
        cout << "  ✓ ForEachFromBottom visits all elements" << endl;

        cout << "  Stack content (top to bottom): ";
        count = 0;
        stack.ForEachFromTop([&](int idx, const int& val) {
            cout << val << " ";
            count++;
        });
        cout << endl;
        assert(count == 5);
        cout << "  ✓ ForEachFromTop visits all elements" << endl;
    }

    // TEST 4: 查找操作测试
    cout << "\n[TEST 4] Find operations" << endl;
    {
        Stack<int> stack;
        stack.Push(10);
        stack.Push(20);
        stack.Push(30);
        stack.Push(40);
        stack.Push(50);

        assert(stack.Contains(30));
        cout << "  ✓ Contains finds existing element" << endl;

        assert(!stack.Contains(100));
        cout << "  ✓ Contains returns false for missing element" << endl;

        assert(stack.Find(30) == 2);
        cout << "  ✓ Find returns correct index" << endl;

        assert(stack.Find(100) == -1);
        cout << "  ✓ Find returns -1 for missing element" << endl;
    }

    // TEST 5: 比较运算符
    cout << "\n[TEST 5] Comparison operators" << endl;
    {
        Stack<int> stack1, stack2;
        stack1.Push(1);
        stack1.Push(2);
        stack1.Push(3);

        stack2.Push(1);
        stack2.Push(2);
        stack2.Push(3);

        assert(stack1 == stack2);
        cout << "  ✓ Identical stacks equal" << endl;

        int temp_val;
        stack2.Pop(temp_val);
        assert(stack1 != stack2);
        cout << "  ✓ Different stacks not equal" << endl;
    }

    cout << "\n✅ All advanced feature tests passed" << endl;
    return 0;
}
