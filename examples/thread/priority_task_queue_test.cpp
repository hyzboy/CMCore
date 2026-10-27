#include<hgl/concurrency/PriorityTaskQueue.h>

using namespace hgl;

struct PriorityItem
{
    uint32 id;

    bool operator==(const PriorityItem &other)const
    {
        return id==other.id;
    }
};

int main()
{
    PriorityTaskQueue<PriorityItem,4> queue;
    PriorityItem low_first{30};
    PriorityItem high_first{10};
    PriorityItem normal{20};
    PriorityItem high_second{11};

    if(!queue.Submit(low_first,3)
       ||!queue.Submit(high_first,0)
       ||!queue.Submit(normal,2)
       ||!queue.Submit(high_second,0))
        return(1);

    PriorityItem item{};
    uint priority=0;
    const uint32 expected_ids[4]={10,11,20,30};
    const uint expected_priorities[4]={0,0,2,3};

    for(int i=0;i<4;i++)
    {
        if(!queue.TryPop(item,priority)
           ||item.id!=expected_ids[i]
           ||priority!=expected_priorities[i])
            return(2);
    }

    if(queue.TryPop(item,priority)||queue.Submit(low_first,4))
        return(3);

    if(!queue.Submit(high_first,0)
       ||!queue.Submit(normal,2)
       ||queue.GetPendingCount()!=2
       ||queue.GetPriorityPendingCount(0)!=1
       ||queue.GetPriorityPendingCount(2)!=1)
        return(4);

    ValueArray<PriorityItem> cancelled;
    if(queue.CloseAndCancel(cancelled)!=2||cancelled.GetCount()!=2
       ||!queue.IsClosed()||queue.Submit(low_first,0))
        return(5);

    const PriorityTaskQueue<PriorityItem,4>::Statistics stats=queue.GetStatistics();
    if(stats.submitted!=6||stats.popped!=4||stats.cancelled!=2
       ||stats.pending!=0||stats.peak_pending<2)
        return(6);

    PriorityTaskQueue<PriorityItem,4> wait_queue;
    if(wait_queue.WaitPop(item,priority,0.01))
        return(7);

    if(!wait_queue.Submit(normal,1))
        return(8);

    wait_queue.Close();
    if(!wait_queue.WaitPop(item,priority)||item.id!=normal.id||priority!=1)
        return(9);

    if(wait_queue.WaitPop(item,priority))
        return(10);

    return(0);
}
