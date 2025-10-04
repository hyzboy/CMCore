#ifndef HGL_EVENT_ASYNC_INCLUDE
#define HGL_EVENT_ASYNC_INCLUDE

#include<hgl/event/EventProc.h>
#include<hgl/thread/SwapData.h>
#include<hgl/thread/Thread.h>
#include<hgl/time/Time.h>

namespace hgl
{
    using MTEventProcQueue=SemSwapData<EventProcQueue>;
    using MTEventProcPost=PostToSemSwapData<EventProcQueue>;                  ///<多线程事件结果投递对象重定义

    /**
     * 增加事件到异步事件队列
     * @param queue 事件队列
     * @param event 事件
     */
    inline void AddToAsyncEventQueue(MTEventProcQueue *queue,EventProc *event)
    {
        if(!queue||!event)
            return;

        //该模板会自动加锁，在释构时自动解锁，并释放信号
        MTEventProcPost epq(queue);

        epq->Push(event);
    }

    /**
     * 刷新异步执行事件队列
     * @param proc_queue 要执行的事件队列
     * @param wait 是否等待有事件
     * @return -1 出错
     * @return >=0 执行的事件数量
     */
    inline int UpdateAsyncEventQueue(MTEventProcQueue *proc_queue,bool wait=false)
    {
        if(!proc_queue)
            return(-1);

        if(wait)
        {
            if(!proc_queue->WaitSemSwap())
                return(0);
        }
        else
        {
            if(!proc_queue->TrySemSwap())
                return(0);
        }

        EventProcQueue &epq=proc_queue->GetReceive();

        return UpdateEventQueue(&epq);
    }

    /**
     * 异步事件异步执行线程
     */
    class EventThread:public Thread
    {
        MTEventProcQueue *event_proc_queue;

    public:

        EventThread(MTEventProcQueue *queue):event_proc_queue(queue){}
        virtual ~EventThread()=default;

        bool Execute() override
        {
            if(!event_proc_queue)
                return(false);

            UpdateAsyncEventQueue(event_proc_queue,true);

            return(true);
        }
    };//class EventThread:public Thread

    /* 使用范例

    // 公用部分

    MTEventProcQueue event_queue;               ///<事件队列

    // 其它 thread

    class MyEvent:public EventProc{...};        //自有事件

    AddToEventQueue(&event_queue,new MyEvent);  //添加一个事件到事件队列

    // 事件执行线程
    EventThread *et=new EventThread(&event_queue);
    et->Start();
    */
}//namespace hgl
#endif//HGL_EVENT_ASYNC_INCLUDE
