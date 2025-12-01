#pragma once

#include<hgl/color/Color3f.h>

namespace hgl
{
    /**
     * @brief Convert RGB color to HSL (Hue, Saturation, Luminosity)
     * @param rgb The RGB color
     * @return HSL color as Vector3f (h, s, l) where h is 0-360, s and l are 0-1
     */
    const Color3f RGB2HSL(const Color3f &rgb);

    /**
     * @brief Convert HSL color back to RGB
     * @param hsl The HSL color as Vector3f (h, s, l) where h is 0-360, s and l are 0-1
     * @return RGB color
     */
    const Color3f HSL2RGB(const Color3f &hsl);
}// namespace hgl
