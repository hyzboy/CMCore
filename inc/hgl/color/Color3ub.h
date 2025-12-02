#pragma once

#include<hgl/platform/Platform.h>
namespace hgl
{
    class Color3f;  // Forward declaration

    /**
    * r,g,b三原色类(无符号8位整型)
    */
    class Color3ub                                                                                  ///RGB三原色类(0-255范围)
    {
        void Clamp();

    public:

        union
        {
            struct
            {
                uint8 r,g,b;                                                                        ///<rgb 三原色 (0-255)
            };

            uint8 rgb[3];
        };

    public:

        Color3ub(){r=0,g=0,b=0;}                                                                    ///<本类构造函数
        Color3ub(uint8 l){r=l,g=l,b=l;Clamp();}                                                    ///<本类构造函数
        Color3ub(uint8 vr,uint8 vg,uint8 vb){r=vr,g=vg,b=vb;Clamp();}                             ///<本类构造函数
        Color3ub(const Color3ub &v){r=v.r;g=v.g;b=v.b;Clamp();}                                   ///<本类构造函数
        Color3ub(const Color3f &v);                                                                ///<从Color3f构造

        void Zero(){r=0,g=0,b=0;}                                                                   ///<全清为0
        void One() {r=255,g=255,b=255;}                                                             ///<全清为255
        void Rand();                                                                                ///<全随机

        void Set(uint8 vr,uint8 vg,uint8 vb){r=vr,g=vg,b=vb;Clamp();}                              ///<设置颜色
        void Set3f(float vr,float vg,float vb);                                                    ///<按0.0-1.0的范围值设置颜色

        void SetLum(uint8 v){if(v<0)Zero();else if(v>255)One();else{r=g=b=v;}}                     ///<设置颜色

        void To(uint8,uint8,uint8,float);                                                          ///<转换到另一颜色
        void To(const uint8 *rgb,float v){To(*rgb,*(rgb+1),*(rgb+2),v);}                           ///<转换到另一颜色
        void To(Color3ub &c,float v){To(c.r,c.g,c.b,v);}                                           ///<转换到另一颜色

        void Black(){r=0,g=0,b=0;}                                                                  ///<黑色
        void White(){r=255,g=255,b=255;}                                                            ///<白色

        void Red(){r=255,g=0,b=0;}                                                                  ///<红色
        void Green(){r=0,g=255,b=0;}                                                                ///<绿色
        void Blue(){r=0,g=0,b=255;}                                                                 ///<蓝色

        void Yellow(){r=255,g=255,b=0;}                                                             ///<黄色
        void Purple(){r=255,g=0,b=255;}                                                             ///<紫色

        uint8 ToGrey() const;                                                                       ///<转换为灰度值
        void Grey();                                                                                ///<将当前色彩变成灰色

        Color3f ToColor3f() const;                                                                  ///<转换为Color3f

        //操作符重载
        const Color3ub &operator = (const uint8 *v){r=*v++;g=*v++;b=*v;return *this;}
        const Color3ub &operator = (const Color3ub &v){r=v.r;g=v.g;b=v.b;return *this;}
        const Color3ub &operator = (const Color3f &v);

        bool operator == (const Color3ub &)const;
        bool operator != (const Color3ub &)const;

        void operator += (const Color3ub &v){r+=v.r;g+=v.g;b+=v.b;Clamp();}
        void operator -= (const Color3ub &v){r-=v.r;g-=v.g;b-=v.b;Clamp();}
        void operator *= (const Color3ub &v){r*=v.r;g*=v.g;b*=v.b;Clamp();}
        void operator /= (const Color3ub &v){r/=v.r;g/=v.g;b/=v.b;Clamp();}

        void operator *= (float v){r=uint8(r*v);g=uint8(g*v);b=uint8(b*v);Clamp();}
        void operator /= (float v){r=uint8(r/v);g=uint8(g/v);b=uint8(b/v);Clamp();}

        Color3ub operator + (const Color3ub &v){return(Color3ub(r+v.r,g+v.g,b+v.b));}
        Color3ub operator - (const Color3ub &v){return(Color3ub(r-v.r,g-v.g,b-v.b));}
        Color3ub operator * (const Color3ub &v){return(Color3ub(r*v.r,g*v.g,b*v.b));}
        Color3ub operator / (const Color3ub &v){return(Color3ub(r/v.r,g/v.g,b/v.b));}

        Color3ub operator * (float v){return(Color3ub(uint8(r*v),uint8(g*v),uint8(b*v)));}
        Color3ub operator / (float v){return(Color3ub(uint8(r/v),uint8(g/v),uint8(b/v)));}

        operator uint8 *() const {return((uint8 *)this);}                                          //使得本类可以直接当做uint8 *使用
        operator const uint8 *() const {return((const uint8 *)this);}                              //使得本类可以直接当做const uint8 *使用
    };//class Color3ub

    const Color3ub BlackColor3ub(0,0,0);
    const Color3ub WhiteColor3ub(255,255,255);
    const Color3ub RedColor3ub(255,0,0);
    const Color3ub GreenColor3ub(0,255,0);
    const Color3ub BlueColor3ub(0,0,255);
    const Color3ub YellowColor3ub(255,255,0);

    #define DEF_RGB_U8_TO_COLOR3UB(r,g,b)        Color3ub(r,g,b)
}//namespace hgl
