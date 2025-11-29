#pragma once

#include<hgl/math/Math.h>

namespace hgl
{
    constexpr const double GAMMA      =2.4f;
    constexpr const double INV_GAMMA  =1.0f/GAMMA;
    constexpr const double SRGB_ALPHA =0.055f;

    template<typename T>
    inline constexpr T sRGB2Linear(const T &in)
    {
        if(in<=0.04045f)
            return (double)in/12.92f;
        else
            return pow((double(in)+SRGB_ALPHA)/(1.0f+SRGB_ALPHA), GAMMA);
    }

    template<>
    inline constexpr uint8 sRGB2Linear(const uint8 &in)
    {
        if(in<=0x0A)
            return in/12.92f;
        else
            return pow((double(in)+SRGB_ALPHA)/(1.0f+SRGB_ALPHA), GAMMA);
    }

    template<typename T>
    inline constexpr T sRGB2Linear(const T &in,const double &gamma,const double &srgb_alpha)
    {
        if(in<=0.04045f)
            return (double)in/12.92f;
        else
            return pow((double(in)+srgb_alpha)/(1.0f+srgb_alpha),gamma);
    }

    template<>
    inline constexpr uint8 sRGB2Linear(const uint8 &in,const double &gamma,const double &srgb_alpha)
    {
        if(in<=0x0A)
            return in/12.92f;
        else
            return pow((double(in)+srgb_alpha)/(1.0f+srgb_alpha), gamma);
    }
    
    template<typename T>
    inline constexpr T Linear2sRGB(const T &in)
    {
        if(in<=0.0031308f)
            return double(in)*12.92f;
        else
            return pow(double(in), INV_GAMMA)*(1.0f+SRGB_ALPHA)-SRGB_ALPHA;
    }

    template<>
    inline constexpr uint8 Linear2sRGB(const uint8 &in)
    {
        if(in<=0x03)
            return double(in)*12.92f;
        else
            return pow(double(in), INV_GAMMA)*(1.0f+SRGB_ALPHA)-SRGB_ALPHA;
    }
    
    template<typename T>
    inline constexpr T Linear2sRGB(const T &in,const double &inv_gamma,const double &srgb_alpha)
    {
        if(in<=0.0031308f)
            return double(in)*12.92f;
        else
            return pow(double(in),inv_gamma)*(1.0f+srgb_alpha)-srgb_alpha;
    }

    template<>
    inline constexpr uint8 Linear2sRGB(const uint8 &in,const double &inv_gamma,const double &srgb_alpha)
    {
        if(in<=0x03)
            return double(in)*12.92f;
        else
            return pow(double(in), inv_gamma)*(1.0f+srgb_alpha)-srgb_alpha;
    }

    template<typename T>
    inline constexpr T sRGB2LinearCheaper(const T &in,const double &gamma=GAMMA)
    {
        return (T)pow(double(in),gamma);
    }

    template<typename T>
    inline constexpr T Linear2sRGBCheaper(const T &in,const double &inv_gamma=INV_GAMMA)
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
}//namespace hgl
