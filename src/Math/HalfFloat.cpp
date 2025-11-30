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
            int32 exponent=(value&0x7F800000)>>23;  // 使用int32以支持负数运算
            uint32 mantissa=value&0x007FFFFF;

            // 处理无穷大和NaN
            if(exponent==0xFF)
            {
                if(mantissa==0)
                    dst[i]=(sign_bit>>16)|0x7C00;  // 无穷大
                else
                    dst[i]=0x7FFF;  // NaN
                continue;
            }

            // 处理零和非规格化数
            if(exponent==0)
            {
                if(mantissa==0)
                {
                    dst[i]=sign_bit>>16;  // 零
                }
                else
                {
                    // 非规格化数，需要规格化处理
                    uint32 mantissa_bit=0x00800000;
                    int32 shift=0;

                    while((mantissa&mantissa_bit)==0)
                    {
                        mantissa_bit>>=1;
                        ++shift;
                    }

                    exponent=1-shift;
                    mantissa&=~mantissa_bit;
                    mantissa<<=shift;
                }
            }

            // 转换指数：从float32偏移127转换为float16偏移15
            exponent+=15-127;

            // 检查上溢
            if(exponent>=0x1F)
            {
                dst[i]=(sign_bit>>16)|0x7C00;  // 溢出，设为无穷大
                continue;
            }

            // 检查下溢
            if(exponent<=0)
            {
                if(exponent<-10)
                {
                    dst[i]=sign_bit>>16;  // 太小，变为零
                }
                else
                {
                    // 转换为非规格化数
                    mantissa|=0x00800000;  // 添加隐含的1
                    mantissa>>=(-exponent+1);
                    mantissa>>=13;
                    dst[i]=sign_bit>>16|mantissa;
                }
                continue;
            }

            // 正常数值：压缩尾数从23位到10位
            mantissa>>=13;
            dst[i]=(sign_bit>>16)|(exponent<<10)|mantissa;
        }
    }
}//namespace hgl
