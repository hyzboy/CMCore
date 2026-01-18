#include<hgl/type/Stack.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST 3: Batch Operations" << endl;
    cout << "========================================" << endl;

    Stack<int> stack;

    cout << "\n[3.1] Batch push:" << endl;
    int batch_data[] = {10, 20, 30, 40, 50};
    assert(stack.Push(batch_data, 5));
    assert(stack.GetCount() == 5);
    cout << "  ✓ Pushed 5 elements in batch" << endl;

    cout << "\n[3.2] Batch pop:" << endl;
    int popped_data[3];
    assert(stack.Pop(popped_data, 3));

    // 批量弹出从底部开始返回：30, 40, 50（存储顺序）
    assert(popped_data[0] == 30);
    assert(popped_data[1] == 40);
    assert(popped_data[2] == 50);
    assert(stack.GetCount() == 2);
    cout << "  ✓ Popped 3 elements: " << popped_data[0] << ", "
         << popped_data[1] << ", " << popped_data[2] << endl;

    cout << "\n[3.3] Batch pop exceeding size:" << endl;
    int large_buffer[10];
    assert(!stack.Pop(large_buffer, 10));  // 应该失败，只剩2个元素
    cout << "  ✓ Batch pop correctly failed" << endl;

    cout << "\n✅ TEST 3 PASSED" << endl;
    return 0;
}
