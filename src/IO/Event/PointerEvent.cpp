#include<hgl/io/event/PointerEvent.h>

namespace hgl::io
{
    EventProcResult PointerEvent::OnEvent(const EventHeader &header,const uint64 data)
    {
        if(header.type==InputEventSource::Pointer)
        {
            ped=(PointerEventData *)&data;

            position.x=ped->x;
            position.y=ped->y;

            const PointerDeviceType device_type = PointerDeviceType(ped->device_type);
            const PointerButton button = PointerButton(ped->button);
            const uint16 pressure = ped->pressure;

            switch(PointerEventID(header.id))
            {
                case PointerEventID::Down:      pressed_statues[ped->button]=true;
                                                if(OnDown   (position,device_type,button,pressure,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
                case PointerEventID::Up:        pressed_statues[ped->button]=false;
                                                if(OnUp     (position,device_type,button,pressure,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
                case PointerEventID::Update:    if(OnUpdate (position,device_type,button,pressure,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
                case PointerEventID::Enter:     if(OnEnter  (position,device_type,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
                case PointerEventID::Leave:     if(OnLeave  (position,device_type,extended_info)==EventProcResult::Break)return EventProcResult::Break;break;
            }
        }

        if(EventDispatcher::OnEvent(header,data)==EventProcResult::Break)
            return EventProcResult::Break;

        return EventProcResult::Continue;
    }
}//namespace hgl::io
