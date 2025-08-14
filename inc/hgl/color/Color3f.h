#pragma once

#include<hgl/color/Lum.h>
namespace hgl
{
    class Color3ub;  // Forward declaration

    /**
    * r,g,b三原色类
    */
    class Color3f                                                                                   ///RGB三原色类
    {
        void Clamp();

    public:

        union
        {
            struct
            {
                float r,g,b;                                                                                ///<rgb 三原色
            };

            float rgb[3];
        };

    public:

        Color3f(){r=0,g=0,b=0;}                                                                     ///<本类构造函数
        Color3f(float l){r=l,g=l,b=l;Clamp();}                                                      ///<本类构造函数
        Color3f(float vr,float vg,float vb){r=vr,g=vg,b=vb;Clamp();}                                ///<本类构造函数
        Color3f(const Color3f &v){r=v.r;g=v.g;b=v.b;Clamp();}                                       ///<本类构造函数
        Color3f(const Color3ub &v);                                                                 ///<从Color3ub构造

        void Zero(){r=0,g=0,b=0;}                                                                   ///<全清为0
        void One() {r=1,g=1,b=1;}                                                                   ///<全清为1
        void Rand();                                                                                ///<全随机

        void Set(float vr,float vg,float vb){r=vr,g=vg,b=vb;Clamp();}                               ///<设置颜色
        void Set3i(int vr,int vg,int vb)                                                            ///<按0-255的范围值设置颜色
        {r=float(vr)/255.0f,g=float(vg)/255.0f,b=float(vb)/255.0f;Clamp();}

        void SetLum(float v){if(v<0)Zero();else if(v>1)One();else{r=g=b=v;}}                        ///<设置颜色

        void To(float,float,float,float);                                                           ///<转换到另一颜色
        void To(const float *rgb,float v){To(*rgb,*(rgb+1),*(rgb+2),v);}                            ///<转换到另一颜色
        void To(Color3f &c,float v){To(c.r,c.g,c.b,v);}                                             ///<转换到另一颜色

        void Black(){r=0,g=0,b=0;}                                                                  ///<黑色
        void White(){r=1,g=1,b=1;}                                                                  ///<白色

        void Red(){r=1,g=0,b=0;}                                                                    ///<红色
        void Green(){r=0,g=1,b=0;}                                                                  ///<绿色
        void Blue(){r=0,g=0,b=1;}                                                                   ///<蓝色

        void Yellow(){r=1,g=1,b=0;}                                                                 ///<黄色
        void Purple(){r=1,g=0,b=1;}                                                                 ///<紫色

        float ToGrey(){return RGB2Lum(r,g,b);}
        void Grey();                                                                                ///<将当前色彩变成灰色

        Color3ub ToColor3ub() const;                                                                ///<转换为Color3ub

        //操作符重载
        const Color3f &operator = (const float *v){r=*v++;g=*v++;b=*v;return *this;}
        const Color3f &operator = (const Color3f &v){r=v.r;g=v.g;b=v.b;return *this;}
        const Color3f &operator = (const Color3ub &v);

        bool operator == (const Color3f &)const;
        bool operator != (const Color3f &)const;

        void operator += (const Color3f &v){r+=v.r;g+=v.g;b+=v.b;Clamp();}
        void operator -= (const Color3f &v){r-=v.r;g-=v.g;b-=v.b;Clamp();}
        void operator *= (const Color3f &v){r*=v.r;g*=v.g;b*=v.b;Clamp();}
        void operator /= (const Color3f &v){r/=v.r;g/=v.g;b/=v.b;Clamp();}

        void operator *= (float v){r*=v;g*=v;b*=v;Clamp();}
        void operator /= (float v){r/=v;g/=v;b/=v;Clamp();}

        Color3f operator + (const Color3f &v){return(Color3f(r+v.r,g+v.g,b+v.b));}
        Color3f operator - (const Color3f &v){return(Color3f(r-v.r,g-v.g,b-v.b));}
        Color3f operator * (const Color3f &v){return(Color3f(r*v.r,g*v.g,b*v.b));}
        Color3f operator / (const Color3f &v){return(Color3f(r/v.r,g/v.g,b/v.b));}

        Color3f operator * (float v){return(Color3f(r*v,g*v,b*v));}
        Color3f operator / (float v){return(Color3f(r/v,g/v,b/v));}

        operator float *() const {return((float *)this);}                                           //使得本类可以直接当做float *使用
        operator const float *() const {return((const float *)this);}                               //使得本类可以直接当做const float *使用
    };//class Color3f

    const Color3f BlackColor3f(0,0,0);
    const Color3f WhiteColor3f(1,1,1);
    const Color3f RedColor3f(1,1,0);
    const Color3f GreenColor3f(0,1,0);
    const Color3f BlueColor3f(0,0,1);
    const Color3f YellowColor3f(1,1,0);

    #define DEF_RGB_U8_TO_COLOR3F(r,g,b)        Color3f(float(r)/255.0f,float(g)/255.0f,float(b)/255.0f)
}//namespace hgl
