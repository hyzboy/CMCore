#include <hgl/io/event/InputMapper.h>

namespace hgl::io
{
    void InputMapper::PopContext()
    {
        if (!context_stack.IsEmpty())
        {
            InputContext* dummy;
            context_stack.Pop(dummy);
        }
    }

    const InputBinding* InputMapper::FindBindingInContextStack(const PhysicalInput& physical) const
    {
        const InputBinding* binding = nullptr;
        context_stack.ForEachFromTop([&](int index, InputContext* ctx)
        {
            if (!binding && ctx)
                binding = ctx->FindBinding(physical);
        });
        return binding;
    }

    bool InputMapper::ProcessPhysicalInput(const EventHeader& header, const uint64 data)
    {
        PhysicalInput physical;
        physical.source = header.type;
        physical.index = header.index;

        bool is_pressed = false;
        bool is_released = false;
        ActionValue value;

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
                return true; // ignore char, keep forwarding
        }
        else if (header.type == InputEventSource::Mouse)
        {
            MouseEventData* md = (MouseEventData*)&data;
            const MouseAction action = MouseAction(header.id);

            if (action == MouseAction::Wheel)
            {
                physical.code = uint16(MouseButton::None);

                const InputBinding* binding = FindBindingInContextStack(physical);
                if (!binding)
                    return true; // no binding, forward physical

                float analog_value = static_cast<float>(md->y) * binding->scale;
                if (binding->negate)
                    analog_value = -analog_value;

                ActionEvent evt;
                evt.action = binding->action;
                evt.value = ActionValue::Analog1D(analog_value);
                evt.state = ActionEventState::Ongoing;
                evt.elapsed_time = 0.0;
                evt.trigger.source = InputEventSource::Logic;
                evt.trigger.index = 0;
                evt.trigger.code = uint16(binding->action);

                if (action_callback)
                    action_callback(evt);

                return binding->forward_physical_input;
            }

            physical.code = md->button;

            if (action == MouseAction::Pressed)
            {
                is_pressed = true;
                value = ActionValue::Digital(true);
            }
            else if (action == MouseAction::Released)
            {
                is_released = true;
                value = ActionValue::Digital(false);
            }
            else
                return true; // ignore move/dblclick for mapping
        }
        else if (header.type == InputEventSource::Joystick)
        {
            return true; // joystick mapping TODO
        }
        else
        {
            return true; // unsupported source
        }

        const InputBinding* binding = FindBindingInContextStack(physical);
        if (!binding)
            return true; // no binding, forward physical

        const bool forward_physical = binding->forward_physical_input;

        ActionID action_id = binding->action;
        ActionState state;

        if(!action_states.Get(action_id,state))
            state.active = false;

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
                evt.state = ActionEventState::Ongoing;
                evt.elapsed_time = current_time - state.start_time;
            }
        }
        else if (is_released)
        {
            if (state.active)
            {
                evt.state = ActionEventState::Completed;
                evt.elapsed_time = current_time - state.start_time;

                action_states.DeleteByKey(action_id);
            }
            else
            {
                return forward_physical;
            }
        }

        if (action_callback)
            action_callback(evt);

        return forward_physical;
    }
}
