/**
 * TrigConstants.h - 三角函数常量与角度转换
 * 
 * 包含：
 * - 常用角度的 sin/cos 预定义值 (0°, 45°, 90°, 135°, 180°, 225°, 270°, 315°)
 * - 常用角度的弧度值 (0, 45, 90, 135, 180, 225, 270, 315, 360)
 * - 角度与弧度转换函数 (deg2rad, rad2deg)
 */

#pragma once

#include<hgl/math/MathConstants.h>

namespace hgl
{
    // ==================== 预定义角度的余弦值 ====================
    
    constexpr const double HGL_COS_0   = 1;                                                      ///<cos(0°) = 1
    constexpr const double HGL_COS_45  = 0.707106781187;                                         ///<cos(45°) = √2/2
    constexpr const double HGL_COS_90  = 0;                                                      ///<cos(90°) = 0
    constexpr const double HGL_COS_135 = -0.707106781187;                                        ///<cos(135°) = -√2/2
    constexpr const double HGL_COS_180 = -1;                                                     ///<cos(180°) = -1
    constexpr const double HGL_COS_225 = -0.707106781187;                                        ///<cos(225°) = -√2/2
    constexpr const double HGL_COS_270 = 0;                                                      ///<cos(270°) = 0
    constexpr const double HGL_COS_315 = 0.707106781187;                                         ///<cos(315°) = √2/2

    // ==================== 预定义角度的正弦值 ====================
    
    constexpr const double HGL_SIN_0   = 0;                                                      ///<sin(0°) = 0
    constexpr const double HGL_SIN_45  = 0.707106781187;                                         ///<sin(45°) = √2/2
    constexpr const double HGL_SIN_90  = 1;                                                      ///<sin(90°) = 1
    constexpr const double HGL_SIN_135 = 0.707106781187;                                         ///<sin(135°) = √2/2
    constexpr const double HGL_SIN_180 = 0;                                                      ///<sin(180°) = 0
    constexpr const double HGL_SIN_225 = -0.707106781187;                                        ///<sin(225°) = -√2/2
    constexpr const double HGL_SIN_270 = -1;                                                     ///<sin(270°) = -1
    constexpr const double HGL_SIN_315 = -0.707106781187;                                        ///<sin(315°) = -√2/2

    // ==================== 预定义角度的弧度值 ====================
    
    #define HGL_DEF_DEG2RAD(ang)    constexpr const double HGL_RAD_##ang = double(ang) * (HGL_PI / 180.0);

    HGL_DEF_DEG2RAD(0)                                                                           ///<0° = 0 rad
    HGL_DEF_DEG2RAD(45)                                                                          ///<45° = π/4 rad
    HGL_DEF_DEG2RAD(90)                                                                          ///<90° = π/2 rad
    HGL_DEF_DEG2RAD(135)                                                                         ///<135° = 3π/4 rad
    HGL_DEF_DEG2RAD(180)                                                                         ///<180° = π rad
    HGL_DEF_DEG2RAD(225)                                                                         ///<225° = 5π/4 rad
    HGL_DEF_DEG2RAD(270)                                                                         ///<270° = 3π/2 rad
    HGL_DEF_DEG2RAD(315)                                                                         ///<315° = 7π/4 rad
    HGL_DEF_DEG2RAD(360)                                                                         ///<360° = 2π rad

    #undef HGL_DEF_DEG2RAD

    // ==================== 角度与弧度转换 ====================
    
    /**
     * 角度转弧度
     * @param deg 角度值
     * @return 对应的弧度值
     */
    constexpr float deg2rad(const float deg)
    {
        return deg * (HGL_PI / 180.0f);
    }

    /**
     * 弧度转角度
     * @param rad 弧度值
     * @return 对应的角度值
     */
    constexpr double rad2deg(const float rad)
    {
        return rad * (180.0f / HGL_PI);
    }
}//namespace hgl
