#pragma once

#include<hgl/type/HashIDMap.h>
#include<hgl/type/KeyValue.h>
#include<hgl/type/ValueArray.h>
#include<hgl/type/Pool.h>

namespace hgl
{
    /**
    * 基于哈希的索引数据模板（O(1) 平均查找时间）
    */
    template<typename K, typename V, typename KVData, int MAX_COLLISION = 4>
    class HashMapTemplate
    {
    protected:

        using ThisClass = HashMapTemplate<K, V, KVData, MAX_COLLISION>;

        using KVDataPool = ObjectPool<KVData>;
        using KVDataList = ValueArray<KVData *>;

        KVDataPool data_pool;
        KVDataList data_list;
        HashIDMap<MAX_COLLISION> hash_map;

        // 哈希函数（FNV-1a）
        uint64 ComputeHash(const K& key) const
        {
            // 对于基础类型，使用指针转换
            const uint8* bytes = (const uint8*)&key;
            const int size = sizeof(K);

            uint64 hash = 14695981039346656037ULL;
            for(int i = 0; i < size; i++) {
                hash ^= (uint64)bytes[i];
                hash *= 1099511628211ULL;
            }
            return hash;
        }

    public:

        KVData **begin() const { return data_list.begin(); }
        KVData **end() const { return data_list.end(); }

    public: // 方法

        HashMapTemplate() = default;
        virtual ~HashMapTemplate() = default;

        const int   GetCount() const { return data_list.GetCount(); }           ///<取得数据总量
        const bool  IsEmpty() const { return data_list.IsEmpty(); }             ///<是否为空

        // ==================== 添加 ====================
        KVData *Add(const K& key, const V& value)                              ///<添加一个数据，如果索引已存在，返回nullptr
        {
            uint64 hash = ComputeHash(key);
            
            // 检查是否已存在
            int existing_id = hash_map.Find(hash, [&](int id) {
                return data_list[id]->key == key;
            });

            if(existing_id != -1)
                return nullptr;  // 已存在

            // 从池中获取或创建新数据
            KVData* new_data = nullptr;
            if(!data_pool.GetOrCreate(new_data))
                return nullptr;  // 池满或分配失败

            new_data->key = key;
            new_data->value = value;

            int new_id = data_list.GetCount();
            data_list.Add(new_data);
            
            if(!hash_map.Add(hash, new_id)) {
                // 哈希表满，但数据已添加
                // 可以考虑记录警告日志
            }

            return new_data;
        }

        // ==================== 查找 ====================
        int Find(const K& key) const                                           ///<查找数据是否存在，返回-1表示数据不存在
        {
            uint64 hash = ComputeHash(key);
            return hash_map.Find(hash, [&](int id) {
                return data_list[id]->key == key;
            });
        }

        int FindByValue(const V& value) const                                  ///<查找数据是否存在，返回-1表示数据不存在
        {
            const int count = data_list.GetCount();
            KVData **idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                if((*idp)->value == value)
                    return i;
                ++idp;
            }
            return -1;
        }

        bool ContainsKey(const K& key) const { return (Find(key) != -1); }    ///<确认这个数据是否存在
        bool ContainsValue(const V& value) const { return (FindByValue(value) != -1); } ///<确认这个数据是否存在

        bool Check(const K& key, const V& value) const                         ///<确认数据是否是这个
        {
            int index = Find(key);
            if(index == -1)
                return false;
            return data_list[index]->value == value;
        }

        // ==================== 获取 ====================
        virtual V* GetValuePointer(const K& key) const                         ///<取得数据指针
        {
            int index = Find(key);
            if(index == -1)
                return nullptr;
            return &(data_list[index]->value);
        }

        virtual int GetValueAndSerial(const K& key, V& value) const          ///<取得数据与索引
        {
            int index = Find(key);
            if(index == -1)
                return -1;
            value = data_list[index]->value;
            return index;
        }

        bool Get(const K& key, V& value) const { return (GetValueAndSerial(key, value) >= 0); } ///<取得数据

        // ==================== 删除 ====================
        virtual bool GetAndDelete(const K& key, V& value)                      ///<将指定数据从列表中移除，并获得这个数据
        {
            int index = Find(key);
            if(index == -1)
                return false;

            value = data_list[index]->value;
            return DeleteAt(index);
        }

        virtual bool DeleteByKey(const K& key)                                 ///<根据索引将指定数据从列表中移除
        {
            return DeleteAt(Find(key));
        }

