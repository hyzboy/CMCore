#include<hgl/io/event/EventDispatcher.h>

namespace hgl::io
{
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
}//namespace hgl::io
