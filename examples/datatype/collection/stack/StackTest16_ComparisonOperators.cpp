#include<hgl/platform/Platform.h>
#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 16: Comparison Operators (== and !=)" << endl;
    cout << "========================================" << endl;

    cout << "\n[16.1] operator== - Two empty stacks:" << endl;
    Stack<int> empty1, empty2;
    assert(empty1 == empty2);
    cout << "  ✓ Two empty stacks are equal" << endl;

    cout << "\n[16.2] operator== - Identical content:" << endl;
    Stack<int> stack1, stack2;

    for (int i = 0; i < 5; i++)
    {
        stack1.Push(i * 10);
        stack2.Push(i * 10);
    }

    assert(stack1 == stack2);
    cout << "  ✓ Two stacks with identical content are equal" << endl;

    cout << "\n[16.3] operator== - Different lengths:" << endl;
    Stack<int> stack3, stack4;

    stack3.Push(10);
    stack3.Push(20);
    stack3.Push(30);

    stack4.Push(10);
    stack4.Push(20);

    assert(!(stack3 == stack4));
    cout << "  ✓ Stacks with different lengths are not equal" << endl;

    cout << "\n[16.4] operator== - Same length, different content:" << endl;
    Stack<int> stack5, stack6;

    stack5.Push(10);
    stack5.Push(20);
    stack5.Push(30);

    stack6.Push(10);
    stack6.Push(20);
    stack6.Push(99);  // 不同的最后一个元素

    assert(!(stack5 == stack6));
    cout << "  ✓ Stacks with same length but different content are not equal" << endl;

    cout << "\n[16.5] operator== - Self comparison:" << endl;
    Stack<int> stack7;
    stack7.Push(100);
    stack7.Push(200);

    assert(stack7 == stack7);
    cout << "  ✓ Stack is equal to itself" << endl;

    cout << "\n[16.6] operator== - After modifications:" << endl;
    Stack<int> stack8, stack9;

    stack8.Push(10);
    stack8.Push(20);

    stack9.Push(10);
    stack9.Push(20);

    assert(stack8 == stack9);

    // 修改一个栈
    stack8.Push(30);
    assert(!(stack8 == stack9));

    // 修改另一个栈使其相等
    stack9.Push(30);
    assert(stack8 == stack9);
    cout << "  ✓ Equality changes correctly after modifications" << endl;

    cout << "\n[16.7] operator!= - Basic functionality:" << endl;
    Stack<int> stack10, stack11;

    stack10.Push(100);
    stack11.Push(200);

    assert(stack10 != stack11);
    cout << "  ✓ operator!= returns true for different stacks" << endl;

    cout << "\n[16.8] operator!= - Consistency with operator==:" << endl;
    Stack<int> stack12, stack13;

    stack12.Push(10);
    stack12.Push(20);

    stack13.Push(10);
    stack13.Push(20);

    // operator== 为 true 时，operator!= 应为 false
    assert(stack12 == stack13);
    assert(!(stack12 != stack13));

    // 修改后
    stack12.Push(30);
    assert(!(stack12 == stack13));
    assert(stack12 != stack13);
    cout << "  ✓ operator!= is consistent with operator==" << endl;

    cout << "\n[16.9] Large stack comparison (1000 elements):" << endl;
    Stack<int> large1, large2;
    large1.Reserve(1000);
    large2.Reserve(1000);

    for (int i = 0; i < 1000; i++)
    {
        large1.Push(i);
        large2.Push(i);
    }

    assert(large1 == large2);
    cout << "  ✓ Large stacks with 1000 identical elements are equal" << endl;

    // 修改一个元素
    int* data = large2.begin();
    data[500] = 99999;  // 修改中间元素

    assert(!(large1 == large2));
    assert(large1 != large2);
    cout << "  ✓ Large stacks differ after single element change" << endl;

    cout << "\n[16.10] Empty vs non-empty:" << endl;
    Stack<int> empty_test, non_empty_test;
    non_empty_test.Push(42);

    assert(!(empty_test == non_empty_test));
    assert(empty_test != non_empty_test);
    cout << "  ✓ Empty stack is not equal to non-empty stack" << endl;

    cout << "\n[16.11] Order matters:" << endl;
    Stack<int> order1, order2;

    order1.Push(10);
    order1.Push(20);
    order1.Push(30);

    order2.Push(30);
    order2.Push(20);
    order2.Push(10);

    assert(!(order1 == order2));
    cout << "  ✓ Stacks with same elements in different order are not equal" << endl;

    cout << "\n[16.12] After clear operations:" << endl;
    Stack<int> clear1, clear2;

    clear1.Push(10);
    clear1.Push(20);
    clear2.Push(30);
    clear2.Push(40);

    assert(clear1 != clear2);

    clear1.Clear();
    clear2.Clear();

    assert(clear1 == clear2);
    cout << "  ✓ Both stacks are equal after clearing" << endl;

    cout << "\n[16.13] Stress test - Identical large datasets:" << endl;
    Stack<int> stress1, stress2;
    stress1.Reserve(10000);
    stress2.Reserve(10000);

    for (int i = 0; i < 10000; i++)
    {
        stress1.Push(i % 100);  // 0-99 循环
        stress2.Push(i % 100);
    }

    assert(stress1 == stress2);
    cout << "  ✓ Stress test: 10000 elements compared successfully" << endl;

    cout << "\n✅ TEST 16 PASSED - All comparison operators work correctly!" << endl;
    return 0;
}