        virtual bool DeleteByValue(const V& value)                             ///<根据数据将指定数据从列表中移除
        {
            return DeleteAt(FindByValue(value));
        }

        virtual bool DeleteAt(int index)                                       ///<根据序号将指定数据从列表中移除
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;

            KVData* item = data_list[index];
            data_list.Delete(index, 1);
            data_pool.Release(item);  // Release 从 Active 中移除，再放回 Idle

            // 重建哈希映射（因为索引变化了）
            RebuildHashMap();
            return true;
        }

        virtual bool DeleteAt(int start, int count)                            ///<根据序号将指定数据从列表中批量移除
        {
            if(start < 0 || count <= 0)
                return false;

            const int end = start + count;
            if(end > data_list.GetCount())
                return false;

            for(int i = start; i < end; i++)
                data_pool.Release(data_list[i]);  // Release 而不是 AppendToIdle

            data_list.Delete(start, count);
            RebuildHashMap();
            return true;
        }

        // ==================== 修改 ====================
        virtual bool ChangeOrAdd(const K& key, const V& value)                 ///<更改一个数据的内容(如不存在则添加)
        {
            int index = Find(key);
            if(index != -1) {
                data_list[index]->value = value;
                return true;
            }
            return Add(key, value) != nullptr;
        }

        virtual bool Change(const K& key, const V& value)                      ///<更改一个数据的内容(如不存在则更改失效)
        {
            int index = Find(key);
            if(index == -1)
                return false;
            data_list[index]->value = value;
            return true;
        }

        // ==================== 清除 ====================
        virtual void Free()                                                    ///<清除所有数据，并释放内存
        {
            const int count = data_list.GetCount();
            KVData **idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                data_pool.Release(*idp);  // Release 而不是 AppendToIdle
                ++idp;
            }

            data_list.Free();
            hash_map.Clear();
        }

        virtual void Clear()                                                   ///<清除所有数据，但不释放内存
        {
            const int count = data_list.GetCount();
            KVData **idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                data_pool.Release(*idp);  // Release 而不是 AppendToIdle
                ++idp;
            }

            data_list.Clear();
            hash_map.Clear();
        }

        // ==================== 列表操作 ====================
        KVDataList& GetList() { return data_list; }                            ///<取得线性列表
        KVData** GetDataList() const { return data_list.GetData(); }           ///<取得纯数据线性列表

        template<typename IT>
        int GetKeyList(IT& il_list)                                            ///<取得所有索引合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            KVData **idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                il_list.Add((*idp)->key);
                ++idp;
            }

            return count;
        }

        template<typename IT>
        int GetValueList(IT& il_list)                                          ///<取得所有数值合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            KVData **idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                il_list.Add((*idp)->value);
                ++idp;
            }

            return count;
        }

        template<typename ITK, typename ITV>
        int GetList(ITK& key_list, ITV& value_list)                            ///<取得所有索引合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            KVData **idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                key_list.Add((*idp)->key);
                value_list.Add((*idp)->value);
                ++idp;
            }

            return count;
        }

        KVData* GetItem(int n) const { return data_list[n]; }                  ///<取指定序号的数据

        bool GetBySerial(int index, K& key, V& value) const                    ///<取指定序号的数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            key = data_list[index]->key;
            value = data_list[index]->value;
            return true;
        }

        bool GetKey(int index, K& key)                                         ///<取指定序号的索引
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            key = data_list[index]->key;
            return true;
        }

        bool GetValue(int index, V& value)                                     ///<取指定序号的数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            value = data_list[index]->value;
            return true;
        }

        bool SetValueBySerial(int index, V& value)                             ///<根据序号设置数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            data_list[index]->value = value;
            return true;
        }

        // ==================== 枚举 ====================
        template<typename F>
        void EnumKV(F&& func)                                                  ///<枚举所有键值对(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            KVData **idp = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func((*idp)->key, (*idp)->value);
                ++idp;
            }
        }

        template<typename F>
        void EnumKeys(F&& func)                                                ///<枚举所有键(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            KVData **idp = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func((*idp)->key);
                ++idp;
            }
        }

        template<typename F>
        void EnumValues(F&& func)                                              ///<枚举所有值(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            KVData **idp = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func((*idp)->value);
                ++idp;
            }
        }

        // ==================== 哈希统计接口 ====================
        int GetCollisionCount() const { return hash_map.GetCollisionCount(); }
        float GetLoadFactor() const { return hash_map.GetLoadFactor(data_list.GetCount()); }
        float GetAverageCollisionChainLength() const { return hash_map.GetAverageCollisionChainLength(); }
        int GetCollisionOverflowCount() const { return hash_map.GetCollisionOverflowCount(); }

    protected:

        // 重建哈希映射（在删除后使用）
        void RebuildHashMap()
        {
            hash_map.Clear();

            const int count = data_list.GetCount();
            for(int i = 0; i < count; i++)
            {
                uint64 hash = ComputeHash(data_list[i]->key);
                hash_map.Add(hash, i);
            }
        }

        void operator=(const ThisClass&);  // 禁用赋值
    };//class HashMapTemplate

    /**
    * 基于哈希的键值对映射（O(1) 平均查找时间）
    */
    template<typename K, typename V, int MAX_COLLISION = 4>
    class HashMap : public HashMapTemplate<K, V, KeyValue<K, V>, MAX_COLLISION>
    {
    public:

        using Iterator = KeyValue<K, V>;

    public:

        HashMap() = default;
        virtual ~HashMap() = default;

        V* operator[](const K& key) const
        {
            return this->GetValuePointer(key);
        }
    };//class HashMap

    /**
    * 基于哈希的对象指针映射模板（O(1) 平均查找时间）
    */
    template<typename K, typename V, typename KVData, int MAX_COLLISION = 4>
    class ObjectHashMapTemplate : public HashMapTemplate<K, V*, KVData, MAX_COLLISION>
    {
    protected:
        typedef HashMapTemplate<K, V*, KVData, MAX_COLLISION> SuperClass;

        void DeleteObject(KVData* ds)
        {
            if(!ds) return;
            V*& p = ds->value;
            if(p) { delete p; p = nullptr; }
        }

        void DeleteObject(int index)
        {
            if(index >= 0 && index < this->data_list.GetCount())
                DeleteObject(this->data_list[index]);
        }

    public:
        ObjectHashMapTemplate() = default;
        virtual ~ObjectHashMapTemplate() { Clear(); }

        // ==================== Unlink（不删除对象） ====================
        bool UnlinkByKey(const K& flag) { return UnlinkBySerial(SuperClass::Find(flag)); }
        bool UnlinkByValue(V* tp) { return UnlinkBySerial(this->FindByValue(tp)); }

        bool UnlinkBySerial(int index)
        {
            if(index < 0 || index >= this->data_list.GetCount())
                return false;
            SuperClass::DeleteAt(index);
            return true;
        }

        void UnlinkAll() { SuperClass::Free(); }

        // ==================== Delete（删除对象） ====================
        bool DeleteByKey(const K& flag) { return DeleteAt(SuperClass::Find(flag)); }
        bool DeleteByValue(V* tp) { return DeleteAt(this->FindByValue(tp)); }

        bool DeleteAt(int index)
        {
            if(index < 0 || index >= this->data_list.GetCount())
                return false;
            DeleteObject(index);
            SuperClass::DeleteAt(index);
            return true;
        }

        void DeleteAll()
        {
            for(auto& it : this->data_list)
                DeleteObject(it);
            SuperClass::Free();
        }

        // ==================== Update & Change ====================
        void Update(const K& flag, V* data)
        {
            int index = this->Find(flag);
            if(index != -1)
            {
                DeleteObject(index);
                KVData* dp = this->data_list[index];
                if(dp) dp->value = data;
            }
            else
                this->Add(flag, data);
        }

        bool Change(const K& flag, V* data)
        {
            int index = this->Find(flag);
            if(index != -1)
            {
                DeleteObject(index);
                KVData* dp = this->data_list[index];
                if(!dp) return false;
                dp->value = data;
                return true;
            }
            return false;
        }

        void Clear() { DeleteAll(); }

        V* operator[](const K& index) const
        {
            V** ptr = this->GetValuePointer(index);
            return ptr ? *ptr : nullptr;
        }
    };//ObjectHashMapTemplate

    /**
    * 基于哈希的对象指针映射（O(1) 平均查找时间）
    */
    template<typename K, typename V, int MAX_COLLISION = 4>
    class ObjectHashMap : public ObjectHashMapTemplate<K, V, KeyValue<K, V*>, MAX_COLLISION>
    {
    public:
        ObjectHashMap() = default;
        virtual ~ObjectHashMap() = default;
    };

    // ==================== 字符串特化哈希函数 ====================
    template<int MAX_COLLISION>
    class HashMapTemplate<const char*, const char*, KeyValue<const char*, const char*>, MAX_COLLISION>
    {
        // 字符串特化可以使用更高效的哈希算法
        // 此处预留，具体实现可参考 ConstStringSet
    };

}//namespace hgl
