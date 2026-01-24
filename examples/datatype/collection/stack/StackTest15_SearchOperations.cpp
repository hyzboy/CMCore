#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 15: Search Operations (Contains & Find)" << endl;
    cout << "========================================" << endl;

    cout << "\n[15.1] Contains() - Basic functionality:" << endl;
    ValueStack<int> stack;
    
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);  // 0, 10, 20, 30, 40
    
    assert(stack.Contains(0));
    assert(stack.Contains(20));
    assert(stack.Contains(40));
    assert(!stack.Contains(5));
    assert(!stack.Contains(100));
    cout << "  ✓ Contains() correctly identifies existing and non-existing elements" << endl;

    cout << "\n[15.2] Contains() - Empty stack:" << endl;
    ValueStack<int> empty_stack;
    assert(!empty_stack.Contains(0));
    assert(!empty_stack.Contains(42));
    cout << "  ✓ Contains() returns false for empty stack" << endl;

    cout << "\n[15.3] Contains() - Duplicate values:" << endl;
    ValueStack<int> dup_stack;
    dup_stack.Push(100);
    dup_stack.Push(200);
    dup_stack.Push(100);  // 重复值
    dup_stack.Push(300);
    dup_stack.Push(100);  // 再次重复
    
    assert(dup_stack.Contains(100));
    assert(dup_stack.Contains(200));
    assert(dup_stack.Contains(300));
    assert(!dup_stack.Contains(400));
    cout << "  ✓ Contains() works with duplicate values" << endl;

    cout << "\n[15.4] Find() - Basic functionality:" << endl;
    stack.Clear();
    for (int i = 0; i < 5; i++)
        stack.Push(i * 10);  // 0, 10, 20, 30, 40
    
    assert(stack.Find(0) == 0);    // 栈底
    assert(stack.Find(20) == 2);   // 中间
    assert(stack.Find(40) == 4);   // 栈顶
    assert(stack.Find(5) == -1);   // 不存在
    assert(stack.Find(100) == -1); // 不存在
    cout << "  ✓ Find() returns correct indices (0=bottom, Count-1=top)" << endl;

    cout << "\n[15.5] Find() - Empty stack:" << endl;
    assert(empty_stack.Find(0) == -1);
    assert(empty_stack.Find(42) == -1);
    cout << "  ✓ Find() returns -1 for empty stack" << endl;

    cout << "\n[15.6] Find() - First occurrence of duplicates:" << endl;
    dup_stack.Clear();
    dup_stack.Push(100);  // index 0
    dup_stack.Push(200);  // index 1
    dup_stack.Push(100);  // index 2
    dup_stack.Push(300);  // index 3
    dup_stack.Push(100);  // index 4
    
    int index = dup_stack.Find(100);
    assert(index == 0);  // 应返回第一个出现的索引
    cout << "  ✓ Find() returns first occurrence index: " << index << endl;

    cout << "\n[15.7] Find() - Bottom and top elements:" << endl;
    stack.Clear();
    stack.Push(999);   // index 0 (bottom)
    stack.Push(500);
    stack.Push(888);   // index 2 (top)
    
    assert(stack.Find(999) == 0);  // 栈底
    assert(stack.Find(888) == 2);  // 栈顶
    cout << "  ✓ Find() correctly identifies bottom (0) and top (Count-1)" << endl;

    cout << "\n[15.8] Performance test (10000 elements):" << endl;
    ValueStack<int> large_stack;
    large_stack.Reserve(10000);
    
    for (int i = 0; i < 10000; i++)
        large_stack.Push(i);
    
    // 查找存在的元素
    assert(large_stack.Contains(0));
    assert(large_stack.Contains(5000));
    assert(large_stack.Contains(9999));
    
    // 查找不存在的元素
    assert(!large_stack.Contains(10000));
    assert(!large_stack.Contains(-1));
    
    // 查找索引
    assert(large_stack.Find(0) == 0);
    assert(large_stack.Find(5000) == 5000);
    assert(large_stack.Find(9999) == 9999);
    assert(large_stack.Find(10000) == -1);
    
    cout << "  ✓ Contains() and Find() work with 10000 elements" << endl;

    cout << "\n[15.9] Edge case - Single element:" << endl;
    ValueStack<int> single_stack;
    single_stack.Push(42);
    
    assert(single_stack.Contains(42));
    assert(!single_stack.Contains(0));
    assert(single_stack.Find(42) == 0);
    assert(single_stack.Find(0) == -1);
    cout << "  ✓ Contains() and Find() work with single element" << endl;

    cout << "\n[15.10] Type test - Negative numbers:" << endl;
    ValueStack<int> neg_stack;
    neg_stack.Push(-10);
    neg_stack.Push(0);
    neg_stack.Push(10);
    
    assert(neg_stack.Contains(-10));
    assert(neg_stack.Contains(0));
    assert(neg_stack.Contains(10));
    assert(neg_stack.Find(-10) == 0);
    assert(neg_stack.Find(0) == 1);
    assert(neg_stack.Find(10) == 2);
    cout << "  ✓ Contains() and Find() work with negative numbers" << endl;

    cout << "\n✅ TEST 15 PASSED - All search operations work correctly!" << endl;
    return 0;
}
