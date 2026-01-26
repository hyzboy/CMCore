#pragma once

#include<hgl/type/KeyValue.h>
#include<hgl/type/ValueBuffer.h>
#include<hgl/type/PtrArray.h>
#include<hgl/type/Pool.h>

namespace hgl
{
    /**
     * 在已排序的数组中查找指定元素的位置
     * @param arr 已排序的数组
     * @param value 要查找的值
     * @return 元素所在位置，如果不存在返回-1
     */
    template<typename ArrayType, typename T>
    inline int64 FindDataPositionInSortedArray(const ArrayType& arr, const T& value)
    {
        const int64 count = arr.GetCount();
        if(count <= 0)
            return -1;

        const auto* data = arr.GetData();
        int64 left = 0;
        int64 right = count - 1;

        while(left <= right)
        {
            int64 mid = left + (right - left) / 2;

            if(data[mid] < value)
                left = mid + 1;
            else if(value < data[mid])
                right = mid - 1;
            else
                return mid;  // 找到
        }

        return -1;  // 未找到
    }

    /**
     * 在已排序的数组中查找插入位置
     * @param pos 输出：如果元素存在，返回其位置；否则返回应该插入的位置
     * @param arr 已排序的数组
     * @param value 要查找的值
     * @return true表示元素已存在，false表示元素不存在
     */
    template<typename ArrayType, typename T>
    inline bool FindInsertPositionInSortedArray(int64* pos, const ArrayType& arr, const T& value)
    {
        const int64 count = arr.GetCount();
        if(count <= 0)
        {
            *pos = 0;
            return false;
        }

        const auto* data = arr.GetData();
        int64 left = 0;
        int64 right = count - 1;

        while(left <= right)
        {
            int64 mid = left + (right - left) / 2;

            if(data[mid] < value)
                left = mid + 1;
            else if(value < data[mid])
                right = mid - 1;
            else
            {
                *pos = mid;
                return true;  // 找到
            }
        }

        *pos = left;  // 应该插入的位置
        return false;
    }

    /**
     * 有序键值对映射模板（基于排序数组）
     * @tparam K 键类型，必须支持 operator<
     * @tparam V 值类型
     * @tparam KVData 键值对数据类型
     */
    template<typename K, typename V, typename KVData>
    class OrderedMapTemplate
    {
    protected:

        using ThisClass = OrderedMapTemplate<K, V, KVData>;

        using KVDataPool = ObjectPool<KVData>;
        using KVDataList = ValueArray<KVData *>;

        KVDataPool data_pool;
        KVDataList data_list;

        /**
         * 查找键在排序数组中的位置
         */
        bool FindPos(const K& key, int& pos) const
        {
            const int count = data_list.GetCount();
            if(count <= 0)
            {
                pos = 0;
                return false;
            }

            KVData** data = data_list.GetData();
            int left = 0;
            int right = count - 1;

            while(left <= right)
            {
                int mid = left + (right - left) / 2;
                const K& mid_key = data[mid]->key;

                if(mid_key < key)
                    left = mid + 1;
                else if(key < mid_key)
                    right = mid - 1;
                else
                {
                    pos = mid;
                    return true;  // 找到
                }
            }

            pos = left;
            return false;
        }

        int FindPos(const K& key) const
        {
            int pos;
            return FindPos(key, pos) ? pos : -1;
        }

    public:

        KVData** begin() const { return data_list.begin(); }
        KVData** end() const { return data_list.end(); }

    public: // 方法

        OrderedMapTemplate() = default;
        virtual ~OrderedMapTemplate() = default;

        const int   GetCount() const { return data_list.GetCount(); }              ///<取得数据总量
        const bool  IsEmpty() const { return data_list.IsEmpty(); }                ///<是否为空

