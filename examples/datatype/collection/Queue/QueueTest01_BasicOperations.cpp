#include<hgl/type/DataType.h>
#include<hgl/type/Queue.h>
#include<iostream>
#include<cassert>

using namespace hgl;
using namespace std;

int os_main(int, os_char **)
{
    cout << "\n========================================" << endl;
    cout << "QueueTest01_BasicOperations" << endl;
    cout << "========================================" << endl;

    Queue<int> q;
    int val = -1;

    assert(q.IsEmpty());
    assert(q.GetCount() == 0);

    // empty queue operations
    assert(!q.Pop(val));
    assert(!q.Peek(val));

    // push a few items
    q.Push(1);
    q.Push(2);
    q.Push(3);

    assert(!q.IsEmpty());
    assert(q.GetCount() == 3);
    assert(q.Contains(2));

    // peek does not remove
    assert(q.Peek(val) && val == 1);
    assert(q.GetCount() == 3);

    // pop one to move read offset and then push more to the write array
    assert(q.Pop(val) && val == 1);
    assert(!q.Contains(1));

    q.Push(4);
    q.Push(5);
    assert(q.GetCount() == 4);

    const int expected[] = {2, 3, 4, 5};
    for(int i = 0; i < 4; ++i)
    {
        assert(q.Pop(val));
        assert(val == expected[i]);
    }

    assert(q.IsEmpty());

    // cleanup helpers
    q.Push(42);
    q.Clear();
    assert(q.IsEmpty());

    cout << "All basic operation assertions passed." << endl;
    return 0;
}
