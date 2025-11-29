#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    template<typename T> constexpr T RGB2Lum(const T &r,const T &g,const T &b)
    {
        return T(0.299f*r+0.587f*g+0.114f*b);
    }

    template<> constexpr uint8 RGB2Lum(const uint8 &r,const uint8 &g,const uint8 &b)
    {
        return uint8((r*77+g*151+b*28)>>8);
    }

    template<typename T> constexpr T RGB2LumBT709(const T &r, const T &g, const T &b)
    {
        return T(0.2126f*r+0.7152f*g+0.0722f*b);
    }
    
    template<typename T> constexpr T RGB2LumBT2020(const T &r,const T &g,const T &b)
    {
        return T(0.2627f*r+0.6780f*g+0.0593f*b);
    }
}//namespace hgl