        // ==================== 添加 ====================
        KVData* Add(const K& key, const V& value)                                  ///<添加一个数据，如果键已存在，返回nullptr
        {
            int pos;
            if(FindPos(key, pos))
                return nullptr;  // 已存在

            // 从池中获取或创建新数据
            KVData* new_data = nullptr;
            if(!data_pool.GetOrCreate(new_data))
                return nullptr;

            new_data->key = key;
            new_data->value = value;

            // 插入到正确位置以保持排序
            data_list.Insert(pos, new_data);

            return new_data;
        }

        // ==================== 查找 ====================
        int Find(const K& key) const                                               ///<查找数据是否存在，返回-1表示数据不存在
        {
            return FindPos(key);
        }

        int FindByValue(const V& value) const                                      ///<查找数据是否存在，返回-1表示数据不存在
        {
            const int count = data_list.GetCount();
            KVData** idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                if((*idp)->value == value)
                    return i;
                ++idp;
            }
            return -1;
        }

        bool ContainsKey(const K& key) const { return (Find(key) != -1); }        ///<确认这个数据是否存在
        bool ContainsValue(const V& value) const { return (FindByValue(value) != -1); } ///<确认这个数据是否存在

        bool Check(const K& key, const V& value) const                             ///<确认数据是否是这个
        {
            int index = Find(key);
            if(index == -1)
                return false;
            return data_list[index]->value == value;
        }

        // ==================== 获取 ====================
        virtual V* GetValuePointer(const K& key) const                             ///<取得数据指针
        {
            int index = Find(key);
            if(index == -1)
                return nullptr;
            return &(data_list[index]->value);
        }

        virtual int GetValueAndSerial(const K& key, V& value) const               ///<取得数据与索引
        {
            int index = Find(key);
            if(index == -1)
                return -1;
            value = data_list[index]->value;
            return index;
        }

        bool Get(const K& key, V& value) const { return (GetValueAndSerial(key, value) >= 0); } ///<取得数据

        // ==================== 删除 ====================
        virtual bool GetAndDelete(const K& key, V& value)                          ///<将指定数据从列表中移除，并获得这个数据
        {
            int index = Find(key);
            if(index == -1)
                return false;

            value = data_list[index]->value;
            return DeleteAt(index);
        }

        virtual bool DeleteByKey(const K& key)                                     ///<根据索引将指定数据从列表中移除
        {
            return DeleteAt(Find(key));
        }

        virtual bool DeleteByValue(const V& value)                                 ///<根据数据将指定数据从列表中移除
        {
            return DeleteAt(FindByValue(value));
        }

        virtual bool DeleteAt(int index)                                           ///<根据序号将指定数据从列表中移除
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;

            KVData* item = data_list[index];
            data_list.Delete(index, 1);
            data_pool.Release(item);

