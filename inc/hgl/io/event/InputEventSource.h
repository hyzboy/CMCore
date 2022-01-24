#ifndef HGL_IO_INPUT_DEVICE_SOURCE_INCLUDE
#define HGL_IO_INPUT_DEVICE_SOURCE_INCLUDE

#include<hgl/TypeFunc.h>
namespace hgl
{
    namespace io
    {
        enum class InputEventSource
        {
            Root=0,

            OS,
            Window,

            Keyboard,
            Mouse,
            Joystick,

            ENUM_CLASS_RANGE(Root,Joystick)
        };
    }//namespace io
}//namespace hgl
#endif//HGL_IO_INPUT_DEVICE_SOURCE_INCLUDE