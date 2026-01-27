#include<hgl/io/event/GestureEvent.h>

namespace hgl::io
{
    EventProcResult GestureEvent::OnEvent(const EventHeader &header,const uint64 data)
    {
        if(header.type==InputEventSource::Gesture)
        {
            ged=(GestureEventData *)&data;

            position.x=ged->x;
            position.y=ged->y;
            current_gesture_type=GestureType(ged->gesture_type);

            switch(GestureEventID(header.id))
            {
                case GestureEventID::Begin:     if(OnBegin  (position,current_gesture_type,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
                case GestureEventID::End:       if(OnEnd    (position,current_gesture_type,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
                case GestureEventID::Update:    if(OnUpdate (position,current_gesture_type,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
            }
        }

        if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
            return EventProcResult::Break;

        return EventProcResult::Continue;
    }
}//namespace hgl::io
