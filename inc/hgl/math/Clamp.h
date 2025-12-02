/**
 * Clamp.h - 数值范围限制函数
 * 
 * 包含：
 * - Clamp 通用模板函数（限制到指定范围）
 * - ClampU8/ClampU16 整数类型快速限制
 */

#pragma once

#include<hgl/type/DataType.h>

namespace hgl
{
    /**
     * 将数值限制在指定范围内
     * @param value 要限制的值
     * @param min_value 最小值
     * @param max_value 最大值
     * @return 限制后的值
     */
    template<typename T,typename T2>
    constexpr T Clamp(const T &value,const T2 &min_value,const T2 &max_value)
    {
        if(value<min_value)return min_value;
        if(value>max_value)return max_value;

        return value;
    }

    /**
     * 将数值限制在 [0, 1] 范围内
     * @param value 要限制的值
     * @return 限制后的值
     */
    template<typename T>
    constexpr T Clamp(const T &value)
    {
        return Clamp<T>(value,0,1);
    }

    /**
     * 将整数限制到 uint8 范围 [0, 255]
     * @param value 要限制的整数值
     * @return 限制后的 uint8 值
     */
    constexpr uint8 ClampU8(const int value)
    {
        if(value<0)return 0;
        if(value>0xFF)return 0xFF;
        return static_cast<uint8>(value);
    }

    /**
     * 将整数限制到 uint16 范围 [0, 65535]
     * @param value 要限制的整数值
     * @return 限制后的 uint16 值
     */
    constexpr uint16 ClampU16(const int value)
    {
        if(value<0)return 0;
        if(value>0xFFFF)return 0xFFFF;
        return static_cast<uint16>(value);
    }
}//namespace hgl
