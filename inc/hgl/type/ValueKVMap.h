#pragma once

#include <hgl/type/KeyValue.h>
#include <hgl/type/ValueArray.h>
#include <utility>

namespace hgl
{
    // AI NOTE: Sorted key/value list stored as ValueArray of KeyValue.
    // Uses binary search for keys; inserts shift elements (O(n)).
    // Cached index speeds repeated lookups but invalidates on mutation.
    /**
    * ValueKVMap: 按值存储的紧凑型有序映射。
    * - 不使用对象池与指针数组，节省小数据额外开销。
    * - 注意：返回的指针在容器扩容或插入/删除后可能失效。
    */
    template<typename K, typename V>
    class ValueKVMap
    {
    public:
        using KVData = KeyValue<K, V>;
        using KVDataList = ValueArray<KVData>;

    private:
        KVDataList data_list; // 按值存储
        mutable int cache_idx = -1; // 上次查找的位置，加速重复查找

    public:
        ValueKVMap() = default;
        ~ValueKVMap() = default;

        class AssignProxy
        {
            ValueKVMap* map;
            K key;

        public:
            AssignProxy(ValueKVMap* in_map, const K& in_key) : map(in_map), key(in_key) {}
            AssignProxy(ValueKVMap* in_map, K&& in_key) : map(in_map), key(std::move(in_key)) {}

            AssignProxy& operator=(const V& value)
            {
                map->AssignValue(key, value);
                return *this;
            }

            AssignProxy& operator=(V&& value)
            {
                map->AssignValue(key, value);
                return *this;
            }
        };

        //迭代支持（只读）
        const KVData * begin () const
        {
            return data_list.begin();
        }

        const KVData * end () const
        {
            return data_list.end();
        }

        const KVData * Begin() const
        {
            return data_list.begin();
        }

        const KVData * End() const
        {
            return data_list.end();
        }

        // 基本信息
        int GetCount () const
        {
            return data_list.GetCount();
        }

        bool IsEmpty () const
        {
            return data_list.IsEmpty();
        }

        AssignProxy operator[](const K& key) { return AssignProxy(this, key); }
        AssignProxy operator[](K&& key) { return AssignProxy(this, std::move(key)); }

        // 查找
        bool FindPos (const K &key, int &pos) const;
        int FindPos (const K &key) const
        {
            int p;
            FindPos(key, p);
            return p;
        }

        int Find (const K &key) const;
        int FindByValue (const V &value) const;
        bool ContainsKey (const K &key) const
        {
            return Find(key) != -1;
        }

        bool ContainsValue (const V &value) const
        {
            return FindByValue(value) != -1;
        }

        bool Check (const K &key, const V &value) const;

        //访问
        V * GetValuePointer (const K &key) const; // 注意：可能在修改后失效
        int GetValueAndSerial (const K &key, V &out) const;
        bool Get (const K &key, V &out) const
        {
            return GetValueAndSerial(key, out) >=0;
        }

        // 修改
        KVData * Add (const K &key, const V &value); // 返回插入后的元素地址（可能在后续操作后失效）
        bool GetAndDelete (const K &key, V &out_value);
        bool DeleteByKey (const K &key)
        {
            return DeleteAt(Find(key));
        }

        int DeleteByKey (const K *keys, int count);
        bool DeleteByValue (const V &value)
        {
            return DeleteAt(FindByValue(value));
        }

        bool DeleteAt (int index);
        bool DeleteAt (int start, int number);
        bool Change (const K &key, const V &value);
        void Free () { data_list.Free(); }
        void Clear () { data_list.Clear(); }

    private:
        bool AssignValue (const K &key, const V &value);

        // 批量导出
        template<typename IT>
        int GetKeyList(IT &out_list) const
        {
            const int count = data_list.GetCount();
            if (count <=0) return count;
            const KVData *p = data_list.GetData();
            for (int i =0; i < count; ++i) { out_list.Add(p[i].key); }
            return count;
        }

        template<typename IT>
        int GetValueList(IT &out_list) const
        {
            const int count = data_list.GetCount();
            if (count <=0) return count;
            const KVData *p = data_list.GetData();
            for (int i =0; i < count; ++i) { out_list.Add(p[i].value); }
            return count;
        }

        template<typename ITK, typename ITV>
        int GetList(ITK &key_list, ITV &value_list) const
        {
            const int count = data_list.GetCount();
            if (count <=0) return count;
            const KVData *p = data_list.GetData();
            for (int i =0; i < count; ++i) { key_list.Add(p[i].key); value_list.Add(p[i].value); }
            return count;
        }

        // 按序号访问
        KVData * GetItem (int n)
        {
            return (n >=0 && n < data_list.GetCount()) ? data_list.At(n) : nullptr;
        }

        bool GetBySerial (int index, K &out_key, V &out_value) const;
        bool GetKey (int index, K &out_key) const;
        bool GetValue (int index, V &out_value) const;
        bool SetValueBySerial (int index, const V &value);

        // 枚举
        void Enum (void (*enum_func)(const K &, V &));
        void EnumKey (void (*enum_func)(const K &));
        void EnumAllValue (void (*enum_func)(V &));
        void EnumValue (bool (*enum_func)(V &));

        // 集合运算（按 key）
        void WithList (KVDataList &with_list, const ValueArray<K> &in_list) const;
        void WithoutList (KVDataList &without_list, const ValueArray<K> &in_list) const;
    };
}

#include <hgl/type/ValueKVMap.cpp>
