#pragma once

#include <cstddef>
#include <hgl/type/IDName.h>
#include <hgl/io/event/InputEventSource.h>
#include <hgl/io/event/KeyboardEvent.h>
#include <hgl/io/event/MouseEvent.h>
#include <hgl/io/event/JoystickEvent.h>

namespace hgl::io
{
    /**
     * 逻辑动作ID（应用层自定义）
     * 示例：Fire=1, Jump=2, MoveForward=3, Reload=4 等
     */
    using ActionID = uint32;

    constexpr ActionID ACTION_NONE = 0;

    /**
     * 预置动作ID配置
     */
    struct FpsActionConfig
    {
        ActionID move_forward = 1;
        ActionID move_backward = 2;
        ActionID move_left = 3;
        ActionID move_right = 4;
        ActionID fire = 5;
        ActionID jump = 6;
        ActionID crouch = 7;
        ActionID run = 8;
        ActionID prone = 9;
        ActionID aim = 10;
        ActionID reload = 11;
        ActionID use = 12;
        ActionID melee = 13;
        ActionID grenade = 14;
        ActionID weapon1 = 15;
        ActionID weapon2 = 16;
        ActionID weapon3 = 17;
    };

    /**
     * 动作值类型
     */
    enum class ActionValueType
    {
        Digital,    ///< 数字量：按下/释放（0/1）
        Analog1D,   ///< 模拟量1D：单轴（如扳机 0.0~1.0）
        Analog2D,   ///< 模拟量2D：双轴（如摇杆 xy）
        Analog3D    ///< 模拟量3D：三轴（如陀螺仪 xyz）
    };

    /**
     * 动作值（可携带不同维度的数据）
     */
    struct ActionValue
    {
        ActionValueType type = ActionValueType::Digital;

        union
        {
            bool digital;           ///< 数字量
            float analog_1d;        ///< 单轴模拟量
            float analog_2d[2];     ///< 双轴模拟量
            float analog_3d[3];     ///< 三轴模拟量
        };

        ActionValue() : digital(false) {}

        static ActionValue Digital(bool pressed)
        {
            ActionValue v;
            v.type = ActionValueType::Digital;
            v.digital = pressed;
            return v;
        }

        static ActionValue Analog1D(float value)
        {
            ActionValue v;
            v.type = ActionValueType::Analog1D;
            v.analog_1d = value;
            return v;
        }

        static ActionValue Analog2D(float x, float y)
        {
            ActionValue v;
            v.type = ActionValueType::Analog2D;
            v.analog_2d[0] = x;
            v.analog_2d[1] = y;
            return v;
        }
    };

    /**
     * 物理输入描述（唯一标识一个物理按键/按钮/轴）
     */
    struct PhysicalInput
    {
        InputEventSource    source;     ///< 输入源类型
        uint8               index;      ///< 设备索引
        uint16              code;       ///< 按键/按钮编号（键盘键值、鼠标按钮、手柄按钮等）

        PhysicalInput() : source(InputEventSource::Root), index(0), code(0) {}
        PhysicalInput(InputEventSource s, uint8 i, uint16 c) : source(s), index(i), code(c) {}

        bool operator==(const PhysicalInput& other) const
        {
            return source == other.source && index == other.index && code == other.code;
        }

        bool operator<(const PhysicalInput& other) const
        {
            if (source != other.source) return source < other.source;
            if (index != other.index) return index < other.index;
            return code < other.code;
        }

        // 便捷构造函数
        static PhysicalInput Keyboard(KeyboardButton key, uint8 device_index = 0)
        {
            return PhysicalInput(InputEventSource::Keyboard, device_index, uint16(key));
        }

        static PhysicalInput Mouse(MouseButton button, uint8 device_index = 0)
        {
            return PhysicalInput(InputEventSource::Mouse, device_index, uint16(button));
        }

        static PhysicalInput Joystick(JoystickButton button, uint8 device_index = 0)
        {
            return PhysicalInput(InputEventSource::Joystick, device_index, uint16(button));
        }
    };

}//namespace hgl::io

namespace std
{
    template<>
    struct hash<hgl::io::PhysicalInput>
    {
        size_t operator()(const hgl::io::PhysicalInput& v) const noexcept
        {
            const hgl::uint32 source = static_cast<hgl::uint32>(v.source) & 0xFFu;
            const hgl::uint32 index = static_cast<hgl::uint32>(v.index) & 0xFFu;
            const hgl::uint32 code = static_cast<hgl::uint32>(v.code) & 0xFFFFu;
            const hgl::uint32 packed = (source << 24) | (index << 16) | code;
            return static_cast<size_t>(packed);
        }
    };
}

namespace hgl::io
{
    /**
     * 动作事件状态
     */
    enum class ActionEventState
    {
        Started,        ///< 动作开始（首次按下）
        Ongoing,        ///< 动作进行中（持续按住或模拟量变化）
        Completed,      ///< 动作完成（释放）
        Canceled        ///< 动作取消（被其他输入中断）
    };

    /**
     * 逻辑动作事件（输出给游戏逻辑层）
     */
    struct ActionEvent
    {
        ActionID            action;         ///< 动作ID
        ActionEventState    state;          ///< 事件状态
        ActionValue         value;          ///< 动作值
        double              elapsed_time;   ///< 从Started开始的持续时间（秒）
        PhysicalInput       trigger;        ///< 触发此动作的物理输入

        ActionEvent() : action(ACTION_NONE), state(ActionEventState::Started), elapsed_time(0.0) {}
    };

    /**
     * 输入绑定：物理输入 → 逻辑动作的映射规则
     */
    struct InputBinding
    {
        PhysicalInput   physical;       ///< 物理输入
        ActionID        action;         ///< 目标动作
        ActionValueType value_type;     ///< 值类型
        float           scale;          ///< 缩放系数（用于模拟量）
        bool            negate;         ///< 是否取反（用于方向反转）
        bool            forward_physical_input;  ///< 是否继续分发物理输入

        InputBinding() : action(ACTION_NONE), value_type(ActionValueType::Digital), scale(1.0f), negate(false), forward_physical_input(false) {}

        InputBinding(PhysicalInput phys, ActionID act, ActionValueType vt = ActionValueType::Digital, bool forward_physical = false)
            : physical(phys), action(act), value_type(vt), scale(1.0f), negate(false), forward_physical_input(forward_physical) {}

        bool operator==(const InputBinding& other) const
        {
            return physical == other.physical
                && action == other.action
                && value_type == other.value_type
                && scale == other.scale
                && negate == other.negate
                && forward_physical_input == other.forward_physical_input;
        }
    };
}//namespace hgl::io
