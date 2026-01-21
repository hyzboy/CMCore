#pragma once

#include<hgl/io/event/EventDispatcher.h>
#include<hgl/math/Vector.h>

namespace hgl::io
{
    using namespace hgl::math;

    /**
    * 手势类型枚举
    */
    enum class GestureType
    {
        None=0,

        Begin,          // 手势开始
        End,            // 手势结束
        Zoom,           // 缩放
        Pan,            // 平移
        Rotate,         // 旋转
        TwoFingerTap,   // 双指点击
        PressAndTap,    // 按住并点击

        ENUM_CLASS_RANGE(None,PressAndTap)
    };

    enum class GestureEventID
    {
        Begin=0,
        End,
        Update
    };

    union GestureEventData
    {
        uint64 data;

        struct
        {
            int16 x,y;
            uint8 gesture_type;     // GestureType
            uint8 flags;
            uint16 reserved;
        };
    };

    constexpr size_t GestureEventDataBytes=sizeof(GestureEventData);

    /**
     * 手势扩展信息
     */
    struct GestureExtendedInfo
    {
        uint64 gesture_id;      // 手势ID
        uint32 timestamp;       // 时间戳
        
        int32 distance;         // 距离（缩放时使用）
        int32 argument;         // 参数（旋转角度等）
        
        uint32 sequence_id;     // 序列ID
        
        GestureExtendedInfo()
        {
            gesture_id = 0;
            timestamp = 0;
            distance = 0;
            argument = 0;
            sequence_id = 0;
        }
    };
        
    class GestureEvent:public EventDispatcher
    {
        GestureEventData *ged=nullptr;

        Vector2i position{};
        GestureType current_gesture_type = GestureType::None;

    public:

        GestureExtendedInfo extended_info;

        GestureEvent():EventDispatcher(InputEventSource::Gesture){}
        virtual ~GestureEvent()=default;

        virtual EventProcResult OnEvent     (const EventHeader &header,const uint64 data) override;

        virtual EventProcResult OnBegin     (const Vector2i &,GestureType,const GestureExtendedInfo &){return EventProcResult::Continue;}
        virtual EventProcResult OnEnd       (const Vector2i &,GestureType,const GestureExtendedInfo &){return EventProcResult::Continue;}
        virtual EventProcResult OnUpdate    (const Vector2i &,GestureType,const GestureExtendedInfo &){return EventProcResult::Continue;}

        /**
         * 判断手势类型
         */
        const GestureType GetGestureType()const{return current_gesture_type;}
        const bool IsZoom()const{return current_gesture_type==GestureType::Zoom;}
        const bool IsPan()const{return current_gesture_type==GestureType::Pan;}
        const bool IsRotate()const{return current_gesture_type==GestureType::Rotate;}

    public:

        const Vector2i &GetGestureCoord()const{return position;}
        const GestureExtendedInfo &GetExtendedInfo()const{return extended_info;}
    };//class GestureEvent
}//namespace hgl::io
