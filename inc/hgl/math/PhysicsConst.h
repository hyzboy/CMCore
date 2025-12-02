#pragma once

namespace hgl
{
    constexpr const double HGL_GOLDEN_RATIO                       =0.61803398874989484820458683436563811772030917980576;//黄金比例
    constexpr const double HGL_SILVER_RATIO                       =2.4142135623730950488; //白银比例

    constexpr const double HGL_SPEED_OF_SOUND                     =331.3;                 //音速(米/秒)
    constexpr const double HGL_SPEED_OF_LIGHT                     =299792458;             //光速(米/秒)

    constexpr const double HGL_ABSOLUTE_ZERO                      =-273.15;               //绝对零度

    constexpr const double HGL_UNIVERSAL_GRAVITATION              =6.67384e-11;           //万有引力常数

    constexpr const double HGL_EARTH_GRAVITATIONAL_ACCELERATION   =9.80665;               //地球上的重力加速度(牛顿)
    constexpr const double HGL_EARTH_MASS                         =5.9722e+24;            //地球质量
    constexpr const double HGL_EARTH_RADIUS                       =6371000;               //地球半径(米)

    /**
     * 物体万有引力计算
     * @param m1 星球质量
     * @param m2 物体质量
     * @param length 到星球中心的距离
     */
    template<typename T>
    inline T UniversalGravitation(const T m1,const T m2,const T length)
    {
        return HGL_UNIVERSAL_GRAVITATION*((m1*m2)/(length*length));
    }

    /**
     * 星球重力加速度计算<br>
     * 理论上: (地球质量*万有引力常数)/(地球半径的平方)=地球上的重力加速度
     * @param m 星球质量
     * @param raidus 星球半径
     * @return 星球的重力加速度(牛顿)
     */
    template<typename T>
    inline T UniversalGravitation(const T m,const T radius)
    {
        return (HGL_UNIVERSAL_GRAVITATION*m)/(radius*radius);
    }

    /**
     * 向心力计算
     * @param m 质量
     * @param v 速度
     * @param r 距离
     */
    template<typename T>
    inline T CentripetalForce(const T m,const T v,const T r)
    {
        return (m*v*v)/r;
    }

    /**
     * 加速度计算
     * @param power 推力
     * @param weight 质量
     */
    template<typename T>
    inline T AddSpeed(const T &power,const T &weight)
    {
        return power/weight;
    }

    /**
     * 物体运动质量计算(物体运动越快，质量越大)
     * @param m0 静止质量
     * @param v 运动速度
     */
    template<typename T>
    inline T RunWeight(const T &m0,const T &v)
    {
        return m0/sqrt(1-(v*v)/(HGL_SPEED_OF_LIGHT*HGL_SPEED_OF_LIGHT));
    }

    /** 
     * 摄氏度到华氏度
     */
    template<typename T>
    inline T Celsius2Fahrenheit(const T &c)
    {
        return c*1.8+32;
    }

    /**
     * 华氏度到摄氏度
     */
    template<typename T>
    inline T Fahrenheit2Celsius(const T &f)
    {
        return (f - 32) / 1.8;
    }
    
    /**
     * 摄氏度到开尔文度
     */
    template<typename T>
    inline T Celsius2Kelvin(const T &c)
    {
        return c + 273.15;
    }

    /**
     * 开尔文度到摄氏度
     */
    template<typename T>
    inline T Kelvin2Celsius(const T &k)
    {
        return k - 273.15;
    }

    /**
     * 公里到英里.
     */
    template<typename T>
    inline T Kilometer2Mile(const T &k)
    {
        return k * 0.621371192;
    }

    /**
     * 英里到公里.
     */
    template<typename T>
    inline T Mile2Kilometer(const T &m)
    {
        return m * 1.609344;
    }

    /**
     * 英寸到厘米.
     */
    template<typename T>
    inline T Inch2Centimeter(const T &i)
    {
        return i * 2.54;
    }

    /**
     * 厘米到英寸.
     */
    template<typename T>
    inline T Centimeter2Inch(const T &c)
    {
        return c * 0.393700787;
    }

    /**
     * 公里到海里.
     */
    template<typename T>
    inline T Kilometer2NauticalMile(const T &k)
    {
        return k * 0.539956803;
    }

    /**
     * 海里到公里.
     */
    template<typename T>
    inline T NauticalMile2Kilometer(const T &n)
    {
        return n * 1.852;
    }
}//namespace hgl
