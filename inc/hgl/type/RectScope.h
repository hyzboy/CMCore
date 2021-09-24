﻿#ifndef HGL_RECTSCOPE_INCLUDE
#define HGL_RECTSCOPE_INCLUDE

#include<hgl/math/Math.h>
namespace hgl
{
    /**
    * 这个类用于描述和处理一个矩形范围
    */
    template<typename T> class RectScope2                                                           ///矩形范围类
    {
    public:

        using VEC2=glm::vec<2,T,glm::defaultp>;

        T Left;                                                                                     ///<矩形左边所在的坐标
        T Top;                                                                                      ///<矩形上边所在的坐标
        T Width;                                                                                    ///<矩形的宽度
        T Height;                                                                                   ///<矩形的高度

    public:

        T GetLeft   ()const{return Left;}
        T GetTop    ()const{return Top;}
        T GetWidth  ()const{return Width;}
        T GetHeight ()const{return Height;}
        T GetBottom ()const{return Height+Top;}
        T GetRight  ()const{return Width+Left;}

        void SetLeft    (T v){Left=v;}
        void SetTop     (T v){Top=v;}
        void SetWidth   (T v){Width=v;}
        void SetHeight  (T v){Height=v;}
        void SetBottom  (T v){Height=v-Top;}
        void SetRight   (T v){Width=v-Left;}

        T GetCenterX()const{return Left+(Width/2);}
        T GetCenterY()const{return Top+(Height/2);}

        const VEC2 GetLeftTop    ()const{return VEC2(Left,        Top);}
        const VEC2 GetLeftBottom ()const{return VEC2(Left,        Top+Height);}
        const VEC2 GetRightTop   ()const{return VEC2(Left+Width,  Top);}
        const VEC2 GetRightBottom()const{return VEC2(Left+Width,  Top+Height);}
        const VEC2 GetSize       ()const{return VEC2(Width,       Height);}

    public:

        RectScope2();
        RectScope2(T,T,T,T);
        template<typename N> RectScope2(const RectScope2<N> &);

        void Clear()
        {
            Left=0;
            Top=0;
            Width=0;
            Height=0;
        }

        void Set(T,T,T,T);

        void SetPosition(T l,T t)
        {
            Left=l;
            Top=t;
        }

        void SetSize(T w,T h)
        {
            Width=w;
            Height=h;
        }

        bool PointIn(T,T)const;

        bool PointIn(const Vector2f &v)const{return PointIn(v.x,v.y);}
        bool PointIn(const Vector3f &v)const{return PointIn(v.x,v.y);}

        template<typename N>
        void operator = (const RectScope2<N> &rs)
        {
            Left    =rs.Left;
            Top     =rs.Top;
            Width   =rs.Width;
            Height  =rs.Height;
        }

        template<typename N>
        bool operator == (const RectScope2<N> &rs) const
        {
            if((Left    ==rs.Left   )
             &&(Top     ==rs.Top    )
             &&(Width   ==rs.Width  )
             &&(Height  ==rs.Height ))return(true);
            else
                return(false);
        }

        template<typename N>
        bool operator != (const RectScope2<N> &rs) const
        {
            return(!operator==(rs));
        }

        template<typename N>
        void operator += (const RectScope2<N> &rs)
        {
            const T r=hgl_max(GetRight(),rs.GetRight());
            const T b=hgl_max(GetBottom(),rs.GetBottom());

            Left=hgl_min(Left,rs.Left);
            Top =hgl_max(Top,rs.Top);
            Width=r-Left;
            Height=b-Top;
        }

        template<typename N>
        RectScope2<T> operator + (const N &v) const
        {
            return RectScope2<T>(Left+v[0],Top+v[1],Width,Height);
        }

        template<typename N>
        RectScope2<T> operator - (const N &v) const
        {
            return RectScope2<T>(Left+v[0],Top+v[1],Width,Height);
        }

        template<typename N>
        RectScope2<T> &operator += (const N &v)
        {
            Left+=v[0];
            Top+=v[1];

            return(*this);
        }

        template<typename N>
        RectScope2<T> &operator -= (const N &v)
        {
            Left-=v[0];
            Top-=v[1];

            return(*this);
        }
    };//class RectScope2

    typedef RectScope2<double>  RectScope2d;
    typedef RectScope2<float>   RectScope2f;
    typedef RectScope2<int>     RectScope2i;
    typedef RectScope2<uint>    RectScope2ui;
//    typedef RectScope2<short>   RectScope2s;
//    typedef RectScope2<ushort>  RectScope2us;

    template<typename T,typename S>
    inline void UVFloatFromPixel(RectScope2<T> &target,const RectScope2<S> &source,const double width,const double height)
    {
        target.Left     =double(source.Left)     /width;
        target.Top      =double(source.Top)      /height;
        target.Width    =double(source.Width)    /width;
        target.Height   =double(source.Height)   /height;
    }

    template<typename D,typename S>
    inline D *WriteRect(D *target,const RectScope2<S> &source)
    {
        *target=D(source.GetLeft   ());++target;
        *target=D(source.GetTop    ());++target;
        *target=D(source.GetRight  ());++target;
        *target=D(source.GetBottom ());++target;

        return target;
    }

    template<typename D,typename S>
    inline D *WriteRect(D *target,const S &left,const S &top,const S &width,const S &height)
    {
        *target=D(left      );++target;
        *target=D(top       );++target;
        *target=D(left+width);++target;
        *target=D(top+height);++target;

        return target;
    }
}//namespace hgl
#include<hgl/type/RectScope.cpp>
#endif//HGL_RECTSCOPE_INCLUDE
