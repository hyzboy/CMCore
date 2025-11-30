#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    /**
     * 浮点数位操作 - Split/Merge浮点数的符号位、指数位和尾数位
     */

    // Split - 拆分浮点数为符号位、指数和尾数
    inline void SplitFloat32(bool &sign_bit, uint &exponent, uint &mantissa, const float value)
    {
        uint32 *p = (uint32 *)&value;

        sign_bit = (*p & 0x80000000) != 0;
        exponent = (*p & 0x7F800000) >> 23;
        mantissa = (*p & 0x007FFFFF);
    }

    inline void SplitFloat64(bool &sign_bit, uint &exponent, uint64 &mantissa, const double value)
    {
        uint64 *p = (uint64 *)&value;

        sign_bit = (*p & 0x8000000000000000) != 0;
        exponent = (*p & 0x7FF0000000000000) >> 52;
        mantissa = (*p & 0x000FFFFFFFFFFFFF);
    }

    // Merge - 合并符号位、指数和尾数为浮点数
    constexpr half_float MergeFloat16(const bool sign_bit, const uint exponent, const uint mantissa)
    {
        return half_float((sign_bit ? 0x8000 : 0) | (exponent << 10) | mantissa);
    }

    constexpr float MergeFloat32(const bool sign_bit, const uint exponent, const uint mantissa)
    {
        uint32 value = ((sign_bit ? 0x80000000 : 0) | (exponent << 23) | mantissa);

        return *(float *)&value;
    }

    constexpr double MergeFloat64(const bool sign_bit, const uint exponent, const uint64 mantissa)
    {
        uint64 value = ((sign_bit ? 0x8000000000000000 : 0) | (uint64(exponent) << 52) | mantissa);

        return *(double *)&value;
    }
}//namespace hgl
