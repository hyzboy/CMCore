#ifndef HGL_MATH_PHYSICS_CONST_INCLUDE
#define HGL_MATH_PHYSICS_CONST_INCLUDE

namespace hgl
{
    constexpr double HGL_GOLDEN_RATIO                       =0.61803398874989484820458683436563811772030917980576;//黄金比例
    constexpr double HGL_SILVER_RATIO                       =2.4142135623730950488; //白银比例

    constexpr double HGL_SPEED_OF_SOUND                     =331.3f;                //音速(米/秒)
    constexpr double HGL_SPEED_OF_LIGHT                     =299792458;             //光速(米/秒)

    constexpr double HGL_ABSOLUTE_ZERO                      =-273.15f;              //绝对零度

    constexpr double HGL_UNIVERSAL_GRAVITATION              =6.67384e-11;           //万有引力常数

    constexpr double HGL_EARTH_GRAVITATIONAL_ACCELERATION   =9.80665;               //地球上的重力加速度(牛顿)
    constexpr double HGL_EARTH_MASS                         =5.9722e+24;            //地球质量
    constexpr double HGL_EARTH_RADIUS                       =6371000;               //地球半径(米)

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
}//namespace hgl
#endif//HGL_MATH_PHYSICS_CONST_INCLUDE
