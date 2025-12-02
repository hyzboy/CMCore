#pragma once

#include<hgl/math/Clamp.h>

namespace hgl
{
    // YCbCr conversion coefficients (ITU-R BT.601)
    constexpr const double YCBCR_Y_R        = 0.299;
    constexpr const double YCBCR_Y_G        = 0.587;
    constexpr const double YCBCR_Y_B        = 0.114;

    constexpr const double YCBCR_CB_R       = 0.168736;
    constexpr const double YCBCR_CB_G       = 0.331264;
    constexpr const double YCBCR_CB_B       = 0.5;

    constexpr const double YCBCR_CR_R       = 0.5;
    constexpr const double YCBCR_CR_G       = 0.418688;
    constexpr const double YCBCR_CR_B       = 0.081312;

    constexpr const double YCBCR_OFFSET     = 0.5;

    // YCbCr to RGB conversion coefficients
    constexpr const double YCBCR2RGB_CR     = 1.402;
    constexpr const double YCBCR2RGB_CB_G   = 0.344136;
    constexpr const double YCBCR2RGB_CR_G   = 0.714136;
    constexpr const double YCBCR2RGB_CB     = 1.772;

    template<typename T>
    inline void RGB2YCbCr(T &y,T &cb,T &cr,const T &r,const T &g,const T &b)
    {
        y=YCBCR_Y_R*r+YCBCR_Y_G*g+YCBCR_Y_B*b;
        cb=YCBCR_OFFSET-YCBCR_CB_R*r-YCBCR_CB_G*g+YCBCR_CB_B*b;
        cr=YCBCR_OFFSET+YCBCR_CR_R*r-YCBCR_CR_G*g-YCBCR_CR_B*b;
    }

    template<typename T>
    inline void YCbCr2RGB(T &r,T &g,T &b,const T &y,const T &cb,const T &cr)
    {
        r=y+YCBCR2RGB_CR*(cr-YCBCR_OFFSET);
        g=y-YCBCR2RGB_CB_G*(cb-YCBCR_OFFSET)-YCBCR2RGB_CR_G*(cr-YCBCR_OFFSET);
        b=y+YCBCR2RGB_CB*(cb-YCBCR_OFFSET);
    }

    inline void RGB2YCbCr(uint8 &y,uint8 &cb,uint8 &cr,const uint8 &r,const uint8 &g,const uint8 &b)
    {
        y =uint8(Clamp(YCBCR_Y_R*r+YCBCR_Y_G*g+YCBCR_Y_B*b));
        cb=uint8(Clamp(YCBCR_OFFSET-YCBCR_CB_R*r-YCBCR_CB_G*g+YCBCR_CB_B*b));
        cr=uint8(Clamp(YCBCR_OFFSET+YCBCR_CR_R*r-YCBCR_CR_G*g-YCBCR_CR_B*b));
    }
    
    inline void YCbCr2RGB(uint8 &r,uint8 &g,uint8 &b,const uint8 &y,const uint8 &cb,const uint8 &cr)
    {
        r=uint8(Clamp(y+YCBCR2RGB_CR*(cr-YCBCR_OFFSET)));
        g=uint8(Clamp(y-YCBCR2RGB_CB_G*(cb-YCBCR_OFFSET)-YCBCR2RGB_CR_G*(cr-YCBCR_OFFSET)));
        b=uint8(Clamp(y+YCBCR2RGB_CB*(cb-YCBCR_OFFSET)));
    }

    template<typename T>
    constexpr T RGB2Cb(const T &r, const T &g, const T &b)
    {
        return YCBCR_OFFSET-YCBCR_CB_R*r-YCBCR_CB_G*g+YCBCR_CB_B*b;
    }
    
    template<>
    constexpr uint8 RGB2Cb(const uint8 &r, const uint8 &g, const uint8 &b)
    {
        return uint8(Clamp(YCBCR_OFFSET-YCBCR_CB_R*r-YCBCR_CB_G*g+YCBCR_CB_B*b));
    }
    
    template<typename T>
    constexpr T RGB2Cr(const T &r, const T &g, const T &b)
    {
        return YCBCR_OFFSET+YCBCR_CR_R*r-YCBCR_CR_G*g-YCBCR_CR_B*b;
    }

    template<>
    constexpr uint8 RGB2Cr(const uint8 &r, const uint8 &g, const uint8 &b)
    {
        return uint8(Clamp(YCBCR_OFFSET+YCBCR_CR_R*r-YCBCR_CR_G*g-YCBCR_CR_B*b));
    }
}//namespace hgl
