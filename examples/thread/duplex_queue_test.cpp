#include<hgl/thread/DuplexQueue.h>
#include<hgl/thread/Thread.h>
#include<hgl/time/Time.h>

using namespace hgl;

static const uint32 ItemCount=20000;

class RequestProcessor:public Thread
{
    DuplexQueue<uint32,uint32> *queue;

public:

    uint32 errors=0;
    uint32 processed_count=0;

    explicit RequestProcessor(DuplexQueue<uint32,uint32> *q):queue(q){}

    bool Execute()override
    {
        uint32 processed=0;
        const double deadline=GetUptimeSec()+10.0;

        while(processed<ItemCount)
        {
            if(IsStopRequested())
                return(false);
            if(GetUptimeSec()>deadline)
            {
                ++errors;
                return(false);
            }

            uint32 request;

            if(!queue->Recv(request))
                continue;

            const uint32 response=request+100;

            while(!queue->SendResponse(response))
            {
                if(IsStopRequested())
                    return(false);
                if(GetUptimeSec()>deadline)
                {
                    ++errors;
                    return(false);
                }
            }

            ++processed;
            ++processed_count;
        }

        return(false);
    }
};

class RequestClient:public Thread
{
    DuplexQueue<uint32,uint32> *queue;

public:

    uint32 errors=0;
    uint32 completed_count=0;

    explicit RequestClient(DuplexQueue<uint32,uint32> *q):queue(q){}

    bool Execute()override
    {
        const double deadline=GetUptimeSec()+10.0;

        for(uint32 i=0;i<ItemCount;i++)
        {
            if(IsStopRequested())
                return(false);
            if(GetUptimeSec()>deadline)
            {
                ++errors;
                return(false);
            }

            while(!queue->Send(i))
            {
                if(IsStopRequested())
                    return(false);
                if(GetUptimeSec()>deadline)
                {
                    ++errors;
                    return(false);
                }
            }

            uint32 response=0;
            while(!queue->RecvResponse(response))
            {
                if(IsStopRequested())
                    return(false);
                if(GetUptimeSec()>deadline)
                {
                    ++errors;
                    return(false);
                }
            }

            if(response!=i+100)
                ++errors;
            ++completed_count;
        }

        return(false);
    }
};

int main()
{
    DuplexQueue<uint32,uint32> queue(64);

    if(!queue.IsValid())
        return(1);

    RequestProcessor processor(&queue);
    RequestClient client(&queue);

    if(!processor.Start())
        return(2);
    if(!client.Start())
    {
        client.WaitExit();
        processor.WaitExit();
        return(3);
    }

    if(!client.Join(12.0)||!processor.Join(12.0))
    {
        client.WaitExit(2.0);
        processor.WaitExit(2.0);
        return(4);
    }

    return client.errors==0&&processor.errors==0
        &&client.completed_count==ItemCount
        &&processor.processed_count==ItemCount
        &&queue.GetRequestCount()==0
        &&queue.GetResponseCount()==0?0:5;
}
