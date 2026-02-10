#pragma once

#include <hgl/io/event/InputContext.h>

namespace hgl::io
{
    /**
     * 模型观察（模型居中）配置模板
     */
    struct ModelViewerActionConfig
    {
        ActionID rotate = 1001;      // 旋转
        ActionID pan = 1002;         // 平移
        ActionID zoom_in = 1003;     // 放大
        ActionID zoom_out = 1004;    // 缩小
        ActionID reset_view = 1005;  // 重置视角
        ActionID focus = 1006;       // 视角聚焦
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
        static void ApplyFpsTemplate(InputContext& context, const FpsActionConfig& cfg = {});

        /**
         * COD惯用配置模板
         */
        static void ApplyCodTemplate(InputContext& context, const FpsActionConfig& cfg = {});

        /**
         * 战地惯用配置模板
         */
        static void ApplyBattlefieldTemplate(InputContext& context, const FpsActionConfig& cfg = {});

        /**
         * 模型观察惯用配置模板
         */
        static void ApplyModelViewerTemplate(InputContext& context, const ModelViewerActionConfig& cfg = {});
    };
}//namespace hgl::io
