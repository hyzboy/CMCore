#include<hgl/color/BlendColor.h>
#include<hgl/color/HSL.h>

namespace hgl
{
    const Color3f BlendColor(BlendColorMode blend_mode, const Color3f &src, const Color3f &dst, const float alpha)
    {
        // Helper function for channel blending
        auto BlendChannel = [](float src_c, float dst_c, float a, BlendColorMode mode) -> float
        {
            float blended;
            
            switch(mode)
            {
                case BlendColorMode::None:
                    blended = dst_c;
                    break;
                    
                case BlendColorMode::Alpha:
                    // Standard alpha blending: src * alpha + dst * (1 - alpha)
                    blended = src_c * a + dst_c * (1.0f - a);
                    break;
                    
                case BlendColorMode::Add:
                case BlendColorMode::LinearDodge:
                    // Additive blending: src + dst
                    blended = src_c + dst_c;
                    break;
                    
                case BlendColorMode::Subtract:
                    // Subtractive blending: dst - src
                    blended = dst_c - src_c;
                    break;
                    
                case BlendColorMode::Multiply:
                    // Multiplicative blending: src * dst
                    blended = src_c * dst_c;
                    break;
                    
                case BlendColorMode::Screen:
                    // Screen blending: 1 - (1 - src) * (1 - dst)
                    blended = 1.0f - (1.0f - src_c) * (1.0f - dst_c);
                    break;
                    
                case BlendColorMode::Overlay:
                    // Overlay blending: combine multiply and screen based on dst value
                    if (dst_c < 0.5f)
                    {
                        blended = 2.0f * src_c * dst_c;
                    }
                    else
                    {
                        blended = 1.0f - 2.0f * (1.0f - src_c) * (1.0f - dst_c);
                    }
                    break;
                    
                case BlendColorMode::Darken:
                    // Darken: select the darker color
                    blended = src_c < dst_c ? src_c : dst_c;
                    break;
                    
                case BlendColorMode::Lighten:
                    // Lighten: select the lighter color
                    blended = src_c > dst_c ? src_c : dst_c;
                    break;
                    
                case BlendColorMode::ColorDodge:
                    // Color dodge: src / (1 - dst)
                    if (dst_c < 1.0f)
                    {
                        blended = src_c / (1.0f - dst_c);
                    }
                    else
                    {
                        blended = 1.0f;
                    }
                    break;
                    
                case BlendColorMode::ColorBurn:
                    // Color burn: 1 - (1 - src) / dst
                    if (dst_c > 0.0f)
                    {
                        blended = 1.0f - (1.0f - src_c) / dst_c;
                    }
                    else
                    {
                        blended = 0.0f;
                    }
                    break;
                    
                case BlendColorMode::HardLight:
                    // Hard light: like overlay but based on src
                    if (src_c < 0.5f)
                    {
                        blended = 2.0f * src_c * dst_c;
                    }
                    else
                    {
                        blended = 1.0f - 2.0f * (1.0f - src_c) * (1.0f - dst_c);
                    }
                    break;
                    
                case BlendColorMode::SoftLight:
                    // Soft light: subtle version of overlay
                    if (dst_c < 0.5f)
                    {
                        blended = 2.0f * src_c * dst_c + src_c * src_c * (1.0f - 2.0f * dst_c);
                    }
                    else
                    {
                        blended = 2.0f * src_c * (1.0f - dst_c) + (2.0f * dst_c - 1.0f) * (1.0f - (1.0f - src_c) * (1.0f - src_c));
                    }
                    break;
                    
                case BlendColorMode::Difference:
                    // Difference: |src - dst|
                    blended = src_c > dst_c ? src_c - dst_c : dst_c - src_c;
                    break;
                    
                case BlendColorMode::Exclusion:
                    // Exclusion: src + dst - 2 * src * dst
                    blended = src_c + dst_c - 2.0f * src_c * dst_c;
                    break;
                    
                case BlendColorMode::LinearBurn:
                    // Linear burn: dst + src - 1
                    blended = dst_c + src_c - 1.0f;
                    break;
                    
                case BlendColorMode::VividLight:
                    // Vivid light: ColorDodge if src > 0.5, else ColorBurn
                    if (src_c < 0.5f)
                    {
                        // Color burn branch
                        if (dst_c > 0.0f)
                        {
                            blended = 1.0f - (1.0f - 2.0f * src_c) / (2.0f * dst_c);
                        }
                        else
                        {
                            blended = 0.0f;
                        }
                    }
                    else
                    {
                        // Color dodge branch
                        if (dst_c < 1.0f)
                        {
                            blended = 2.0f * src_c - 1.0f / (1.0f - dst_c);
                        }
                        else
                        {
                            blended = 1.0f;
                        }
                    }
                    break;
                    
                case BlendColorMode::LinearLight:
                    // Linear light: LinearDodge if src > 0.5, else LinearBurn
                    if (src_c < 0.5f)
                    {
                        blended = dst_c + 2.0f * src_c - 1.0f;
                    }
                    else
                    {
                        blended = dst_c + 2.0f * (src_c - 0.5f);
                    }
                    break;
                    
                case BlendColorMode::PinLight:
                    // Pin light: Lighten if src > 0.5, else Darken
                    if (src_c < 0.5f)
                    {
                        blended = src_c < dst_c ? src_c : dst_c;
                    }
                    else
                    {
                        blended = src_c > dst_c ? src_c : dst_c;
                    }
                    break;
                    
                case BlendColorMode::HardMix:
                    // Hard mix: posterize to 0 or 1
                    if (src_c < 0.5f)
                    {
                        // Color burn
                        if (dst_c > 0.0f)
                        {
                            blended = (1.0f - (1.0f - src_c) / dst_c) < 0.5f ? 0.0f : 1.0f;
                        }
                        else
                        {
                            blended = 0.0f;
                        }
                    }
                    else
                    {
                        // Color dodge
                        if (dst_c < 1.0f)
                        {
                            blended = (src_c / (1.0f - dst_c)) < 0.5f ? 0.0f : 1.0f;
                        }
                        else
                        {
                            blended = 1.0f;
                        }
                    }
                    break;
                    
                case BlendColorMode::Divide:
                    // Divide: dst / src
                    if (src_c > 0.0f)
                    {
                        blended = dst_c / src_c;
                    }
                    else
                    {
                        blended = 1.0f;
                    }
                    break;
                    
                default:
                    blended = dst_c;
                    break;
            }
            
            // Clamp the result to [0, 1]
            return blended < 0.0f ? 0.0f : (blended > 1.0f ? 1.0f : blended);
        };
        
        // For luminance-based modes that need special handling
        if (blend_mode == BlendColorMode::DarkerColor)
        {
            return GetLuminance(src) < GetLuminance(dst) ? src : dst;
        }
        else if (blend_mode == BlendColorMode::LighterColor)
        {
            return GetLuminance(src) > GetLuminance(dst) ? src : dst;
        }
        else if (blend_mode == BlendColorMode::Hue)
        {
            Color3f src_hsl = RGB2HSL(src);
            Color3f dst_hsl = RGB2HSL(dst);
            Color3f result_hsl(src_hsl.r, dst_hsl.g, dst_hsl.b);
            Color3f result = HSL2RGB(result_hsl);
            return BlendColor(BlendColorMode::Alpha, result, dst, alpha);
        }
        else if (blend_mode == BlendColorMode::Saturation)
        {
            Color3f src_hsl = RGB2HSL(src);
            Color3f dst_hsl = RGB2HSL(dst);
            Color3f result_hsl(dst_hsl.r, src_hsl.g, dst_hsl.b);
            Color3f result = HSL2RGB(result_hsl);
            return BlendColor(BlendColorMode::Alpha, result, dst, alpha);
        }
        else if (blend_mode == BlendColorMode::Color)
        {
            Color3f src_hsl = RGB2HSL(src);
            Color3f dst_hsl = RGB2HSL(dst);
            Color3f result_hsl(src_hsl.r, src_hsl.g, dst_hsl.b);
            Color3f result = HSL2RGB(result_hsl);
            return BlendColor(BlendColorMode::Alpha, result, dst, alpha);
        }
        else if (blend_mode == BlendColorMode::Luminosity)
        {
            Color3f src_hsl = RGB2HSL(src);
            Color3f dst_hsl = RGB2HSL(dst);
            Color3f result_hsl(dst_hsl.r, dst_hsl.g, src_hsl.b);
            Color3f result = HSL2RGB(result_hsl);
            return BlendColor(BlendColorMode::Alpha, result, dst, alpha);
        }
        
        // Apply blending to each channel for standard modes
        float r = BlendChannel(src.r, dst.r, alpha, blend_mode);
        float g = BlendChannel(src.g, dst.g, alpha, blend_mode);
        float b = BlendChannel(src.b, dst.b, alpha, blend_mode);
        
        return Color3f(r, g, b);
    }
}//namespace hgl
