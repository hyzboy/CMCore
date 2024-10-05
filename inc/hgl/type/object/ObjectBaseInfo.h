#pragma once

#include<hgl/SourceCodeLocation.h>

namespace hgl
{
    class ObjectManager;

    struct ObjectBaseInfo
    {
        size_t          hash_code;          ///<对象数据类型的hash值
        ObjectManager * object_manager;     ///<对象管理器
        size_t          serial_number;      ///<对象序列号

        SourceCodeLocation source_code_location;
    };
}//namespace hgl

