#include <hgl/io/event/InputConfig.h>

namespace hgl::io
{
    void InputConfig::ApplyFpsTemplate(InputContext& context, const FpsActionConfig& cfg)
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

    void InputConfig::ApplyCodTemplate(InputContext& context, const FpsActionConfig& cfg)
    {
        ApplyFpsTemplate(context, cfg);

        context.BindKey(cfg.use, KeyboardButton::F);
        context.BindKey(cfg.melee, KeyboardButton::V);
    }

    void InputConfig::ApplyBattlefieldTemplate(InputContext& context, const FpsActionConfig& cfg)
    {
        ApplyFpsTemplate(context, cfg);

        context.BindKey(cfg.use, KeyboardButton::E);
        context.BindKey(cfg.melee, KeyboardButton::F);
    }

    void InputConfig::ApplyModelViewerTemplate(InputContext& context, const ModelViewerActionConfig& cfg)
    {
        context.BindMouse(cfg.rotate, MouseButton::Left);
        context.BindMouse(cfg.pan, MouseButton::Right);

        context.BindKey(cfg.zoom_in, KeyboardButton::PageUp);
        context.BindKey(cfg.zoom_in, KeyboardButton::Equals);
        context.BindKey(cfg.zoom_out, KeyboardButton::PageDown);
        context.BindKey(cfg.zoom_out, KeyboardButton::Minus);

        context.BindKey(cfg.reset_view, KeyboardButton::R);
        context.BindKey(cfg.focus, KeyboardButton::F);
    }
}
