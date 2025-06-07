#pragma once

#include<source_location>
#include<typeinfo>

namespace hgl
{
    /**
     * 对象简单信息
     */
    struct ObjectBaseInfo
    {
        const std::type_info *info;     ///<对象数据类型的type_info指针
        size_t unique_id;               ///<唯一序列号

        std::source_location scl;       ///<对象创建位置
    };

    struct ObjectTypeInfo
    {
        const std::type_info *info;

        size_t count;
    };

    void RegistryObjectTypeInfo(const size_t &hash_code,const std::type_info *);

    const ObjectTypeInfo *GetObjectTypeInfoByHash(const size_t &hash_code);

    template<typename T>
    inline const ObjectTypeInfo *GetObjectTypeInfo()
    {
        return GetObjectTypeInfoByHash(typeid(T).hash_code());
    }
}//namespace hgl

