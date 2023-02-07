#ifndef HGL_COLOR_LUM_INCLUDE
#define HGL_COLOR_LUM_INCLUDE

#include<hgl/platform/Platform.h>

namespace hgl
{
    template<typename T> constexpr T RGB2Lum(const T &r, const T &g, const T &b)
    {
        return 0.2126f*r+0.7152f*g+0.0722f*b;
    }
    
    template<> constexpr uint8 RGB2Lum(const uint8 &r, const uint8 &g, const uint8 &b)
    {
        return (r*54+g*183+b*19)>>8;
    }
}//namespace hgl
#endif//HGL_COLOR_LUM_INCLUDE
