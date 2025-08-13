#include<hgl/io/event/EventDispatcher.h>

namespace hgl::io
{
    EventProcResult EventDispatcher::OnEvent(const EventHeader &header,const uint64 data)
    {
        if(!RangeCheck(header.type))
            return(EventProcResult::Break);

        if(exclusive_dispatcher)
        {
            if(exclusive_dispatcher->OnEvent(header,data) == EventProcResult::Break)
                return EventProcResult::Break;
        }

        if(!child_dispatchers.IsEmpty())
        {
            for(EventDispatcher *ie:child_dispatchers)
                if(ie->OnEvent(header,data)==EventProcResult::Break)
                    return EventProcResult::Break;
        }

        return(EventProcResult::Continue);
    }

    bool EventDispatcher::AddChildDispatcher(EventDispatcher *ie)
    {
        if(!ie)
            return(false);

        const InputEventSource ies=ie->GetInputEventSource();

        if(!RangeCheck(ies))
            return(false);

        ie->SetParent(this);

        return(child_dispatchers.Add(ie)!=-1);
    }

    bool EventDispatcher::RemoveChildDispatcher(EventDispatcher *ie)
    {
        if(!ie)return(false);

        const InputEventSource ies=ie->GetInputEventSource();

        if(!RangeCheck(ies))
            return(false);

        ie->SetParent(nullptr);

        return child_dispatchers.Delete(ie);
    }

    bool EventDispatcher::SetExclusiveDispatcher(EventDispatcher *ie)
    {
        if(!ie)
            return(false);

        if(exclusive_dispatcher==ie)
            return(true); //已经是独占分发器了

        const InputEventSource ies=ie->GetInputEventSource();

        if(!RangeCheck(ies))
            return(false);

        if(exclusive_dispatcher)
            exclusive_dispatcher->SetParent(nullptr);

        exclusive_dispatcher=ie;
        ie->SetParent(this);
        return(true);
    }

    bool EventDispatcher::RemoveExclusiveDispatcher(EventDispatcher *ie)
    {
        if(!ie)
            return(false);

        if(exclusive_dispatcher!=ie)
            return(false); //不是独占分发器

        exclusive_dispatcher=nullptr;
        ie->SetParent(nullptr);

        return(true);
    }
}//namespace hgl::io
