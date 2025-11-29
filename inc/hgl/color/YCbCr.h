#ifndef HGL_COLOR_YCbCr_INCLUDE
#define HGL_COLOR_YCbCr_INCLUDE

#include<hgl/math/Math.h>
namespace hgl
{
    template<typename T>
    inline void RGB2YCbCr(T &y,T &cb,T &cr,const T &r,const T &g,const T &b)
    {
        y=0.299f*r+0.587f*g+0.114f*b;
        cb=0.5f-0.168736f*r-0.331264f*g+0.5f*b;
        cr=0.5f+0.5f*r-0.418688f*g-0.081312f*b;
    }

    template<typename T>
    inline void YCbCr2RGB(T &r,T &g,T &b,const T &y,const T &cb,const T &cr)
    {
        r=y+1.402f*(cr-0.5f);
        g=y-0.344136f*(cb-0.5f)-0.714136f*(cr-0.5f);
        b=y+1.772f*(cb-0.5f);
    }

    inline void RGB2YCbCr(uint8 &y,uint8 &cb,uint8 &cr,const uint8 &r,const uint8 &g,const uint8 &b)
    {
        y =uint8(Clamp(0.299f*r+0.587f*g+0.114f*b));
        cb=uint8(Clamp(0.5f-0.168736f*r-0.331264f*g+0.5f*b));
        cr=uint8(Clamp(0.5f+0.5f*r-0.418688f*g-0.081312f*b));
    }
    
    inline void YCbCr2RGB(uint8 &r,uint8 &g,uint8 &b,const uint8 &y,const uint8 &cb,const uint8 &cr)
    {
        r=uint8(Clamp(y+1.402f*(cr-0.5f)));
        g=uint8(Clamp(y-0.344136f*(cb-0.5f)-0.714136f*(cr-0.5f)));
        b=uint8(Clamp(y+1.772f*(cb-0.5f)));
    }

    template<typename T>
    inline constexpr T RGB2Cb(const T &r, const T &g, const T &b)
    {
        return 0.5f-0.168736f*r-0.331264f*g+0.5f*b;
    }
    
    template<>
    inline constexpr uint8 RGB2Cb(const uint8 &r, const uint8 &g, const uint8 &b)
    {
        return uint8(Clamp(0.5f-0.168736f*r-0.331264f*g+0.5f*b));
    }
    
    template<typename T>
    inline constexpr T RGB2Cr(const T &r, const T &g, const T &b)
    {
        return 0.5f+0.5f*r-0.418688f*g-0.081312f*b;
    }

    template<>
    inline constexpr uint8 RGB2Cr(const uint8 &r, const uint8 &g, const uint8 &b)
    {
        return uint8(Clamp(0.5f+0.5f*r-0.418688f*g-0.081312f*b));
    }
}//namespace hgl
#endif//HGL_COLOR_YCbCr_INCLUDE
    