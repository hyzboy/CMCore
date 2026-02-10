#pragma once

#include<hgl/type/Collection.h>

namespace hgl
{
    // AI NOTE: Typed wrapper over Collection that forwards to void* storage.
    // Provides typed Add/Get/Set without changing Collection's layout.
    /**
    * 元素合集模板是Collection类的一个模板化版本，它的数据计数和长度都是以模板类型为基准。
    */
    template<typename T> class TypedCollection:public Collection
    {
    public:

                T *ToArray()        {return Collection::ToArray<T>();}
        const   T *ToArray()const   {return Collection::ToArray<T>();}

                T *begin()          {return (T *)Collection::begin();}
                T *end()            {return (T *)Collection::end();}

    public:

        TypedCollection(MemoryBlock *mb=new MemoryBlock):Collection(sizeof(T),mb){}
        virtual ~TypedCollection()=default;

        bool Add(const T &element){return Collection::Add(&element);}

        bool Get(const uint64 index,T &element)const{return Collection::Get(index,&element);}
        bool Set(const uint64 index,const T &element){return Collection::Set(index,&element);}

        bool Insert(const uint64 offset,const T &element){return Collection::Insert(offset,&element);}

        T *     Map(const uint64,const uint64 range){return (T *)Collection::Map(range,range);}

        int64   indexOf (const T &value)const{return Collection::indexOfValue<T>(&cee);}
        bool    Contains(const T &value)const{return Collection::ContainsValue<T>(value);}

        int64   Remove(const T &value,int max_count=1)
        {
            return Collection::RemoveValue(condition,max_count);
        }
    };//class TypedCollection
}//namespace hgl
