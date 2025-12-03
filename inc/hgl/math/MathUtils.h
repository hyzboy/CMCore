/**
 * MathUtils.h - 数学工具函数
 * 
 * 包含：
 * - 浮点数精度截取 (hgl_clip_float)
 * - 几何体积计算 (SphereVolume, EllipsoidVolume)
 */

#pragma once

#include<hgl/math/MathConstants.h>
#include<cmath>

namespace hgl
{
    // ==================== 浮点数处理 ====================
    
    /**
     * 浮点数截取小数点后指定位数
     * @param value 要截取的浮点数
     * @param num 要截取的位数
     * @return 截取后的浮点数
     * 
     * 示例：
     * hgl_clip_float(3.14159f, 2) 返回 3.14
     * hgl_clip_float(3.14159f, 0) 返回 3
     */
    template<typename T>
    inline T hgl_clip_float(const T value, const int num)
    {
        if (num <= 0)
            return int64(value);

        if (value == 0)
            return (0);

        double per = pow(10, num);

        return double(floor(value * per)) / per;
    }

    // ==================== 体积计算 ====================
    
    /**
     * 球体积计算
     * @param radius 球半径
     * @return 球体积 = (4/3)πr³
     */
    constexpr double SphereVolume(const double radius)
    {
        return (radius * radius * radius * (HGL_PI * 4.0f)) / 3.0f;
    }

    /**
     * 椭球体积计算
     * @param x_radius x方向半径
     * @param y_radius y方向半径
     * @param z_radius z方向半径
     * @return 椭球体积 = (4/3)π·x·y·z
     */
    constexpr double EllipsoidVolume(const double x_radius, const double y_radius, const double z_radius)
    {
        return (x_radius * y_radius * z_radius * (HGL_PI * 4.0f)) / 3.0f;
    }
}//namespace hgl
