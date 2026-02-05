#pragma once

#include<hgl/type/IDName.h>
#include<hgl/type/UnorderedMap.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/Stack.h>
#include<hgl/io/event/KeyboardEvent.h>
#include<hgl/io/event/MouseEvent.h>
#include<hgl/io/event/JoystickEvent.h>
#include<functional>

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

    /**
     * 输入上下文：管理一组输入绑定（可堆栈式激活/停用）
     * 示例：MenuContext、GameplayContext、VehicleContext
     */
    class InputContext
    {
        U8String                    name;
        ValueArray<InputBinding>     bindings;
        UnorderedMap<PhysicalInput, int>     physical_to_binding_index;  ///< 快速查找

    public:

        InputContext() = default;
        InputContext(const U8String& ctx_name) : name(ctx_name) {}

        const U8String& GetName() const { return name; }

        /**
         * 添加绑定：物理输入 → 动作
         */
        void AddBinding(const InputBinding& binding)
        {
            int index = bindings.GetCount();
            bindings.Add(binding);
            physical_to_binding_index.Add(binding.physical, index);
        }

        /**
         * 便捷添加：键盘按键 → 动作
         */
        void BindKey(ActionID action, KeyboardButton key, uint8 device_index = 0, bool forward_physical = false)
        {
            AddBinding(InputBinding(PhysicalInput::Keyboard(key, device_index), action, ActionValueType::Digital, forward_physical));
        }

        /**
         * 便捷添加：鼠标按钮 → 动作
         */
        void BindMouse(ActionID action, MouseButton button, uint8 device_index = 0, bool forward_physical = false)
        {
            AddBinding(InputBinding(PhysicalInput::Mouse(button, device_index), action, ActionValueType::Digital, forward_physical));
        }

        /**
         * 便捷添加：手柄按钮 → 动作
         */
        void BindJoystick(ActionID action, JoystickButton button, uint8 device_index = 0, bool forward_physical = false)
        {
            AddBinding(InputBinding(PhysicalInput::Joystick(button, device_index), action, ActionValueType::Digital, forward_physical));
        }

        /**
         * 查找绑定（根据物理输入）
         */
        const InputBinding *FindBinding(const PhysicalInput& physical) const
        {
            int index;

            if(!physical_to_binding_index.Get(physical,index))
                return nullptr;

            return bindings.At(index);
        }

        /**
         * 移除绑定
         */
        bool RemoveBinding(const PhysicalInput& physical)
        {
            int index;

            if (!physical_to_binding_index.Get(physical,index))
                return false;

            bindings.Delete(index);
            physical_to_binding_index.DeleteByKey(physical);

            return true;
        }

        /**
         * 清空所有绑定
         */
        void Clear()
        {
            bindings.Clear();
            physical_to_binding_index.Clear();
        }

        const ValueArray<InputBinding>& GetBindings() const { return bindings; }
    };

    /**
     * 输入配置助手
     */
    class InputConfig
    {
    public:
        /**
         * FPS惯用配置模板
         * - WSAD / 方向键：前后左右
         * - 鼠标左键：射击
         * - 鼠标右键：瞄准
         * - 空格：跳跃
         * - 左Shift：奔跑
         * - 左Ctrl/C：蹲下
         * - Z：趴下
         * - R：换弹
         * - E：交互
         * - V：近战
         * - G：手雷
         * - 1/2/3：切换武器
         */
        static void ApplyFpsTemplate(InputContext& context, const FpsActionConfig& cfg = {})
        {
            context.BindKey(cfg.move_forward, KeyboardButton::W);
            context.BindKey(cfg.move_forward, KeyboardButton::Up);

            context.BindKey(cfg.move_backward, KeyboardButton::S);
            context.BindKey(cfg.move_backward, KeyboardButton::Down);

            context.BindKey(cfg.move_left, KeyboardButton::A);
            context.BindKey(cfg.move_left, KeyboardButton::Left);

            context.BindKey(cfg.move_right, KeyboardButton::D);
            context.BindKey(cfg.move_right, KeyboardButton::Right);

            context.BindMouse(cfg.fire, MouseButton::Left);
            context.BindMouse(cfg.aim, MouseButton::Right);

            context.BindKey(cfg.jump, KeyboardButton::Space);
            context.BindKey(cfg.run, KeyboardButton::LeftShift);

            context.BindKey(cfg.crouch, KeyboardButton::LeftCtrl);
            context.BindKey(cfg.crouch, KeyboardButton::C);
            context.BindKey(cfg.prone, KeyboardButton::Z);

            context.BindKey(cfg.reload, KeyboardButton::R);
            context.BindKey(cfg.use, KeyboardButton::E);
            context.BindKey(cfg.melee, KeyboardButton::V);
            context.BindKey(cfg.grenade, KeyboardButton::G);

            context.BindKey(cfg.weapon1, KeyboardButton::_1);
            context.BindKey(cfg.weapon2, KeyboardButton::_2);
            context.BindKey(cfg.weapon3, KeyboardButton::_3);
        }

        /**
         * COD惯用配置模板
         */
        static void ApplyCodTemplate(InputContext& context, const FpsActionConfig& cfg = {})
        {
            ApplyFpsTemplate(context, cfg);

            context.BindKey(cfg.use, KeyboardButton::F);
            context.BindKey(cfg.melee, KeyboardButton::V);
        }

        /**
         * 战地惯用配置模板
         */
        static void ApplyBattlefieldTemplate(InputContext& context, const FpsActionConfig& cfg = {})
        {
            ApplyFpsTemplate(context, cfg);

            context.BindKey(cfg.use, KeyboardButton::E);
            context.BindKey(cfg.melee, KeyboardButton::F);
        }
    };

    /**
     * 动作状态（跟踪持续时间等）
     */
    struct ActionState
    {
        bool            active;
        ActionValue     current_value;
        double          start_time;
        PhysicalInput   trigger;

        ActionState() : active(false), start_time(0.0) {}
    };

    /**
     * 输入映射器：核心处理器
     * - 接收物理输入事件
     * - 查找当前激活的上下文
     * - 将物理输入转换为逻辑动作事件
     * - 通过回调通知应用层
     */
    class InputMapper : public EventDispatcher
    {
    public:

        using ActionCallback = std::function<void(const ActionEvent&)>;

    private:

        Stack<InputContext*>                context_stack;          ///< 上下文堆栈（栈顶优先）
        UnorderedMap<ActionID, ActionState> action_states;          ///< 当前激活的动作状态
        ActionCallback                      action_callback;        ///< 动作事件回调
        double                              current_time;           ///< 当前时间（秒）

    public:

        InputMapper() : EventDispatcher(InputEventSource::Root), current_time(0.0) {}
        virtual ~InputMapper() = default;

        /**
         * 设置动作事件回调
         */
        void SetActionCallback(ActionCallback callback)
        {
            action_callback = callback;
        }

        /**
         * 推入上下文（压栈，优先级最高）
         */
        void PushContext(InputContext* context)
        {
            if (context)
                context_stack.Push(context);
        }

        /**
         * 弹出上下文（出栈）
         */
        void PopContext()
        {
            if (!context_stack.IsEmpty())
            {
                InputContext* dummy;
                context_stack.Pop(dummy);
            }
        }

        /**
         * 清空所有上下文
         */
        void ClearContexts()
        {
            context_stack.Clear();
        }

        /**
         * 更新时间（每帧调用）
         */
        virtual bool Update() override
        {
            // 可在此处理持续动作的Ongoing事件
            return EventDispatcher::Update();
        }

        /**
         * 处理物理输入事件（重写EventDispatcher）
         */
        virtual EventProcResult OnEvent(const EventHeader& header, const uint64 data) override
        {
            // 尝试将物理输入转换为逻辑动作
            const bool forward_physical = ProcessPhysicalInput(header, data);

            if (!forward_physical)
                return EventProcResult::Break;
            return EventDispatcher::OnEvent(header, data);
        }

        /**
         * 设置当前时间（用于计算elapsed_time）
         */
        void SetCurrentTime(double time)
        {
            current_time = time;
        }


    private:

        /**
         * 在上下文堆栈中查找绑定（从栈顶优先）
         */
        const InputBinding* FindBindingInContextStack(const PhysicalInput& physical) const
        {
            const InputBinding* binding = nullptr;
            context_stack.ForEachFromTop([&](int index, InputContext* ctx)
            {
                if (!binding && ctx)
                    binding = ctx->FindBinding(physical);
            });
            return binding;
        }

        /**
         * 处理物理输入 → 逻辑动作的转换
         */
        bool ProcessPhysicalInput(const EventHeader& header, const uint64 data)
        {
            // 构造物理输入描述
            PhysicalInput physical;
            physical.source = header.type;
            physical.index = header.index;

            bool is_pressed = false;
            bool is_released = false;
            ActionValue value;

            // 根据输入源解析事件
            if (header.type == InputEventSource::Keyboard)
            {
                KeyboardEventData* kd = (KeyboardEventData*)&data;
                physical.code = kd->key;

                if (KeyboardEventID(header.id) == KeyboardEventID::Pressed)
                {
                    is_pressed = true;
                    value = ActionValue::Digital(true);
                }
                else if (KeyboardEventID(header.id) == KeyboardEventID::Released)
                {
                    is_released = true;
                    value = ActionValue::Digital(false);
                }
                else
                    return true; // 忽略Char事件，继续分发物理输入
            }
            else if (header.type == InputEventSource::Mouse)
            {
                MouseEventData* md = (MouseEventData*)&data;
                physical.code = md->button;

                if (MouseAction(header.id) == MouseAction::Pressed)
                {
                    is_pressed = true;
                    value = ActionValue::Digital(true);
                }
                else if (MouseAction(header.id) == MouseAction::Released)
                {
                    is_released = true;
                    value = ActionValue::Digital(false);
                }
                else
                    return true; // 暂不处理Move和Wheel，继续分发物理输入
            }
            else if (header.type == InputEventSource::Joystick)
            {
                // TODO: 解析手柄事件（需要JoystickEventData定义）
                return true;
            }
            else
            {
                return true; // 不支持的输入源，继续分发物理输入
            }

            // 从上下文堆栈查找绑定（栈顶优先）
            const InputBinding* binding = FindBindingInContextStack(physical);

            if (!binding)
                return true; // 未找到绑定，继续分发物理输入

            const bool forward_physical = binding->forward_physical_input;

            // 触发动作事件
            ActionID action_id = binding->action;
            ActionState state;

            if(!action_states.Get(action_id,state))
                return forward_physical;

            ActionEvent evt;
            evt.action = action_id;
            evt.value = value;
            evt.trigger.source = InputEventSource::Logic;
            evt.trigger.index = 0;
            evt.trigger.code = uint16(action_id);

            if (is_pressed)
            {
                if (!state.active)
                {
                    // 新动作开始
                    ActionState new_state;
                    new_state.active = true;
                    new_state.current_value = value;
                    new_state.start_time = current_time;
                    new_state.trigger = evt.trigger;
                    action_states.Add(action_id, new_state);

                    evt.state = ActionEventState::Started;
                    evt.elapsed_time = 0.0;
                }
                else
                {
                    // 动作持续中
                    evt.state = ActionEventState::Ongoing;
                    evt.elapsed_time = current_time - state.start_time;
                }
            }
            else if (is_released)
            {
                if (state.active)
                {
                    // 动作完成
                    evt.state = ActionEventState::Completed;
                    evt.elapsed_time = current_time - state.start_time;

                    action_states.DeleteByKey(action_id);
                }
                else
                {
                        return forward_physical; // 未激活的动作，忽略释放事件
                }
            }

            // 触发回调
            if (action_callback)
                action_callback(evt);

            return forward_physical;
        }
    };

}//namespace hgl::io
