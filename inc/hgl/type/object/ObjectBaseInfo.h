#pragma once

#include<hgl/SourceCodeLocation.h>
#include<typeinfo>

namespace hgl
{
    /**
     * 对象简单信息
     */
    struct ObjectBaseInfo
    {
        size_t hash_code;               ///<对象数据类型的hash值
        size_t unique_id;               ///<唯一序列号

        SourceCodeLocation scl;
    };

    struct ObjectTotalInfo
    {
        size_t hash_code;

        std::type_info *info;

        size_t count;
    };

    void RegistryObjectHash(const size_t &hash_code,const std::type_info *);

    const ObjectTotalInfo *GetObjectTotalInfo(const size_t &hash_code);
}//namespace hgl

