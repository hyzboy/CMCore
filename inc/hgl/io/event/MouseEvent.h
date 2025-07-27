#pragma once

#include<hgl/io/event/EventDispatcher.h>
#include<hgl/math/Vector.h>
namespace hgl::io
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
        
    class MouseEvent:public EventDispatcher
    {
        MouseEventData *med=nullptr;

        Vector2i position{};

        bool pressed_statues[size_t(MouseButton::RANGE_SIZE)]{};

    public:

        MouseEvent():EventDispatcher(InputEventSource::Mouse){}
        virtual ~MouseEvent()=default;

        virtual EventProcResult OnEvent     (const EventHeader &header,const uint64 data) override;

        virtual bool            OnMove      (const Vector2i &){return false;}
        virtual bool            OnWheel     (const Vector2i &){return false;}

        virtual bool            OnPressed   (const Vector2i &,MouseButton){return false;}
        virtual bool            OnReleased  (const Vector2i &,MouseButton){return false;}
        virtual bool            OnDblClicked(const Vector2i &,MouseButton){return false;}

        /**
         * 某按键是否按下
         */
        const bool HasPressed(const MouseButton &mb)const
        {
            if(!RangeCheck(mb))return(false);

            return pressed_statues[size_t(mb)];
        }

    public:

        const Vector2i &GetMouseCoord()const{return position;}
    };//class MouseEvent
}//namespace hgl::io
