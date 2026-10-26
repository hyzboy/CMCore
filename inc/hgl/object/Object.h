#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/String.h>
#include<hgl/object/ObjectIdentity.h>
#include<hgl/log/Log.h>

namespace hgl
{
    /**
    * 对象基类
    *
    * 提供三件事：
    * 1. 身份：进程内唯一的 ObjectId + 由类型名推导的 ObjectTypeId(类型链可上溯)
    * 2. 生命周期：Init()/Release() 是非虚模板方法，派生类只实现 OnInit()/OnRelease()
    * 3. 日志：内置 ObjectLogger，Init() 成功后自动与对象身份绑定
    *
    * 派生类必须在类体内声明 HGL_OBJECT(类名,父类名)，以获得类型信息注册能力。
    *
    * @code
    * class MyResource:public hgl::Object
    * {
    *     HGL_OBJECT(MyResource,hgl::Object)
    *
    * protected:
    *
    *     bool OnInit() override
    *     {
    *         LogInfo(u8"申请资源");
    *         return(true);
    *     }
    *
    *     void OnRelease() override
    *     {
    *         LogInfo(u8"释放资源");
    *     }
    * };
    * @endcode
    *
    * 注意：派生类不要再使用 OBJECT_LOGGER 宏，否则会与本类的 Log 成员冲突。
    */
    class Object
    {
    public:     // 拷贝/移动语义

        NO_COPY(Object)
        NO_MOVE(Object)

    protected:  // 内置数据

        logger::ObjectLogger Log;                                   ///<内置对象日志器(Init后已绑定对象身份)

    private:    // 身份与状态

        ObjectId        object_id;                                  ///<对象唯一ID(构造时分配)
        ObjectState     object_state;                               ///<对象生命周期状态

    protected:  // 派生类接口

        /**
        * 初始化钩子：派生类在此申请资源
        * @return true 初始化成功；false 初始化失败(对象进入InitFailed状态)
        */
        virtual bool OnInit()=0;

        /**
        * 释放钩子：派生类在此释放OnInit及其后申请的全部资源
        * @note 由Release()保证只调用一次，但实现仍应保持幂等
        */
        virtual void OnRelease()=0;

        /**
        * 生命周期事件通知(可选覆盖)：供诊断工具、对象查看器采集
        * @note 基类构造函数期间不会派发Construct事件(此时虚函数不可用)
        */
        virtual void OnLifecycleEvent(const ObjectLifecycleEvent){}

        /**
        * 派发生命周期事件
        */
        void NotifyLifecycle(const ObjectLifecycleEvent ev)
        {
            OnLifecycleEvent(ev);
        }

        /**
        * 将logger与对象身份同步(Init()时自动调用)
        */
        void BindLoggerIdentity();

    public:     // 构造/析构

        Object();
        virtual ~Object();

    public:     // 类型信息

        /**
        * 取本类的静态类型信息(由HGL_OBJECT宏覆盖)
        */
        static const ObjectTypeInfo *StaticTypeInfo();

        /**
        * 取当前对象(最派生类)的类型信息
        */
        virtual const ObjectTypeInfo *GetTypeInfo()const{return StaticTypeInfo();}

        const ObjectTypeId GetTypeId()const;

        const os_char *GetTypeName()const;

        /**
        * 判断当前对象是否是指定类型的实例(或其派生类实例)
        */
        template<typename T> const bool IsA()const
        {
            const ObjectTypeInfo *ti=GetTypeInfo();
            const ObjectTypeInfo *tti=T::StaticTypeInfo();

            return(ti&&tti?ti->IsDerivedFrom(tti->type_id):false);
        }

        /**
        * 安全的向下转换：类型不匹配时返回nullptr
        */
        template<typename T> T *Cast()
        {
            return IsA<T>()?static_cast<T *>(this):nullptr;
        }

        template<typename T> const T *Cast()const
        {
            return IsA<T>()?static_cast<const T *>(this):nullptr;
        }

    public:     // 身份信息

        const ObjectId GetObjectId()const{return object_id;}

        /**
        * 设置人类可读的实例名(不作为唯一标识，可随时修改)
        */
        void SetInstanceName(const OSString &name){Log.SetLoggerInstanceName(name);}

        const OSString &GetInstanceName()const{return Log.GetLoggerInstanceName();}

        logger::ObjectLogger &GetLogger(){return Log;}

        const logger::ObjectLogger &GetLogger()const{return Log;}

    public:     // 生命周期

        const ObjectState GetObjectState()const{return object_state;}

        const bool IsInitialized()const{return object_state==ObjectState::Initialized;}

        const bool IsReleased()const{return object_state==ObjectState::Released;}

        /**
        * 初始化对象(幂等：已初始化时直接返回true)
        * @return true 初始化成功
        */
        bool Init();

        /**
        * 释放对象(幂等：已释放时直接返回)
        */
        void Release();
    };//class Object

    /**
    * 声明对象类型信息，必须在继承自Object的类体内使用
    * @param CLASS_NAME 本类名(同时作为类型名参与类型ID计算，需进程内唯一)
    * @param SUPER_CLASS_NAME 父类名(其StaticTypeInfo()将被注册为父类型)
    * @note 必须写在类的 public: 区段内，否则 IsA<T>()/Cast<T>() 无法访问目标类型的 StaticTypeInfo()
    */
    #define HGL_OBJECT(CLASS_NAME,SUPER_CLASS_NAME)                             \
        using ObjectSuper=SUPER_CLASS_NAME;                                     \
                                                                                \
        static const ::hgl::ObjectTypeInfo *StaticTypeInfo()                    \
        {                                                                       \
            static const ::hgl::ObjectTypeInfo *info=                           \
                ::hgl::RegisterObjectType(OS_TEXT(#CLASS_NAME),                 \
                                          SUPER_CLASS_NAME::StaticTypeInfo());  \
            return(info);                                                       \
        }                                                                       \
                                                                                \
        const ::hgl::ObjectTypeInfo *GetTypeInfo()const override                 \
        {                                                                       \
            return(StaticTypeInfo());                                           \
        }

    /**
    * 声明对象类型信息，并指定该类型日志类别的默认输出级别
    */
    #define HGL_OBJECT_WITH_LEVEL(CLASS_NAME,SUPER_CLASS_NAME,DEFAULT_LEVEL)    \
        using ObjectSuper=SUPER_CLASS_NAME;                                     \
                                                                                \
        static const ::hgl::ObjectTypeInfo *StaticTypeInfo()                    \
        {                                                                       \
            static const ::hgl::ObjectTypeInfo *info=                           \
                ::hgl::RegisterObjectType(OS_TEXT(#CLASS_NAME),                 \
                                          SUPER_CLASS_NAME::StaticTypeInfo(),   \
                                          DEFAULT_LEVEL);                       \
            return(info);                                                       \
        }                                                                       \
                                                                                \
        const ::hgl::ObjectTypeInfo *GetTypeInfo()const override                 \
        {                                                                       \
            return(StaticTypeInfo());                                           \
        }
}//namespace hgl
