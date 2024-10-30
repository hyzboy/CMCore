#pragma once

#include<hgl/SourceCodeLocation.h>

namespace hgl
{
    class ObjectManager;

    /**
     * 对象简单信息
     */
    struct ObjectSimpleInfo
    {
        size_t hash_code;               ///<对象数据类型的hash值
        size_t serial_number;           ///<对象序列号
    };

    /**
     * 对象基本信息
     */
    struct ObjectBaseInfo:public ObjectSimpleInfo
    {
        ObjectManager *     object_manager;     ///<对象管理器

        SourceCodeLocation  source_code_location;    ///<对象创建的源代码位置
    };
}//namespace hgl

