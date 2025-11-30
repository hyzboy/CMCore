#pragma once

#include<hgl/math/Math.h>

namespace hgl
{
    // Gamma correction constants
    constexpr const double GAMMA      =2.4;
    constexpr const double INV_GAMMA  =1.0/GAMMA;
    constexpr const double SRGB_ALPHA =0.055;

    // sRGB threshold values
    constexpr const double SRGB_LINEAR_THRESHOLD    = 0.04045;
    constexpr const double SRGB_LINEAR_DIVISOR      = 12.92;
    constexpr const double LINEAR_SRGB_THRESHOLD    = 0.0031308;

    // sRGB to XYZ (D65) conversion matrix coefficients
    constexpr const double SRGB2XYZ_X_R = 0.4124;
    constexpr const double SRGB2XYZ_X_G = 0.3576;
    constexpr const double SRGB2XYZ_X_B = 0.1805;

    constexpr const double SRGB2XYZ_Y_R = 0.2126;
    constexpr const double SRGB2XYZ_Y_G = 0.7152;
    constexpr const double SRGB2XYZ_Y_B = 0.0722;

    constexpr const double SRGB2XYZ_Z_R = 0.0193;
    constexpr const double SRGB2XYZ_Z_G = 0.1192;
    constexpr const double SRGB2XYZ_Z_B = 0.9505;

    // XYZ to sRGB (D65) conversion matrix coefficients
    constexpr const double XYZ2SRGB_R_X =  3.2406;
    constexpr const double XYZ2SRGB_R_Y = -1.5373;
    constexpr const double XYZ2SRGB_R_Z = -0.4986;

    constexpr const double XYZ2SRGB_G_X = -0.9689;
    constexpr const double XYZ2SRGB_G_Y =  1.8758;
    constexpr const double XYZ2SRGB_G_Z =  0.0416;

    constexpr const double XYZ2SRGB_B_X =  0.0557;
    constexpr const double XYZ2SRGB_B_Y = -0.2040;
    constexpr const double XYZ2SRGB_B_Z =  1.0570;

    template<typename T>
    constexpr T sRGB2Linear(const T &in)
    {
        if(in<=SRGB_LINEAR_THRESHOLD)
            return (double)in/SRGB_LINEAR_DIVISOR;
        else
            return pow((double(in)+SRGB_ALPHA)/(1.0+SRGB_ALPHA), GAMMA);
    }

    template<>
    constexpr uint8 sRGB2Linear(const uint8 &in)
    {
        if(in<=0x0A)
            return in/SRGB_LINEAR_DIVISOR;
        else
            return pow((double(in)+SRGB_ALPHA)/(1.0+SRGB_ALPHA), GAMMA);
    }

    template<typename T>
    constexpr T sRGB2Linear(const T &in,const double &gamma,const double &srgb_alpha)
    {
        if(in<=SRGB_LINEAR_THRESHOLD)
            return (double)in/SRGB_LINEAR_DIVISOR;
        else
            return pow((double(in)+srgb_alpha)/(1.0+srgb_alpha),gamma);
    }

    template<>
    constexpr uint8 sRGB2Linear(const uint8 &in,const double &gamma,const double &srgb_alpha)
    {
        if(in<=0x0A)
            return in/SRGB_LINEAR_DIVISOR;
        else
            return pow((double(in)+srgb_alpha)/(1.0+srgb_alpha), gamma);
    }
    
    template<typename T>
    constexpr T Linear2sRGB(const T &in)
    {
        if(in<=LINEAR_SRGB_THRESHOLD)
            return double(in)*SRGB_LINEAR_DIVISOR;
        else
            return pow(double(in), INV_GAMMA)*(1.0+SRGB_ALPHA)-SRGB_ALPHA;
    }

    template<>
    constexpr uint8 Linear2sRGB(const uint8 &in)
    {
        if(in<=0x03)
            return double(in)*SRGB_LINEAR_DIVISOR;
        else
            return pow(double(in), INV_GAMMA)*(1.0+SRGB_ALPHA)-SRGB_ALPHA;
    }
    
    template<typename T>
    constexpr T Linear2sRGB(const T &in,const double &inv_gamma,const double &srgb_alpha)
    {
        if(in<=LINEAR_SRGB_THRESHOLD)
            return double(in)*SRGB_LINEAR_DIVISOR;
        else
            return pow(double(in),inv_gamma)*(1.0+srgb_alpha)-srgb_alpha;
    }

    template<>
    constexpr uint8 Linear2sRGB(const uint8 &in,const double &inv_gamma,const double &srgb_alpha)
    {
        if(in<=0x03)
            return double(in)*SRGB_LINEAR_DIVISOR;
        else
            return pow(double(in), inv_gamma)*(1.0+srgb_alpha)-srgb_alpha;
    }

    template<typename T>
    constexpr T sRGB2LinearCheaper(const T &in,const double &gamma=GAMMA)
    {
        return (T)pow(double(in),gamma);
    }

    template<typename T>
    constexpr T Linear2sRGBCheaper(const T &in,const double &inv_gamma=INV_GAMMA)
    {
        return (T)pow((double)in,inv_gamma);
    }

    template<typename T>
    constexpr T sRGB2LinearCheapest(const T &in)
    {
        return in*in;
    }

    template<typename T>
    inline void sRGB2LinearFast(T &x,T &y,T &z,const T &r,const T &g,const T &b)
    {
        x=SRGB2XYZ_X_R*r+SRGB2XYZ_X_G*g+SRGB2XYZ_X_B*b;
        y=SRGB2XYZ_Y_R*r+SRGB2XYZ_Y_G*g+SRGB2XYZ_Y_B*b;
        z=SRGB2XYZ_Z_R*r+SRGB2XYZ_Z_G*g+SRGB2XYZ_Z_B*b;
    }

    template<typename T>
    inline void Linear2sRGBFast(T &r,T &g,T &b,const T &x,const T &y,const T &z)
    {
        r=XYZ2SRGB_R_X*x+XYZ2SRGB_R_Y*y+XYZ2SRGB_R_Z*z;
        g=XYZ2SRGB_G_X*x+XYZ2SRGB_G_Y*y+XYZ2SRGB_G_Z*z;
        b=XYZ2SRGB_B_X*x+XYZ2SRGB_B_Y*y+XYZ2SRGB_B_Z*z;
    }
}//namespace hgl
