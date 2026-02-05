#pragma once

#include <hgl/type/UnorderedMap.h>
#include <hgl/type/ValueArray.h>
#include <hgl/type/Stack.h>
#include <hgl/io/event/InputTypes.h>

namespace hgl::io
{
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

        const U8String& GetName() const { return name; } // context name

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

        const ValueArray<InputBinding>& GetBindings() const { return bindings; } // bindings
    };
}//namespace hgl::io
