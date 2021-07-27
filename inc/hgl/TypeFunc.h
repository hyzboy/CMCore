#ifndef HGL_TYPE_FUNC_INCLUDE
#define HGL_TYPE_FUNC_INCLUDE

#include<hgl/platform/Platform.h>
#include<math.h>
namespace hgl
{
    #define HGL_OFFICAL_WEB "www.hyzgame.com"
    #define HGL_OFFICAL_WEB_U8 u8"www.hyzgame.com"
    #define HGL_OFFICAL_WEB_URL "http://www.hyzgame.com"
    #define HGL_OFFICAL_WEB_URL_u8 u8"http://www.hyzgame.com"

    #define ENUM_CLASS_RANGE(begin,end)     BEGIN_RANGE=begin,END_RANGE=end,RANGE_SIZE=(END_RANGE-BEGIN_RANGE)+1

    template<typename T>
    inline bool RangeCheck(const T &value)
    {
        if(value<T::BEGIN_RANGE)return(false);
        if(value>T::END_RANGE)return(false);
        return(true);
    }

    #define ENUM_CLASS_FOR(ECTYPE,ctype,value)    \
            for(ctype value= (ctype)ECTYPE::BEGIN_RANGE;    \
                      value<=(ctype)ECTYPE::END_RANGE;  \
                      value++)

    #define HGL_CONVER_TO_MEM_ALIGN(x)      ((((x)+HGL_MEM_ALIGN-1)/HGL_MEM_ALIGN)*HGL_MEM_ALIGN)                   //内存对齐转换宏

    #ifndef NULL
    #define NULL 0
    #endif//

    constexpr u16char HGL_FULL_SPACE=U16_TEXT('　');      //全角空格

    template<typename T>
    inline T *zero_new(const int count)
    {
        T *result=new T[count];

        if(!result)
            return(nullptr);

        memset(result,0,count*sizeof(T));
        return result;
    }

    template<typename T>
    inline T *zero_malloc(const int count)
    {
        T *result=hgl_malloc(count*sizeof(T));

        if(!result)
            return(nullptr);

        memset(result,0,count*sizeof(T));
        return result;
    }

    template<typename T>
    inline void hgl_call_construct(T *obj)      //呼叫构造函数
    {
        new (static_cast<void *>(obj)) T();
    }

    #define HGL_BIT(n)      (1<<(n))
    #define HGL_64BIT(n)    (1L<<(n))

    template<typename T>
    inline bool hgl_is_one(const T value,int off)
    {
        return value&(1<<off);
    }

    template<typename T>
    inline bool hgl_is_zero(const T value,int off)
    {
        return !(value&(1<<off));
    }

    template<typename T>
    inline void hgl_set_one(T &value,int off)
    {
        value|=(1<<off);
    }

    template<typename T>
    inline void hgl_set_zero(T &value,int off)
    {
        value&=~T(1<<off);
    }

    template<typename T>
    inline void hgl_set_bit(T &value,int off,bool b)
    {
        if(b)
            hgl_set_one(value,off);
        else
            hgl_set_zero(value,off);
    }

    template<typename T>
    inline int hgl_bit_count(const T value)
    {
        int count=0;
        T bit=1;

        for(int i=0;i<sizeof(T);i++)
        {
            if(value&bit)
                ++count;

            bit<<=1;
        }

        return count;
    }

    template<typename T>
    inline int hgl_bit_count(const T value,int size)
    {
        int count=0;
        T bit=1;

        for(int i=0;i<size;i++)
        {
            if(value&bit)
                ++count;

            bit<<=1;
        }

        return count;
    }

    #define DEF_RGB_U8_TO_F(r,g,b)      {float(r)/255.0f,float(g)/255.0f,float(b)/255.0f}
    #define DEF_RGBA_U8_TO_F(r,g,b,a)   {float(r)/255.0f,float(g)/255.0f,float(b)/255.0f,float(a)/255.0f}

    constexpr uint      HGL_SIZE_1KB    =1024;
    constexpr uint      HGL_SIZE_1MB    =HGL_SIZE_1KB*1024;
    constexpr uint      HGL_SIZE_1GB    =HGL_SIZE_1MB*1024;
    constexpr uint64    HGL_SIZE_1TB    =HGL_SIZE_1GB*1024ULL;
    constexpr uint64    HGL_SIZE_1PB    =HGL_SIZE_1TB*1024ULL;
    constexpr uint64    HGL_SIZE_1EB    =HGL_SIZE_1PB*1024ULL;
//  constexpr uint128    HGL_SIZE_1ZB   =HGL_SIZE_1EB*1024ULL;
//  constexpr uint128    HGL_SIZE_1YB   =HGL_SIZE_1ZB*1024ULL;

    constexpr uint8     HGL_U8_MAX      =0xFF;
    constexpr uint16    HGL_U16_MAX     =0xFFFF;
    constexpr uint32    HGL_U32_MAX     =0xFFFFFFFF;
    constexpr uint64    HGL_U64_MAX     =0xFFFFFFFFFFFFFFFFULL;

