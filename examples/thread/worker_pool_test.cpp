#include<hgl/thread/WorkerPool.h>
#include<hgl/time/Time.h>

using namespace hgl;
using namespace hgl::workflow;

struct WorkItem
{
    uint32 value;
};

class CountingWorkProc:public WorkProc<WorkItem>
{
public:

    atom_uint processed{0};
    atom_uint64 sum{0};
    atom_uint cancelled{0};

    void OnWork(uint,WorkItem *work)override
    {
        processed.fetch_add(1);
        sum.fetch_add(work->value);
    }

    void OnCancel(WorkItem *)override
    {
        cancelled.fetch_add(1);
    }
};

class GatedWorkProc:public WorkProc<WorkItem>
{
public:

    atom_bool entered{false};
    atom_bool release{false};
    atom_uint processed{0};
    atom_uint cancelled{0};

    void OnWork(uint,WorkItem *)override
    {
        entered.store(true);

        while(!release.load())
            SleepSecond(0.001);

        processed.fetch_add(1);
    }

    void OnCancel(WorkItem *)override
    {
        cancelled.fetch_add(1);
    }
};

class ReleaseThread:public Thread
{
    atom_bool *release;

public:

    explicit ReleaseThread(atom_bool *flag):release(flag){}

    bool Execute()override
    {
        SleepSecond(0.02);
        release->store(true);
        return(false);
    }
};

int main()
{
    static const uint32 DrainCount=12000;
    WorkItem drain_items[DrainCount];
    CountingWorkProc counting_proc;
    WorkerPool<WorkItem> drain_pool(&counting_proc);

    if(!drain_pool.Start(4))
        return(1);

    uint64 expected_sum=0;
    for(uint32 i=0;i<DrainCount;i++)
    {
        drain_items[i].value=i+1;
        expected_sum+=i+1;
    }

    WorkItem *initial_batch[2]={&drain_items[0],&drain_items[1]};
    if(drain_pool.Submit(initial_batch,2)!=2)
        return(2);

    for(uint32 i=2;i<DrainCount;i++)
        if(!drain_pool.Submit(&drain_items[i]))
            return(3);

    if(!drain_pool.Stop(ShutdownMode::Drain))
        return(4);

    if(counting_proc.processed.load()!=DrainCount
       ||counting_proc.sum.load()!=expected_sum
       ||counting_proc.cancelled.load()!=0
       ||drain_pool.Submit(&drain_items[0])
       ||drain_pool.GetPendingCount()!=0)
       return(5);

    static const uint32 CancelCount=256;
    WorkItem cancel_items[CancelCount];
    GatedWorkProc gated_proc;
    WorkerPool<WorkItem> cancel_pool(&gated_proc);

    if(!cancel_pool.Start(1))
        return(6);

    cancel_items[0].value=0;
    if(!cancel_pool.Submit(&cancel_items[0]))
        return(7);

    const double wait_start=GetUptimeSec();
    while(!gated_proc.entered.load())
    {
        if(GetUptimeSec()-wait_start>5.0)
        {
            gated_proc.release.store(true);
            cancel_pool.Stop(ShutdownMode::CancelPending);
            return(8);
        }

        SleepSecond(0.001);
    }

    for(uint32 i=1;i<CancelCount;i++)
    {
        cancel_items[i].value=i;
        if(!cancel_pool.Submit(&cancel_items[i]))
        {
            gated_proc.release.store(true);
            cancel_pool.Stop(ShutdownMode::Drain);
            return(9);
        }
    }

    ReleaseThread release_thread(&gated_proc.release);
    if(!release_thread.Start())
    {
        gated_proc.release.store(true);
        cancel_pool.Stop(ShutdownMode::CancelPending);
        return(10);
    }

    const bool cancelled=cancel_pool.Stop(ShutdownMode::CancelPending);
    const bool released=release_thread.Join();

    if(!cancelled||!released
       ||gated_proc.processed.load()!=1
       ||gated_proc.cancelled.load()!=CancelCount-1
       ||cancel_pool.Submit(&cancel_items[0])
       ||cancel_pool.GetPendingCount()!=0)
        return(11);

    return(0);
}
