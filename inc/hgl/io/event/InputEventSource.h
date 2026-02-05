#pragma once

#include<hgl/type/EnumUtil.h>
namespace hgl::io
{
    enum class InputEventSource:uint8
    {
        Root=0,

        OS,
        Window,

        Logic,      ///< 逻辑输入源（虚拟输入设备，参见InputMapping.h）

        Keyboard,
        Mouse,
        Joystick,
        Pointer,
        Touch,
        Gesture,

        ENUM_CLASS_RANGE(Root,Gesture)
    };
}//namespace hgl::io
