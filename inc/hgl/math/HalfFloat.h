#pragma once

#include<hgl/math/PrimaryMathematics.h>

namespace hgl
{
    /**
     * Half Float (16位浮点数) 转换函数
     * Bit depth: 16 - Sign: 1bit, Exponent: 5bits, Mantissa: 10bits
     */

    /**
    * 标准版float32转float16处理
    */
    void Float32toFloat16(half_float *output, const float *input, const uint count);

    /**
     * half_float转float (单个值)
     */
    inline float half2float(const half_float &hf)
    {
        union
        {
            float f;
            uint32 u;
        }x;

        x.u = (((hf & 0x8000) << 16) | (((hf & 0x7c00) + 0x1C000) << 13) | ((hf & 0x03FF) << 13));

        return x.f;
    }

    /**
     * half_float批量转换为float (位格式)
     */
    inline void half_to_float(uint32 *target, const half_float *source, uint32 count)
    {
        while (count--)
        {
            *target = (((*source & 0x8000) << 16) | (((*source & 0x7c00) + 0x1C000) << 13) | ((*source & 0x03FF) << 13));
            ++target;
            ++source;
        }
    }

    /**
     * half_float转uint16 (归一化到0-65535)
     */
    inline void half2u16(uint16 *output, const half_float *input, const uint count)
    {
        union
        {
            float f;
            uint32 u;
        }x;

        for (uint i = 0; i < count; i++)
        {
            x.u = ((((*input) & 0x8000) << 16) | ((((*input) & 0x7c00) + 0x1C000) << 13) | (((*input) & 0x03FF) << 13));

            *output = Clamp(x.f) * 65535;

            ++output;
            ++input;
        }
    }

    /**
     * half_float转uint8 (归一化到0-255)
     */
    inline void half2u8(uint8 *output, const half_float *input, const uint count)
    {
        union
        {
            float f;
            uint32 u;
        }x;

        for (uint i = 0; i < count; i++)
        {
            x.u = ((((*input) & 0x8000) << 16) | ((((*input) & 0x7c00) + 0x1C000) << 13) | (((*input) & 0x03FF) << 13));

            *output = Clamp(x.f) * 255;

            ++output;
            ++input;
        }
    }
}//namespace hgl
