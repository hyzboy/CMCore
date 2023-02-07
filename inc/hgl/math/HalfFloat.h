#ifndef HGL_HALF_FLOAT_INCLUDE
#define HGL_HALF_FLOAT_INCLUDE

#include<hgl/platform/Platform.h>

namespace hgl
{
    using half_float=uint16;

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
