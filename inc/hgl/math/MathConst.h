#pragma once

#include<hgl/type/DataType.h>
#include<limits>

namespace hgl
{
    constexpr const float   HGL_FLOAT_MIN       =std::numeric_limits<float>::min();             ///<最小浮点数
    constexpr const float   HGL_FLOAT_MAX       =std::numeric_limits<float>::max();             ///<最大浮点数
    constexpr const float   HGL_FLOAT_EPSILON   =std::numeric_limits<float>::epsilon();         ///<浮点数精度最小值

    constexpr const double  HGL_DOUBLE_MIN      =std::numeric_limits<double>::min();            ///<最小双精度浮点数
    constexpr const double  HGL_DOUBLE_MAX      =std::numeric_limits<double>::max();            ///<最大双精度浮点数
    constexpr const double  HGL_DOUBLE_EPSILON  =std::numeric_limits<double>::epsilon();        ///<双精度浮点数精度最小值

    constexpr const float   HGL_HALF_FLOAT_ERROR=0.001f;                                        ///<半精度浮点数最小误差值
    constexpr const float   HGL_FLOAT_ERROR     =0.0001f;                                       ///<浮点数最小误差值
    constexpr const double  HGL_DOUBLE_ERROR    =0.00000001;                                    ///<双精度浮点数最小误差值

    inline bool IsNearlyZero(const float value,const float err=HGL_FLOAT_ERROR){return(fabsf(value) <= err);}
    inline bool IsNearlyZero(const double value,const double err=HGL_DOUBLE_ERROR){return(fabs(value) <= err);}

    inline bool IsNearlyEqual(const float a,const float b,const float err=HGL_FLOAT_ERROR){return(fabsf(a - b) <= err);}
    inline bool IsNearlyEqual(const double a,const double b,const double err=HGL_DOUBLE_ERROR){return(fabs(a - b) <= err); }

    inline bool IsNearlyEqualArray(const float *a,const float *b,const int count,const float err=HGL_FLOAT_ERROR)
    {
        for(int i=0;i<count;i++)
            if(fabsf(a[i]-b[i])>err)
                return(false);
        return(true);
    }

    constexpr const double HGL_E          =2.7182818284590452353602874713526624977572470936999595749669676277240766303535475945713821785251664274;        //欧拉数(自然对数的底数)
    constexpr const double HGL_LOG2E      =1.44269504088896340736;
    constexpr const double HGL_LOG10E     =0.434294481903251827651;
    constexpr const double HGL_LN2        =0.693147180559945309417;
    constexpr const double HGL_LN10       =2.30258509299404568402;
    constexpr const double HGL_PI         =3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068;
    constexpr const double HGL_PI_2       =1.57079632679489661923;
    constexpr const double HGL_PI_4       =0.785398163397448309616;
    constexpr const double HGL_PI_3_4     =2.356194490192344928938;
    constexpr const double HGL_1_PI       =0.318309886183790671538;
    constexpr const double HGL_2_PI       =0.636619772367581343076;
    constexpr const double HGL_2_SQRTPI   =1.12837916709551257390;
    constexpr const double HGL_SQRT2      =1.41421356237309504880168872420969807;
    constexpr const double HGL_SQRT1_2    =0.707106781186547524401;

    constexpr const double HGL_COS_0  =1;
    constexpr const double HGL_COS_45 =0.707106781187;
    constexpr const double HGL_COS_90 =0;
    constexpr const double HGL_COS_135=-0.707106781187;
    constexpr const double HGL_COS_180=-1;
    constexpr const double HGL_COS_225=-0.707106781187;
    constexpr const double HGL_COS_270=0;
    constexpr const double HGL_COS_315=0.707106781187;

    constexpr const double HGL_SIN_0  =0;
    constexpr const double HGL_SIN_45 =0.707106781187;
    constexpr const double HGL_SIN_90 =1;
    constexpr const double HGL_SIN_135=0.707106781187;
    constexpr const double HGL_SIN_180=0;
    constexpr const double HGL_SIN_225=-0.707106781187;
    constexpr const double HGL_SIN_270=-1;
    constexpr const double HGL_SIN_315=-0.707106781187;

    #define HGL_DEF_DEG2RAD(ang)    constexpr const double HGL_RAD_##ang=double(ang)*(HGL_PI/180.0);

    HGL_DEF_DEG2RAD(0)
    HGL_DEF_DEG2RAD(45)
    HGL_DEF_DEG2RAD(90)
    HGL_DEF_DEG2RAD(135)
    HGL_DEF_DEG2RAD(180)
    HGL_DEF_DEG2RAD(225)
    HGL_DEF_DEG2RAD(270)
    HGL_DEF_DEG2RAD(315)
    HGL_DEF_DEG2RAD(360)

    #undef HGL_DEF_DEG2RAD

    /**
     * 角度转弧度
     */
    inline constexpr float deg2rad(const float deg)
    {
        return deg*(HGL_PI/180.0f);
    }

    /**
     * 弧度转角度
     */
    inline constexpr double rad2deg(const float rad)
    {
        return rad*(180.0f/HGL_PI);
    }

    /**
     * 浮点数截取小数点后指定位度
     * @param value 要截取的浮点数
     * @param num 要截取的位数
     */
    template<typename T>
    inline T hgl_clip_float(const T value,const int num)
    {
        if(num<=0)
            return int64(value);

        if(value==0)return(0);

        double per=pow(10,num);

        return double(floor(value*per))/per;
    }

    /**
     * 球体积计算
     * @param radius 球半径
     */
    inline constexpr double SphereVolume(const double radius)
    {
        return(radius*radius*radius*(HGL_PI*4.0f))/3.0f;
    }

    /**
     * 椭球体积计算
     * @param x_radius x半径
     * @param y_radius y半径
     * @param z_radius z半径
     */
    inline constexpr double EllipsoidVolume(const double x_radius,const double y_radius,const double z_radius)
    {
        return(x_radius*y_radius*z_radius*(HGL_PI*4.0f))/3.0f;
    }
}//namespace hgl
