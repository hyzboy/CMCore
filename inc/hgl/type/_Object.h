#pragma once

#include <typeindex>
#include <typeinfo>

namespace hgl
{
    //对象属性位定义
    #define HGL_OBJECT_ATTRIBUTE_BIT_TICK    0x00000001

    /**
    * 最终基类
    */
    class _Object                                                                                       ///最终基类
    {
    private:

        unsigned int object_attribute_bits=0;                                                           ///<对象属性位集合

    public:

        virtual ~_Object()=default;                                                                     ///<本类析构函数
    };//class _Object

    typedef void (_Object::*ObjectMemberFunc)();

    template <int> struct MethodPtr
    {
        template<class M>
        static void *Convert(M Mthd)
        {
            union u
            {
                M m;
                void *v;
            };

            return ((u *)(&Mthd))->v;
        }
    };//struct MethodPtr

    //此代码取自AngelScript，感谢
    #define GetMemberFuncPointer(c,m) MethodPtr<sizeof(void (c::*)())>::Convert((void (c::*)())(&c::m))
}//namespace hgl
