#pragma once

#include<hgl/io/event/EventDispatcher.h>
#include<hgl/math/Vector.h>

namespace hgl::io
{
    using namespace hgl::math;

    /**
    * 触摸点信息
    */
    struct TouchPoint
    {
        uint32 id;              // 触摸点ID
        int32 x, y;             // 触摸点位置
        int32 cx, cy;           // 触摸点大小
        uint32 flags;           // 触摸点标志
        uint32 mask;            // 有效字段掩码
        uint32 time;            // 时间戳

        TouchPoint()
        {
            id = 0;
            x = y = 0;
            cx = cy = 0;
            flags = 0;
            mask = 0;
            time = 0;
        }
    };

    enum class TouchEventID
    {
        Down=0,
        Up,
        Move
    };

    union TouchEventData
    {
        uint64 data;

        struct
        {
            int16 x,y;
            uint16 touch_count;     // 触摸点数量
            uint16 reserved;
        };
    };

    constexpr const size_t TouchEventDataBytes=sizeof(TouchEventData);

    constexpr const size_t MAX_TOUCH_POINTS = 16;

    class TouchEvent:public EventDispatcher
    {
        TouchEventData *ted=nullptr;

        Vector2i position{};

        TouchPoint touch_points[MAX_TOUCH_POINTS];

        uint16 current_touch_count = 0;

    public:

        TouchEvent():EventDispatcher(InputEventSource::Touch){}
        virtual ~TouchEvent()=default;

        virtual EventProcResult OnEvent     (const EventHeader &header,const uint64 data) override;

        virtual EventProcResult OnDown      (const Vector2i &,uint16 touch_count){return EventProcResult::Continue;}
        virtual EventProcResult OnUp        (const Vector2i &,uint16 touch_count){return EventProcResult::Continue;}
        virtual EventProcResult OnMove      (const Vector2i &,uint16 touch_count){return EventProcResult::Continue;}

        /**
         * 获取触摸点数量
         */
        const uint16 GetTouchCount()const{return current_touch_count;}

        /**
         * 获取触摸点信息
         */
        const TouchPoint *GetTouchPoint(uint16 index)const
        {
            if(index>=current_touch_count)return nullptr;
            return &touch_points[index];
        }

        /**
         * 获取所有触摸点
         */
        const TouchPoint *GetAllTouchPoints()const{return touch_points;}

    public:

        const Vector2i &GetTouchCoord()const{return position;}

        /**
         * Set touch point information (used by platform-specific handlers)
         */
        void SetTouchPoint(uint16 index, const TouchPoint& point)
        {
            if(index < MAX_TOUCH_POINTS)
                touch_points[index] = point;
        }

        /**
         * Set touch count (used by platform-specific handlers)
         */
        void SetTouchCount(uint16 count)
        {
            if(count <= MAX_TOUCH_POINTS)
                current_touch_count = count;
        }
    };//class TouchEvent
}//namespace hgl::io
