#include<hgl/type/object/ObjectManager.h>
//#include<hgl/type/object/Object.h>
//#include<iostream>
#include<tsl/robin_map.h>

namespace hgl
{
    namespace
    {
        using ObjectManagerMap=tsl::robin_map<size_t,ObjectManager *>;
        static ObjectManagerMap *object_manager_map=nullptr;

        ObjectManagerMap *GetObjectManagerMap()
        {
            if(!object_manager_map)
                object_manager_map=new ObjectManagerMap;

            return(object_manager_map);
        }
    }//namespace

    bool RegistryObjectManager(ObjectManager *om)
    {
        if(!om)
            return(false);

        const size_t hc=om->GetTypeHash();

        auto *omm=GetObjectManagerMap();

        if(omm->contains(hc))
            return(false);

        omm->insert({hc,om});

        //std::cout<<"RegistryObjectManager("<<std::hex<<hc<<":"<<om->GetTypename()<<")"<<std::endl;

        return(true);
    }

    void UnregistryObjectManager(ObjectManager *om)
    {
        if(!om)
            return;

        const size_t hc=om->GetTypeHash();

        auto *omm=GetObjectManagerMap();

        auto omi=omm->find(hc);

        if(omi.key()!=hc)
            return;

        //std::cout<<"UnregistryObjectManager("<<std::hex<<hc<<":"<<om->GetTypename()<<")"<<std::endl;

        omm->erase(hc);
    }

    ObjectManager *GetObjectManager(const size_t &hash_code)
    {
        auto *omm=GetObjectManagerMap();

        auto omi=omm->find(hash_code);

        if(omi.key()==hash_code)
            return(omi.value());
        else
            return(nullptr);
    }
}//namespace hgl
