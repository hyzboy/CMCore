#include<hgl/type/Color3f.h>
#include<hgl/type/Color.h>
namespace hgl
{
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
        if(pos==0)return;
        if(pos==1)
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
    void Color3f::Grey(float v1,float v2,float v3)
    {
        float lum=RGB2Lum(v1,v2,v3);

        r=lum;
        g=lum;
        b=lum;
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
    bool Color3f::operator == (const Color3f &v)
    {
        if(r!=v.r)return(false);
        if(g!=v.g)return(false);
        if(b!=v.b)return(false);

        return(true);
    }
    //--------------------------------------------------------------------------------------------------
    bool Color3f::operator != (const Color3f &v)
    {
        if(r!=v.r)return(true);
        if(g!=v.g)return(true);
        if(b!=v.b)return(true);

        return(false);
    }
    //--------------------------------------------------------------------------------------------------
}
