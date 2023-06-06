#include<hgl/math/HalfFloat.h>

namespace hgl
{
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
}//namespace hgl