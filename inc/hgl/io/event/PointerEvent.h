#pragma once

#include<hgl/io/event/EventDispatcher.h>
#include<hgl/math/Vector.h>

namespace hgl::io
{
    using namespace hgl::math;

    /**
    * 指针设备类型枚举
    */
    enum class PointerDeviceType
    {
        None=0,

        Pen,            // 触控笔
        Touch,          // 触摸
        Mouse,          // 鼠标

        ENUM_CLASS_RANGE(None,Mouse)
    };

    /**
    * 指针按钮枚举
    */
    enum class PointerButton
    {
        None=0,

        Primary,        // 主按钮
        Secondary,      // 次按钮
        Tertiary,       // 第三按钮

        ENUM_CLASS_RANGE(None,Tertiary)
    };

    enum class PointerEventID
    {
        Down=0,
        Up,
        Update,
        Enter,
        Leave
    };

    union PointerEventData
    {
        uint64 data;

        struct
        {
            int16 x,y;
            uint8 device_type;      // PointerDeviceType
            uint8 button;           // PointerButton
            uint16 pressure;        // 压力值 (0-1024)
        };
    };

    constexpr size_t PointerEventDataBytes=sizeof(PointerEventData);
        
    class PointerEvent:public EventDispatcher
    {
        PointerEventData *ped=nullptr;

        Vector2i position{};

        bool pressed_statues[size_t(PointerButton::RANGE_SIZE)]{};

    public:

        PointerEvent():EventDispatcher(InputEventSource::Pointer){}
        virtual ~PointerEvent()=default;

        virtual EventProcResult OnEvent     (const EventHeader &header,const uint64 data) override;

        virtual EventProcResult OnDown      (const Vector2i &,PointerDeviceType,PointerButton,uint16){return EventProcResult::Continue;}
        virtual EventProcResult OnUp        (const Vector2i &,PointerDeviceType,PointerButton,uint16){return EventProcResult::Continue;}
        virtual EventProcResult OnUpdate    (const Vector2i &,PointerDeviceType,PointerButton,uint16){return EventProcResult::Continue;}
        virtual EventProcResult OnEnter     (const Vector2i &,PointerDeviceType){return EventProcResult::Continue;}
        virtual EventProcResult OnLeave     (const Vector2i &,PointerDeviceType){return EventProcResult::Continue;}

        /**
         * 某按键是否按下
         */
        const bool HasPressed(const PointerButton &pb)const
        {
            if(!RangeCheck(pb))return(false);

            return pressed_statues[size_t(pb)];
        }

    public:

        const Vector2i &GetPointerCoord()const{return position;}
    };//class PointerEvent
}//namespace hgl::io
