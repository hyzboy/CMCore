#pragma once

#include<hgl/type/Queue.h>

namespace hgl
{
    /**
     * 事件基类
     */
    class EventProc
    {
    public:

        EventProc()=default;
        virtual ~EventProc()=default;

        virtual bool Proc()=0;
    };//class EventProc

    using EventProcQueue=ValueQueue<EventProc *>;

    /**
        * 执行一个事件
        */
    inline bool UpdateEvent(EventProcQueue *epq)
    {
        if(!epq||epq->GetCount()<=0)
            return(false);

        EventProc *e;

        if(epq->Pop(e))
        {
            e->Proc();
            delete e;

            return(true);
        }

        return(false);
    }

    /**
        * 刷新执行事件队列
        * @param epq 事件队列
        * @param max_count 最大刷新个数(-1表示无限制)
        */
    inline int UpdateEventQueue(EventProcQueue *epq,int max_count=-1)
    {
        if(!epq||epq->GetCount()<=0||max_count==0)
            return(0);

        int count=0;
        EventProc *e;

        while(epq->Pop(e))
        {
            if(e)
            {
                e->Proc();
                delete e;
            }

            ++count;

            if(--max_count==0)
                break;
        }

        return count;
    }
}//namespace hgl
