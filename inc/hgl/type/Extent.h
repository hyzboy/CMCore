#ifndef HGL_EXTENT_2D_INCLUDE
#define HGL_EXTENT_2D_INCLUDE

#include<hgl/type/DataType.h>
#include<hgl/Comparator.h>
namespace hgl
{
    template<typename T> union Extent2D
    {
        struct{T w,h;};
        struct{T width,height;};
        struct{T left,right;};
        struct{T top,bottom;};

    public:

        Extent2D(const T &x,const T &y)
        {
            w=x;
            h=y;
        }

        template<typename D>
        Extent2D(const Extent2D<D> &e)
        {
            w=e.w;
            h=e.h;
        }

        template<typename D>
        const Extent2D &operator = (const Extent2D<D> &e)
        {
            w=e.w;
            h=e.h;
        }

        CompOperatorMemcmp(const Extent2D<T> &);
    };//template<typename T> union Extent2D

    typedef Extent2D<int8>      Extent2i8,Extent2b;
    typedef Extent2D<uint8>     Extent2u8,Extent2ub;

    typedef Extent2D<int16>     Extent2i16;
    typedef Extent2D<uint16>    Extent2u16;

    typedef Extent2D<int32>     Extent2i32,Extent2i;
    typedef Extent2D<uint32>    Extent2u32,Extent2u;

    typedef Extent2D<int64>     Extent2i64;
    typedef Extent2D<uint64>    Extent2u64;

    typedef Extent2D<float>     Extent2f;
    typedef Extent2D<double>    Extent2d;
}//namespace hgl
#endif//HGL_EXTENT_2D_INCLUDE
