#pragma once

#include<hgl/type/DataType.h>

namespace hgl
{
    /**
    * 尺寸模板
    */
    template<typename T> struct Size2
    {
        T width,height;

    public:

        Size2()
        {
            width=height=0;
        }

        Size2(const T &w,const T &h)
        {
            Set(w,h);
        }

        void Set(const T &w,const T &h)
        {
            width=w;
            height=h;
        }

        const bool isLandscape()const{return width>height;}                     ///<是否横的
        const bool isPortrait()const{return width<height;}                      ///<是否竖的

        void Swap()
        {
            T t=width;
            width=height;
            height=t;
        }

        Size2<T> Swapped()const{return Size2<T>(height,width);}                 ///<获取一个横竖交换的尺寸

        const bool operator == (const Size2<T> &s) const
        {
            if(width!=s.width)return(false);
            if(height!=s.height)return(false);
            return(true);
        }

        const bool operator != (const Size2<T> &s) const
        {
            return !operator==(s);
        }

        Size2 operator + (const Size2<T> &s) const
        {
            return Size2<T>(width+s.width,height+s.height);
        }

        Size2 operator - (const Size2<T> &s) const
        {
            return Size2<T>(width-s.width,height-s.height);
        }

        #define SIZE2_OPERATOR_SELF(op)    template<typename N> const Size2 &operator op (const N &n) {width op n;height op n;return *this;}
            SIZE2_OPERATOR_SELF(+=)
            SIZE2_OPERATOR_SELF(-=)
            SIZE2_OPERATOR_SELF(*=)
            SIZE2_OPERATOR_SELF(/=)
        #undef SIZE2_OPERATOR_SELF

        #define SIZE2_OPERATOR_INTERACTIVE(op) template<typename N> Size2 operator op (const N &n) const {return Size2<T>(width op n,height op n);}
            SIZE2_OPERATOR_INTERACTIVE(+)
            SIZE2_OPERATOR_INTERACTIVE(-)
            SIZE2_OPERATOR_INTERACTIVE(*)
            SIZE2_OPERATOR_INTERACTIVE(/)
        #undef SIZE2_OPERATOR_INTERACTIVE

        /**
        * 计算另一个尺寸在当前尺寸内的等比缩放多大可以正好
        * @param allow_over 允许超出
        */
        const float alcScale(const Size2<T> &s,const bool allow_over) const
        {
            if(width<=0||height<=0)return(0);
            if(s.width<=0||s.height<=0)return(0);

            float scale=float(width)/float(s.width);

            if(allow_over)
            {
                if(scale*float(s.height)<float(height))
                    scale=float(height)/float(s.height);
            }
            else
            {
                if(scale*float(s.height)>float(height))
                    scale=float(height)/float(s.height);
            }

            return(scale);
        }
    };//template<typename T> struct Size2

    using Size2i    =Size2<int>;
    using Size2ui   =Size2<uint>;
    using Size2s    =Size2<int16>;
    using Size2us   =Size2<uint16>;
    using Size2f    =Size2<float>;
    using Size2d    =Size2<double>;
}//namespace hgl
