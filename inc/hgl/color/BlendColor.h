#pragma once

#include<hgl/color/Color3ub.h>
#include<hgl/color/Color3f.h>

namespace hgl
{
    enum class BlendColorMode
    {
        None=0,
        Alpha,
        Add,
        Subtract,
        Multiply,
        Screen,
        Overlay,
        Darken,
        Lighten,
        ColorDodge,
        ColorBurn,
        HardLight,
        SoftLight,
        Difference,
        Exclusion,
        
        // P0 - High Priority (commonly used, significant visual differences)
        LinearBurn,         // dst + src - 1
        LinearDodge,        // dst + src (same as Add)
        VividLight,         // ColorDodge + ColorBurn based on src brightness
        LinearLight,        // LinearDodge + LinearBurn based on src brightness
        PinLight,           // Darken + Lighten based on src brightness
        HardMix,            // Posterize to 0 or 1
        DarkerColor,        // Select color with lower luminance
        LighterColor,       // Select color with higher luminance
        
        // P1 - Medium Priority (HSL-based, need RGB↔HSL conversion)
        Hue,                // Use source hue with destination saturation and luminosity
        Saturation,         // Use source saturation with destination hue and luminosity
        Color,              // Use source hue and saturation with destination luminosity
        Luminosity,         // Use source luminosity with destination hue and saturation
        
        // P2 - Low Priority (special/uncommon)
        Divide,             // dst / src
    };


    /**
     * @brief Calculate luminance (brightness) of a color
     * @param color The RGB color
     * @return Luminance value (0.0 to 1.0)
     */
    constexpr inline float GetLuminance(const Color3f &color)
    {
        // Using standard luminance formula: 0.299*R + 0.587*G + 0.114*B
        return 0.299f * color.r + 0.587f * color.g + 0.114f * color.b;
    }

    /**
     * @brief Blend two colors based on blend mode
     * @param blend_mode The color blending mode
     * @param src The source color (foreground)
     * @param dst The destination color (background)
     * @param alpha The alpha value of source color (0.0 to 1.0)
     * @return Blended color
     * 
     * Color blending modes:
     * - None: Returns destination color unchanged
     * - Alpha: Standard alpha blending (src over dst)
     * - Add: Additive blending (src + dst)
     * - Subtract: Subtractive blending (dst - src)
     * - Multiply: Multiplicative blending (src * dst)
     * - Screen: Screen blending (inverse multiply)
     * - Overlay: Overlay blending (combines multiply and screen)
     * - Darken: Selects darker of two colors
     * - Lighten: Selects lighter of two colors
     * - ColorDodge: Dodge blending mode
     * - ColorBurn: Burn blending mode
     * - HardLight: Hard light blending (based on source brightness)
     * - SoftLight: Soft light blending (subtle version of overlay)
     * - Difference: Difference blending
     * - Exclusion: Exclusion blending (similar to difference but lower contrast)
     * - LinearBurn: dst + src - 1
     * - LinearDodge: dst + src
     * - VividLight: ColorDodge + ColorBurn based on src brightness
     * - LinearLight: LinearDodge + LinearBurn based on src brightness
     * - PinLight: Darken + Lighten based on src brightness
     * - HardMix: Posterize to 0 or 1
     * - DarkerColor: Select color with lower luminance
     * - LighterColor: Select color with higher luminance
     * - Hue: Use source hue with destination saturation and luminosity
     * - Saturation: Use source saturation with destination hue and luminosity
     * - Color: Use source hue and saturation with destination luminosity
     * - Luminosity: Use source luminosity with destination hue and saturation
     * - Divide: dst / src
     */
    const Color3f BlendColor(BlendColorMode blend_mode, const Color3f &src, const Color3f &dst, const float alpha);
}// namespace hgl
