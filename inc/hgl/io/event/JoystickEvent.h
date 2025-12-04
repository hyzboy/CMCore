#pragma once

#include<hgl/type/EnumUtil.h>
namespace hgl::io
{      
    /**
    * 手柄按键枚举
    */
    enum class JoystickButton
    {
        NONE=0,

        Select,
        Start,

        Up,
        Down,
        Left,
        Right,

        _0,        _1,        _2,        _3,        _4,        _5,        _6,        _7,
        _8,        _9,        _10,       _11,       _12,       _13,       _14,       _15,
        _16,       _17,       _18,       _19,       _20,       _21,       _22,       _23,
        _24,       _25,       _26,       _27,       _28,       _29,       _30,       _31,

        ENUM_CLASS_RANGE(NONE,_31)
    };//enum class JoystickButton
}//namespace hgl::io
