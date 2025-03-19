#pragma once
#include<hgl/type/DataType.h>
#include<hgl/type/object/ObjectBaseInfo.h>

namespace hgl
{
    /**
     * 基础对象
     */
    class Object
    {
        ObjectBaseInfo object_info;

    public:

        const ObjectBaseInfo &  GetObjectBaseInfo   () const noexcept { return object_info; }                          ///<获取对象简单信息

        const size_t            GetTypeHash         () const noexcept { return object_info.hash_code; }                ///<获取对象数据类型的hash值
        const size_t            GetUniqueID         () const noexcept { return object_info.unique_id; }                ///<获取对象的唯一序列号

    public:

        NO_COPY(Object)
        NO_MOVE(Object)

        Object(const ObjectBaseInfo &oi) noexcept { object_info=oi; }
        virtual ~Object()=default;
    };//class Object

    template<typename T> inline T *New(const SourceCodeLocation &scl)
    {
        static size_t new_count=0;

        ObjectBaseInfo obi;

        obi.hash_code=GetTypeHash<T>();
        obi.unique_id=new_count;
        obi.scl=scl;

        ++new_count;

        T *obj=new T(obi);

        return obj;
    }
}//namespace hgl
