#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    /**
     * 浮点数验证函数 - 检测NaN、无穷大、零等特殊值
     * 支持 half_float, float, double 三种类型
     */

    // IsNaN - 检测是否为NaN
    constexpr bool IsNaN(const half_float hf)
    {
        return ((hf & 0x7C00) == 0x7C00) && ((hf & 0x03FF) != 0);
    }

    constexpr bool IsNaN(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return ((bits & 0x7F800000) == 0x7F800000) && ((bits & 0x007FFFFF) != 0);
    }

    constexpr bool IsNaN(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return ((bits & 0x7FF0000000000000) == 0x7FF0000000000000) && ((bits & 0x000FFFFFFFFFFFFF) != 0);
    }

    // IsInfinite - 检测是否为无穷大（正负）
    constexpr bool IsInfinite(const half_float hf)
    {
        return ((hf & 0x7C00) == 0x7C00) && ((hf & 0x03FF) == 0);
    }

    constexpr bool IsInfinite(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return ((bits & 0x7F800000) == 0x7F800000) && ((bits & 0x007FFFFF) == 0);
    }

    constexpr bool IsInfinite(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return ((bits & 0x7FF0000000000000) == 0x7FF0000000000000) && ((bits & 0x000FFFFFFFFFFFFF) == 0);
    }

    // IsPositiveInfinite - 检测是否为正无穷大
    constexpr bool IsPositiveInfinite(const half_float hf)
    {
        return (hf & 0x7FFF) == 0x7C00;
    }

    constexpr bool IsPositiveInfinite(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return bits == 0x7F800000;
    }

    constexpr bool IsPositiveInfinite(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return bits == 0x7FF0000000000000;
    }

    // IsNegativeInfinite - 检测是否为负无穷大
    constexpr bool IsNegativeInfinite(const half_float hf)
    {
        return hf == 0xFC00;
    }

    constexpr bool IsNegativeInfinite(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return bits == 0xFF800000;
    }

    constexpr bool IsNegativeInfinite(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return bits == 0xFFF0000000000000;
    }

    // IsZero - 检测是否为零（正负）
    constexpr bool IsZero(const half_float hf)
    {
        return (hf & 0x7FFF) == 0;
    }

    constexpr bool IsZero(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return (bits & 0x7FFFFFFF) == 0;
    }

    constexpr bool IsZero(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return (bits & 0x7FFFFFFFFFFFFFFF) == 0;
    }

    // IsPositiveZero - 检测是否为正零
    constexpr bool IsPositiveZero(const half_float hf)
    {
        return hf == 0;
    }

    constexpr bool IsPositiveZero(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return bits == 0;
    }

    constexpr bool IsPositiveZero(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return bits == 0;
    }

    // IsNegativeZero - 检测是否为负零
    constexpr bool IsNegativeZero(const half_float hf)
    {
        return hf == 0x8000;
    }

    constexpr bool IsNegativeZero(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return bits == 0x80000000;
    }

    constexpr bool IsNegativeZero(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return bits == 0x8000000000000000;
    }

    // IsFinite - 检测是否为有限数
    constexpr bool IsFinite(const half_float hf)
    {
        return (hf & 0x7C00) != 0x7C00;
    }

    constexpr bool IsFinite(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return (bits & 0x7F800000) != 0x7F800000;
    }

    constexpr bool IsFinite(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return (bits & 0x7FF0000000000000) != 0x7FF0000000000000;
    }

    // IsDenormalized - 检测是否为非规格化数（仅half_float）
    constexpr bool IsDenormalized(const half_float hf)
    {
        return ((hf & 0x7C00) == 0) && ((hf & 0x03FF) != 0);
    }

    // IsNormalized - 检测是否为规格化数（仅half_float）
    constexpr bool IsNormalized(const half_float hf)
    {
        uint exp = (hf & 0x7C00) >> 10;
        return exp > 0 && exp < 0x1F;
    }

    // IsValid - 检测浮点数是否有效（不是NaN、无穷大）
    constexpr bool IsValid(const half_float hf)
    {
        return (hf & 0x7C00) != 0x7C00;
    }

    constexpr bool IsValid(const float value)
    {
        uint32 bits = *(uint32*)&value;
        return (bits & 0x7F800000) != 0x7F800000;
    }

    constexpr bool IsValid(const double value)
    {
        uint64 bits = *(uint64*)&value;
        return (bits & 0x7FF0000000000000) != 0x7FF0000000000000;
    }
}//namespace hgl
