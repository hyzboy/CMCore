#include<hgl/type/Queue.h>
#include<iostream>
#include<cassert>
#include<vector>

using namespace hgl;
using namespace std;

struct Tracked
{
    int id;
    static int copy_ctor_count;
    static int dtor_count;

    Tracked(int v = 0) : id(v) {}
    Tracked(const Tracked &other) : id(other.id) { ++copy_ctor_count; }
    Tracked &operator=(const Tracked &other)
    {
        id = other.id;
        ++copy_ctor_count; // treat assignment as an observable copy for test purposes
        return *this;
    }
    ~Tracked() { ++dtor_count; }
};

int Tracked::copy_ctor_count = 0;
int Tracked::dtor_count = 0;

static void ResetTrackedCounters()
{
    Tracked::copy_ctor_count = 0;
    Tracked::dtor_count = 0;
}

int os_main(int, os_char **)
{
    cout << "\n========================================" << endl;
    cout << "QueueTest03_GetDataVariants" << endl;
    cout << "========================================" << endl;

    // Queue snapshot should merge read/write views and preserve order
    Queue<int> vq;
    vq.Push(10);
    vq.Push(20);
    vq.Push(30);

    int dropped = 0;
    vq.Pop(dropped); // consume one to advance read_offset

    vq.Push(40);
    vq.Push(50);

    auto value_snapshot = vq.GetUnreadSnapshot();
    assert(value_snapshot.GetCount() == 4);
    assert(value_snapshot[0] == 20);
    assert(value_snapshot[1] == 30);
    assert(value_snapshot[2] == 40);
    assert(value_snapshot[3] == 50);
    assert(vq.GetCount() == 4); // snapshot must not mutate the queue

    // Queue with pointer type - caller manages object lifetime
    ResetTrackedCounters();
    Queue<Tracked*> pq;
    pq.Push(new Tracked(1));
    pq.Push(new Tracked(2));
    pq.Push(new Tracked(3));

    Tracked* tmp = nullptr;
    pq.Pop(tmp); // advance read_offset
    delete tmp;
    pq.Push(new Tracked(4));

    const int baseline_copies = Tracked::copy_ctor_count;
    auto ptr_snapshot = pq.GetUnreadSnapshot();
    const int copies_from_snapshot = Tracked::copy_ctor_count - baseline_copies;

    assert(ptr_snapshot.GetCount() == 3);

    vector<int> ids;
    for(int i = 0; i < ptr_snapshot.GetCount(); ++i)
        ids.push_back(ptr_snapshot[i]->id);

    assert(ids.size() == 3);
    assert(ids[0] == 2 && ids[1] == 3 && ids[2] == 4);
    assert(pq.GetCount() == 3);

    // cleanup
    for(int i = 0; i < ptr_snapshot.GetCount(); ++i)
        delete ptr_snapshot[i];

    cout << "GetUnreadSnapshot assertions passed." << endl;
    return 0;
}
