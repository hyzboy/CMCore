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

        Size2<T> Swapped(){return Size2<T>(height,width);}                      ///<获取一个横竖交换的尺寸
    };//template<typename T> struct Size2

    using Size2i    =Size2<int>;
    using Size2ui   =Size2<uint>;
    using Size2s    =Size2<int16>;
    using Size2us   =Size2<uint16>;
    using Size2f    =Size2<float>;
    using Size2d    =Size2<double>;
}//namespace hgl
