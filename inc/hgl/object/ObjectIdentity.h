#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/String.h>
#include<hgl/log/LogCategory.h>
#include<hgl/log/LogLevel.h>

namespace hgl
{
    /**
    * 对象唯一标识(进程内唯一，单调递增分配)
    */
    using ObjectId=uint64;

    /**
    * 对象类型标识(由类型名 FNV1a 计算，跨平台稳定)
    */
    using ObjectTypeId=uint32;

    constexpr ObjectId     InvalidObjectId    =0;
    constexpr ObjectTypeId InvalidObjectTypeId=0;

    /**
    * 对象生命周期状态
    */
    enum class ObjectState:uint8
    {
        Constructed=0,      ///<已构造，尚未初始化
        Initializing,       ///<正在初始化
        Initialized,        ///<初始化完成，可正常使用
        InitFailed,         ///<初始化失败
        Releasing,          ///<正在释放
        Released,           ///<已释放(可再次Init)
    };//enum class ObjectState

    const os_char *ToString(ObjectState);

    /**
    * 对象生命周期事件(用于诊断/查看器)
    */
    enum class ObjectLifecycleEvent:uint8
    {
        Construct=0,
        InitBegin,
        InitSuccess,
        InitFailed,
        PayloadLoadBegin,
        PayloadLoaded,
        PayloadLoadFailed,
        PayloadUnloadBegin,
        PayloadUnloaded,
        ReleaseBegin,
        ReleaseEnd,
        DestructWithoutRelease,
        Destruct,
    };//enum class ObjectLifecycleEvent

    const os_char *ToString(ObjectLifecycleEvent);

    /**
    * 对象类型信息(静态，生命周期等同进程)
    */
    struct ObjectTypeInfo
    {
        ObjectTypeId            type_id    =InvalidObjectTypeId;              ///<类型ID(FNV1a of type_name)
        const os_char *         type_name  =nullptr;                          ///<类型名
        const ObjectTypeInfo *  parent_type=nullptr;                          ///<父类型(根类型为空)
        logger::LogCategoryId   category_id=logger::InvalidLogCategoryId;     ///<绑定的日志类别

        bool IsValid()const{return type_id!=InvalidObjectTypeId;}

        /**
        * 判断本类型是否派生自指定类型
        */
        bool IsDerivedFrom(const ObjectTypeId other)const
        {
            if(other==InvalidObjectTypeId)
                return(false);

            for(const ObjectTypeInfo *cur=this;cur;cur=cur->parent_type)
                if(cur->type_id==other)
                    return(true);

            return(false);
        }

        /**
        * 判断本类型是否派生自指定类型
        */
        bool IsDerivedFrom(const ObjectTypeInfo *other)const
        {
            return(other?IsDerivedFrom(other->type_id):false);
        }
    };//struct ObjectTypeInfo

    /**
    * 计算类型名对应的对象类型ID
    */
    ObjectTypeId ComputeObjectTypeId(const os_char *type_name);

    /**
    * 注册对象类型(幂等：同名类型重复注册返回同一份ObjectTypeInfo)
    * @param type_name 类型名(必须为进程生命周期内稳定的字符串常量)
    * @param parent_type 父类型信息(根类型传nullptr)
    * @param default_level 该类型日志类别的默认输出级别
    * @return 类型信息指针，参数非法时返回nullptr
    */
    const ObjectTypeInfo *RegisterObjectType(const os_char *type_name,
                                            const ObjectTypeInfo *parent_type,
                                            const LogLevel default_level=LogLevel::Verbose);

    /**
    * 按类型ID查找已注册的对象类型信息
    */
    const ObjectTypeInfo *FindObjectType(const ObjectTypeId);

    /**
    * 按类型名查找已注册的对象类型信息
    */
    const ObjectTypeInfo *FindObjectType(const os_char *type_name);

    /**
    * 取已注册对象类型总数
    */
    const uint GetObjectTypeCount();

    /**
    * 分配一个进程内唯一的对象ID(从1开始单调递增)
    */
    ObjectId AllocateObjectId();
}//namespace hgl
