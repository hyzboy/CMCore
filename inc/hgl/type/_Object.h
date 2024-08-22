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

    /**
    * 取得一个类型的哈希值
    */
    template<typename T> inline constexpr const size_t GetTypeHash()
    {
        return typeid(T).hash_code();
    }

    class IDObject :public _Object
    {
    private:

        size_t object_type_hash;               ///<最终类的类型哈希值

    public:

        IDObject(const size_t &oth)
        {
            object_type_hash=oth;
        }

        virtual ~IDObject()=default;

        const size_t GetObjectTypeHash()const { return object_type_hash; }
    };//class IDObject
}//namespace hgl
