#pragma once

#include<hgl/type/SortedSet.h>
#include<hgl/io/event/InputEventSource.h>
namespace hgl::io
{
    struct EventHeader
    {
        uint8 type;      ///<输入源类型
        uint8 index;     ///<输入源索引(比如同一设备有多个)
        uint16 id;       ///<事件id
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

    class InputEvent
    {
    protected:

        InputEventSource source_type;

        SortedSet<InputEvent *> sub_event_proc[size_t(InputEventSource::RANGE_SIZE)];

    public:

        const InputEventSource GetInputEventSource()const{return source_type;}

        virtual EventProcResult OnEvent(const EventHeader &header,const uint64 data)
        {
            if(!RangeCheck((InputEventSource)header.type))
                return(EventProcResult::Break);

            if(sub_event_proc[header.type].GetCount()>0)
            {
                for(InputEvent *ie:sub_event_proc[header.type])
                    if(ie->OnEvent(header,data)==EventProcResult::Break)
                        return EventProcResult::Break;
            }

            if(sub_event_proc[size_t(InputEventSource::Root)].GetCount()>0
                &&InputEventSource(header.type)!=InputEventSource::Root)
            {
                for(InputEvent *ie:sub_event_proc[size_t(InputEventSource::Root)])
                    if(ie->OnEvent(header,data)==EventProcResult::Break)
                        return EventProcResult::Break;
            }

            return(EventProcResult::Continue);
        }

    public:

        InputEvent()
        {
            source_type=InputEventSource::Root;
        }

        InputEvent(InputEventSource ies)
        {
            source_type=ies;
        }

        virtual ~InputEvent()=default;

        virtual bool Join(InputEvent *ie)
        {
            if(!ie)
                return(false);

            const InputEventSource ies=ie->GetInputEventSource();

            if(!RangeCheck(ies))
                return(false);

            return(sub_event_proc[size_t(ies)].Add(ie)!=-1);
        }

        bool Unjoin(InputEvent *ie)
        {
            if(!ie)return(false);

            const InputEventSource ies=ie->GetInputEventSource();

            if(!RangeCheck(ies))
                return(false);

            return sub_event_proc[size_t(ies)].Delete(ie);
        }
    };//class InputEvent
}//namespace hgl::io