    constexpr int8      HGL_S8_MAX      =0x7F;
    constexpr int16     HGL_S16_MAX     =0x7FFF;
    constexpr int32     HGL_S32_MAX     =0x7FFFFFFF;
    constexpr int64     HGL_S64_MAX     =0x7FFFFFFFFFFFFFFFLL;

    constexpr int8      HGL_S8_MIN      =(-0x80);
    constexpr int16     HGL_S16_MIN     =(-0x8000);

#ifdef _MSC_VER
    constexpr int32     HGL_S32_MIN     =(-0x80000000i32);
    constexpr int64     HGL_S64_MIN     =(-0x8000000000000000i64);
#else
    constexpr int32     HGL_S32_MIN     =(-0x80000000L);
    constexpr int64     HGL_S64_MIN     =(-0x8000000000000000LL);
#endif//_MSC_VER

    template<typename T> T HGL_INTEGER_MAX();
    template<typename T> T HGL_INTEGER_MIN();

    template<> inline uint8     HGL_INTEGER_MAX<uint8   >() { return HGL_U8_MAX; }
    template<> inline uint16    HGL_INTEGER_MAX<uint16  >() { return HGL_U16_MAX; }
    template<> inline uint32    HGL_INTEGER_MAX<uint32  >() { return HGL_U32_MAX; }
    template<> inline uint64    HGL_INTEGER_MAX<uint64  >() { return HGL_U64_MAX; }

    template<> inline int8      HGL_INTEGER_MAX<int8    >() { return HGL_S8_MAX; }
    template<> inline int16     HGL_INTEGER_MAX<int16   >() { return HGL_S16_MAX; }
    template<> inline int32     HGL_INTEGER_MAX<int32   >() { return HGL_S32_MAX; }
    template<> inline int64     HGL_INTEGER_MAX<int64   >() { return HGL_S64_MAX; }

    template<> inline int8      HGL_INTEGER_MIN<int8    >() { return HGL_S8_MIN; }
    template<> inline int16     HGL_INTEGER_MIN<int16   >() { return HGL_S16_MIN; }
    template<> inline int32     HGL_INTEGER_MIN<int32   >() { return HGL_S32_MIN; }
    template<> inline int64     HGL_INTEGER_MIN<int64   >() { return HGL_S64_MIN; }

