#include<hgl/object/ObjectIdentity.h>
#include<hgl/util/hash/FNV1a.h>
#include<hgl/sync/Mutex.h>
#include<hgl/sync/Atomic.h>
#include<hgl/Charset.h>
#include<hgl/log/LogCategory.h>

namespace hgl
{
    const os_char *ToString(const ObjectState state)
    {
        switch(state)
        {
            case ObjectState::Constructed:  return OS_TEXT("Constructed");
            case ObjectState::Initializing: return OS_TEXT("Initializing");
            case ObjectState::Initialized:  return OS_TEXT("Initialized");
            case ObjectState::InitFailed:   return OS_TEXT("InitFailed");
            case ObjectState::Releasing:    return OS_TEXT("Releasing");
            case ObjectState::Released:     return OS_TEXT("Released");
        }

        return OS_TEXT("Unknown");
    }

    const os_char *ToString(const ObjectLifecycleEvent event)
    {
        switch(event)
        {
            case ObjectLifecycleEvent::Construct:               return OS_TEXT("Construct");
            case ObjectLifecycleEvent::InitBegin:               return OS_TEXT("InitBegin");
            case ObjectLifecycleEvent::InitSuccess:             return OS_TEXT("InitSuccess");
            case ObjectLifecycleEvent::InitFailed:              return OS_TEXT("InitFailed");
            case ObjectLifecycleEvent::PayloadLoadBegin:        return OS_TEXT("PayloadLoadBegin");
            case ObjectLifecycleEvent::PayloadLoaded:           return OS_TEXT("PayloadLoaded");
            case ObjectLifecycleEvent::PayloadLoadFailed:       return OS_TEXT("PayloadLoadFailed");
            case ObjectLifecycleEvent::PayloadUnloadBegin:      return OS_TEXT("PayloadUnloadBegin");
            case ObjectLifecycleEvent::PayloadUnloaded:         return OS_TEXT("PayloadUnloaded");
            case ObjectLifecycleEvent::ReleaseBegin:            return OS_TEXT("ReleaseBegin");
            case ObjectLifecycleEvent::ReleaseEnd:              return OS_TEXT("ReleaseEnd");
            case ObjectLifecycleEvent::DestructWithoutRelease:  return OS_TEXT("DestructWithoutRelease");
            case ObjectLifecycleEvent::Destruct:                return OS_TEXT("Destruct");
        }

        return OS_TEXT("Unknown");
    }

    namespace
    {
        /**
        * 对象类型注册表容量。
        * 使用静态存储的原因：ObjectTypeInfo 的指针会被各类型的 HGL_OBJECT 宏长期持有，
        * 必须保证地址稳定；且注册可能发生在任意翻译单元的静态初始化阶段，
        * 此阶段函数内静态对象的构造时机不可依赖，故存储本身保持零初始化POD形态。
        */
        constexpr uint OBJECT_TYPE_REGISTRY_CAPACITY=1024;

        ObjectTypeInfo ObjectTypeRegistry[OBJECT_TYPE_REGISTRY_CAPACITY];
        uint           ObjectTypeCount=0;

        hgl::atom_uint64 GlobalObjectId=0;

        Mutex *GetObjectTypeLock()
        {
            static Mutex lock;
            return &lock;
        }
    }//namespace

    ObjectTypeId ComputeObjectTypeId(const os_char *type_name)
    {
        if(!type_name||!*type_name)
            return InvalidObjectTypeId;

        // 统一按 UTF-8 字节计算，保证不同平台上 os_char 宽度差异不影响结果
        const U8String u8_name=ToU8String(type_name);
        const uint32 length=static_cast<uint32>(u8_name.Length());

        if(length==0)
            return InvalidObjectTypeId;

        uint32 hash=hash::FNV1aInit<uint32>();

        hash=hash::FNV1aAppendValueBytes(hash,length);
        hash=hash::FNV1aAppendBytes(hash,u8_name.c_str(),length);

        return hash?hash:1;
    }

    const ObjectTypeInfo *RegisterObjectType(const os_char *type_name,
                                            const ObjectTypeInfo *parent_type,
                                            const LogLevel default_level)
    {
        if(!type_name||!*type_name)
            return(nullptr);

        const ObjectTypeId type_id=ComputeObjectTypeId(type_name);

        if(type_id==InvalidObjectTypeId)
            return(nullptr);

        MutexLock lock(GetObjectTypeLock());

        for(uint i=0;i<ObjectTypeCount;i++)
            if(ObjectTypeRegistry[i].type_id==type_id)
                return(ObjectTypeRegistry+i);

        if(ObjectTypeCount>=OBJECT_TYPE_REGISTRY_CAPACITY)
            return(nullptr);

        ObjectTypeInfo *info=ObjectTypeRegistry+ObjectTypeCount;

        info->type_id     =type_id;
        info->type_name   =type_name;
        info->parent_type =parent_type;
        info->category_id =logger::RegisterLogCategory(OSString(type_name),default_level);

        ++ObjectTypeCount;

        return info;
    }

    const ObjectTypeInfo *FindObjectType(const ObjectTypeId type_id)
    {
        if(type_id==InvalidObjectTypeId)
            return(nullptr);

        MutexLock lock(GetObjectTypeLock());

        for(uint i=0;i<ObjectTypeCount;i++)
            if(ObjectTypeRegistry[i].type_id==type_id)
                return(ObjectTypeRegistry+i);

        return(nullptr);
    }

    const ObjectTypeInfo *FindObjectType(const os_char *type_name)
    {
        return FindObjectType(ComputeObjectTypeId(type_name));
    }

    const uint GetObjectTypeCount()
    {
        MutexLock lock(GetObjectTypeLock());

        return ObjectTypeCount;
    }

    ObjectId AllocateObjectId()
    {
        return ++GlobalObjectId;
    }
}//namespace hgl
