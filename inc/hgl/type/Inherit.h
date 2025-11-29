#pragma once
#include<hgl/type/Smart.h>
#include<hgl/type/_Object.h>

namespace hgl
{
    template<typename ParentClass,typename SubClass> class Inherit:public ParentClass
    {
    public:

        template<typename ... ARGS> Inherit(ARGS...args):ParentClass(args...) {}

        virtual ~Inherit()=default;

        template<typename ... ARGS> static SharedPtr<SubClass> CreateObject(ARGS &&...args)
        {
            SubClass *sc=new SubClass(args...);

            return SharedPtr<SubClass>(sc);
        }

        const size_t    sizeofObject()const noexcept override { return sizeof(SubClass); }
        const char *    className   ()const noexcept override { return typeid(SubClass).name(); }
        const size_t    classHash   ()const noexcept override { return typeid(SubClass).hash_code(); }
    };//template<typename ParentClass,typename SubClass> class Inherit

    template<typename SubClass> class Inherit<_Object,SubClass>:public _Object
    {
    };
}//namespace hgl
