#include<hgl/async/AsyncEvent.h>
#include<hgl/sync/Atomic.h>

using namespace hgl;

static const uint32 EventCount=4000;

class CountEvent:public EventAction
{
    atom_uint *count;
    atom_uint64 *sum;
    uint32 value;

public:

    void Set(atom_uint *event_count,atom_uint64 *event_sum,uint32 event_value)
    {
        count=event_count;
        sum=event_sum;
        value=event_value;
    }

    bool Execute()override
    {
        count->fetch_add(1);
        sum->fetch_add(value);
        return(true);
    }
};

int main()
{
    AsyncEventQueue queue;
    AsyncEventThread event_thread(&queue);
    CountEvent events[EventCount];
    atom_uint count{0};
    atom_uint64 sum{0};

    if(!event_thread.Start())
        return(1);

    uint64 expected_sum=0;
    for(uint32 i=0;i<EventCount;i++)
    {
        events[i].Set(&count,&sum,i+1);
        expected_sum+=i+1;

        if(!PostAsyncEvent(&queue,&events[i]))
        {
            event_thread.StopAndJoin();
            return(2);
        }
    }

    if(!event_thread.StopAndJoin(5.0))
        return(3);

    if(PostAsyncEvent(&queue,&events[0]))
        return(4);

    return count.load()==EventCount&&sum.load()==expected_sum?0:5;
}
