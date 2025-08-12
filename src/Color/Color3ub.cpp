#include<hgl/color/Color3ub.h>
#include<hgl/color/Color3f.h>
#include<hgl/color/Lum.h>
#include<cstdlib>
namespace hgl
{
    void Color3ub::Clamp()
    {
        // uint8 values are naturally clamped to 0-255, but we check for arithmetic overflow
        // This is mostly for consistency with the float version
    }

    //--------------------------------------------------------------------------------------------------
    Color3ub::Color3ub(const Color3f &v)
    {
        r = uint8(v.r * 255.0f);
        g = uint8(v.g * 255.0f);
        b = uint8(v.b * 255.0f);
        Clamp();
    }

    //--------------------------------------------------------------------------------------------------
    void Color3ub::Set3f(float vr, float vg, float vb)
    {
        r = uint8(vr * 255.0f);
        g = uint8(vg * 255.0f);
        b = uint8(vb * 255.0f);
        Clamp();
    }

    //--------------------------------------------------------------------------------------------------
    void Color3ub::Rand()
    {
        r = uint8(rand() % 256);
        g = uint8(rand() % 256);
        b = uint8(rand() % 256);
    }

    //--------------------------------------------------------------------------------------------------
    /**
    * 求当前颜色过渡到另一颜色时某一比例时的颜色
    * @param nr,ng,nb 新的颜色
    * @param pos 过渡比例,0时为当前的颜色,1时为nr,ng,nb
    */
    void Color3ub::To(uint8 nr, uint8 ng, uint8 nb, float pos)
    {
        if(pos<=0)return;
        if(pos>=1)
        {
            r=nr;
            g=ng;
            b=nb;
            return;
        }
        
        r = uint8(r + (nr-r)*pos);
        g = uint8(g + (ng-g)*pos);
        b = uint8(b + (nb-b)*pos);
    }

    //--------------------------------------------------------------------------------------------------
    uint8 Color3ub::ToGrey() const
    {
        // Convert to float, apply RGB2Lum, then back to uint8
        float fr = float(r) / 255.0f;
        float fg = float(g) / 255.0f;
        float fb = float(b) / 255.0f;
        return uint8(RGB2Lum(fr, fg, fb) * 255.0f);
    }

    //--------------------------------------------------------------------------------------------------
    void Color3ub::Grey()
    {
        uint8 lum = ToGrey();
        r = lum;
        g = lum;
        b = lum;
    }

    //--------------------------------------------------------------------------------------------------
    Color3f Color3ub::ToColor3f() const
    {
        return Color3f(float(r)/255.0f, float(g)/255.0f, float(b)/255.0f);
    }

    //--------------------------------------------------------------------------------------------------
    const Color3ub &Color3ub::operator = (const Color3f &v)
    {
        r = uint8(v.r * 255.0f);
        g = uint8(v.g * 255.0f);
        b = uint8(v.b * 255.0f);
        Clamp();
        return *this;
    }

    //--------------------------------------------------------------------------------------------------
    bool Color3ub::operator == (const Color3ub &v) const
    {
        if(r!=v.r)return(false);
        if(g!=v.g)return(false);
        if(b!=v.b)return(false);

        return(true);
    }

    //--------------------------------------------------------------------------------------------------
    bool Color3ub::operator != (const Color3ub &v) const
    {
        if(r!=v.r)return(true);
        if(g!=v.g)return(true);
        if(b!=v.b)return(true);

        return(false);
    }
    //--------------------------------------------------------------------------------------------------
}