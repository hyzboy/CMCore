#pragma once

#include<hgl/type/EnumUtil.h>
namespace hgl::io
{
    enum class InputEventSource:uint8
    {
        Root=0,

        OS,
        Window,

        Keyboard,
        Mouse,
        Joystick,
        Pointer,

        ENUM_CLASS_RANGE(Root,Pointer)
    };
}//namespace hgl::io
