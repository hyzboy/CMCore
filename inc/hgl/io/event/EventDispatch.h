#pragma once

#include<hgl/type/SortedSet.h>
#include<hgl/io/event/InputEventSource.h>

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

    class EventDispatch
    {
    protected:

        InputEventSource            source_type;

        EventDispatch *             parent_input_event;

        SortedSet<EventDispatch *>  event_dispatch_subscribers;

    protected:

        void SetParent(EventDispatch *ie){parent_input_event=ie;}

    public:

        const InputEventSource GetInputEventSource()const{return source_type;}

        virtual EventProcResult OnEvent(const EventHeader &header,const uint64 data)
        {
            if(!RangeCheck(header.type))
                return(EventProcResult::Break);

            if(!event_dispatch_subscribers.IsEmpty())
            {
                for(EventDispatch *ie:event_dispatch_subscribers)
                    if(ie->OnEvent(header,data)==EventProcResult::Break)
                        return EventProcResult::Break;
            }

            return(EventProcResult::Continue);
        }

    public:

        EventDispatch()
        {
            source_type=InputEventSource::Root;
            parent_input_event=nullptr;
        }

        EventDispatch(InputEventSource ies)
        {
            source_type=ies;
            parent_input_event=nullptr;
        }

        virtual ~EventDispatch()
        {
            if(parent_input_event)
                parent_input_event->UnregisterEventDispatch(this);
        }

        virtual bool RegisterEventDispatch(EventDispatch *ie)
        {
            if(!ie)
                return(false);

            const InputEventSource ies=ie->GetInputEventSource();

            if(!RangeCheck(ies))
                return(false);

            ie->SetParent(this);

            return(event_dispatch_subscribers.Add(ie)!=-1);
        }

        bool UnregisterEventDispatch(EventDispatch *ie)
        {
            if(!ie)return(false);

            const InputEventSource ies=ie->GetInputEventSource();

            if(!RangeCheck(ies))
                return(false);

            ie->SetParent(nullptr);

            return event_dispatch_subscribers.Delete(ie);
        }

        virtual bool Update(){return true;}
    };//class EventDispatch
}//namespace hgl::io
