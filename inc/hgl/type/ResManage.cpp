#ifndef HGL_RES_MANAGE_CPP
#define HGL_RES_MANAGE_CPP

#include<hgl/type/ResManage.h>
namespace hgl
{
    template<typename K,typename V>
    ResManage<K,V>::~ResManage()
    {
        Clear();
    }

    template<typename K,typename V>
    void ResManage<K,V>::Clear()
    {
        int n=items.GetCount();

        while(n--)
        {
            ResItem *obj=items.GetItem(n);

            Clear(obj->value);
        }

        items.Clear();
    }

    template<typename K,typename V>
    void ResManage<K,V>::ClearFree()
    {
        int n=items.GetCount();

        while(n--)
        {
            ResItem *obj=items.GetItem(n);

            if(obj->ref_count<=0)
            {
                Clear(obj->value);
                items.DeleteBySerial(n);
            }
        }
    }

    template<typename K,typename V>
    bool ResManage<K,V>::Add(const K &flag,V *obj)
    {
        if(!obj)return(false);

        if(items.KeyExist(flag))
            return(false);

        items.Add(flag,obj);
        return(true);
    }

    template<typename K,typename V>
    V *ResManage<K,V>::Find(const K &flag)
    {
        int index=items.Find(flag);

        if(index==-1)
            return(nullptr);

        ResItem *obj=items.GetItem(index);

        return obj->value;
    }

    template<typename K,typename V>
    V *ResManage<K,V>::Get(const K &flag)
    {
        int index=items.Find(flag);

        if(index!=-1)
        {
            ResItem *obj=items.GetItem(index);

            ++obj->ref_count;

            return obj->value;
        }

        return(nullptr);
    }

    /**
     * 确认指定数据是否存在
     */
    template<typename K,typename V>
    bool ResManage<K,V>::ValueExist(V *value)
    {
        return(items.FindByValue(value)!=-1);
    }

    /**
     * 获取指定数据的Key和引用计数
     * @param value 数据
     * @param key Key存放地址
     * @param ref_count 引用计数存放地址
     * @param 是否增加引用计数
     */
    template<typename K,typename V>
    bool ResManage<K,V>::GetKeyByValue(V *value,K *key,uint *ref_count,bool inc_ref_count)
    {
        int index=items.FindByValue(value);

        if(index==-1)return(false);

        ResItem *obj=items.GetItem(index);

        if(inc_ref_count)
            ++obj->ref_count;

        if(key)
            *key=obj->key;

        if(ref_count)
            *ref_count=obj->ref_count;

        return(true);
    }

    template<typename K,typename V>
    int ResManage<K,V>::ReleaseBySerial(int index,bool zero_clear)
    {
        if(index==-1)
        {
//          ErrorHint(u"所释放的资源不存在");
            return(-1);
        }

        ResItem *obj=items.GetItem(index);

        --obj->ref_count;

        if(obj->ref_count>0)
            return obj->ref_count;

        if(zero_clear)
        {
            Clear(obj->value);

            items.DeleteBySerial(index);
        }

        return 0;
    }

    template<typename K,typename V>
    int ResManage<K,V>::Release(const K &flag,bool zero_clear)
    {
        return ReleaseBySerial(items.Find(flag),zero_clear);
    }

    template<typename K,typename V>
    int ResManage<K,V>::Release(V *td,bool zero_clear)
    {
        return ReleaseBySerial(items.FindByValue(td),zero_clear);
    }
}//namespace hgl
#endif//HGL_RES_MANAGE_CPP
