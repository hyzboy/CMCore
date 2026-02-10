#include <hgl/io/event/InputConfig.h>

namespace hgl::io
{
    void InputConfig::ApplyFpsTemplate(InputContext& context)
    {
        FpsActionConfig cfg;
        ApplyFpsTemplate(context, cfg);
    }

    void InputConfig::ApplyFpsTemplate(InputContext& context, const FpsActionConfig& cfg)
    {
        context.BindKey(cfg[FpsAction::MoveForward], KeyboardButton::W);
        context.BindKey(cfg[FpsAction::MoveForward], KeyboardButton::Up);

        context.BindKey(cfg[FpsAction::MoveBackward], KeyboardButton::S);
        context.BindKey(cfg[FpsAction::MoveBackward], KeyboardButton::Down);

        context.BindKey(cfg[FpsAction::MoveLeft], KeyboardButton::A);
        context.BindKey(cfg[FpsAction::MoveLeft], KeyboardButton::Left);

        context.BindKey(cfg[FpsAction::MoveRight], KeyboardButton::D);
        context.BindKey(cfg[FpsAction::MoveRight], KeyboardButton::Right);

        context.BindMouse(cfg[FpsAction::Fire], MouseButton::Left);
        context.BindMouse(cfg[FpsAction::Aim], MouseButton::Right);

        context.BindKey(cfg[FpsAction::Jump], KeyboardButton::Space);
        context.BindKey(cfg[FpsAction::Run], KeyboardButton::LeftShift);

        context.BindKey(cfg[FpsAction::Crouch], KeyboardButton::LeftCtrl);
        context.BindKey(cfg[FpsAction::Crouch], KeyboardButton::C);
        context.BindKey(cfg[FpsAction::Prone], KeyboardButton::Z);

        context.BindKey(cfg[FpsAction::Reload], KeyboardButton::R);
        context.BindKey(cfg[FpsAction::Use], KeyboardButton::E);
        context.BindKey(cfg[FpsAction::Melee], KeyboardButton::V);
        context.BindKey(cfg[FpsAction::Grenade], KeyboardButton::G);

        context.BindKey(cfg[FpsAction::Weapon1], KeyboardButton::_1);
        context.BindKey(cfg[FpsAction::Weapon2], KeyboardButton::_2);
        context.BindKey(cfg[FpsAction::Weapon3], KeyboardButton::_3);
    }

    void InputConfig::ApplyCodTemplate(InputContext& context, const FpsActionConfig& cfg)
    {
        ApplyFpsTemplate(context, cfg);

        context.BindKey(cfg[FpsAction::Use], KeyboardButton::F);
        context.BindKey(cfg[FpsAction::Melee], KeyboardButton::V);
    }

    void InputConfig::ApplyBattlefieldTemplate(InputContext& context, const FpsActionConfig& cfg)
    {
        ApplyFpsTemplate(context, cfg);

        context.BindKey(cfg[FpsAction::Use], KeyboardButton::E);
        context.BindKey(cfg[FpsAction::Melee], KeyboardButton::F);
    }

    void InputConfig::ApplyModelViewerTemplate(InputContext& context, const ModelViewerActionConfig& cfg)
    {
        context.BindMouse(cfg[ModelViewerAction::Rotate], MouseButton::Left);
        context.BindMouse(cfg[ModelViewerAction::Pan], MouseButton::Right);

        context.BindKey(cfg[ModelViewerAction::ZoomIn], KeyboardButton::PageUp);
        context.BindKey(cfg[ModelViewerAction::ZoomIn], KeyboardButton::Equals);
        context.BindKey(cfg[ModelViewerAction::ZoomOut], KeyboardButton::PageDown);
        context.BindKey(cfg[ModelViewerAction::ZoomOut], KeyboardButton::Minus);

        context.BindKey(cfg[ModelViewerAction::ResetView], KeyboardButton::R);
        context.BindKey(cfg[ModelViewerAction::Focus], KeyboardButton::F);
    }

    void InputConfig::ApplyCodTemplate(InputContext& context)
    {
        FpsActionConfig cfg;
        ApplyCodTemplate(context, cfg);
    }

    void InputConfig::ApplyBattlefieldTemplate(InputContext& context)
    {
        FpsActionConfig cfg;
        ApplyBattlefieldTemplate(context, cfg);
    }

    void InputConfig::ApplyModelViewerTemplate(InputContext& context)
    {
        ModelViewerActionConfig cfg;
        ApplyModelViewerTemplate(context, cfg);
    }
}
