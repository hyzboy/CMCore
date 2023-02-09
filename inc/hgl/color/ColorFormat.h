#ifndef HGL_COLOR_FORMAT_INCLUDE
#define HGL_COLOR_FORMAT_INCLUDE

#include<hgl/math/HalfFloat.h>

namespace hgl
{
    constexpr uint16 RGB8toRGB565(const uint8 r,const uint8 g,const uint8 b)
    {
        return ((r<<8)&0xF800)
              |((g<<3)&0x7E0)
              | (b>>3);
    }

    inline void RGB8toRGB565(uint16 *target,uint8 *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=((src[0]<<8)&0xF800)
                   |((src[1]<<3)&0x7E0)
                   | (src[2]>>3);

            ++target;
            src+=3;
        }
    }

    constexpr uint16 RGBA8toA1RGB5(const uint8 r,const uint8 g,const uint8 b,const uint8 a)
    {
           return ((a>>7)<<15)
                 |((r<<7)&0x7C00)
                 |((g<<2)&0x3E0)
                 | (b>>3);
    }

    inline void RGBA8toA1RGB5(uint16 *target,uint8 *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=( (src[3]>>7)<<15)
                    |((src[0]<<7)&0x7C00)
                    |((src[1]<<2)&0x3E0)
                    | (src[2]>>3);
           
            ++target;
            src+=4;
        }
    }

    // Bit depth    Sign bit present    Exponent bits   Mantissa bits
    //  32              Yes                 8               23
    //  16              Yes                 5               10
    //  11              No                  5               6
    //  10              No                  5               5
    
    constexpr uint32 RGB16FtoB10GR11UF(const half_float r, const half_float g, const half_float b)
    {
        return ((b&0x7FE0)<<17)
              |((g&0x7FF0)<<7)
              | (r&0x7FF0)>>4;
    }

    inline void RGB16FtoB10GR11UF(uint32 *target,half_float *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=((src[2]&0x7FE0)<<17)
                   |((src[1]&0x7FF0)<<7)
                   | (src[0]&0x7FF0)>>4;

            ++target;
            src+=3;
        }
    }
    
    constexpr uint16 RGBA8toBGRA4(const uint8 r, const uint8 g, const uint8 b, const uint8 a)
    {
        return ((b<<8)&0xF000)
              |((g<<4)&0xF00)
              |((r   )&0xF0)
              | (a>>4);
    }

    inline void RGBA8toBGRA4(uint16 *target,uint8 *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=((src[2]<<8)&0xF000)
                   |((src[1]<<4)&0xF00)
                   |((src[0]   )&0xF0)
                   | (src[3]>>4);

            ++target;
            src+=4;
        }
    }

    constexpr uint16 RGBA8toRGBA4(const uint8 r, const uint8 g, const uint8  b, const uint8 a)
    {
        return ((r<<8)&0xF000)
              |((g<<4)&0xF00)
              |((b   )&0xF0)
              | (a>>4);
    }

    inline void RGBA8toRGBA4(uint16 *target,uint8 *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=((src[0]<<8)&0xF000)
                   |((src[1]<<4)&0xF00)
                   |((src[2]   )&0xF0)
                   | (src[3]>>4);

            ++target;
            src+=4;
        }
    }

    constexpr uint16 RGBA8toA1RGB5(const uint8 r,const uint8 g,const uint8 b,const uint8 a)
    {
        return ((a<<8)&0x8000)
              |((r<<7)&0x7C00)
              |((g<<2)&0x3E0)
              | (b>>3);
    }
   
    inline void RGBA8toA1RGB5(uint16 *target,uint8 *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=((src[3]<<8)&0x8000)
                   |((src[0]<<7)&0x7C00)
                   |((src[1]<<2)&0x3E0)
                   | (src[2]>>3);

            ++target;
            src+=4;
        }
    }

    constexpr uint32 RGBA16toA2BGR10(const uint16 r,const uint16 g,const uint16 b,const uint16 a)
    {
        return ((a<<16)&0xC0000000)
              |((b<<14)&0x3FF00000)
              |((g<< 4)&0xFFC00)
               |(a>> 6);
    }

    inline void RGBA16toA2BGR10(uint32 *target,uint16 *src,uint size)
    {
        for(uint i=0;i<size;i++)
        {
            *target=((src[3]<<16)&0xC0000000)
                   |((src[2]<<14)&0x3FF00000)
                   |((src[1]<< 4)&0xFFC00)
                   | (src[0]>> 6);

            ++target;
            src+=4;
        }
    }

    inline void Pattle16ToRGBA8(uint32 *target,const uint8 *source,const uint32 *pattle,const uint32 width,const uint32 height)
    {
        for(uint y=0;y<height;y++)
        {
            for(uint x=0;x<width;x+=2)
            {
                *target=pattle[(*source)>>4];
                ++target;

                *target=pattle[(*source)&0xF];
                ++target;
                
                ++source;
            }
        }
    }

    inline void Pattle256ToRGBA8(uint32 *target,const uint8 *source,const uint32 *pattle,const uint32 width,const uint32 height)
    {
        for(uint y=0;y<height;y++)
        {
            for(uint x=0;x<width;x++)
            {
                *target=pattle[*source];
                   
                ++target;
                ++source;
            }
        }
    }
}//namespace hgl
#endif//HGL_COLOR_FORMAT_INCLUDE
