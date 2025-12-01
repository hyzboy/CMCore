#include<hgl/color/HSL.h>
#include<cmath>

namespace hgl
{
    /**
     * @brief Convert RGB color to HSL (Hue, Saturation, Luminosity)
     * @param rgb The RGB color
     * @return HSL color as Vector3f (h, s, l) where h is 0-360, s and l are 0-1
     */
    const Color3f RGB2HSL(const Color3f &rgb)
    {
        float r = rgb.r;
        float g = rgb.g;
        float b = rgb.b;
        
        float max_val = (r > g) ? (r > b ? r : b) : (g > b ? g : b);
        float min_val = (r < g) ? (r < b ? r : b) : (g < b ? g : b);
        float delta = max_val - min_val;
        
        // Luminosity (Lightness)
        float l = (max_val + min_val) / 2.0f;
        
        // Hue and Saturation
        float h = 0.0f;
        float s = 0.0f;
        
        if (delta != 0.0f)
        {
            // Saturation
            s = l < 0.5f ? delta / (max_val + min_val) : delta / (2.0f - max_val - min_val);
            
            // Hue
            if (max_val == r)
            {
                h = std::fmod((g - b) / delta, 6.0f);
            }
            else if (max_val == g)
            {
                h = (b - r) / delta + 2.0f;
            }
            else
            {
                h = (r - g) / delta + 4.0f;
            }
            
            h = h * 60.0f;
            if (h < 0.0f)
                h += 360.0f;
        }
        
        return Color3f(h, s, l);
    }

    /**
     * @brief Convert HSL color back to RGB
     * @param hsl The HSL color as Vector3f (h, s, l) where h is 0-360, s and l are 0-1
     * @return RGB color
     */
    const Color3f HSL2RGB(const Color3f &hsl)
    {
        float h = fmod(hsl.r, 360.0f);
        float s = hsl.g;
        float l = hsl.b;
        
        if (h < 0.0f)
            h += 360.0f;
        
        float c = (1.0f - fabs(2.0f * l - 1.0f)) * s;
        float h_prime = h / 60.0f;
        float x = c * (1.0f - fabs(fmod(h_prime, 2.0f) - 1.0f));
        
        float r = 0.0f, g = 0.0f, b = 0.0f;
        
        if (h_prime >= 0.0f && h_prime < 1.0f)
        {
            r = c; g = x; b = 0.0f;
        }
        else if (h_prime >= 1.0f && h_prime < 2.0f)
        {
            r = x; g = c; b = 0.0f;
        }
        else if (h_prime >= 2.0f && h_prime < 3.0f)
        {
            r = 0.0f; g = c; b = x;
        }
        else if (h_prime >= 3.0f && h_prime < 4.0f)
        {
            r = 0.0f; g = x; b = c;
        }
        else if (h_prime >= 4.0f && h_prime < 5.0f)
        {
            r = x; g = 0.0f; b = c;
        }
        else
        {
            r = c; g = 0.0f; b = x;
        }
        
        float m = l - c / 2.0f;
        return Color3f(r + m, g + m, b + m);
    }
}// namespace hgl
