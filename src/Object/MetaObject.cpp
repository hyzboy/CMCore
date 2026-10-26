#include<hgl/object/MetaObject.h>

namespace hgl
{
    const os_char *ToString(const PayloadState state)
    {
        switch(state)
        {
            case PayloadState::MetaOnly:    return(OS_TEXT("MetaOnly"));
            case PayloadState::Loading:     return(OS_TEXT("Loading"));
            case PayloadState::Resident:    return(OS_TEXT("Resident"));
            case PayloadState::Unloading:   return(OS_TEXT("Unloading"));
            case PayloadState::LoadFailed:  return(OS_TEXT("LoadFailed"));
            case PayloadState::Disposed:    return(OS_TEXT("Disposed"));
        }

        return(OS_TEXT("Unknown"));
    }

    MetaObject::MetaObject()
        :payload_state(PayloadState::MetaOnly)
    {
        // 引用归零只卸载负载，保留meta壳
        SetRefZeroPolicy(RefZeroPolicy::KeepMetaUnloadPayload);
    }

    bool MetaObject::OnInit()
    {
        payload_state=PayloadState::MetaOnly;

        return(OnInitMeta());
    }

    void MetaObject::OnRelease()
    {
        UnloadPayload();

        OnReleaseMeta();

        payload_state=PayloadState::Disposed;
    }

    bool MetaObject::LoadPayload()
    {
        if(payload_state==PayloadState::Resident)
            return(true);

        if(payload_state==PayloadState::Loading
         ||payload_state==PayloadState::Unloading)
            return(false);

        if(!IsInitialized())
        {
            Log.Warning(std::source_location::current(),
                        u8"LoadPayload() 前必须先 Init()：type=%s,id=%llu,state=%s",
                        GetTypeName(),GetObjectId(),ToString(GetObjectState()));
            return(false);
        }

        payload_state=PayloadState::Loading;

        NotifyLifecycle(ObjectLifecycleEvent::PayloadLoadBegin);

        if(OnLoadPayload())
        {
            payload_state=PayloadState::Resident;
            NotifyLifecycle(ObjectLifecycleEvent::PayloadLoaded);
            return(true);
        }

        payload_state=PayloadState::LoadFailed;
        NotifyLifecycle(ObjectLifecycleEvent::PayloadLoadFailed);
        return(false);
    }

    void MetaObject::UnloadPayload()
    {
        if(payload_state!=PayloadState::Resident)
            return;

        payload_state=PayloadState::Unloading;

        NotifyLifecycle(ObjectLifecycleEvent::PayloadUnloadBegin);

        OnUnloadPayload();

        payload_state=PayloadState::MetaOnly;

        NotifyLifecycle(ObjectLifecycleEvent::PayloadUnloaded);
    }
}//namespace hgl
