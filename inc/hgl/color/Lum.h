#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    // RGB to Luminance conversion coefficients (ITU-R BT.601)
    constexpr const double LUM_COEFF_R          = 0.299;
    constexpr const double LUM_COEFF_G          = 0.587;
    constexpr const double LUM_COEFF_B          = 0.114;

    // RGB to Luminance conversion coefficients (ITU-R BT.709, HDTV)
    constexpr const double LUM_BT709_COEFF_R    = 0.2126;
    constexpr const double LUM_BT709_COEFF_G    = 0.7152;
    constexpr const double LUM_BT709_COEFF_B    = 0.0722;

    // RGB to Luminance conversion coefficients (ITU-R BT.2020, UHDTV)
    constexpr const double LUM_BT2020_COEFF_R   = 0.2627;
    constexpr const double LUM_BT2020_COEFF_G   = 0.6780;
    constexpr const double LUM_BT2020_COEFF_B   = 0.0593;

    template<typename T> constexpr T RGB2Lum(const T &r,const T &g,const T &b)
    {
        return T(LUM_COEFF_R*r+LUM_COEFF_G*g+LUM_COEFF_B*b);
    }

    template<> constexpr uint8 RGB2Lum(const uint8 &r,const uint8 &g,const uint8 &b)
    {
        return uint8((r*77+g*151+b*28)>>8);
    }

    template<typename T> constexpr T RGB2LumBT709(const T &r, const T &g, const T &b)
    {
        return T(LUM_BT709_COEFF_R*r+LUM_BT709_COEFF_G*g+LUM_BT709_COEFF_B*b);
    }
    
    template<typename T> constexpr T RGB2LumBT2020(const T &r,const T &g,const T &b)
    {
        return T(LUM_BT2020_COEFF_R*r+LUM_BT2020_COEFF_G*g+LUM_BT2020_COEFF_B*b);
    }
}//namespace hgl
