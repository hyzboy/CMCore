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
        Touch,
        Gesture,

        ENUM_CLASS_RANGE(Root,Gesture)
    };
}//namespace hgl::io
