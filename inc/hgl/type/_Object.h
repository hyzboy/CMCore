#pragma once

#include <typeindex>
#include <typeinfo>

namespace hgl
{
    /**
    * 最终基类
    */
    class _Object                                                                                       ///最终基类
    {
    public:

        virtual ~_Object()=default;                                                                     ///<本类析构函数
    };//class _Object

    typedef void (_Object::*ObjectMemberFunc)();

    //此代码取自AngelScript，感谢
    #define GetMemberFuncPointer(c,m) MethodPtr<sizeof(void (c::*)())>::Convert((void (c::*)())(&c::m))

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

    template<size_t ClassCounter> class IDObject:public _Object
    {
    public:

        static const size_t GetClassID(){return ClassID;}
        static const char *GetClassName(){return ClassName;}
        static const char *GetClassRawName(){return ClassRawName;}

    public:

        virtual ~IDObject()=default;
    };

    #define HGL_IDOBJECT    IDObject<__COUNTER__>
}//namespace hgl
