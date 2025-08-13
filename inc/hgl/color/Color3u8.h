#pragma once

#include <cstdint>
#include <algorithm>

namespace hgl
{
    class Color3u8
    {
        static inline uint8_t clampi(int v){ return v < 0 ? 0 : (v > 255 ? 255 : uint8_t(v)); }
        static inline uint8_t clampu(uint16_t v){ return v > 255 ? 255 : uint8_t(v); }

    public:
        union
        {
            struct { uint8_t r,g,b; };
            uint8_t rgb[3];
        };

    public:
        Color3u8(){ r=0; g=0; b=0; }
        explicit Color3u8(uint8_t v){ r=v; g=v; b=v; }
        Color3u8(uint8_t vr,uint8_t vg,uint8_t vb){ r=vr; g=vg; b=vb; }
        Color3u8(int vr,int vg,int vb){ r=clampi(vr); g=clampi(vg); b=clampi(vb); }
        Color3u8(const Color3u8 &v){ r=v.r; g=v.g; b=v.b; }

        void Zero(){ r=0; g=0; b=0; }
        void One(){ r=255; g=255; b=255; }

        void Set(uint8_t vr,uint8_t vg,uint8_t vb){ r=vr; g=vg; b=vb; }
        void Set3i(int vr,int vg,int vb){ r=clampi(vr); g=clampi(vg); b=clampi(vb); }

        // Greyscale based on Rec. 709 luma approximation
        uint8_t ToGrey() const { return uint8_t((uint16_t(r)*54 + uint16_t(g)*183 + uint16_t(b)*19 + 128) >> 8); }
        void Grey(){ uint8_t y=ToGrey(); r=y; g=y; b=y; }

        // Operators
        const Color3u8 &operator=(const uint8_t *v){ r=*v++; g=*v++; b=*v; return *this; }
        const Color3u8 &operator=(const Color3u8 &v){ r=v.r; g=v.g; b=v.b; return *this; }

        bool operator==(const Color3u8 &v)const{ return r==v.r && g==v.g && b==v.b; }
        bool operator!=(const Color3u8 &v)const{ return !(*this==v); }

        void operator+=(const Color3u8 &v){ r=clampu(uint16_t(r)+v.r); g=clampu(uint16_t(g)+v.g); b=clampu(uint16_t(b)+v.b); }
        void operator-=(const Color3u8 &v){ r=clampi(int(r)-v.r); g=clampi(int(g)-v.g); b=clampi(int(b)-v.b); }
        void operator*=(const Color3u8 &v){ r=uint8_t((uint16_t(r)*v.r + 127)/255); g=uint8_t((uint16_t(g)*v.g + 127)/255); b=uint8_t((uint16_t(b)*v.b + 127)/255); }
        void operator/=(const Color3u8 &v){ r = v.r? uint8_t(std::min(255, (int(r)*255 + (v.r/2))/v.r)) : 255; g = v.g? uint8_t(std::min(255, (int(g)*255 + (v.g/2))/v.g)) : 255; b = v.b? uint8_t(std::min(255, (int(b)*255 + (v.b/2))/v.b)) : 255; }

        void operator*=(float f){ r=clampi(int(r*f)); g=clampi(int(g*f)); b=clampi(int(b*f)); }
        void operator/=(float f){ if(f!=0.0f){ r=clampi(int(r/f)); g=clampi(int(g/f)); b=clampi(int(b/f)); } }

        Color3u8 operator+(const Color3u8 &v)const{ Color3u8 o(*this); o+=v; return o; }
        Color3u8 operator-(const Color3u8 &v)const{ Color3u8 o(*this); o-=v; return o; }
        Color3u8 operator*(const Color3u8 &v)const{ Color3u8 o(*this); o*=v; return o; }
        Color3u8 operator/(const Color3u8 &v)const{ Color3u8 o(*this); o/=v; return o; }

        Color3u8 operator*(float f)const{ Color3u8 o(*this); o*=f; return o; }
        Color3u8 operator/(float f)const{ Color3u8 o(*this); o/=f; return o; }

        operator uint8_t *() const { return (uint8_t*)this; }
        operator const uint8_t *() const { return (const uint8_t*)this; }
    };//class Color3u8
}//namespace hgl
