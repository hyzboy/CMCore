/**
 * FloatPrecision.h - 浮点数精度常量与比较
 * 
 * 包含：
 * - 浮点数/双精度浮点数的 MIN/MAX/EPSILON 常量
 * - 误差阈值常量 (FLOAT_ERROR, DOUBLE_ERROR)
 * - 近似零检测 (IsNearlyZero)
 * - 近似相等检测 (IsNearlyEqual, IsNearlyEqualArray)
 */

#pragma once

#include<hgl/CoreType.h>
#include<limits>
#include<cmath>

namespace hgl
{
    // ==================== 浮点数极限值 ====================
    
    constexpr const float   HGL_FLOAT_MIN       = std::numeric_limits<float>::min();             ///<最小浮点数
    constexpr const float   HGL_FLOAT_MAX       = std::numeric_limits<float>::max();             ///<最大浮点数
    constexpr const float   HGL_FLOAT_EPSILON   = std::numeric_limits<float>::epsilon();         ///<浮点数精度最小值

    constexpr const double  HGL_DOUBLE_MIN      = std::numeric_limits<double>::min();            ///<最小双精度浮点数
    constexpr const double  HGL_DOUBLE_MAX      = std::numeric_limits<double>::max();            ///<最大双精度浮点数
    constexpr const double  HGL_DOUBLE_EPSILON  = std::numeric_limits<double>::epsilon();        ///<双精度浮点数精度最小值

    // ==================== 误差阈值 ====================
    
    constexpr const float   HGL_HALF_FLOAT_ERROR = 0.001f;                                       ///<半精度浮点数最小误差值
    constexpr const float   HGL_FLOAT_ERROR      = 0.0001f;                                      ///<浮点数最小误差值
    constexpr const double  HGL_DOUBLE_ERROR     = 0.00000001;                                   ///<双精度浮点数最小误差值

    // ==================== 近似零检测 ====================
    
    /**
     * 判断浮点数是否近似为零
     * @param value 要检测的值
     * @param err 误差阈值，默认为 HGL_FLOAT_ERROR
     * @return 如果 |value| <= err 返回 true
     */
    inline bool IsNearlyZero(const float value, const float err = HGL_FLOAT_ERROR)
    {
        return (fabsf(value) <= err);
    }

    /**
     * 判断双精度浮点数是否近似为零
     * @param value 要检测的值
     * @param err 误差阈值，默认为 HGL_DOUBLE_ERROR
     * @return 如果 |value| <= err 返回 true
     */
    inline bool IsNearlyZero(const double value, const double err = HGL_DOUBLE_ERROR)
    {
        return (fabs(value) <= err);
    }

    // ==================== 近似相等检测 ====================
    
    /**
     * 判断两个浮点数是否近似相等
     * @param a 第一个值
     * @param b 第二个值
     * @param err 误差阈值，默认为 HGL_FLOAT_ERROR
     * @return 如果 |a - b| <= err 返回 true
     */
    inline bool IsNearlyEqual(const float a, const float b, const float err = HGL_FLOAT_ERROR)
    {
        return (fabsf(a - b) <= err);
    }

    /**
     * 判断两个双精度浮点数是否近似相等
     * @param a 第一个值
     * @param b 第二个值
     * @param err 误差阈值，默认为 HGL_DOUBLE_ERROR
     * @return 如果 |a - b| <= err 返回 true
     */
    inline bool IsNearlyEqual(const double a, const double b, const double err = HGL_DOUBLE_ERROR)
    {
        return (fabs(a - b) <= err);
    }

    /**
     * 判断两个浮点数数组是否近似相等
     * @param a 第一个数组
     * @param b 第二个数组
     * @param count 数组元素个数
     * @param err 误差阈值，默认为 HGL_FLOAT_ERROR
     * @return 如果所有对应元素都近似相等返回 true
     */
    inline bool IsNearlyEqualArray(const float *a, const float *b, const int count, const float err = HGL_FLOAT_ERROR)
    {
        for (int i = 0; i < count; i++)
            if (fabsf(a[i] - b[i]) > err)
                return (false);
        return (true);
    }
}//namespace hgl
