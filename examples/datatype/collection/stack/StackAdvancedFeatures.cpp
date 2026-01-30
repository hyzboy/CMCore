#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║  Stack 高级特性测试                  ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // TEST 1: 移动语义测试
    cout << "\n[TEST 1] 移动语义测试" << endl;
    {
        Stack<int> stack1;
        stack1.Push(1);
        stack1.Push(2);
        stack1.Push(3);

        Stack<int> stack2 = move(stack1);
        assert(stack2.GetCount() == 3);
        assert(stack1.GetCount() == 0);
        cout << "  ✓ 移动构造后原栈为空" << endl;

        int value;
        assert(stack2.Pop(value) && value == 3);
        cout << "  ✓ 移动的栈数据可用" << endl;
    }

    // TEST 2: 拷贝赋值测试
    cout << "\n[TEST 2] 拷贝赋值测试" << endl;
    {
        Stack<int> stack1, stack2;
        stack1.Push(10);
        stack1.Push(20);

        stack2 = stack1;
        assert(stack2.GetCount() == 2);
        assert(stack1.GetCount() == 2);
        cout << "  ✓ 赋值后两栈独立" << endl;

        int v1, v2;
        assert(stack1.Pop(v1) && v1 == 20);
        assert(stack2.Pop(v2) && v2 == 20);
        cout << "  ✓ 拷贝后数据一致" << endl;
    }

    // TEST 3: 遍历操作测试
    cout << "\n[TEST 3] 遍历操作测试" << endl;
    {
        Stack<int> stack;
        for (int i = 1; i <= 5; i++)
            stack.Push(i);

        cout << "  栈内容(从底到顶): ";
        int count = 0;
        stack.ForEachFromBottom([&](const int& val, int idx) {
            cout << val << " ";
            count++;
        });
        cout << endl;
        assert(count == 5);
        cout << "  ✓ ForEachFromBottom遍历所有元素" << endl;

        cout << "  栈内容(从顶到底): ";
        count = 0;
        stack.ForEachFromTop([&](int idx, const int& val) {
            cout << val << " ";
            count++;
        });
        cout << endl;
        assert(count == 5);
        cout << "  ✓ ForEachFromTop遍历所有元素" << endl;
    }

    // TEST 4: 查找操作测试
    cout << "\n[TEST 4] 查找操作测试" << endl;
    {
        Stack<int> stack;
        stack.Push(10);
        stack.Push(20);
        stack.Push(30);
        stack.Push(40);
        stack.Push(50);

        assert(stack.Contains(30));
        cout << "  ✓ Contains找到存在的元素" << endl;

        assert(!stack.Contains(100));
        cout << "  ✓ Contains不存在的元素返回false" << endl;

        assert(stack.Find(30) == 2);
        cout << "  ✓ Find返回正确的索引" << endl;

        assert(stack.Find(100) == -1);
        cout << "  ✓ Find不存在的元素返回-1" << endl;
    }

    // TEST 5: 比较运算符
    cout << "\n[TEST 5] 比较运算符" << endl;
    {
        Stack<int> stack1, stack2;
        stack1.Push(1);
        stack1.Push(2);
        stack1.Push(3);

        stack2.Push(1);
        stack2.Push(2);
        stack2.Push(3);

        assert(stack1 == stack2);
        cout << "  ✓ 相同栈相等" << endl;

        int temp_val;
        stack2.Pop(temp_val);
        assert(stack1 != stack2);
        cout << "  ✓ 不同栈不相等" << endl;
    }

    cout << "\n✅ 高级特性测试全部通过" << endl;
    return 0;
}
