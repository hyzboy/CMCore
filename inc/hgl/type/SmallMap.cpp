#pragma once

#include <hgl/type/SmallMap.h>

namespace hgl
{
    template<typename K,typename V>
    int SmallMap<K,V>::Find(const K &key) const
    {
        int left = 0,right = data_list.GetCount() - 1;
        if(right < 0) return -1;
        const KVData *arr = data_list.GetData();
        while(left <= right)
        {
            int mid = left + (right - left) / 2;
            const K &mk = arr[mid].key;
            if(mk == key) return mid;
            if(mk < key) left = mid + 1; else right = mid - 1;
        }
        return -1;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::FindPos(const K &key,int &pos) const
    {
        const int count = data_list.GetCount();
        if(count == 0) { pos = 0; return false; }
        const KVData *arr = data_list.GetData();
        int left = 0,right = count - 1;
        while(left <= right)
        {
            int mid = left + (right - left) / 2;
            const K &mk = arr[mid].key;
            if(mk == key) { pos = mid; return true; }
            if(mk < key) left = mid + 1; else right = mid - 1;
        }
        pos = left; // lower_bound
        return false;
    }

    template<typename K,typename V>
    int SmallMap<K,V>::FindByValue(const V &value) const
    {
        const int count = data_list.GetCount();
        if(count <= 0) return -1;
        const KVData *arr = data_list.GetData();
        for(int i = 0; i < count; ++i)
        {
            if(hgl_cmp(arr[i].value,value) == 0) return i;
        }
        return -1;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::Check(const K &key,const V &value) const
    {
        int idx = Find(key);
        if(idx < 0) return false;
        const KVData *arr = data_list.GetData();
        return arr[idx].value == value;
    }

    template<typename K,typename V>
    typename SmallMap<K,V>::KVData *SmallMap<K,V>::Add(const K &key,const V &value)
    {
        int pos;
        if(FindPos(key,pos)) return nullptr; // 已存在
        KVData kv{ key,value };
        data_list.Insert(pos,kv);
        return data_list.At(pos);
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::Delete(const K &key,V &out_value)
    {
        int idx = Find(key);
        if(idx < 0) return false;
        out_value = data_list.At(idx)->value;
        data_list.DeleteShift(idx);
        return true;
    }

    template<typename K,typename V>
    int SmallMap<K,V>::DeleteByKey(const K *keys,int count)
    {
        if(!keys || count <= 0) return 0;
        int total = 0;
        for(int i = 0; i < count; ++i)
        {
            if(DeleteByKey(keys[i])) ++total;
        }
        return total;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::DeleteAt(int index)
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        data_list.DeleteShift(index);
        return true;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::DeleteAt(int start,int number)
    {
        return data_list.Delete(start,number);
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::ChangeOrAdd(const K &key,const V &value)
    {
        int pos;
        if(FindPos(key,pos))
        {
            data_list.At(pos)->value = value;
            return true;
        }
        KVData kv{ key,value };
        data_list.Insert(pos,kv);
        return true;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::Change(const K &key,const V &value)
    {
        int idx = Find(key);
        if(idx < 0) return false;
        data_list.At(idx)->value = value;
        return true;
    }

    template<typename K,typename V>
    V *SmallMap<K,V>::GetValuePointer(const K &key) const
    {
        int idx = Find(key);
        if(idx < 0) return nullptr;
        const KVData *arr = data_list.GetData();
        // 注意：返回的指针在容器变动后可能失效
        return const_cast<V *>(&(arr[idx].value));
    }

    template<typename K,typename V>
    int SmallMap<K,V>::GetValueAndSerial(const K &key,V &out) const
    {
        int idx = Find(key);
        if(idx < 0) return -1;
        const KVData *arr = data_list.GetData();
        out = arr[idx].value;
        return idx;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::GetBySerial(int index,K &out_key,V &out_value) const
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        const KVData *arr = data_list.GetData();
        out_key = arr[index].key;
        out_value = arr[index].value;
        return true;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::GetKey(int index,K &out_key) const
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        const KVData *arr = data_list.GetData();
        out_key = arr[index].key;
        return true;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::GetValue(int index,V &out_value) const
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        const KVData *arr = data_list.GetData();
        out_value = arr[index].value;
        return true;
    }

    template<typename K,typename V>
    bool SmallMap<K,V>::SetValueBySerial(int index,const V &value)
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        data_list.At(index)->value = value;
        return true;
    }

    template<typename K,typename V>
    void SmallMap<K,V>::Enum(void (*enum_func)(const K &,V &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i) enum_func(p[i].key,p[i].value);
    }

    template<typename K,typename V>
    void SmallMap<K,V>::EnumKey(void (*enum_func)(const K &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i) enum_func(p[i].key);
    }

    template<typename K,typename V>
    void SmallMap<K,V>::EnumAllValue(void (*enum_func)(V &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i) enum_func(p[i].value);
    }

    template<typename K,typename V>
    void SmallMap<K,V>::EnumValue(bool (*enum_func)(V &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i)
        {
            if(!enum_func(p[i].value)) return;
        }
    }

    template<typename K,typename V>
    void SmallMap<K,V>::WithList(typename SmallMap<K,V>::KVDataList &with_list,const ArrayList<K> &in_list) const
    {
        with_list.Clear();
        const int count = GetCount();
        if(count <= 0) return;
        with_list.Reserve(count);
        const KVData *sp = data_list.GetData();
        for(int i = 0; i < count; ++i)
        {
            if(in_list.Contains(sp[i].key)) with_list.Add(sp[i]);
        }
    }

    template<typename K,typename V>
    void SmallMap<K,V>::WithoutList(typename SmallMap<K,V>::KVDataList &without_list,const ArrayList<K> &in_list) const
    {
        without_list.Clear();
        const int count = GetCount();
        if(count <= 0) return;
        without_list.Reserve(count);
        const KVData *sp = data_list.GetData();
        for(int i = 0; i < count; ++i)
        {
            if(!in_list.Contains(sp[i].key)) without_list.Add(sp[i]);
        }
    }
}
