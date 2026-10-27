#pragma once

#include<hgl/type/Queue.h>

namespace hgl
{
    /**
     * 事件基类
     */
    class EventAction
    {
    public:

        EventAction()=default;
        virtual ~EventAction()=default;

        virtual bool Execute()=0;
    };//class EventAction

    // Queue entries are borrowed; the submitter owns each event action.
    using EventActionQueue=Queue<EventAction *>;

    /**
        * 执行一个事件
        */
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
        * 刷新执行事件队列
        * @param epq 事件队列
        * @param max_count 最大刷新个数(-1表示无限制)
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
