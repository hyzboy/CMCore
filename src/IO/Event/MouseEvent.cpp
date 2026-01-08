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

            if(MouseEventID(header.id)==MouseEventID::Wheel)
            {
                if(OnWheel(position)==EventProcResult::Break)
                    return EventProcResult::Break;
            }
            else
            {
                switch(MouseEventID(header.id))
                {
                    case MouseEventID::Move:        if(OnMove       (position)                         ==EventProcResult::Break)return EventProcResult::Break;break;
                    case MouseEventID::Pressed:     pressed_statues[med->button]=true;
                                                    if(OnPressed    (position,MouseButton(med->button))==EventProcResult::Break)return EventProcResult::Break;break;
                    case MouseEventID::Released:    pressed_statues[med->button]=false;
                                                    if(OnReleased   (position,MouseButton(med->button))==EventProcResult::Break)return EventProcResult::Break;break;
                    case MouseEventID::DblClicked:  if(OnDblClicked (position,MouseButton(med->button))==EventProcResult::Break)return EventProcResult::Break;break;
                }
            }
        }

        if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
            return EventProcResult::Break;

        return EventProcResult::Continue;
    }
}//namespace hgl::io
