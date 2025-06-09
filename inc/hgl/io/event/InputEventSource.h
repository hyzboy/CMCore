#pragma once

#include<hgl/TypeFunc.h>
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

        ENUM_CLASS_RANGE(Root,Joystick)
    };
}//namespace hgl::io
