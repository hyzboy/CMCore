#include<hgl/concurrency/SpscQueue.h>
#include<hgl/thread/Thread.h>

using namespace hgl;

static const uint32 ItemCount=100000;

class ProducerThread:public Thread
{
    SpscQueue<uint32> *queue;

public:

    explicit ProducerThread(SpscQueue<uint32> *q):queue(q){}

    bool Execute()override
    {
        for(uint32 i=0;i<ItemCount;i++)
        {
            while(!queue->Push(i))
            {
                if(IsStopRequested())
                    return(false);
            }
        }

        return(false);
    }
};

class ConsumerThread:public Thread
{
    SpscQueue<uint32> *queue;

public:

    uint32 errors=0;

    explicit ConsumerThread(SpscQueue<uint32> *q):queue(q){}

    bool Execute()override
    {
        uint32 received=0;

        while(received<ItemCount)
        {
            if(IsStopRequested())
                return(false);

            uint32 value;

            if(!queue->Pop(value))
                continue;

            if(value!=received)
                ++errors;

            ++received;
        }

        return(false);
    }
};

int main()
{
    // CN: 覆盖容量规范化、无效分配、满队列拒绝/计数，以及双线程 FIFO 顺序。
    // EN: Cover capacity normalization, invalid allocation, full-queue rejection/counting, and two-thread FIFO order.
    SpscQueue<uint32> invalid_queue(0xffffffffu);
    if(invalid_queue.IsValid()||invalid_queue.GetCapacity()!=0)
        return(1);

    SpscQueue<uint32> full_queue(1);

    if(!full_queue.IsValid()||full_queue.GetCapacity()!=8)
        return(2);

    for(uint32 i=0;i<8;i++)
        if(!full_queue.Push(i))
            return(3);

    if(full_queue.Push(9)||full_queue.GetDroppedCount()!=1)
        return(4);

    uint32 first;
    if(!full_queue.Pop(first)||first!=0||!full_queue.Push(10))
        return(5);

    SpscQueue<uint32> queue(128);
    ProducerThread producer(&queue);
    ConsumerThread consumer(&queue);

    if(!consumer.Start())
        return(6);
    if(!producer.Start())
    {
        producer.StopAndJoin();
        consumer.StopAndJoin();
        return(7);
    }

    if(!producer.Join()||!consumer.Join())
        return(8);

    return consumer.errors==0&&queue.IsEmpty()?0:9;
}
