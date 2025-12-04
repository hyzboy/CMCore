#pragma once
#include<hgl/type/Smart.h>
#include<hgl/Comparator.h>
#include<hgl/type/object/ObjectBaseInfo.h>
#include<source_location>

#include<tsl/robin_map.h>

namespace hgl
{
    /**
     * 基础对象
     */
    class Object
    {
        ObjectBaseInfo object_info;

    protected:

        tsl::robin_map<size_t,void *> inherit_class_map;        ///<继承类的hash值与this指针的映射表

        bool RegisterInheritClass(const size_t &hash_code,void *class_this)noexcept
        {
            if(inherit_class_map.find(hash_code)!=inherit_class_map.end())
                return(false);

            inherit_class_map[hash_code]=class_this;

            return(true);
        }

        template<typename T> bool RegisterInheritClass(T *class_this)noexcept
        {
            return RegisterInheritClass(hgl::GetTypeHash<T>(),class_this);
        }

    public:

        const ObjectBaseInfo &  GetObjectBaseInfo   () const noexcept { return object_info; }                           ///<获取对象简单信息

        virtual const size_t    GetTypeHash         () const noexcept { return object_info.info->hash_code(); }         ///<获取对象数据类型的hash值
        virtual const char *    GetTypeName         () const noexcept { return object_info.info->name(); }              ///<获取对象数据类型的名称
                const size_t    GetUniqueID         () const noexcept { return object_info.unique_id; }                 ///<获取对象的唯一序列号

        template<typename T> T *Cast()
        {
            auto it=inherit_class_map.find(hgl::GetTypeHash<T>());

            if(it!=inherit_class_map.end())
                return (T *)it->second;

            return nullptr;
        }

        template<typename T> operator T *(){return Cast<T>();}

    public:

        NO_COPY(Object)
        NO_MOVE(Object)

        Object(const ObjectBaseInfo &oi) noexcept { object_info=oi; }
        virtual ~Object()=default;

        bool operator == (const Object &obj) const noexcept
        {
            if(GetUniqueID()!=obj.GetUniqueID())
                return(false);

            //为什么GetTypeHash可以比较

            // 因为GetTypeHash获取的是object_info中的TypeHash，而不是inherit_class_map中的TypeHash
            // 只有NewObjectSCL函数在创建对象时会产生object_info。

            if(GetTypeHash()!=obj.GetTypeHash())
                return(false);

            return(true);
        }
    };//class Object

#define HGL_OBJECT_BODY(class_name)  public:\
        static  const size_t StaticHash()noexcept { return typeid(class_name).hash_code(); } \
        static  const char * StaticName()noexcept { return typeid(class_name).name(); } \
        virtual const size_t GetTypeHash()const noexcept override { return class_name::StaticHash();} \
        virtual const char * GetTypeName()const noexcept override { return class_name::StaticName();}

#define HGL_OBJECT_CONSTRUCT(class_name)                class_name(const ObjectBaseInfo &obi):Object(obi){}
#define HGL_OBJECT_CONSTRUCT_SC(class_name,super_class)    class_name(const ObjectBaseInfo &obi):super_class(obi){}

    template<typename T> inline T *NewObjectSCL(const std::source_location scl=std::source_location::current())
    {
        static size_t new_count=0;

        ObjectBaseInfo obi;

        obi.info        =&typeid(T);
        obi.unique_id   =new_count;
        obi.scl         =scl;

        ++new_count;

        T *obj=new T(obi);

        return obj;
    }

    #define NewObject(type) NewObjectSCL<type>(std::source_location::current())
}//namespace hgl
