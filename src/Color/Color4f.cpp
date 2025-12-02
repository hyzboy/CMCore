#include<hgl/color/Color4f.h>
#include<hgl/color/Color4ub.h>
#include<hgl/color/Color.h>
#include<hgl/math/MathConst.h>
namespace hgl
{
    //--------------------------------------------------------------------------------------------------
    Color4f::Color4f(const Color4ub &v)
    {
        r = float(v.r) / 255.0f;
        g = float(v.g) / 255.0f;
        b = float(v.b) / 255.0f;
        a = float(v.a) / 255.0f;
        Clamp();
    }

    //--------------------------------------------------------------------------------------------------
    void Color4f::Clamp()
    {
        if(r<0)r=0;if(r>1)r=1;
        if(g<0)g=0;if(g>1)g=1;
        if(b<0)b=0;if(b>1)b=1;
        if(a<0)a=0;if(a>1)a=1;
    }
    //--------------------------------------------------------------------------------------------------
    /**
    * 求当前颜色过渡到另一颜色时某一比例时的颜色
    * @param nr,ng,nb 新的颜色
    * @param pos 过渡比例,0时为当前的颜色,1时为nr,ng,nb
    */
    void Color4f::To(float nr,float ng,float nb,float pos)
    {
        if(pos<=0)return;
        if(pos>=1)
        {
            r=nr;
            g=ng;
            b=nb;
            return;
        }

        r+=(nr-r)*pos;
        g+=(ng-g)*pos;
        b+=(nb-b)*pos;
    }
    //--------------------------------------------------------------------------------------------------
    void Color4f::Grey()
    {
        float lum=RGB2Lum(r,g,b);

        r=lum;
        g=lum;
        b=lum;
    }
    //--------------------------------------------------------------------------------------------------
    Color4ub Color4f::ToColor4ub() const
    {
        return Color4ub(uint8(r * 255.0f), uint8(g * 255.0f), uint8(b * 255.0f), uint8(a * 255.0f));
    }

    //--------------------------------------------------------------------------------------------------
    const Color4f &Color4f::operator = (const Color4ub &v)
    {
        r = float(v.r) / 255.0f;
        g = float(v.g) / 255.0f;
        b = float(v.b) / 255.0f;
        a = float(v.a) / 255.0f;
        Clamp();
        return *this;
    }

    //--------------------------------------------------------------------------------------------------
    bool Color4f::operator == (const Color4f &v) const
    {
        if(!IsNearlyEqual(r,v.r))return(false);
        if(!IsNearlyEqual(g,v.g))return(false);
        if(!IsNearlyEqual(b,v.b))return(false);
        if(!IsNearlyEqual(a,v.a))return(false);

        return(true);
    }
    //--------------------------------------------------------------------------------------------------
    bool Color4f::operator != (const Color4f &v) const
    {
        if(!IsNearlyEqual(r,v.r))return(true);
        if(!IsNearlyEqual(g,v.g))return(true);
        if(!IsNearlyEqual(b,v.b))return(true);
        if(!IsNearlyEqual(a,v.a))return(true);

        return(false);
    }
    //--------------------------------------------------------------------------------------------------
}
