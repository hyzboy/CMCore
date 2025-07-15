#pragma once

#include<hgl/type/SortedSet.h>
#include<hgl/io/event/InputEventSource.h>

/*
* 现阶段事件分发流程：
*
* 操作系统事件    ->      Window  ->   RenderFramework    -> WorkManager
* 
*/

namespace hgl::io
{
    struct EventHeader
    {
        InputEventSource    type;       ///<输入源类型
        uint8               index;      ///<输入源索引(比如同一设备有多个)
        uint16              id;         ///<事件id
    };

    constexpr size_t EventHeaderBytes=sizeof(EventHeader);

    /**
     * 事件处理结果
     */
    enum class EventProcResult
    {
        Continue,
        Break,
    };

    class EventDispatcher
    {
    protected:

        InputEventSource                source_type;

        EventDispatcher *               parent_input_event;

        SortedSet<EventDispatcher *>    event_subscribers;

    protected:

        void SetParent(EventDispatcher *ie){parent_input_event=ie;}

    public:

        const InputEventSource GetInputEventSource()const{return source_type;}

        virtual EventProcResult OnEvent(const EventHeader &header,const uint64 data)
        {
            if(!RangeCheck(header.type))
                return(EventProcResult::Break);

            if(!event_subscribers.IsEmpty())
            {
                for(EventDispatcher *ie:event_subscribers)
                    if(ie->OnEvent(header,data)==EventProcResult::Break)
                        return EventProcResult::Break;
            }

            return(EventProcResult::Continue);
        }

    public:

        EventDispatcher()
        {
            source_type=InputEventSource::Root;
            parent_input_event=nullptr;
        }

        EventDispatcher(InputEventSource ies)
        {
            source_type=ies;
            parent_input_event=nullptr;
        }

        virtual ~EventDispatcher()
        {
            if(parent_input_event)
                parent_input_event->UnsubscribeEventDispatcher(this);
        }

        virtual bool SubscribeEventDispatcher(EventDispatcher *ie)
        {
            if(!ie)
                return(false);

            const InputEventSource ies=ie->GetInputEventSource();

            if(!RangeCheck(ies))
                return(false);

            ie->SetParent(this);

            return(event_subscribers.Add(ie)!=-1);
        }

        bool UnsubscribeEventDispatcher(EventDispatcher *ie)
        {
            if(!ie)return(false);

            const InputEventSource ies=ie->GetInputEventSource();

            if(!RangeCheck(ies))
                return(false);

            ie->SetParent(nullptr);

            return event_subscribers.Delete(ie);
        }

        virtual bool Update(){return true;}
    };//class EventDispatcher
}//namespace hgl::io
