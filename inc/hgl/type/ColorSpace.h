#ifndef HGL_COLOR_SPACE_INCLUDE
#define HGL_COLOR_SPACE_INCLUDE

#include<math.h>

namespace hgl
{
    enum class ColorSpace
    {
        Linear=0,
        sRGB,
        YCbCr,

        ENUM_CLASS_RANGE(Linear,YCbCr)
    };//enum class ColorSpace

    constexpr double GAMMA      =2.4f;
    constexpr double INV_GAMMA  =1.0f/GAMMA;
    constexpr double SRGB_ALPHA =0.055f;

    template<typename T>
    inline constexpr T sRGB2Linear(const T &in,const T &gamma=GAMMA,const T &srgb_alpha=SRGB_ALPHA)
    {
        if(in<=0.4045)
            return (double)in/12.92;
        else
            return pow((double(in)+srgb_alpha)/(1.0f+srgb_alpha),gamma);
    }

    template<typename T>
    inline constexpr T Linear2sRGB(const T &in,const T &inv_gamma=INV_GAMMA,const T &srgb_alpha=SRGB_ALPHA)
    {
        if(in<=0.0031308f)
            return double(in)*12.92f;
        else
            return pow(double(in),inv_gamma)*(1.0f+srgb_alpha)-srgb_alpha;
    }

    template<typename T>
    inline constexpr T sRGB2LinearCheaper(const T &in,const T &gamma=GAMMA)
    {
        return (T)pow(double(in),gamma);
    }

    template<typename T>
    inline constexpr T Linear2sRGBCheaper(const T &in,const T &inv_gamma=INV_GAMMA)
    {
        return (T)pow((double)in,inv_gamma);
    }

    template<typename T>
    inline constexpr T sRGB2LinearCheapest(const T &in)
    {
        return in*in;
    }

    template<typename T>
    inline void sRGB2LinearFast(T &x,T &y,T &z,const T &r,const T &g,const T &b)
    {
        x=0.4124f*r+0.3576f*g+0.1805f*b;
        y=0.2126f*r+0.7152f*g+0.0722f*b;
        z=0.0193f*r+0.1192f*g+0.9505f*b;
    }

    template<typename T>
    inline void Linear2sRGBFast(T &r,T &g,T &b,const T &x,const T &y,const T &z)
    {
        r= 3.2406f*x-1.5373f*y-0.4986f*z;
        g=-0.9689f*x+1.8758f*y+0.0416f*z;
        b= 0.0557f*x-0.2040f*y+1.0570f*z;
    }

    template<typename T>
    inline constexpr T Clamp(const T &value,const T &min_value,const T &max_value)
    {
        if(value<min_value)return min_value;
        if(value>max_value)return max_value;

        return value;
    }

    template<typename T>
    inline constexpr T Clamp(const T &value)
    {
        return Clamp<T>(value,T(0),T(1));
    }

    template<typename T>
    inline constexpr T RGB2Lum(const T &r,const T &g,const T &b)
    {
        return 0.299f*r+0.587f*g+0.114f*b;
    }

    template<typename T>
    inline constexpr T RGB2Cb(const T &r,const T &g,const T &b)
    {
        return -0.168736f*r-0.331264f*g+0.5f*b;
    }
    
    template<typename T>
    inline constexpr T RGB2Cr(const T &r,const T &g,const T &b)
    {
        return 0.5f*r-0.418688f*g-0.081312f*b;
    }

    template<typename T>
    inline void RGB2YCbCr(T &y,T &cb,T &cr,const T &r,const T &g,const T &b)
    {
        y =RGB2Lum(r,g,b);
        cb=RGB2Cb(r,g,b);
        cr=RGB2Cr(r,g,b);
    }
}//namespace hgl
#endif//HGL_COLOR_SPACE_INCLUDE
