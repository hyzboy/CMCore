#pragma once

#include<hgl/event/EventAction.h>
#include<hgl/concurrency/SwapMailbox.h>
#include<hgl/thread/Thread.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    using AsyncEventQueue=SwapMailbox<EventAction *>;

    /**
    * The queue borrows event; caller-owned storage must remain valid until
    * Execute has run. A rejected event is not queued.
    */
    inline bool PostAsyncEvent(AsyncEventQueue *queue,EventAction *event)
    {
        return queue&&event&&queue->Post(event);
    }

    /**
    * Consume one mailbox batch.
    * @return -1 invalid/closed and drained, 0 no work in non-waiting mode,
    *         or the number of events consumed.
    */
    inline int ProcessAsyncEvents(AsyncEventQueue *proc_queue,bool wait=false)
    {
        if(!proc_queue)
            return(-1);

        const bool swapped=wait?proc_queue->WaitAndSwap():proc_queue->TrySwap();

        if(!swapped)
        {
            if(proc_queue->IsClosed())
                return(-1);

            return(0);
        }

        ValueArray<EventAction *> &events=proc_queue->GetReceive();
        const int count=events.GetCount();
        EventAction **event=events.GetData();

        for(int i=0;i<count;i++)
        {
            if(event[i])
                event[i]->Execute();
        }

        proc_queue->ReleaseReceive();
        return count;
    }

    /**
    * Asynchronous event worker. Stop closes the queue; already accepted batches
    * are drained before the thread exits.
    */
    class AsyncEventThread:public Thread
    {
        AsyncEventQueue *event_proc_queue;

    protected:

        void OnStopRequested()override
        {
            if(event_proc_queue)
                event_proc_queue->Close();
        }

    public:

        explicit AsyncEventThread(AsyncEventQueue *queue):event_proc_queue(queue){}
        virtual ~AsyncEventThread()=default;

        bool Execute()override
        {
            if(!event_proc_queue)
                return(false);

            for(;;)
            {
                if(ProcessAsyncEvents(event_proc_queue,true)<0)
                    return(false);
            }
        }
    };//class AsyncEventThread
}//namespace hgl
