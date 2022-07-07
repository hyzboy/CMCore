#ifndef HGL_EVENT_PROC_INCLUDE
#define HGL_EVENT_PROC_INCLUDE

#include<hgl/type/Queue.h>

namespace hgl
{
    /**
     * �¼�����
     */
    class EventProc
    {
    public:

        EventProc()=default;
        virtual ~EventProc()=default;

        virtual bool Proc()=0;
    };//class EventProc

    using EventProcQueue=Queue<EventProc *>;

    /**
        * ִ��һ���¼�
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
        * ˢ��ִ���¼�����
        * @param epq �¼�����
        * @param max_count ���ˢ�¸���(-1��ʾ������)
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
#endif//HGL_EVENT_PROC_INCLUDE
