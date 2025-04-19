#include<hgl/type/object/ObjectBaseInfo.h>
#include<tsl/robin_map.h>
#include<typeinfo>

namespace hgl
{
    namespace
    {
        tsl::robin_map<size_t,ObjectTypeInfo *> type_info_map;
    }//namespace

    void RegistryObjectTypeInfo(const size_t &hash_code,const std::type_info *info)
    {
        if(!info)
            return;

        if(type_info_map.contains(hash_code))
            return;

        ObjectTypeInfo *oti=new ObjectTypeInfo;

        oti->count=0;
        oti->info=info;

        type_info_map.emplace(hash_code,oti);
    }

    const ObjectTypeInfo *GetObjectTypeInfoByHash(const size_t &hash_code)
    {
        auto it=type_info_map.find(hash_code);

        if ( it==type_info_map.end() )
            return nullptr;

        return it.value();
    }
}//namespace hgl
