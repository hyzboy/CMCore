#pragma once

#include <functional>
#include <hgl/io/event/EventDispatcher.h>
#include <hgl/io/event/InputContext.h>

namespace hgl::io
{
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
     * 输入映射器：核心处理器（仅转换器）
     * - 接收物理输入事件
     * - 查找当前激活的上下文
     * - 将物理输入转换为逻辑动作事件
     * - 通过回调通知应用层
     */
    class InputMapper
    {
    public:

        using ActionCallback = std::function<void(const ActionEvent&)>;

    private:

        Stack<InputContext*>                context_stack;          ///< 上下文堆栈（栈顶优先）
        UnorderedMap<ActionID, ActionState> action_states;          ///< 当前激活的动作状态
        ActionCallback                      action_callback;        ///< 动作事件回调
        double                              current_time;           ///< 当前时间（秒）

    public:

        InputMapper() : current_time(0.0) {}
        virtual ~InputMapper() = default;

        void SetActionCallback(ActionCallback callback){ action_callback = callback; } // action callback

        void PushContext(InputContext* context){ if (context) context_stack.Push(context); } // push context

        void PopContext();
        void ClearContexts(){ context_stack.Clear(); } // clear all contexts

        void SetCurrentTime(double time){ current_time = time; } // current time (sec)

        /**
         * 处理物理输入 → 逻辑动作的转换
         * @return 是否继续分发物理输入
         */
        bool ProcessPhysicalInput(const EventHeader& header, const uint64 data);

    private:

        const InputBinding* FindBindingInContextStack(const PhysicalInput& physical) const;
    };
}//namespace hgl::io
