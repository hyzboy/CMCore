#ifndef HGL_COMP_OPERATOR_INCLUDE
#define HGL_COMP_OPERATOR_INCLUDE

#include<hgl/TypeFunc.h>
namespace hgl
{
    #define CompOperator(name,compfunc) const bool operator > (name i)const     {return compfunc(i)>0;} \
                                        const bool operator < (name i)const     {return compfunc(i)<0;} \
                                        const bool operator >=(name i)const     {return compfunc(i)>=0;}\
                                        const bool operator <=(name i)const     {return compfunc(i)<=0;}\
                                        const bool operator ==(name i)const     {return compfunc(i)==0;}\
                                        const bool operator !=(name i)const     {return compfunc(i)!=0;}

    #define CompOperatorMemcmp(name)    int _Comp(name data)const{return memcmp(this,&data,sizeof(name));}  \
                                        CompOperator(name,_Comp)

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
            hgl_swap(a,b);
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
}//namespace hgl
#endif//HGL_COMP_OPERATOR_INCLUDE
