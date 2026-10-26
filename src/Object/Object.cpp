#include<hgl/object/Object.h>

namespace hgl
{
    const ObjectTypeInfo *Object::StaticTypeInfo()
    {
        static const ObjectTypeInfo *info=RegisterObjectType(OS_TEXT("hgl::Object"),nullptr);

        return(info);
    }

    Object::Object()
        :object_id(AllocateObjectId()),
         object_state(ObjectState::Constructed)
    {
        // 构造期先绑定对象ID，类型信息要等最派生类构造完成后的Init()再补
        Log.BindObjectIdentity(object_id,InvalidObjectTypeId);
    }

    Object::~Object()
    {
        if(object_state==ObjectState::Initialized
         ||object_state==ObjectState::InitFailed)
        {
            // 析构期不能调用虚函数OnRelease()(派生类部分已销毁)，只能报告违规
            Log.Error(std::source_location::current(),
                      u8"对象析构前未调用Release()：type=%s,id=%llu",
                      GetTypeName(),object_id);
        }
    }

    const ObjectTypeId Object::GetTypeId()const
    {
        const ObjectTypeInfo *info=GetTypeInfo();

        return(info?info->type_id:InvalidObjectTypeId);
    }

    const os_char *Object::GetTypeName()const
    {
        const ObjectTypeInfo *info=GetTypeInfo();

        return(info&&info->type_name?info->type_name:OS_TEXT("hgl::Object"));
    }

    void Object::BindLoggerIdentity()
    {
        const ObjectTypeInfo *info=GetTypeInfo();

        if(!info)
        {
            Log.BindObjectIdentity(object_id,InvalidObjectTypeId);
            return;
        }

        Log.BindObjectIdentity(object_id,info->type_id,info->category_id,info->type_name);
    }

    bool Object::Init()
    {
        if(object_state==ObjectState::Initialized)
            return(true);

        if(object_state==ObjectState::Initializing)                 // 重入保护：不做嵌套初始化
            return(false);

        object_state=ObjectState::Initializing;

        BindLoggerIdentity();

        NotifyLifecycle(ObjectLifecycleEvent::InitBegin);

        if(OnInit())
        {
            object_state=ObjectState::Initialized;
            NotifyLifecycle(ObjectLifecycleEvent::InitSuccess);
            return(true);
        }

        object_state=ObjectState::InitFailed;
        NotifyLifecycle(ObjectLifecycleEvent::InitFailed);
        return(false);
    }

    void Object::Release()
    {
        if(object_state==ObjectState::Released
         ||object_state==ObjectState::Releasing)
            return;

        const bool need_release=(object_state==ObjectState::Initialized
                               ||object_state==ObjectState::InitFailed);

        object_state=ObjectState::Releasing;

        NotifyLifecycle(ObjectLifecycleEvent::ReleaseBegin);

        if(need_release)
            OnRelease();

        object_state=ObjectState::Released;

        NotifyLifecycle(ObjectLifecycleEvent::ReleaseEnd);
    }
}//namespace hgl

