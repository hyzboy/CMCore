#pragma once

#include<hgl/type/object/ObjectManager.h>

namespace hgl
{
    template<typename T,typename ...ARGS> SafePtr<T> DefaultCreateObject(const char *source_file,const char *source_function,const size_t source_line,ARGS...args)
    {
        const size_t hc=GetTypeHash<T>();

        ObjectManager *om=GetObjectManager(hc);

        if(!om)
            return SafePtr<T>();
    
        DefaultObjectManager<T> *dom=static_cast<DefaultObjectManager<T> *>(om);

        const SourceCodeLocation scl
        {
            .file=source_file,
            .func=source_function,
            .line=source_line
        };

        SafePtrData<T> *spd=dom->CreateObject(scl,args...);

        return SafePtr<T>(spd);
    }

    #define HGL_NEW_OBJECT(class_name,...) DefaultCreateObject<class_name>(__FILE__,__FUNCTION__,__LINE__ __VA_OPT__(,) __VA_ARGS__)

    #define HGL_DEFINE_OBJECT(class_name,type_name,...) SafePtr<class_name> type_name=HGL_NEW_OBJECT(class_name __VA_OPT__(,) __VA_ARGS__)
}//namespace hgl

