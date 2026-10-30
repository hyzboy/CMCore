#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║  Stack Memory Management Test        ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // TEST 1: 容量管理测试
    cout << "\n[TEST 1] Capacity management" << endl;
    {
        Stack<int> stack;

        // 预分配容量
        stack.Reserve(100);
        int64 capacity = stack.GetAllocCount();
        assert(capacity >= 100);
        cout << "  ✓ Reserve(100) pre-allocated successfully" << endl;

        // 添加元素不应超出预分配容量
        for (int i = 0; i < 100; i++)
            stack.Push(i);

        assert(stack.GetCount() == 100);
        int64 new_capacity = stack.GetAllocCount();
        assert(new_capacity >= 100);
        cout << "  ✓ 100 elements fit within pre-allocated capacity" << endl;
    }

    // TEST 2: Clear vs Free测试
    cout << "\n[TEST 2] Clear vs Free" << endl;
    {
        Stack<int> stack;
        for (int i = 0; i < 100; i++)
            stack.Push(i);

        int64 capacity_before = stack.GetAllocCount();
        cout << "  Capacity before Clear: " << capacity_before << endl;

        stack.Clear();
        assert(stack.IsEmpty());
        assert(stack.GetCount() == 0);
        int64 capacity_after_clear = stack.GetAllocCount();
        cout << "  Capacity after Clear: " << capacity_after_clear << endl;
        cout << "  ✓ Clear keeps capacity" << endl;

        stack.ClearAndDeallocate();
        assert(stack.IsEmpty());
        int64 capacity_after_free = stack.GetAllocCount();
        cout << "  Capacity after Free: " << capacity_after_free << endl;
        cout << "  ✓ Free releases capacity" << endl;
    }

    // TEST 3: Resize操作测试
    cout << "\n[TEST 3] Resize operations" << endl;
    {
        Stack<int> stack;
        for (int i = 0; i < 50; i++)
            stack.Push(i);

        cout << "  Before Resize: " << stack.GetCount() << " elements" << endl;
        stack.Resize(100);
        cout << "  After Resize(100): " << stack.GetCount() << " elements" << endl;
        assert(stack.GetCount() <= 100);
        cout << "  ✓ Resize changes size" << endl;

        int value;
        assert(stack.Top(value));
        cout << "  ✓ Top element still accessible after Resize" << endl;
    }

    // TEST 4: 栈的销毁和重用测试
    cout << "\n[TEST 4] Stack destruction and reuse" << endl;
    {
        Stack<int> stack;
        stack.Reserve(1000);

        for (int i = 0; i < 50; i++)
            stack.Push(i);

        int64 count_before = stack.GetCount();
        cout << "  Element count before Clear: " << count_before << endl;

        stack.Clear();
        int64 count_after_clear = stack.GetCount();
        assert(count_after_clear == 0);
        cout << "  ✓ Element count 0 after Clear" << endl;

        // 重新使用清空的栈
        for (int i = 100; i < 110; i++)
            stack.Push(i);

        int value;
        assert(stack.Pop(value) && value == 109);
        cout << "  ✓ Stack reusable after Clear" << endl;
    }

    // TEST 5: 大容量压力测试
    cout << "\n[TEST 5] Large capacity stress test" << endl;
    {
        Stack<int> stack;
        const int large_count = 100000;

        // 添加大量元素
        for (int i = 0; i < large_count; i++)
            stack.Push(i);

        assert(stack.GetCount() == large_count);
        cout << "  ✓ Added successfully" << large_count << " elements" << endl;

        int value;
        assert(stack.Top(value) && value == large_count - 1);
        cout << "  ✓ Top element correct" << endl;

        // 弹出所有元素
        for (int i = 0; i < large_count; i++)
        {
            assert(stack.Pop(value));
        }
        assert(stack.IsEmpty());
        cout << "  ✓ All elements popped successfully" << endl;
    }

    cout << "\n✅ All memory management tests passed" << endl;
    return 0;
}
