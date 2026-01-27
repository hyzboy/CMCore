#include<hgl/io/event/TouchEvent.h>

namespace hgl::io
{
    EventProcResult TouchEvent::OnEvent(const EventHeader &header,const uint64 data)
    {
        if(header.type==InputEventSource::Touch)
        {
            ted=(TouchEventData *)&data;

            position.x=ted->x;
            position.y=ted->y;
            current_touch_count=ted->touch_count;

            switch(TouchEventID(header.id))
            {
                case TouchEventID::Down:    if(OnDown   (position,current_touch_count)==EventProcResult::Break)return EventProcResult::Break;break;
                case TouchEventID::Up:      if(OnUp     (position,current_touch_count)==EventProcResult::Break)return EventProcResult::Break;break;
                case TouchEventID::Move:    if(OnMove   (position,current_touch_count)==EventProcResult::Break)return EventProcResult::Break;break;
            }
        }

        if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
            return EventProcResult::Break;

        return EventProcResult::Continue;
    }
}//namespace hgl::io