    constexpr char LowerHexChar[16]={'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};  ///<小写16进制字符
    constexpr char UpperHexChar[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};  ///<大写16进制字符

    /**
     * 取适合正巧大于当前数的2次幂值
     */
    template<typename T>
    inline T power_to_2(T value)
    {
        T result=1;

        while(result<value)
            result<<=1;

        return(result);
    }
    
    /**
     * 向上对齐
     */
    template<typename T> inline T align_up(T val, T alignment)      // align val to the next multiple of alignment
    {
        return (val + alignment - (T)1) & ~(alignment - (T)1);
    }

    /**
     * 向下对齐
     */
    template<typename T> inline T align_down(T val, T alignment)    // align val to the previous multiple of alignment
    {
        return val & ~(alignment - (T)1);
    }
    
    /**
     * 求对齐数量
     */
    template<typename T> inline T divide_rounding_up(T x, T y)
    {
        return (x + y - (T)1) / y;
    }

    template<typename T> inline void hgl_swap(T &x,T &y)
    {
        T t;

        memcpy(&t,&x,sizeof(T));
        memcpy(&x,&y,sizeof(T));
        memcpy(&y,&t,sizeof(T));
    }
    
    template<typename T> T hgl_clamp(const T &cur,const T &min_value,const T &max_value)
    {
        if(cur<min_value)return min_value;
        if(cur>max_value)return max_value;

        return cur;
    }

    template<typename T> inline T hgl_abs(const T &v){return (v>=0?v:-v);}

    template<typename T> inline T hgl_min(const T &a,const T &b){return (a>b?b:a);}
    template<typename T> inline T hgl_max(const T &a,const T &b){return (a>b?a:b);}

    template<typename T> inline T hgl_min(const T *data,int count,T min_value)
    {
        T value=min_value;

        for(int i=0;i<count;i++)
        {
            if(*data<value)
                value=*data;

            ++data;
        }

        return(value);
    }

    template<typename T> inline T hgl_max(const T *data,int count,T max_value)
    {
        T value=max_value;

        for(int i=0;i<count;i++)
        {
            if(*data>value)
                value=*data;

            ++data;
        }

        return(value);
    }

    inline const uint GetMipLevel(const uint size)
    {
        return static_cast<uint>(floor(log2(size)))+1;
    }

    inline const uint GetMipLevel(const uint width,const uint height)
    {
        return GetMipLevel(hgl_max(width,height));
    }

    inline const uint GetMipLevel(const uint width,const uint height,const uint depth)
    {
        return GetMipLevel(hgl_max(hgl_max(width,height),depth));
    }

    /**
     * 同类型复制
     */
    template<typename T>
    inline void hgl_cpy(T &dst,const T &src)
    {
        memcpy(&dst,&src,sizeof(T));
    }

    /**
     * 同类型指针数据复制
     */
    template<typename T>
    inline void hgl_cpy(T *dst,const T *src)
    {
        memcpy(dst,src,sizeof(T));
    }

    /**
     * 数据类型转换赋值
     */
    template<typename S,typename D>
    inline void typeconv(D *dst,const S *src,const size_t count)
    {
        for(size_t i=0;i<count;i++)
        {
            *dst=D(*src);
            ++dst;
            ++src;
        }
    }

    /**
     * 同类型数据块复制
     */
    template<typename T>
    inline void hgl_cpy(T *dst,const T *src,const size_t count)
    {
        memcpy(dst,src,count*sizeof(T));
    }

    /**
     * 同类型数据块移动
     */
    template<typename T>
    inline void hgl_move(T *dst,const T *src,const size_t count)
    {
        memmove(dst,src,count*sizeof(T));
    }

    /**
     * 指定类型数据块赋值
     */
    template<typename T>
    inline void hgl_set(T *data,const T value,const size_t count)
    {
        for(size_t i=0;i<count;i++)
        {
            *data=value;
            ++data;
        }
    }

    /**
     * 指定类型数据块清0
     */
    template<typename T>
    inline void hgl_zero(T *data,const size_t count)
    {
        memset(data,0,count*sizeof(T));
    }

    /**
     * 分配指定类型数据块并清0
     */
    template<typename T>
    inline T *hgl_zero_new()
    {
        T *data=new T;
        memset(data,0,sizeof(T));
        return data;
    }

    /**
     * 分配指定类型数据块并清0
     */
    template<typename T>
    inline T *hgl_zero_new(const size_t count)
    {
        if(count<=0)return(nullptr);

        T *data=new T[count];
        memset(data,0,count*sizeof(T));
        return data;
    }

    template<typename T>
    inline T *hgl_copy_new(const size_t count,const T *src)
    {
        if(count<=0)return(nullptr);

        T *data=new T[count];
        memcpy(data,src,count*sizeof(T));
        return data;
    }

    /**
     * 指定类型数据清0
     */
    template<typename T>
    inline void hgl_zero(T &data)
    {
        memset(&data,0,sizeof(T));
    }
}//namespace hgl

/**
* 比较处理模板基类
*/
template<typename T> class Comparator                                                               ///比较处理模板基类
{
public:

    /**
    * 比较函数，需被特例化或派生实现
    */
    virtual int compare(const T &a,const T &b)const
    {
        return 0;   //  如 return(a-b); ，但这个函数正确情况下不应该会被调用
    }

    /**
     * 交换函数
     */
    virtual void exchange(T &a,T &b)
    {
        T t;

        memcpy(&t,&a,sizeof(T));
        memcpy(&a,&b,sizeof(T));
        memcpy(&b,&t,sizeof(T));
    }

    /**
     * 复制数据
     */
    virtual void cpy(T *t,T *s)
    {
        memcpy(t,s,sizeof(T));
    }
};//class Comparator

//针对原生类型的特例化版本，做适当加速
#define COMPARATOR_ORIGIN_TYPE(type)    template<> class Comparator<type>   \
                                        {   \
                                        public: \
                                            int compare(const type &a,const type &b)const{return a-b;}  \
                                            void exchange(type &a,type &b){type t;t=a;a=b;b=t;} \
                                            void cpy(type *t,type *s){*t=*s;}   \
                                        };

    COMPARATOR_ORIGIN_TYPE(hgl::int8)
    COMPARATOR_ORIGIN_TYPE(hgl::int16)
    COMPARATOR_ORIGIN_TYPE(hgl::int32)
    COMPARATOR_ORIGIN_TYPE(hgl::int64)

    COMPARATOR_ORIGIN_TYPE(hgl::uint8)
    COMPARATOR_ORIGIN_TYPE(hgl::uint16)
    COMPARATOR_ORIGIN_TYPE(hgl::uint32)
    COMPARATOR_ORIGIN_TYPE(hgl::uint64)

    COMPARATOR_ORIGIN_TYPE(float)
    COMPARATOR_ORIGIN_TYPE(double)

    COMPARATOR_ORIGIN_TYPE(char)
    COMPARATOR_ORIGIN_TYPE(wchar_t)

#if HGL_OS != HGL_OS_Windows
    COMPARATOR_ORIGIN_TYPE(u16char)
#endif//HGL_OS != HGL_OS_Windows

    COMPARATOR_ORIGIN_TYPE(char32_t)
#undef COMPARATOR_ORIGIN_TYPE
#endif//HGL_TYPE_FUNC_INCLUDE
