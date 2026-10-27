#include<hgl/concurrency/SwapMailbox.h>
#include<hgl/thread/Thread.h>
#include<hgl/time/Time.h>

using namespace hgl;

static const uint32 ProducerCount=3;
static const uint32 ItemsPerProducer=4000;
static const uint32 TotalCount=ProducerCount*ItemsPerProducer;

struct MailboxItem
{
    uint32 producer;
    uint32 index;

    bool operator==(const MailboxItem &other)const
    {
        return producer==other.producer&&index==other.index;
    }
};

class MailboxProducer:public Thread
{
    SwapMailbox<MailboxItem> *mailbox;
    uint32 producer_index;

public:

    uint32 errors=0;

    MailboxProducer(SwapMailbox<MailboxItem> *box,uint32 index)
        :mailbox(box),producer_index(index)
    {
    }

    bool Execute()override
    {
        for(uint32 i=0;i<ItemsPerProducer;i++)
        {
            const MailboxItem item{producer_index,i};
            if(!mailbox->Post(item))
                ++errors;
        }

        return(false);
    }
};

class MailboxConsumer:public Thread
{
    SwapMailbox<MailboxItem> *mailbox;

public:

    uint32 counts[ProducerCount]={0,0,0};
    uint32 errors=0;

    explicit MailboxConsumer(SwapMailbox<MailboxItem> *box):mailbox(box){}

    bool Execute()override
    {
        uint32 consumed=0;

        while(consumed<TotalCount)
        {
            if(!mailbox->WaitAndSwap())
                return(false);

            ValueArray<MailboxItem> &batch=mailbox->GetReceive();

            for(int i=0;i<batch.GetCount();i++)
            {
                const MailboxItem &item=batch[i];

                if(item.producer>=ProducerCount||item.index>=ItemsPerProducer)
                    ++errors;
                else
                    ++counts[item.producer];

                ++consumed;
            }

            mailbox->ReleaseReceive();
        }

        return(false);
    }
};

int main()
{
    SwapMailbox<MailboxItem> mailbox;
    MailboxConsumer consumer(&mailbox);
    MailboxProducer producer0(&mailbox,0);
    MailboxProducer producer1(&mailbox,1);
    MailboxProducer producer2(&mailbox,2);

    if(!consumer.Start())
        return(1);
    if(!producer0.Start()||!producer1.Start()||!producer2.Start())
    {
        mailbox.Close();
        consumer.StopAndJoin();
        producer0.StopAndJoin();
        producer1.StopAndJoin();
        producer2.StopAndJoin();
        return(2);
    }

    const bool producers_joined=producer0.Join()&&producer1.Join()&&producer2.Join();
    const bool consumer_joined=consumer.Join();
    mailbox.Close();

    return producers_joined&&consumer_joined
        &&producer0.errors==0&&producer1.errors==0&&producer2.errors==0
        &&consumer.errors==0
        &&consumer.counts[0]==ItemsPerProducer
        &&consumer.counts[1]==ItemsPerProducer
        &&consumer.counts[2]==ItemsPerProducer?0:3;
}
