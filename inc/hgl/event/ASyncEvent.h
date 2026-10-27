#pragma once

#include<hgl/event/EventProc.h>
#include<hgl/thread/SwapMailbox.h>
#include<hgl/thread/Thread.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    using MTEventProcQueue=SwapMailbox<EventProc *>;

    /**
    * The queue borrows event; caller-owned storage must remain valid until
    * Proc has run. A rejected event is not queued.
    */
    inline bool AddToAsyncEventQueue(MTEventProcQueue *queue,EventProc *event)
    {
        return queue&&event&&queue->Post(event);
    }

    /**
    * Consume one mailbox batch.
    * @return -1 invalid/closed and drained, 0 no work in non-waiting mode,
    *         or the number of events consumed.
    */
    inline int UpdateAsyncEventQueue(MTEventProcQueue *proc_queue,bool wait=false)
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

        ValueArray<EventProc *> &events=proc_queue->GetReceive();
        const int count=events.GetCount();
        EventProc **event=events.GetData();

        for(int i=0;i<count;i++)
        {
            if(event[i])
                event[i]->Proc();
        }

        proc_queue->ReleaseReceive();
        return count;
    }

    /**
    * Asynchronous event worker. Stop closes the queue; already accepted batches
    * are drained before the thread exits.
    */
    class EventThread:public Thread
    {
        MTEventProcQueue *event_proc_queue;

    protected:

        void OnStopRequested()override
        {
            if(event_proc_queue)
                event_proc_queue->Close();
        }

    public:

        explicit EventThread(MTEventProcQueue *queue):event_proc_queue(queue){}
        virtual ~EventThread()=default;

        bool Execute()override
        {
            if(!event_proc_queue)
                return(false);

            for(;;)
            {
                if(UpdateAsyncEventQueue(event_proc_queue,true)<0)
                    return(false);

                if(!event_proc_queue->IsClosed())
                    return(true);
            }
        }
    };//class EventThread
}//namespace hgl
