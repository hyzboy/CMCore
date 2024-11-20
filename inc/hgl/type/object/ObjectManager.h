#pragma once
#include"Object.h"
#include"ObjectAllocator.h"
#include<tsl/robin_set.h>
#include<tsl/robin_map.h>

namespace hgl
{
    class Object;

    /**
     * 对象管理器基类.
     */
    class ObjectManager
    {
        size_t      object_hash_code;
        const char *object_type_name;
        size_t      object_count;

    protected:

        const size_t AcquireSerialNumber(){return ++object_count;}

    public:

        const size_t GetTypeHash()const{return object_hash_code;}
        const char * GetTypename()const{return object_type_name;}
        const size_t GetCount()const{return object_count;}

    public:

        ObjectManager(const size_t &hc,const char *tn)
        {
            object_hash_code=hc;
            object_type_name=tn;
            object_count=0;
        }

        virtual ~ObjectManager()=default;
    };//class ObjectManager

    bool RegistryObjectManager(ObjectManager *om);                              ///<注册一个对象管理器
    void UnregistryObjectManager(ObjectManager *om);                            ///<注销一个对象管理器

    /**
     * 缺省对象管理器
     */
    template<typename T> class DefaultObjectManager:public ObjectManager
    {
        tsl::robin_set<T *> object_set;
        tsl::robin_map<size_t,SafePtrData<T> *> object_map;

        ObjectAllocator *object_allocator;

    public:

        DefaultObjectManager(ObjectAllocator *oa):ObjectManager(typeid(T).hash_code(),typeid(T).name())
        {
            object_allocator=oa;

            object_allocator->Join(this);
            RegistryObjectManager(this);
        }

        virtual ~DefaultObjectManager()
        {
            if(object_set.size())
            {
                if(object_allocator->PreUnjoin(object_set.size()))      //这里返回的是是否需要逐个释放
                {
                    for(auto &it:object_set)
                    {
                        object_allocator->CleanNotReleaseObject(it);
                    }
                }
            }

            object_allocator->Unjoin(this);
            UnregistryObjectManager(this);
        }

    protected:

        template<typename T,typename ...ARGS> friend SafePtr<T> DefaultCreateObject(const char *source_file,const char *source_function,const size_t source_line,ARGS...args);

        template<typename ...ARGS>
        SafePtrData<T> *CreateObject(const SourceCodeLocation &scl,ARGS...args)
        {
            ObjectBaseInfo obi;

            obi.hash_code      =GetTypeHash();
            obi.object_manager =this;
            obi.serial_number  =AcquireSerialNumber();
            obi.source_code_location=scl;

            Object *obj=object_allocator->Create(obi);

            SafePtrData<T> *spd=new SafePtrData<T>((T *)obj);

            object_set.insert({(T *)obj});
            object_map.insert({obj->GetSerialNumber(),spd});

            spd->ptr->Initailize(args...);

            return spd;
        }

        template<typename T> friend class SafePtr;

        void ReleaseObject(SafePtrData<T> *spd)
        {
            if(!spd)
                return;

            object_map.erase(spd->ptr->GetSerialNumber());
            object_set.erase(spd->ptr);

            if(spd->ptr)
            {
                spd->ptr->Deinitailize();
                object_allocator->Release(spd->ptr);
                spd->ptr=nullptr;
            }

            if(spd->count>1)
            {
                --spd->count;
            }
            else
            {
                delete spd;
            }
        }

        template<typename T> friend SafePtr<T> GetObjectBySerial(const size_t &serial);

        SafePtrData<T> *GetObjectBySerial(const size_t &serial)
        {
            return object_map.at(serial);
        }
    };//class DefaultObjectManager

    #define HGL_DEFINE_DEFAULT_OBJECT_MANAGER(T) namespace \
    {   \
        static DefaultObjectAllocator<T> T##ObjectAllocator; \
        static DefaultObjectManager<T> T##ObjectManager(&T##ObjectAllocator); \
    }

    ObjectManager *GetObjectManager(const size_t &hash_code);                                       ///<取得一个对象管理器

    template<typename T> inline ObjectManager *GetObjectManager()                                   ///<取得一个对象管理器
    {
        return GetObjectManager(typeid(T).hash_code());
    }

    template<typename T> inline SafePtr<T> GetObjectBySerial(const size_t &serial)                  ///<通过序列号取得一个对象
    {
        ObjectManager *om=GetObjectManager<T>();

        if(!om)
            return SafePtr<T>();

        DefaultObjectManager<T> *dom=static_cast<DefaultObjectManager<T> *>(om);

        return SafePtr<T>(dom->GetObjectBySerial(serial));
    }
}//namespace hgl

