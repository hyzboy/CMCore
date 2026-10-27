#pragma once

#include<hgl/type/Queue.h>

namespace hgl
{
    /**
     * @brief CN: 可执行事件/动作的接口基类。
     *        EN: Base interface for executable events or actions.
     * @details CN: DispatchEvent(s) 只调用 Execute，不负责销毁对象；若通过指针入队，
     *              事件对象必须由调用方保证在执行结束前一直有效。
     *          EN: DispatchEvent(s) only calls Execute and does not destroy the object.
     *              When queued by pointer, the caller must keep the event alive until
     *              execution has completed.
     */
    class EventAction
    {
    public:

        EventAction()=default;
        virtual ~EventAction()=default;

        /** @brief CN: 执行事件内容；通用队列分发器不解释返回值。 EN: Execute the event; generic queue dispatchers do not interpret the return value. */
        virtual bool Execute()=0;
    };//class EventAction

    /// @brief CN: 队列只保存借用指针；提交方负责对象所有权并保证其存活到执行完成。
    ///        EN: The queue stores borrowed pointers; the submitter owns objects and keeps them alive through execution.
    using EventActionQueue=Queue<EventAction *>;

    /** @brief CN: 从队列取出并执行一个事件；无队列、空队列或取出失败时返回 false。 EN: Pop and execute one event; return false for a null queue, an empty queue, or a failed pop. */
    inline bool DispatchEvent(EventActionQueue *epq)
    {
        if(!epq||epq->GetCount()<=0)
            return(false);

        EventAction *event;

        if(epq->Pop(event))
        {
            if(event)
                event->Execute();

            return(true);
        }

        return(false);
    }

    /**
     * @brief CN: 批量分发队列中的事件。
     *        EN: Dispatch events from the queue in a batch.
     * @param max_count CN: 本次最多处理数；负数表示不设上限，零表示不处理。
     *                  EN: Maximum number to process; negative means unlimited, zero means none.
     * @return CN: 实际从队列取出的事件数（空指针项也计入），不表示 Execute 的结果。
     *         EN: Number of entries popped, including null pointers; not the Execute result.
     */
    inline int DispatchEvents(EventActionQueue *epq,int max_count=-1)
    {
        if(!epq||epq->GetCount()<=0||max_count==0)
            return(0);

        int count=0;
        EventAction *event;

        while(epq->Pop(event))
        {
            if(event)
                event->Execute();

            ++count;

            if(--max_count==0)
                break;
        }

        return count;
    }
}//namespace hgl
