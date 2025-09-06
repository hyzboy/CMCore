#pragma once

#include<hgl/type/Map.h>
#include<hgl/type/LifecycleTraits.h>
namespace hgl
{
    template<typename K,typename V> struct RefKeyValue:public KeyValue<K,V *>
    {
        int ref_count;
        RefKeyValue():KeyValue<K,V *>(),ref_count(1){}
    };

    template<typename K,typename V> class ObjectManageTemplate
    {
    public:
        using KVObject=RefKeyValue<K,V>;
    protected:
        MapTemplate<K,V *,KVObject> items;

    public:
        ObjectManageTemplate()=default;
        virtual ~ObjectManageTemplate(){ Clear(); }

        virtual void Clear()
        {
            int n=items.GetCount();
            while(n--)
            {
                KVObject *obj=items.GetItem(n);
                if(obj && obj->value)
                {
                    LifecycleTraitsOwningPtr<V>::destroy(&obj->value,1);
                }
            }
            items.Clear();
        }

        virtual void ClearFree()
        {
            int n=items.GetCount();
            while(n--)
            {
                KVObject *obj=items.GetItem(n);
                if(obj->ref_count<=0)
                {
                    if(obj->value)
                        LifecycleTraitsOwningPtr<V>::destroy(&obj->value,1);
                    items.DeleteAt(n);
                }
            }
        }

        const int GetCount()const{return items.GetCount();}

        virtual bool Add(const K &key,V *obj)
        {
            if(!obj) return false;
            if(items.ContainsKey(key)) return false;
            items.Add(key,obj); return true;
        }

        virtual V *Find(const K &key)
        {
            int index=items.Find(key);
            if(index==-1) return nullptr;
            KVObject *obj=items.GetItem(index); return obj->value;
        }

        virtual V *Get(const K &key)
        {
            int index=items.Find(key);
            if(index!=-1)
            {
                KVObject *obj=items.GetItem(index);
                ++obj->ref_count; return obj->value;
            }
            return nullptr;
        }

        virtual bool GetKeyByValue(V *value,K *key=nullptr,uint *count=nullptr,bool add_ref=false)
        {
            int idx=items.FindByValue(value);
            if(idx==-1) return false;
            KVObject *obj=items.GetItem(idx);
            if(key) *key=obj->key;
            if(count) *count=obj->ref_count;
            if(add_ref) ++obj->ref_count;
            return true;
        }

        virtual int ReleaseBySerial(int index,bool zero_clear=false)
        {
            if(index<0||index>=items.GetCount()) return -1;
            KVObject *obj=items.GetItem(index);
            --obj->ref_count;
            if(obj->ref_count<=0)
            {
                if(zero_clear && obj->value)
                    LifecycleTraitsOwningPtr<V>::destroy(&obj->value,1);
                if(zero_clear) items.DeleteAt(index);
            }
            return obj->ref_count;
        }

        virtual int Release(const K &key,bool zero_clear=false)
        { return ReleaseBySerial(items.Find(key),zero_clear); }

        int Release(V *td,bool zero_clear=false)
        { return ReleaseBySerial(items.FindByValue(td),zero_clear); }
    };

    template<typename K,typename V> class ObjectManage:public ObjectManageTemplate<K,V>
    {
    public:
        ObjectManage()=default; virtual ~ObjectManage()=default;
    };

    template<typename K,typename V> class IDObjectManage:public ObjectManage<K,V>
    {
        K id_count=0;
    public:
        using ObjectManage<K,V>::ObjectManage;
        virtual ~IDObjectManage()=default;

        virtual K Add(V *value)
        {
            if(!value) return K(-1);
            {
                K key; uint count; if(ObjectManage<K,V>::GetKeyByValue(value,&key,&count,true)) return key; }
            if(!ObjectManage<K,V>::Add(id_count,value)) return K(-1);
            return id_count++;
        }
    };

    template<typename V> using U32ObjectManage=IDObjectManage<uint32,V>;
    template<typename V> using U64ObjectManage=IDObjectManage<uint64,V>;
}//namespace hgl
