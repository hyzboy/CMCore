#ifndef HGL_HALF_FLOAT_INCLUDE
#define HGL_HALF_FLOAT_INCLUDE

#include<hgl/platform/Platform.h>
#include<hgl/math/Math.h>

namespace hgl
{
    // Bit depth    Sign bit present    Exponent bits   Mantissa bits
    //  32              Yes                 8               23
    //  16              Yes                 5               10
    //  11              No                  5               6
    //  10              No                  5               5

    inline void SplitFloat32(bool &sign_bit,uint &exponent,uint &mantissa,const float value)
    {
        uint32 *p=(uint32 *)&value;
    
        sign_bit=(*p&0x80000000)!=0;
        exponent=(*p&0x7F800000)>>23;
        mantissa=(*p&0x007FFFFF);
    }

    inline void SplitFloat64(bool &sign_bit,uint &exponent,uint64 &mantissa,const double value)
    {
        uint64 *p=(uint64 *)&value;
       
        sign_bit=(*p&0x8000000000000000)!=0;
        exponent=(*p&0x7FF0000000000000)>>52;
        mantissa=(*p&0x000FFFFFFFFFFFFF);
    }

    constexpr half_float MergeFloat16(const bool sign_bit,const uint exponent,const uint mantissa)
    {
        return half_float((sign_bit?0x8000:0)|(exponent<<10)|mantissa);
    }

    constexpr float MergeFloat32(const bool sign_bit,const uint exponent,const uint mantissa)
    {
        uint32 value=((sign_bit?0x80000000:0)|(exponent<<23)|mantissa);

        return *(float *)&value;
    }

    constexpr double MergeFloat64(const bool sign_bit,const uint exponent,const uint64 mantissa)
    {
        uint64 value=((sign_bit?0x8000000000000000:0)|(uint64(exponent)<<52)|mantissa);

        return *(double *)&value;
    }

    /**
    * 标准版float32转float16处理
    */
    void Float32toFloat16(half_float *output,const float *input,const uint count);

    inline const float half2float(const half_float &hf)
    {
        union
        {
            float f;
            uint32 u;
        }x;

        x.u=(((hf&0x8000)<<16) | (((hf&0x7c00)+0x1C000)<<13) | ((hf&0x03FF)<<13));

        return x.f;
    }

    inline void half_to_float(uint32 *target,const half_float *source,uint32 count)
    {
        while (count--)
        {
            *target = (((*source & 0x8000) << 16) | (((*source & 0x7c00) + 0x1C000) << 13) | ((*source & 0x03FF) << 13));
            ++target;
            ++source;
        }
    }

    inline void half2u16(uint16 *output,const half_float *input,const uint count)
    {
        union
        {
            float f;
            uint32 u;
        }x;

        for(uint i=0;i<count;i++)
        {
            x.u=((((*input)&0x8000)<<16) | ((((*input)&0x7c00)+0x1C000)<<13) | (((*input)&0x03FF)<<13));

            *output=Clamp(x.f)*65535;

            ++output;
            ++input;
        }
    }

    inline void half2u8(uint8 *output,const half_float *input,const uint count)
    {
        union
        {
            float f;
            uint32 u;
        }x;

        for(uint i=0;i<count;i++)
        {
            x.u=((((*input)&0x8000)<<16) | ((((*input)&0x7c00)+0x1C000)<<13) | (((*input)&0x03FF)<<13));

            *output=Clamp(x.f)*255;

            ++output;
            ++input;
        }
    }
}//namespace hgl
#endif//HGL_HALF_FLOAT_INCLUDE
