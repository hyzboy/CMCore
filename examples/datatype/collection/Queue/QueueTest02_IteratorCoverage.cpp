#include<hgl/type/DataType.h>
#include<hgl/type/Queue.h>
#include<iostream>
#include<vector>
#include<cassert>

using namespace hgl;
using namespace std;

static void AssertEquals(const vector<int> &actual, initializer_list<int> expected)
{
    assert(actual.size() == expected.size());
    size_t idx = 0;
    for(int v : expected)
    {
        assert(actual[idx] == v);
        ++idx;
    }
}

int os_main(int, os_char **)
{
    cout << "\n========================================" << endl;
    cout << "QueueTest02_IteratorCoverage" << endl;
    cout << "========================================" << endl;

    Queue<int> q;
    for(int i = 0; i < 5; ++i)
        q.Push(i);

    int val = -1;
    q.Pop(val);
    q.Pop(val); // read_offset is now 2 with remaining data in read_array

    q.Push(100);
    q.Push(200); // write_array now has data

    vector<int> iterated;
    for(const auto &item : q)
        iterated.push_back(item);

    AssertEquals(iterated, {2, 3, 4, 100, 200});
    assert(q.GetCount() == 5);

    // drain to move read_offset to the end of read_array
    while(q.Pop(val)){}
    assert(q.IsEmpty());

    // pushing after read_array is fully consumed should still allow iteration to start from write_array
    q.Push(7);
    q.Push(8);

    iterated.clear();
    for(const auto &item : q)
        iterated.push_back(item);

    AssertEquals(iterated, {7, 8});
    assert(q.GetCount() == 2);

    cout << "Iterator coverage assertions passed." << endl;
    return 0;
}
