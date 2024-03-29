﻿#ifndef HGL_IO_MOUSE_EVENT_INCLUDE
#define HGL_IO_MOUSE_EVENT_INCLUDE

#include<hgl/io/event/InputEvent.h>
namespace hgl
{
    namespace io
    {
        /**
        * 鼠标按钮枚举
        */
        enum class MouseButton
        {
            None=0,

            Left,
            Mid,
            Right,

            X1,X2,

            ENUM_CLASS_RANGE(None,X2)
        };

        enum class MouseEventID
        {
            Move    =0,
            Pressed,
            Released,
            DblClicked,
            Wheel
        };

        union MouseEventData
        {
            uint64 data;

            struct
            {
                int16 x,y;
                uint8 button;
            };
        };

        constexpr size_t MouseEventDataBytes=sizeof(MouseEventData);
        
        class MouseEvent:public InputEvent
        {
            MouseEventData *med;

            int x,y;

            bool pressed_statues[size_t(MouseButton::RANGE_SIZE)];

        public:

            MouseEvent():InputEvent(InputEventSource::Mouse)
            {
                med=nullptr;
                x=y=0;
                hgl_zero(pressed_statues);
            }
            virtual ~MouseEvent()=default;
            
            EventProcResult OnEvent(const EventHeader &header,const uint64 data) override
            {
                if(InputEvent::OnEvent(header,data)==EventProcResult::Break)
                    return EventProcResult::Break;

                med=(MouseEventData *)&data;

                if(MouseEventID(header.id)==MouseEventID::Wheel)
                {                    
                    if(OnWheel      (med->x,med->y)             )return EventProcResult::Break;
                }
                else
                {
                    x=med->x;y=med->y;

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

                return EventProcResult::Continue;
            }

            virtual bool OnMove      (int,int){return false;}
            virtual bool OnWheel     (int,int){return false;}

            virtual bool OnPressed   (int,int,MouseButton){return false;}
            virtual bool OnReleased  (int,int,MouseButton){return false;}
            virtual bool OnDblClicked(int,int,MouseButton){return false;}

            /**
             * 某按键是否按下
             */
            const bool HasPressed(const MouseButton &mb)const
            {
                if(!RangeCheck(mb))return(false);

                return pressed_statues[size_t(mb)];
            }

        public:

            const int GetX()const{return x;}
            const int GetY()const{return y;}
        };//class MouseEvent
    }//namespace io
}//namespace hgl
#endif//HGL_IO_MOUSE_EVENT_INCLUDE
