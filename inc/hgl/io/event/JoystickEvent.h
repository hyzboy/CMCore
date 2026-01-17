#pragma once

#include<hgl/type/EnumUtil.h>
namespace hgl::io
{
    /**
    * 手柄按键枚举（通用编号）
    */
    enum class JoystickButton
    {
        NONE=0,

        Select,
        Start,

        Up,
        Down,
        Left,
        Right,

        _0,        _1,        _2,        _3,        _4,        _5,        _6,        _7,
        _8,        _9,        _10,       _11,       _12,       _13,       _14,       _15,
        _16,       _17,       _18,       _19,       _20,       _21,       _22,       _23,
        _24,       _25,       _26,       _27,       _28,       _29,       _30,       _31,

        ENUM_CLASS_RANGE(NONE,_31)
    };//enum class JoystickButton

    // ==================== PlayStation 手柄按键映射 ====================
    namespace PS
    {
        // 面按钮
        constexpr JoystickButton Cross      = JoystickButton::_0;   ///< ✕ 按钮（下）
        constexpr JoystickButton Circle     = JoystickButton::_1;   ///< ○ 按钮（右）
        constexpr JoystickButton Square     = JoystickButton::_2;   ///< □ 按钮（左）
        constexpr JoystickButton Triangle   = JoystickButton::_3;   ///< △ 按钮（上）

        // 肩键
        constexpr JoystickButton L1         = JoystickButton::_4;   ///< 左肩键
        constexpr JoystickButton R1         = JoystickButton::_5;   ///< 右肩键
        constexpr JoystickButton L2         = JoystickButton::_6;   ///< 左扳机
        constexpr JoystickButton R2         = JoystickButton::_7;   ///< 右扳机

        // 摇杆按钮
        constexpr JoystickButton L3         = JoystickButton::_8;   ///< 左摇杆按下
        constexpr JoystickButton R3         = JoystickButton::_9;   ///< 右摇杆按下

        // 系统按钮
        constexpr JoystickButton Share      = JoystickButton::Select;  ///< Share 按钮（PS4）/ Create 按钮（PS5）
        constexpr JoystickButton Options    = JoystickButton::Start;   ///< Options 按钮
        constexpr JoystickButton PS         = JoystickButton::_10;     ///< PS 按钮（主页键）
        constexpr JoystickButton TouchPad   = JoystickButton::_11;     ///< 触摸板按下（PS4/PS5）
        constexpr JoystickButton Mute       = JoystickButton::_12;     ///< 静音按钮（PS5）

        // 方向键（使用通用定义）
        constexpr JoystickButton DPadUp     = JoystickButton::Up;
        constexpr JoystickButton DPadDown   = JoystickButton::Down;
        constexpr JoystickButton DPadLeft   = JoystickButton::Left;
        constexpr JoystickButton DPadRight  = JoystickButton::Right;
    }//namespace PS

    // ==================== Xbox 手柄按键映射 ====================
    namespace Xbox
    {
        // 面按钮
        constexpr JoystickButton A          = JoystickButton::_0;   ///< A 按钮（下）
        constexpr JoystickButton B          = JoystickButton::_1;   ///< B 按钮（右）
        constexpr JoystickButton X          = JoystickButton::_2;   ///< X 按钮（左）
        constexpr JoystickButton Y          = JoystickButton::_3;   ///< Y 按钮（上）

        // 肩键
        constexpr JoystickButton LB         = JoystickButton::_4;   ///< 左肩键（Left Bumper）
        constexpr JoystickButton RB         = JoystickButton::_5;   ///< 右肩键（Right Bumper）
        constexpr JoystickButton LT         = JoystickButton::_6;   ///< 左扳机（Left Trigger）
        constexpr JoystickButton RT         = JoystickButton::_7;   ///< 右扳机（Right Trigger）

        // 摇杆按钮
        constexpr JoystickButton LS         = JoystickButton::_8;   ///< 左摇杆按下（Left Stick）
        constexpr JoystickButton RS         = JoystickButton::_9;   ///< 右摇杆按下（Right Stick）

        // 系统按钮
        constexpr JoystickButton View       = JoystickButton::Select;  ///< View 按钮（原 Back）
        constexpr JoystickButton Menu       = JoystickButton::Start;   ///< Menu 按钮（原 Start）
        constexpr JoystickButton Guide      = JoystickButton::_10;     ///< Xbox 按钮（主页键）
        constexpr JoystickButton Share      = JoystickButton::_11;     ///< Share 按钮（Xbox Series）

        // 方向键（使用通用定义）
        constexpr JoystickButton DPadUp     = JoystickButton::Up;
        constexpr JoystickButton DPadDown   = JoystickButton::Down;
        constexpr JoystickButton DPadLeft   = JoystickButton::Left;
        constexpr JoystickButton DPadRight  = JoystickButton::Right;
    }//namespace Xbox

    // ==================== Nintendo Switch 手柄按键映射 ====================
    namespace Switch
    {
        // 面按钮
        constexpr JoystickButton B          = JoystickButton::_0;   ///< B 按钮（下）
        constexpr JoystickButton A          = JoystickButton::_1;   ///< A 按钮（右）
        constexpr JoystickButton Y          = JoystickButton::_2;   ///< Y 按钮（左）
        constexpr JoystickButton X          = JoystickButton::_3;   ///< X 按钮（上）

        // 肩键
        constexpr JoystickButton L          = JoystickButton::_4;   ///< L 按钮
        constexpr JoystickButton R          = JoystickButton::_5;   ///< R 按钮
        constexpr JoystickButton ZL         = JoystickButton::_6;   ///< ZL 按钮
        constexpr JoystickButton ZR         = JoystickButton::_7;   ///< ZR 按钮

        // 摇杆按钮
        constexpr JoystickButton LStick     = JoystickButton::_8;   ///< 左摇杆按下
        constexpr JoystickButton RStick     = JoystickButton::_9;   ///< 右摇杆按下

        // 系统按钮
        constexpr JoystickButton Minus      = JoystickButton::Select;  ///< - 按钮
        constexpr JoystickButton Plus       = JoystickButton::Start;   ///< + 按钮
        constexpr JoystickButton Home       = JoystickButton::_10;     ///< Home 按钮
        constexpr JoystickButton Capture    = JoystickButton::_11;     ///< 截图按钮

        // 方向键（使用通用定义）
        constexpr JoystickButton DPadUp     = JoystickButton::Up;
        constexpr JoystickButton DPadDown   = JoystickButton::Down;
        constexpr JoystickButton DPadLeft   = JoystickButton::Left;
        constexpr JoystickButton DPadRight  = JoystickButton::Right;
    }//namespace Switch

}//namespace hgl::io
