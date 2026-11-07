#include<hgl/thread/TaskNotification.h>
#include<hgl/thread/Thread.h>
#include<hgl/time/Time.h>
#include<hgl/type/ValueArray.h>

using namespace hgl;

static const uint32 ProducerCount=3;
static const uint32 EventsPerProducer=2000;
static const uint32 TotalEvents=ProducerCount*EventsPerProducer;

// ── 生产者：通过 TaskReceiver 接口投递事件 ──────────────────────────
class EventProducer:public Thread
{
    thread::TaskReceiver *receiver;
    uint32 producer_index;

public:

    uint32 posted=0;
    uint32 errors=0;

    EventProducer(thread::TaskReceiver *in,uint32 index)
        :receiver(in),producer_index(index)
    {
    }

    bool Execute()override
    {
        for(uint32 i=0;i<EventsPerProducer;i++)
        {
            thread::TaskEvent ev;
            ev.task=reinterpret_cast<void *>(static_cast<uintptr_t>(producer_index*16+1));
            ev.task_id=(uint64(producer_index)<<32)|i;
            ev.task_kind=producer_index;
            ev.state=(i+1==EventsPerProducer)?thread::TaskState::Done:thread::TaskState::Progress;
            ev.completed=i;
            ev.total=EventsPerProducer;

            receiver->OnTaskEvent(ev);
            ++posted;
        }

        return(false);
    }
};

// ── 消费者：专用线程 WaitAndDrain 阻塞批量消费 ─────────────────────
class WaitDrainConsumer:public Thread
{
    thread::ThreadMessagePool<thread::TaskEvent> *pool;

public:

    uint32 consumed=0;
    uint32 done_count=0;
    uint32 kinds[ProducerCount]={0,0,0};
    uint32 errors=0;

    explicit WaitDrainConsumer(thread::ThreadMessagePool<thread::TaskEvent> *in):pool(in) {}

    bool Execute()override
    {
        for(;;)
        {
            const int n=pool->WaitAndDrain(
                [this](const thread::TaskEvent &ev)
                {
                    if(ev.task_kind>=ProducerCount){++errors;return;}
                    ++kinds[ev.task_kind];
                    if(ev.state==thread::TaskState::Done)++done_count;
                    ++consumed;
                });

            if(n<=0)
            {
                //超时或池已关闭且排空：关闭后若仍无可交换批次则退出
                if(pool->GetPendingCount()==0)break;
            }
        }

        return(false);
    }
};

int main()
{
    // CN: 三个生产者经 InboxTaskReceiver 把事件压入同一个 ThreadMessagePool，
    //     一个 WaitAndDrain 消费者线程按批消费；主线程最后校验总数、分类数与终态数。
    // EN: Three producers push events into one ThreadMessagePool through an
    //     InboxTaskReceiver; a WaitAndDrain consumer drains batches; main thread
    //     validates totals, per-kind counts and terminal Done count.
    thread::ThreadMessagePool<thread::TaskEvent> pool;
    thread::InboxTaskReceiver inbox(&pool);

    WaitDrainConsumer consumer(&pool);

    ValueArray<EventProducer*> producers;

    for(uint32 i=0;i<ProducerCount;i++)
        producers.Add(new EventProducer(&inbox,i));

    int result=0;

    if(!consumer.Start())
    {
        result=1;
    }
    else
    {
        for(int i=0;i<producers.GetCount();i++)
            if(!producers[i]->Start()){result=2;break;}

        if(result==0)
        {
            for(auto p:producers)p->Join();

            //等待消费者排空后关闭
            while(pool.GetPendingCount()>0)SleepSecond(0.001);

            pool.Close();
            consumer.Join();

            for(auto p:producers)
            {
                if(p->errors!=0)result=3;
                if(p->posted!=EventsPerProducer)result=4;
            }

            if(consumer.errors!=0)result=5;
            if(consumer.consumed!=TotalEvents)result=6;
            if(consumer.done_count!=ProducerCount)result=7;

            for(uint32 i=0;i<ProducerCount;i++)
                if(consumer.kinds[i]!=EventsPerProducer)result=8;
        }
    }

    for(auto p:producers)delete p;

    // ── 同步小场景：Inbox 关闭后 Post 拒绝，但已入池数据仍可排空 ──
    {
        thread::ThreadMessagePool<thread::TaskEvent> p2;
        thread::InboxTaskReceiver in2(&p2);

        thread::TaskEvent ev;
        ev.task_kind=99;
        in2.OnTaskEvent(ev);
        p2.Close();

        thread::TaskEvent ev2;
        ev2.task_kind=100;
        if(p2.Post(ev2))++result;         //关闭后必须拒绝

        uint32 got=0;
        p2.Drain([&got](const thread::TaskEvent &){++got;});
        if(got!=1)++result;               //关闭前排入的 1 条必须可排空
    }

    return(result);
}
