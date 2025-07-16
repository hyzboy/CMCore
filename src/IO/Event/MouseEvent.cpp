#include<hgl/io/event/MouseEvent.h>

namespace hgl::io
{
    EventProcResult MouseEvent::OnEvent(const EventHeader &header,const uint64 data)
    {
        if(header.type==InputEventSource::Mouse)
        {
            med=(MouseEventData *)&data;

            if(MouseEventID(header.id)==MouseEventID::Wheel)
            {                    
                if(OnWheel      (med->x,med->y)             )return EventProcResult::Break;
            }
            else
            {
                position.x=med->x;
                position.y=med->y;

                switch(MouseEventID(header.id))
                {
                    case MouseEventID::Move:        if(OnMove       (med->x,med->y)                         )return EventProcResult::Break;break;
                    case MouseEventID::Pressed:     pressed_statues[med->button]=true;                                
                                                    if(OnPressed    (med->x,med->y,MouseButton(med->button)))return EventProcResult::Break;break;
                    case MouseEventID::Released:    pressed_statues[med->button]=false;
                                                    if(OnReleased   (med->x,med->y,MouseButton(med->button)))return EventProcResult::Break;break;
                    case MouseEventID::DblClicked:  if(OnDblClicked (med->x,med->y,MouseButton(med->button)))return EventProcResult::Break;break;
                }
            }
        }

        if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
            return EventProcResult::Break;

        return EventProcResult::Continue;
    }
}//namespace hgl::io
