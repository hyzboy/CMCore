#include<hgl/io/event/MouseEvent.h>

namespace hgl::io
{
    EventProcResult MouseEvent::OnEvent(const EventHeader &header,const uint64 data)
    {
        if(header.type==InputEventSource::Mouse)
        {
            med=(MouseEventData *)&data;

            position.x=med->x;
            position.y=med->y;

            #ifdef _DEBUG
            if(med->action!=(uint8)header.id)
                return EventProcResult::Break;
            #endif//_DEBUG

            if(MouseAction(med->action)==MouseAction::Wheel)
            {
                if(OnWheel(position)==EventProcResult::Break)
                    return EventProcResult::Break;
            }
            else
            {
                switch(MouseAction(med->action))
                {
                    case MouseAction::Move:        if(OnMove       (position)                         ==EventProcResult::Break)return EventProcResult::Break;break;
                    case MouseAction::Pressed:     pressed_statues[med->button]=true;
                                                    if(OnPressed    (position,MouseButton(med->button))==EventProcResult::Break)return EventProcResult::Break;break;
                    case MouseAction::Released:    pressed_statues[med->button]=false;
                                                    if(OnReleased   (position,MouseButton(med->button))==EventProcResult::Break)return EventProcResult::Break;break;
                    case MouseAction::DblClicked:  if(OnDblClicked (position,MouseButton(med->button))==EventProcResult::Break)return EventProcResult::Break;break;
                }
            }
        }

        if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
            return EventProcResult::Break;

        return EventProcResult::Continue;
    }
}//namespace hgl::io
