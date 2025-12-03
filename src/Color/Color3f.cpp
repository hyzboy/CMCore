#include<hgl/color/Color3f.h>
#include<hgl/color/Color3ub.h>
#include<hgl/color/Color.h>
#include<hgl/math/FloatPrecision.h>
namespace hgl
{
    //--------------------------------------------------------------------------------------------------
    Color3f::Color3f(const Color3ub &v)
    {
        r = float(v.r) / 255.0f;
        g = float(v.g) / 255.0f;
        b = float(v.b) / 255.0f;
        Clamp();
    }

    //--------------------------------------------------------------------------------------------------
    void Color3f::Clamp()
    {
        if(r<0)r=0;if(r>1)r=1;
        if(g<0)g=0;if(g>1)g=1;
        if(b<0)b=0;if(b>1)b=1;
    }
    //--------------------------------------------------------------------------------------------------
    /**
    * 求当前颜色过渡到另一颜色时某一比例时的颜色
    * @param nr,ng,nb 新的颜色
    * @param pos 过渡比例,0时为当前的颜色,1时为nr,ng,nb
    */
    void Color3f::To(float nr,float ng,float nb,float pos)
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
    void Color3f::Grey()
    {
        float lum=RGB2Lum(r,g,b);

        r=lum;
        g=lum;
        b=lum;
    }
    //--------------------------------------------------------------------------------------------------
    Color3ub Color3f::ToColor3ub() const
    {
        return Color3ub(uint8(r * 255.0f), uint8(g * 255.0f), uint8(b * 255.0f));
    }

    //--------------------------------------------------------------------------------------------------
    const Color3f &Color3f::operator = (const Color3ub &v)
    {
        r = float(v.r) / 255.0f;
        g = float(v.g) / 255.0f;
        b = float(v.b) / 255.0f;
        Clamp();
        return *this;
    }

    //--------------------------------------------------------------------------------------------------
    bool Color3f::operator == (const Color3f &v) const
    {
        if(!IsNearlyEqual(r,v.r))return(false);
        if(!IsNearlyEqual(g,v.g))return(false);
        if(!IsNearlyEqual(b,v.b))return(false);

        return(true);
    }
    //--------------------------------------------------------------------------------------------------
    bool Color3f::operator != (const Color3f &v) const
    {
        if(!IsNearlyEqual(r,v.r))return(true);
        if(!IsNearlyEqual(g,v.g))return(true);
        if(!IsNearlyEqual(b,v.b))return(true);

        return(false);
    }
    //--------------------------------------------------------------------------------------------------
}
