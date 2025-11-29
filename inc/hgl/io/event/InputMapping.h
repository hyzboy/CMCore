#pragma once

#include<hgl/type/IDName.h>
#include<hgl/io/event/KeyboardEvent.h>
#include<hgl/io/event/MouseEvent.h>
#include<hgl/io/event/JoystickEvent.h>

namespace hgl::io
{
    /**
     * @brief 输入映射系统的名称
     * 
     * 这个名称用于标识输入映射系统的类型或实例。
     * 它可以用于在不同的输入映射系统之间进行区分。
     * 
     * 注意：这个名称并不是唯一的，可能会有多个输入映射系统使用相同的名称。
     *       因此，在使用时需要确保上下文的正确性。
     *
     * 1.输入映射系统会让开发者定义一些输入事件名称，这些名称可以是任意的字符串。
     *
     * 2.一个输入事件，可能被多个输入设备触发，比如你可以定义键盘上的某个按键、鼠标的左键、手柄的某个按钮均可以触发同一个输入事件。
     *
     * 3.真实的输入部分是可以配置的。比如一个FPS游戏的主角移动，有的用户喜欢用WASD，而有的用户喜欢用方向键。
     *
     * 4.这里映射的并不一定就是物理输入设备的键入，也可以数据流的输入。比如游戏回放，或是网络同步。更有可能是程序自己做的输入。
     */
    HGL_DEFINE_ANSI_IDNAME(InputEventName)

    enum class InputSourceType
    {
        None,       //无效输入
        Button,     //按键输入
        Axis,       //轴输入
        Touch,      //触控输入
        Trigger,    //触发器输入
        Hat,        //方向盘输入

        ENUM_CLASS_RANGE(Button,Hat)
    };

    struct InputSource
    {
        InputSourceType type;  //输入类型
    };

    struct KeyboardInputSource:public InputSource
    {
        KeyboardButton key;     //键盘按键
    };

    struct MouseInputSource:public InputSource
    {
        MouseButton button;     //鼠标按键
    };

    struct JoystickButtonInputSource:public InputSource
    {
        JoystickButton button;  //手柄按键
    };

    class InputMapping
    {
        InputEventName name;  //输入事件名称

        ObjectList<InputSource> sources; //输入源列表

    public:

    };
}//namespace hgl::io
