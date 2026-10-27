#include<hgl/concurrency/TaskQueue.h>
#include<hgl/thread/PolledWorker.h>
#include<hgl/thread/SingleWorker.h>
#include<hgl/sync/Atomic.h>

using namespace hgl;
using namespace hgl::thread;

struct TestWork
{
    uint32 value;
};

class TestHandler:public WorkHandler<TestWork>
{
public:

    atom_uint count{0};
    atom_uint64 sum{0};

    void OnWork(uint,TestWork *work)override
    {
        count.fetch_add(1);
        sum.fetch_add(work->value);
    }
};

int main()
{
    TestWork polled_work[4]={{1},{2},{3},{4}};
    TestHandler polled_handler;
    PolledWorker<TestWork> polled(&polled_handler,7);

    for(TestWork &work:polled_work)
        if(!polled.Submit(&work))
            return(1);

    if(polled.Process(2)!=2||polled.Process()!=2
       ||polled_handler.count.load()!=4||polled_handler.sum.load()!=10)
        return(2);

    TaskQueue<TestWork *> queue;
    TestWork threaded_work[4]={{5},{6},{7},{8}};
    TestHandler threaded_handler;
    SingleWorker<TestWork> worker(&queue,&threaded_handler);

    for(TestWork &work:threaded_work)
        if(!worker.Submit(&work))
            return(3);

    if(!worker.Start()||!worker.Stop(5.0))
        return(4);

    if(threaded_handler.count.load()!=4)
        return(5);
    if(threaded_handler.sum.load()!=26)
        return(6);

    return(0);
}
