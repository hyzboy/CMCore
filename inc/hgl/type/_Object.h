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

    template<typename T> inline constexpr const size_t GetTypeHash(){return typeid(T).hash_code();} ///<取得一个类型的哈希值
    template<typename T> inline constexpr const char * GetTypeName(){return typeid(T).name();}      ///<取得一个类型的名称
}//namespace hgl
