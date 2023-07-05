#ifndef HGL_IO_DEVICE_MOUSE_EVENT_INCLUDE
#define HGL_IO_DEVICE_MOUSE_EVENT_INCLUDE

#include<hgl/TypeFunc.h>
namespace hgl
{
    namespace io
    {        
        /**
        * 手柄按键枚举
        */
        enum class JoystickButton
        {
            NONE=0,

            Up,
            Down,
            Left,
            Right,

            _0,        _1,        _2,        _3,        _4,        _5,        _6,        _7,
            _8,        _9,        _10,       _11,       _12,       _13,       _14,       _15,
            _16,       _17,       _18,       _19,       _20,       _21,       _22,       _23,
            _24,       _25,       _26,       _27,       _28,       _29,       _30,       _31,

            ENUM_CLASS_RANGE(NONE,_31)
        };

        enum class PlayStationButton
        {
            Up          =JoystickButton::Up,
            Down        =JoystickButton::Down,
            Left        =JoystickButton::Left,
            Right       =JoystickButton::Right,

            Fork        =JoystickButton::_0,           // 叉
            Circle      =JoystickButton::_1,           // 圆
            Square      =JoystickButton::_2,           // 方
            Triangle    =JoystickButton::_3,           // 三角

            L1          =JoystickButton::_6,       L2        =JoystickButton::_4,
            R1          =JoystickButton::_7,       R2        =JoystickButton::_5,
            Select      =JoystickButton::_8,       Start     =JoystickButton::_9,
        };

        //enum class XBoxButton
        //{
        //    //DreamCast/XBOX
        //    X=JoystickButton::_2,
        //    Y=JoystickButton::_3,
        //    A=JoystickButton::_0,
        //    B=JoystickButton::_1,
        //    L=JoystickButton::_4,
        //    R=JoystickButton::_5,

        //    //XBOX/XBOX360
        //    //XBOX,                     //西瓜键
        //};
    }//namespace io
}//namespace hgl
#endif//HGL_IO_DEVICE_MOUSE_EVENT_INCLUDE
