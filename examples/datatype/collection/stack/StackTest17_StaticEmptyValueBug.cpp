#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 17: Static Empty Value Bug Verification" << endl;
    cout << "========================================" << endl;
    cout << "⚠️  This test verifies a KNOWN BUG in Stack.h" << endl;
    cout << "    Top() and GetAt() use static empty values" << endl;
    cout << "    which can cause thread-safety and pollution issues" << endl;

    cout << "\n[17.1] Shared static empty value between instances:" << endl;
    Stack<int> stack1, stack2, stack3;
    
    // 所有空栈调用 Top() 应该返回相同的静态对象地址
    const int& ref1 = stack1.Top();
    const int& ref2 = stack2.Top();
    const int& ref3 = stack3.Top();
    
    // 验证它们指向同一个静态对象（地址相同）
    bool same_address = (&ref1 == &ref2) && (&ref2 == &ref3);
    
    if (same_address)
    {
        cout << "  🐛 BUG CONFIRMED: All empty stacks share the same static value" << endl;
        cout << "     Address of stack1.Top(): " << &ref1 << endl;
        cout << "     Address of stack2.Top(): " << &ref2 << endl;
        cout << "     Address of stack3.Top(): " << &ref3 << endl;
    }
    else
    {
        cout << "  ⚠️  Unexpected: Empty stacks don't share static value" << endl;
        cout << "     This might indicate the bug has been fixed" << endl;
    }

    cout << "\n[17.2] Non-const Top() pollution test:" << endl;
    Stack<int> empty_stack1, empty_stack2;
    
    // 通过非 const 版本修改静态空值
    int& mutable_ref = empty_stack1.Top();
    int original_value = mutable_ref;
    mutable_ref = 12345;  // 修改静态空值
    
    // 检查是否影响另一个空栈
    int& other_ref = empty_stack2.Top();
    
    if (other_ref == 12345)
    {
        cout << "  🐛 BUG CONFIRMED: Modifying empty_stack1.Top() affected empty_stack2!" << endl;
        cout << "     empty_stack1.Top() = " << mutable_ref << endl;
        cout << "     empty_stack2.Top() = " << other_ref << endl;
        cout << "     ⚠️  This is a CRITICAL THREAD-SAFETY ISSUE!" << endl;
    }
    else
    {
        cout << "  ✓ Modification didn't affect other stack (bug might be fixed)" << endl;
    }
    
    // 恢复原值以避免影响其他测试
    mutable_ref = original_value;

    cout << "\n[17.3] GetAt() static empty value test:" << endl;
    Stack<int> stack_a, stack_b;
    
    // 越界访问应返回静态空值
    const int& get_a = stack_a.GetAt(-1);
    const int& get_b = stack_b.GetAt(999);
    
    bool same_get_address = (&get_a == &get_b);
    
    if (same_get_address)
    {
        cout << "  🐛 BUG CONFIRMED: GetAt() out-of-bounds returns shared static value" << endl;
        cout << "     Address of GetAt(-1): " << &get_a << endl;
        cout << "     Address of GetAt(999): " << &get_b << endl;
    }
    else
    {
        cout << "  ⚠️  GetAt() doesn't share static value (unexpected)" << endl;
    }

    cout << "\n[17.4] Mixed usage scenario (realistic bug trigger):" << endl;
    Stack<int> user_stack1, user_stack2;
    
    // 用户 A 的栈正常使用
    user_stack1.Push(100);
    user_stack1.Push(200);
    
    // 用户 B 错误地调用空栈的 Top() 并修改
    Stack<int> buggy_empty_stack;
    int& buggy_ref = buggy_empty_stack.Top();
    buggy_ref = 99999;
    
    // 用户 C 使用新的空栈
    Stack<int> innocent_stack;
    int& innocent_ref = innocent_stack.Top();
    
    if (innocent_ref == 99999)
    {
        cout << "  🐛 CRITICAL BUG: User B's modification polluted User C's empty stack!" << endl;
        cout << "     This demonstrates cross-stack data pollution" << endl;
    }
    else
    {
        cout << "  ✓ No pollution detected (bug might be fixed)" << endl;
    }

    cout << "\n[17.5] Recommended fix verification:" << endl;
    cout << "  📝 Proposed Solution 1: Use exceptions" << endl;
    cout << "     const T& Top() const {" << endl;
    cout << "         if (GetCount() <= 0)" << endl;
    cout << "             throw std::out_of_range(\"Top() on empty stack\");" << endl;
    cout << "         return data_array[GetCount() - 1];" << endl;
    cout << "     }" << endl;
    cout << endl;
    cout << "  📝 Proposed Solution 2: Use std::optional (C++17)" << endl;
    cout << "     std::optional<T> Top() const {" << endl;
    cout << "         if (GetCount() <= 0)" << endl;
    cout << "             return std::nullopt;" << endl;
    cout << "         return data_array[GetCount() - 1];" << endl;
    cout << "     }" << endl;
    cout << endl;
    cout << "  📝 Proposed Solution 3: Use assertions (debug mode)" << endl;
    cout << "     const T& Top() const {" << endl;
    cout << "         assert(GetCount() > 0 && \"Top() on empty stack\");" << endl;
    cout << "         return data_array[GetCount() - 1];" << endl;
    cout << "     }" << endl;

    cout << "\n⚠️  TEST 17 COMPLETED - BUG VERIFICATION DONE" << endl;
    cout << "    This test documents a known thread-safety issue" << endl;
    cout << "    Action required: Fix Top() and GetAt() in Stack.h" << endl;
    
    return 0;
}
