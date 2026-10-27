#pragma once

#include<hgl/event/EventAction.h>
#include<hgl/concurrency/SwapMailbox.h>
#include<hgl/thread/Thread.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    using AsyncEventQueue=SwapMailbox<EventAction *>;

    /**
     * @brief CN: 向异步事件邮箱投递一个借用事件指针。
     *        EN: Post a borrowed event pointer to the asynchronous event mailbox.
     * @details CN: 调用方仍拥有事件对象，并须保证其存活到 Execute 执行完成；
     *              队列关闭或参数无效时返回 false，且不会接管或释放事件。
     *          EN: The caller retains ownership and must keep the event alive until
     *              Execute completes. A closed queue or invalid argument returns
     *              false; the event is neither adopted nor released.
     */
    inline bool PostAsyncEvent(AsyncEventQueue *queue,EventAction *event)
    {
        return queue&&event&&queue->Post(event);
    }

    /**
     * @brief CN: 在当前线程处理一个邮箱批次。
     *        EN: Process one mailbox batch on the calling thread.
     * @details CN: wait=false 时没有批次立即返回；wait=true 时阻塞等待数据或队列关闭。
     *              返回 -1 表示队列无效或已关闭且排空，0 表示非等待模式下无工作，
     *              正数为本批从队列取出的事件数。每批处理结束都会释放接收缓冲区。
     *          EN: With wait=false, return immediately when no batch is available;
     *              with wait=true, block until data arrives or the queue closes.
     *              Return -1 for an invalid queue or a closed and drained queue, 0
     *              when non-blocking and idle, or the number of events in the batch.
     *              The receive buffer is released after each batch.
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
     * @brief CN: 专用异步事件消费线程。
     *        EN: Dedicated asynchronous event-consumer thread.
     * @details CN: RequestStop 会关闭关联邮箱以唤醒阻塞消费者；已接受的事件批次仍会
     *              逐批执行并排空，关闭且无剩余数据后线程才自然退出。因此停止此线程
     *              会同时关闭队列的投递端，队列须至少与线程同寿命。
     *          EN: RequestStop closes the associated mailbox to wake a blocked
     *              consumer. Accepted batches are still processed and drained; the
     *              thread exits after the closed queue becomes empty. Stopping this
     *              thread also closes the posting side, so the queue must outlive it.
     */
    class AsyncEventThread:public Thread
    {
        AsyncEventQueue *event_proc_queue; ///< @brief CN: 非拥有指针；队列必须比消费线程活得更久。 EN: Non-owning pointer; the queue must outlive the consumer thread.

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
