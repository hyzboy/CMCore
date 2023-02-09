#ifndef HGL_HALF_FLOAT_INCLUDE
#define HGL_HALF_FLOAT_INCLUDE

#include<hgl/platform/Platform.h>

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

    constexpr float MergeFloat(const bool sign_bit,const uint exponent,const uint mantissa)
    {
        return float((sign_bit?0x80000000:0)|(exponent<<23)|mantissa);
    }

    constexpr double MergeFloat64(const bool sign_bit,const uint exponent,const uint64 mantissa)
    {
        return double((sign_bit?0x8000000000000000:0)|(uint64(exponent)<<52)|mantissa);
    }

    /**
    * 标准版float32转float16处理
    */
    void Float32toFloat16(half_float *output,const float *input,const uint count)
    {
        const uint32 *src=(const uint32 *)input;
        uint16 *dst=output;
    
        for(uint i=0;i<count;i++)
        {
            uint32 value=src[i];
    
            uint32 sign_bit=value&0x80000000;
            uint32 exponent=(value&0x7F800000)>>23;
            uint32 mantissa=value&0x007FFFFF;
    
            if(exponent==0xFF)
            {
                if(mantissa==0)
                    dst[i]=sign_bit>>16;
                else
                    dst[i]=0x7FFF;
            }
            else
            {
                if(exponent==0)
                {
                    if(mantissa==0)
                        dst[i]=sign_bit>>16;
                    else
                    {
                        uint32 mantissa_bit=0x00800000;
                                     
                        while((mantissa&mantissa_bit)==0)
                        {
                            mantissa_bit>>=1;
                            --exponent;
                        }
                                     
                        exponent+=1;
                        mantissa&=~mantissa_bit;
                    }
                }
                else
                {
                    exponent+=15-127;
                    mantissa>>=13;
                }
           
                dst[i]=sign_bit>>16|exponent<<10|mantissa;
            }
        }
    }

    /**
    * 快速版float32转float16处理
    */
    inline const half_float float2half(const float &f)
    {
        const uint32 x = *((uint32 *)&f);

        return ((x>>16)&0x8000)|((((x&0x7f800000)-0x38000000)>>13)&0x7c00)|((x>>13)&0x03ff);
    }
    
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

    inline void float_to_half(half_float *target,const float *source,uint32 count)
    {
        while (count--)
        {
            *target = float2half(*source);
            ++target;
            ++source;
        }
    }   
}//namespace hgl
#endif//HGL_HALF_FLOAT_INCLUDE
