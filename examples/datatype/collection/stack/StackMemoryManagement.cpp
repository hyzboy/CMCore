#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n╔══════════════════════════════════════╗" << endl;
    cout << "║  Stack 内存管理测试                  ║" << endl;
    cout << "╚══════════════════════════════════════╝" << endl;

    // TEST 1: 容量管理测试
    cout << "\n[TEST 1] 容量管理测试" << endl;
    {
        Stack<int> stack;
        
        // 预分配容量
        stack.Reserve(100);
        int64 capacity = stack.GetAllocCount();
        assert(capacity >= 100);
        cout << "  ✓ Reserve(100)成功预分配" << endl;

        // 添加元素不应超出预分配容量
        for (int i = 0; i < 100; i++)
            stack.Push(i);
        
        assert(stack.GetCount() == 100);
        int64 new_capacity = stack.GetAllocCount();
        assert(new_capacity >= 100);
        cout << "  ✓ 100个元素在预分配容量内" << endl;
    }

    // TEST 2: Clear vs Free测试
    cout << "\n[TEST 2] Clear vs Free测试" << endl;
    {
        Stack<int> stack;
        for (int i = 0; i < 100; i++)
            stack.Push(i);

        int64 capacity_before = stack.GetAllocCount();
        cout << "  Clear前容量: " << capacity_before << endl;

        stack.Clear();
        assert(stack.IsEmpty());
        assert(stack.GetCount() == 0);
        int64 capacity_after_clear = stack.GetAllocCount();
        cout << "  Clear后容量: " << capacity_after_clear << endl;
        cout << "  ✓ Clear保留了容量" << endl;

        stack.Free();
        assert(stack.IsEmpty());
        int64 capacity_after_free = stack.GetAllocCount();
        cout << "  Free后容量: " << capacity_after_free << endl;
        cout << "  ✓ Free释放了容量" << endl;
    }

    // TEST 3: Resize操作测试
    cout << "\n[TEST 3] Resize操作测试" << endl;
    {
        Stack<int> stack;
        for (int i = 0; i < 50; i++)
            stack.Push(i);

        cout << "  Resize前: " << stack.GetCount() << " 元素" << endl;
        stack.Resize(100);
        cout << "  Resize(100)后: " << stack.GetCount() << " 元素" << endl;
        assert(stack.GetCount() <= 100);
        cout << "  ✓ Resize调整大小" << endl;

        int value;
        assert(stack.Top(value));
        cout << "  ✓ Resize后栈顶元素仍可访问" << endl;
    }

    // TEST 4: 栈的销毁和重用测试
    cout << "\n[TEST 4] 栈的销毁和重用测试" << endl;
    {
        Stack<int> stack;
        stack.Reserve(1000);

        for (int i = 0; i < 50; i++)
            stack.Push(i);

        int64 count_before = stack.GetCount();
        cout << "  Clear前元素数: " << count_before << endl;

        stack.Clear();
        int64 count_after_clear = stack.GetCount();
        assert(count_after_clear == 0);
        cout << "  ✓ Clear后元素数为0" << endl;

        // 重新使用清空的栈
        for (int i = 100; i < 110; i++)
            stack.Push(i);
        
        int value;
        assert(stack.Pop(value) && value == 109);
        cout << "  ✓ Clear后可重新使用栈" << endl;
    }

    // TEST 5: 大容量压力测试
    cout << "\n[TEST 5] 大容量压力测试" << endl;
    {
        Stack<int> stack;
        const int large_count = 100000;

        // 添加大量元素
        for (int i = 0; i < large_count; i++)
            stack.Push(i);
        
        assert(stack.GetCount() == large_count);
        cout << "  ✓ 成功添加" << large_count << "个元素" << endl;

        int value;
        assert(stack.Top(value) && value == large_count - 1);
        cout << "  ✓ 栈顶元素正确" << endl;

        // 弹出所有元素
        for (int i = 0; i < large_count; i++)
        {
            assert(stack.Pop(value));
        }
        assert(stack.IsEmpty());
        cout << "  ✓ 成功弹出所有元素" << endl;
    }

    cout << "\n✅ 内存管理测试全部通过" << endl;
    return 0;
}
