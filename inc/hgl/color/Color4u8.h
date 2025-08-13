#pragma once

#include <cstdint>
#include <algorithm>
#include <cstring>

namespace hgl
{
    class Color3u8; // fwd

    class Color4u8
    {
        static inline uint8_t clampi(int v){ return v < 0 ? 0 : (v > 255 ? 255 : uint8_t(v)); }
        static inline uint8_t clampu(uint16_t v){ return v > 255 ? 255 : uint8_t(v); }

    public:
        union
        {
            struct { uint8_t r,g,b,a; };
            uint8_t rgba[4];
        };

    public:
        Color4u8(){ r=0; g=0; b=0; a=255; }
        explicit Color4u8(uint8_t v){ r=v; g=v; b=v; a=255; }
        Color4u8(uint8_t vr,uint8_t vg,uint8_t vb,uint8_t va=255){ r=vr; g=vg; b=vb; a=va; }
        Color4u8(int vr,int vg,int vb,int va=255){ r=clampi(vr); g=clampi(vg); b=clampi(vb); a=clampi(va); }
        Color4u8(const Color4u8 &v){ r=v.r; g=v.g; b=v.b; a=v.a; }

        void Zero(){ r=0; g=0; b=0; a=0; }
        void One(){ r=255; g=255; b=255; a=255; }

        void MergeAlpha(const Color4u8 &v,uint8_t va){ r=v.r; g=v.g; b=v.b; a=uint8_t((uint16_t(v.a)*va + 127)/255); }

        void Set(const uint8_t *p){ r=p[0]; g=p[1]; b=p[2]; a=p[3]; }
        void Set(uint8_t vr,uint8_t vg,uint8_t vb){ r=vr; g=vg; b=vb; }
        void Set(uint8_t vr,uint8_t vg,uint8_t vb,uint8_t va){ r=vr; g=vg; b=vb; a=va; }
        void Set4i(int vr,int vg,int vb,int va){ r=clampi(vr); g=clampi(vg); b=clampi(vb); a=clampi(va); }

        void SetLum(uint8_t v){ r=v; g=v; b=v; }

        // Packed conversions
        uint32_t ToRGBA8()const{ return (uint32_t(r)) | (uint32_t(g)<<8) | (uint32_t(b)<<16) | (uint32_t(a)<<24); }
        uint32_t ToBGRA8()const{ return (uint32_t(b)) | (uint32_t(g)<<8) | (uint32_t(r)<<16) | (uint32_t(a)<<24); }
        uint32_t ToARGB8()const{ return (uint32_t(a)) | (uint32_t(r)<<8) | (uint32_t(g)<<16) | (uint32_t(b)<<24); }
        uint32_t ToABGR8()const{ return (uint32_t(a)) | (uint32_t(b)<<8) | (uint32_t(g)<<16) | (uint32_t(r)<<24); }
        uint32_t ToShaderRGBA8()const{ return ToRGBA8(); }

        uint8_t ToGrey() const { return uint8_t((uint16_t(r)*54 + uint16_t(g)*183 + uint16_t(b)*19 + 128) >> 8); }
        void Grey(){ uint8_t y=ToGrey(); r=y; g=y; b=y; }

        // Operators
        const Color4u8 &operator=(const uint8_t *v){ r=*v++; g=*v++; b=*v++; a=*v; return *this; }
        const Color4u8 &operator=(const Color4u8 &v){ r=v.r; g=v.g; b=v.b; a=v.a; return *this; }

        bool operator==(const Color4u8 &v)const{ return r==v.r && g==v.g && b==v.b && a==v.a; }
        bool operator!=(const Color4u8 &v)const{ return !(*this==v); }

        void operator+=(const Color4u8 &v){ r=clampu(uint16_t(r)+v.r); g=clampu(uint16_t(g)+v.g); b=clampu(uint16_t(b)+v.b); a=clampu(uint16_t(a)+v.a); }
        void operator-=(const Color4u8 &v){ r=clampi(int(r)-v.r); g=clampi(int(g)-v.g); b=clampi(int(b)-v.b); a=clampi(int(a)-v.a); }
        void operator*=(const Color4u8 &v){ r=uint8_t((uint16_t(r)*v.r + 127)/255); g=uint8_t((uint16_t(g)*v.g + 127)/255); b=uint8_t((uint16_t(b)*v.b + 127)/255); a=uint8_t((uint16_t(a)*v.a + 127)/255); }
        void operator/=(const Color4u8 &v){ r = v.r? uint8_t(std::min(255, (int(r)*255 + (v.r/2))/v.r)) : 255; g = v.g? uint8_t(std::min(255, (int(g)*255 + (v.g/2))/v.g)) : 255; b = v.b? uint8_t(std::min(255, (int(b)*255 + (v.b/2))/v.b)) : 255; a = v.a? uint8_t(std::min(255, (int(a)*255 + (v.a/2))/v.a)) : 255; }

        void operator*=(float f){ r=clampi(int(r*f)); g=clampi(int(g*f)); b=clampi(int(b*f)); a=clampi(int(a*f)); }
        void operator/=(float f){ if(f!=0.0f){ r=clampi(int(r/f)); g=clampi(int(g/f)); b=clampi(int(b/f)); a=clampi(int(a/f)); } }

        Color4u8 operator+(const Color4u8 &v)const{ Color4u8 o(*this); o+=v; return o; }
        Color4u8 operator-(const Color4u8 &v)const{ Color4u8 o(*this); o-=v; return o; }
        Color4u8 operator*(const Color4u8 &v)const{ Color4u8 o(*this); o*=v; return o; }
        Color4u8 operator/(const Color4u8 &v)const{ Color4u8 o(*this); o/=v; return o; }

        Color4u8 operator*(float f)const{ Color4u8 o(*this); o*=f; return o; }
        Color4u8 operator/(float f)const{ Color4u8 o(*this); o/=f; return o; }

        operator uint8_t *() const { return (uint8_t*)this; }
        operator const uint8_t *() const { return (const uint8_t*)this; }
    };//class Color4u8

    // Helper macros to convert U8 components to float-based colors (follow existing style)
    #define DEF_RGB_U8_TO_COLOR3F(r,g,b)        Color3f(float(r)/255.0f,float(g)/255.0f,float(b)/255.0f)
    #define DEF_RGBA_U8_TO_COLOR4F(r,g,b,a)     Color4f(float(r)/255.0f,float(g)/255.0f,float(b)/255.0f,float(a)/255.0f)
}

