#pragma once

#include <hgl/type/ValueKVMap.h>

namespace hgl
{
    template<typename K,typename V>
    int ValueKVMap<K,V>::Find(const K &key) const
    {
        const int count = data_list.GetCount();
        if(count <= 0) return -1;

        const KVData *arr = data_list.GetData();

        // 检查缓存位置
        if(cache_idx >= 0 && cache_idx < count && arr[cache_idx].key == key)
        {
            return cache_idx;
        }

        // 预计算查询键的哈希值
        const uint64_t key_hash = std::hash<K>{}(key);

        // 二分查找，加入哈希值预过滤
        int left = 0, right = count - 1;
        while(left <= right)
        {
            int mid = left + (right - left) / 2;
            const KVData &mkv = arr[mid];

            // 先比较哈希，哈希不等则键肯定不等
            if(mkv.hash == key_hash && mkv.key == key)
            {
                cache_idx = mid; // 更新缓存
                return mid;
            }

            if(mkv.key < key) left = mid + 1;
            else right = mid - 1;
        }

        return -1;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::FindPos(const K &key,int &pos) const
    {
        const int count = data_list.GetCount();
        if(count == 0) { pos = 0; return false; }

        const KVData *arr = data_list.GetData();
        const uint64_t key_hash = std::hash<K>{}(key);

        int left = 0,right = count - 1;
        while(left <= right)
        {
            int mid = left + (right - left) / 2;
            const KVData &mkv = arr[mid];

            // 先比较哈希，哈希不等则键肯定不等
            if(mkv.hash == key_hash && mkv.key == key)
            {
                pos = mid;
                cache_idx = mid; // 更新缓存
                return true;
            }

            if(mkv.key < key) left = mid + 1; else right = mid - 1;
        }
        pos = left; // lower_bound
        return false;
    }

    template<typename K,typename V>
    int ValueKVMap<K,V>::FindByValue(const V &value) const
    {
        const int count = data_list.GetCount();
        if(count <= 0) return -1;
        const KVData *arr = data_list.GetData();
        for(int i = 0; i < count; ++i)
        {
            if(mem_compare(arr[i].value,value) == 0) return i;
        }
        return -1;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::Check(const K &key,const V &value) const
    {
        int idx = Find(key);
        if(idx < 0) return false;
        const KVData *arr = data_list.GetData();
        return arr[idx].value == value;
    }

    template<typename K,typename V>
    typename ValueKVMap<K,V>::KVData *ValueKVMap<K,V>::Add(const K &key,const V &value)
    {
        int pos;
        if(FindPos(key,pos)) return nullptr; // 已存在
        KVData kv{ key, value, std::hash<K>{}(key) }; // 预计算哈希
        data_list.Insert(pos,kv);
        cache_idx = pos; // 更新缓存位置
        return data_list.At(pos);
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::GetAndDelete(const K &key,V &out_value)
    {
        int idx = Find(key);
        if(idx < 0) return false;
        out_value = data_list.At(idx)->value;
        data_list.DeleteShift(idx);
        cache_idx = -1; // 删除后失效缓存
        return true;
    }

    template<typename K,typename V>
    int ValueKVMap<K,V>::DeleteByKey(const K *keys,int count)
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
    bool ValueKVMap<K,V>::DeleteAt(int index)
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        data_list.DeleteShift(index);
        cache_idx = -1; // 删除后失效缓存
        return true;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::DeleteAt(int start,int number)
    {
        bool result = data_list.Delete(start,number);
        if(result) cache_idx = -1; // 删除后失效缓存
        return result;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::AssignValue(const K &key,const V &value)
    {
        int pos;
        if(FindPos(key,pos))
        {
            data_list.At(pos)->value = value;
            cache_idx = pos; // 更新缓存
            return true;
        }
        KVData kv{ key, value, std::hash<K>{}(key) }; // 预计算哈希
        data_list.Insert(pos,kv);
        cache_idx = pos; // 更新缓存位置
        return true;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::Change(const K &key,const V &value)
    {
        int idx = Find(key);
        if(idx < 0) return false;
        data_list.At(idx)->value = value;
        return true;
    }

    template<typename K,typename V>
    V *ValueKVMap<K,V>::GetValuePointer(const K &key) const
    {
        int idx = Find(key);
        if(idx < 0) return nullptr;
        const KVData *arr = data_list.GetData();
        // 注意：返回的指针在容器变动后可能失效
        return const_cast<V *>(&(arr[idx].value));
    }

    template<typename K,typename V>
    int ValueKVMap<K,V>::GetValueAndSerial(const K &key,V &out) const
    {
        int idx = Find(key);
        if(idx < 0) return -1;
        const KVData *arr = data_list.GetData();
        out = arr[idx].value;
        return idx;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::GetBySerial(int index,K &out_key,V &out_value) const
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        const KVData *arr = data_list.GetData();
        out_key = arr[index].key;
        out_value = arr[index].value;
        return true;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::GetKey(int index,K &out_key) const
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        const KVData *arr = data_list.GetData();
        out_key = arr[index].key;
        return true;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::GetValue(int index,V &out_value) const
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        const KVData *arr = data_list.GetData();
        out_value = arr[index].value;
        return true;
    }

    template<typename K,typename V>
    bool ValueKVMap<K,V>::SetValueBySerial(int index,const V &value)
    {
        if(index < 0 || index >= data_list.GetCount()) return false;
        data_list.At(index)->value = value;
        return true;
    }

    template<typename K,typename V>
    void ValueKVMap<K,V>::Enum(void (*enum_func)(const K &,V &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i) enum_func(p[i].key,p[i].value);
    }

    template<typename K,typename V>
    void ValueKVMap<K,V>::EnumKey(void (*enum_func)(const K &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i) enum_func(p[i].key);
    }

    template<typename K,typename V>
    void ValueKVMap<K,V>::EnumAllValue(void (*enum_func)(V &))
    {
        const int count = data_list.GetCount();
        if(count <= 0) return;
        KVData *p = data_list.GetData();
        for(int i = 0; i < count; ++i) enum_func(p[i].value);
    }

    template<typename K,typename V>
    void ValueKVMap<K,V>::EnumValue(bool (*enum_func)(V &))
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
    void ValueKVMap<K,V>::WithList(typename ValueKVMap<K,V>::KVDataList &with_list,const ValueArray<K> &in_list) const
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
    void ValueKVMap<K,V>::WithoutList(typename ValueKVMap<K,V>::KVDataList &without_list,const ValueArray<K> &in_list) const
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