            return true;
        }

        virtual bool DeleteAt(int start, int count)                                ///<根据序号将指定数据从列表中批量移除
        {
            if(start < 0 || count <= 0)
                return false;

            const int end = start + count;
            if(end > data_list.GetCount())
                return false;

            for(int i = start; i < end; i++)
                data_pool.Release(data_list[i]);

            data_list.Delete(start, count);
            return true;
        }

        // ==================== 修改 ====================
        virtual bool ChangeOrAdd(const K& key, const V& value)                     ///<更改一个数据的内容(如不存在则添加)
        {
            int index = Find(key);
            if(index != -1) {
                data_list[index]->value = value;
                return true;
            }
            return Add(key, value) != nullptr;
        }

        virtual bool Change(const K& key, const V& value)                          ///<更改一个数据的内容(如不存在则更改失效)
        {
            int index = Find(key);
            if(index == -1)
                return false;
            data_list[index]->value = value;
            return true;
        }

        // ==================== 清除 ====================
        virtual void Free()                                                        ///<清除所有数据，并释放内存
        {
            const int count = data_list.GetCount();
            KVData** idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                data_pool.Release(*idp);
                ++idp;
            }

            data_list.Free();
        }

        virtual void Clear()                                                       ///<清除所有数据，但不释放内存
        {
            const int count = data_list.GetCount();
            KVData** idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                data_pool.Release(*idp);
                ++idp;
            }

            data_list.Clear();
        }

        // ==================== 列表操作 ====================
        KVDataList& GetList() { return data_list; }                                ///<取得线性列表
        KVData** GetDataList() const { return data_list.GetData(); }               ///<取得纯数据线性列表

        template<typename IT>
        int GetKeyList(IT& il_list)                                                ///<取得所有索引合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            KVData** idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                il_list.Add((*idp)->key);
                ++idp;
            }

            return count;
        }

        template<typename IT>
        int GetValueList(IT& il_list)                                              ///<取得所有数值合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            KVData** idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                il_list.Add((*idp)->value);
                ++idp;
            }

            return count;
        }

        template<typename ITK, typename ITV>
        int GetList(ITK& key_list, ITV& value_list)                                ///<取得所有索引合集
        {
            const int count = data_list.GetCount();
            if(count <= 0)
                return count;

            KVData** idp = data_list.GetData();

            for(int i = 0; i < count; i++)
            {
                key_list.Add((*idp)->key);
                value_list.Add((*idp)->value);
                ++idp;
            }

            return count;
        }

        KVData* GetItem(int n) const { return data_list[n]; }                      ///<取指定序号的数据

        bool GetBySerial(int index, K& key, V& value) const                        ///<取指定序号的数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            key = data_list[index]->key;
            value = data_list[index]->value;
            return true;
        }

        bool GetKey(int index, K& key)                                             ///<取指定序号的索引
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            key = data_list[index]->key;
            return true;
        }

        bool GetValue(int index, V& value)                                         ///<取指定序号的数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            value = data_list[index]->value;
            return true;
        }

        bool SetValueBySerial(int index, const V& value)                           ///<根据序号设置数据
        {
            if(index < 0 || index >= data_list.GetCount())
                return false;
            data_list[index]->value = value;
            return true;
        }

        // ==================== 枚举 ====================
        template<typename F>
        void EnumKV(F&& func)                                                      ///<枚举所有键值对(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            KVData** idp = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func((*idp)->key, (*idp)->value);
                ++idp;
            }
        }

        template<typename F>
        void EnumKeys(F&& func)                                                    ///<枚举所有键(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            KVData** idp = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func((*idp)->key);
                ++idp;
            }
        }

        template<typename F>
        void EnumValues(F&& func)                                                  ///<枚举所有值(支持lambda)
        {
            const int count = data_list.GetCount();
            if(count <= 0) return;

            KVData** idp = data_list.GetData();
            for(int i = 0; i < count; i++)
            {
                func((*idp)->value);
                ++idp;
            }
        }

    protected:

        void operator=(const ThisClass&);  // 禁用赋值
    };//class OrderedMapTemplate

    /**
     * 有序键值对映射（用于平凡类型）
     */
    template<typename K, typename V>
    class OrderedValueMap : public OrderedMapTemplate<K, V, KeyValue<K, V>>
    {
    public:

        using Iterator = KeyValue<K, V>;

    public:

        OrderedValueMap() = default;
        virtual ~OrderedValueMap() = default;

        V* operator[](const K& key) const
        {
            return this->GetValuePointer(key);
        }
    };//class OrderedValueMap

    /**
     * 有序对象指针映射模板
     */
    template<typename K, typename V, typename KVData>
    class OrderedManagedMapTemplate : public OrderedMapTemplate<K, V*, KVData>
    {
    protected:
        typedef OrderedMapTemplate<K, V*, KVData> SuperClass;

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
        OrderedManagedMapTemplate() = default;
        virtual ~OrderedManagedMapTemplate() { Clear(); }

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
    };//OrderedManagedMapTemplate

    /**
     * 有序对象指针映射（用于非平凡类型）
     */
    template<typename K, typename V>
    class OrderedManagedMap : public OrderedManagedMapTemplate<K, V, KeyValue<K, V*>>
    {
    public:
        OrderedManagedMap() = default;
        virtual ~OrderedManagedMap() = default;
    };

}//namespace hgl
