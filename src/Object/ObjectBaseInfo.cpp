#include<hgl/type/object/ObjectBaseInfo.h>
#include<tsl/robin_map.h>
#include<typeinfo>

namespace hgl
{
    namespace
    {
        tsl::robin_map<size_t,const std::type_info *> type_info_map;
    }//namespace

    void RegistryObjectHash(const size_t &hash_code,const std::type_info *info)
    {
        if(!info)
            return;

        if(type_info_map.contains(hash_code))
            return;

        type_info_map.emplace(hash_code,info);
    }
}//namespace hgl
