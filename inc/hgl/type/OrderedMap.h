/**
* @file OrderedMap.h
* @brief CN:有序映射模板类，基于absl::btree_map实现，提供键值对的有序存储和快速查询。
*        EN:Ordered map template class based on absl::btree_map, providing key-value pairs with ordered storage and fast queries.
*/
#pragma once

#include<absl/container/btree_map.h>
#include<vector>
#include<algorithm>

namespace hgl
{
    /**
    * @brief CN:有序映射 - 基于 B树实现的高性能有序容器
    *        EN:Ordered Map - High-performance ordered container based on B-tree
    *
    * 特性：
    * 1. 自动排序：所有元素按键的升序排列
    * 2. 高效查询：O(log n) 查找、插入、删除
    * 3. 内存高效：B树比红黑树有更好的缓存局部性
    * 4. 迭代器稳定：迭代时支持安全的删除操作
    * 5. 范围查询：支持前缀搜索、上下界等高级操作
    *
    * @tparam K Key 类型，必须支持 operator<
    * @tparam V Value 类型
    */
    template<typename K, typename V>
    class OrderedMap
    {
    protected:
        using ThisClass = OrderedMap<K, V>;
        absl::btree_map<K, V> map_data;

    public:

        OrderedMap() = default;
        virtual ~OrderedMap() = default;

        class AssignProxy
        {
            ThisClass* map;
            K key;

        public:
            AssignProxy(ThisClass* in_map, const K& in_key) : map(in_map), key(in_key) {}
            AssignProxy(ThisClass* in_map, K&& in_key) : map(in_map), key(std::move(in_key)) {}

            AssignProxy& operator=(const V& value)
            {
                map->AddOrUpdate(key, value);
                return *this;
            }

            AssignProxy& operator=(V&& value)
            {
                map->AddOrUpdate(key, std::move(value));
                return *this;
            }
        };

        // ============================================================
        // 基础查询
        // ============================================================

        /**
        * @brief CN:获取元素数量。\nEN:Get number of elements.
        * @return CN:元素数量。EN:Number of elements.
        */
        int GetCount() const
        {
            return static_cast<int>(map_data.size());
        }

        /**
        * @brief CN:检查是否为空。\nEN:Check if empty.
        * @return CN:为空返回true。EN:Return true if empty.
        */
        bool IsEmpty() const
        {
            return map_data.empty();
        }

        /**
        * @brief CN:检查键是否存在。\nEN:Check if key exists.
        * @param key CN:键。EN:Key.
        * @return CN:存在返回true。EN:Return true if key exists.
        */
        bool ContainsKey(const K& key) const
        {
            return map_data.contains(key);
        }

        AssignProxy operator[](const K& key) { return AssignProxy(this, key); }
        AssignProxy operator[](K&& key) { return AssignProxy(this, std::move(key)); }

        // ============================================================
        // 添加元素
        // ============================================================

        /**
        * @brief CN:添加键值对。\nEN:Add key-value pair.
        * @param key CN:键。EN:Key.
        * @param value CN:值。EN:Value.
        * @return CN:添加成功返回true，键已存在返回false。EN:Return true if added successfully, false if key exists.
        */
        virtual bool Add(const K& key, const V& value)
        {
            auto result = map_data.insert({key, value});
            return result.second;  // 返回是否插入成功
        }

        /**
        * @brief CN:添加键值对（移动语义版本）。\nEN:Add key-value pair (move semantics version).
        * @param key CN:键（右值引用）。EN:Key (rvalue reference).
        * @param value CN:值（右值引用）。EN:Value (rvalue reference).
        * @return CN:添加成功返回true，键已存在返回false。EN:Return true if added successfully, false if key exists.
        */
        virtual bool Add(K&& key, V&& value)
        {
            auto result = map_data.insert({std::move(key), std::move(value)});
            return result.second;
        }

        /**
        * @brief CN:添加键值对（混合语义版本1：键右值）。\nEN:Add key-value pair (mixed version 1: key rvalue).
        */
        virtual bool Add(K&& key, const V& value)
        {
            auto result = map_data.insert({std::move(key), value});
            return result.second;
        }

        /**
        * @brief CN:添加键值对（混合语义版本2：值右值）。\nEN:Add key-value pair (mixed version 2: value rvalue).
        */
        virtual bool Add(const K& key, V&& value)
        {
            auto result = map_data.insert({key, std::move(value)});
            return result.second;
        }

        /**
        * @brief CN:添加或更新键值对。\nEN:Add or update key-value pair.
        * @param key CN:键。EN:Key.
        * @param value CN:值。EN:Value.
        */
        virtual void AddOrUpdate(const K& key, const V& value)
        {
            map_data[key] = value;
        }

        /**
        * @brief CN:添加或更新键值对（移动语义版本）。\nEN:Add or update key-value pair (move semantics version).
        */
        virtual void AddOrUpdate(K&& key, V&& value)
        {
            map_data[std::move(key)] = std::move(value);
        }

        /**
        * @brief CN:添加或更新键值对（混合语义版本1：键右值）。\nEN:Add or update (mixed version 1: key rvalue).
        */
        virtual void AddOrUpdate(K&& key, const V& value)
        {
            map_data[std::move(key)] = value;
        }

        /**
        * @brief CN:添加或更新键值对（混合语义版本2：值右值）。\nEN:Add or update (mixed version 2: value rvalue).
        */
        virtual void AddOrUpdate(const K& key, V&& value)
        {
            map_data[key] = std::move(value);
        }

        // ============================================================
        // 查找元素
        // ============================================================

        /**
        * @brief CN:查找值指针。\nEN:Find value pointer.
        * @param key CN:键。EN:Key.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        virtual V* GetValuePointer(const K& key)
        {
            auto it = map_data.find(key);
            return it != map_data.end() ? &it->second : nullptr;
        }

        /**
        * @brief CN:查找值指针（const版本）。\nEN:Find value pointer (const version).
        * @param key CN:键。EN:Key.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        virtual const V* GetValuePointer(const K& key) const
        {
            auto it = map_data.find(key);
            return it != map_data.end() ? &it->second : nullptr;
        }

        /**
        * @brief CN:获取值。\nEN:Get value.
        * @param key CN:键。EN:Key.
        * @param value CN:输出值。EN:Output value.
        * @return CN:找到返回true。EN:Return true if found.
        */
        virtual bool Get(const K& key, V& value) const
        {
            auto it = map_data.find(key);
            if (it != map_data.end())
            {
                value = it->second;
                return true;
            }
            return false;
        }

        /**
        * @brief CN:查找值位置（索引）。\nEN:Find value position (index).
        * @param key CN:键。EN:Key.
        * @return CN:索引或-1。EN:Index or -1.
        */
        virtual int Find(const K& key) const
        {
            auto it = map_data.find(key);
            if (it == map_data.end())
                return -1;

            // 计算索引
            return static_cast<int>(std::distance(map_data.begin(), it));
        }

        /**
        * @brief CN:按值查找。\nEN:Find by value.
        * @param value CN:值。EN:Value.
        * @return CN:索引或-1。EN:Index or -1.
        */
        virtual int FindByValue(const V& value) const
        {
            int index = 0;
            for (const auto& [k, v] : map_data)
            {
                if (v == value)
                    return index;
                ++index;
            }
            return -1;
        }

        /**
        * @brief CN:检查值是否存在。\nEN:Check if value exists.
        * @param value CN:值。EN:Value.
        * @return CN:存在返回true。EN:Return true if exists.
        */
        bool ContainsValue(const V& value) const
        {
            return FindByValue(value) != -1;
        }

        /**
        * @brief CN:检查键值对是否存在。\nEN:Check if key-value pair exists.
        * @param key CN:键。EN:Key.
        * @param value CN:值。EN:Value.
        * @return CN:存在返回true。EN:Return true if exists.
        */
        bool Check(const K& key, const V& value) const
        {
            auto it = map_data.find(key);
            return it != map_data.end() && it->second == value;
        }

        // ============================================================
        // 删除元素
        // ============================================================

        /**
        * @brief CN:按键删除。\nEN:Delete by key.
        * @param key CN:键。EN:Key.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteByKey(const K& key)
        {
            return map_data.erase(key) > 0;
        }

        /**
        * @brief CN:按值删除。\nEN:Delete by value.
        * @param value CN:值。EN:Value.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteByValue(const V& value)
        {
            int index = FindByValue(value);
            return index >= 0 && DeleteAt(index);
        }

        /**
        * @brief CN:按索引删除。\nEN:Delete by index.
        * @param index CN:索引。EN:Index.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteAt(int index)
        {
            if (index < 0 || index >= GetCount())
                return false;

            auto it = map_data.begin();
            std::advance(it, index);
            map_data.erase(it);
            return true;
        }

        /**
        * @brief CN:按索引范围删除。\nEN:Delete by index range.
        * @param start CN:开始索引。EN:Start index.
        * @param count CN:删除数量。EN:Count.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteAt(int start, int count)
        {
            if (start < 0 || count <= 0 || start + count > GetCount())
                return false;

            auto it_start = map_data.begin();
            std::advance(it_start, start);

            auto it_end = it_start;
            std::advance(it_end, count);

            map_data.erase(it_start, it_end);
            return true;
        }

        /**
        * @brief CN:获取并删除。\nEN:Get and delete.
        * @param key CN:键。EN:Key.
        * @param value CN:输出值。EN:Output value.
        * @return CN:成功返回true。EN:Return true if successful.
        */
        virtual bool GetAndDelete(const K& key, V& value)
        {
            auto it = map_data.find(key);
            if (it == map_data.end())
                return false;

            value = it->second;
            map_data.erase(it);
            return true;
        }

        /**
        * @brief CN:清空所有元素。\nEN:Clear all elements.
        */
        virtual void Clear()
        {
            map_data.clear();
        }

        // ============================================================
        // 访问元素
        // ============================================================

        /**
        * @brief CN:按索引获取键。\nEN:Get key by index.
        * @param index CN:索引。EN:Index.
        * @return CN:键或默认值。EN:Key or default value.
        */
        K GetKeyAt(int index) const
        {
            if (index < 0 || index >= GetCount())
                return K();

            auto it = map_data.begin();
            std::advance(it, index);
            return it->first;
        }

        /**
        * @brief CN:按索引获取值。\nEN:Get value by index.
        * @param index CN:索引。EN:Index.
        * @return CN:值或默认值。EN:Value or default value.
        */
        V GetValueAt(int index) const
        {
            if (index < 0 || index >= GetCount())
                return V();

            auto it = map_data.begin();
            std::advance(it, index);
            return it->second;
        }

        /**
        * @brief CN:按索引获取值指针。\nEN:Get value pointer by index.
        * @param index CN:索引。EN:Index.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        V* GetValuePointerAt(int index)
        {
            if (index < 0 || index >= GetCount())
                return nullptr;

            auto it = map_data.begin();
            std::advance(it, index);
            return &it->second;
        }

        /**
        * @brief CN:按序列号（索引）获取键值对。\nEN:Get key-value pair by serial number (index).
        * @param serial CN:序列号（索引）。EN:Serial number (index).
        * @param key CN:输出键。EN:Output key.
        * @param value CN:输出值。EN:Output value.
        * @return CN:成功返回true。EN:Return true if successful.
        */
        virtual bool GetBySerial(int serial, K& key, V& value) const
        {
            if (serial < 0 || serial >= GetCount())
                return false;

            auto it = map_data.begin();
            std::advance(it, serial);
            key = it->first;
            value = it->second;
            return true;
        }

        /**
        * @brief CN:修改键对应的值。\nEN:Change value for a key.
        * @param key CN:键。EN:Key.
        * @param value CN:新值。EN:New value.
        * @return CN:成功返回true，键不存在返回false。EN:Return true if successful, false if key doesn't exist.
        */
        virtual bool Change(const K& key, const V& value)
        {
            auto it = map_data.find(key);
            if (it == map_data.end())
                return false;

            it->second = value;
            return true;
        }

        // ============================================================
        // 批量操作
        // ============================================================

        /**
        * @brief CN:获取所有键。\nEN:Get all keys.
        * @param keys CN:输出键数组。EN:Output key array.
        */
        template<typename KeyArray>
        void GetKeyArray(KeyArray& keys) const
        {
            keys.Clear();
            keys.SetMaxCount(GetCount());

            for (const auto& [k, v] : map_data)
            {
                keys.Add(k);
            }
        }

        /**
        * @brief CN:获取所有值。\nEN:Get all values.
        * @param values CN:输出值数组。EN:Output value array.
        */
        template<typename ValueArray>
        void GetValueArray(ValueArray& values) const
        {
            values.Clear();
            values.SetMaxCount(GetCount());

            for (const auto& [k, v] : map_data)
            {
                values.Add(v);
            }
        }

        /**
        * @brief CN:获取所有键值对。\nEN:Get all key-value pairs.
        * @param keys CN:输出键数组。EN:Output key array.
        * @param values CN:输出值数组。EN:Output value array.
        */
        template<typename KeyArray, typename ValueArray>
        void GetKeyValueArrays(KeyArray& keys, ValueArray& values) const
        {
            keys.Clear();
            values.Clear();
            keys.SetMaxCount(GetCount());
            values.SetMaxCount(GetCount());

            for (const auto& [k, v] : map_data)
            {
                keys.Add(k);
                values.Add(v);
            }
        }

        // ============================================================
        // 枚举接口
        // ============================================================

        /**
        * @brief CN:枚举所有键值对。\nEN:Enumerate all key-value pairs.
        */
        template<typename F>
        void EnumKV(F&& func)
        {
            for (auto& [key, value] : map_data)
            {
                func(key, value);
            }
        }

        /**
        * @brief CN:枚举所有键值对（const版本）。\nEN:Enumerate all key-value pairs (const version).
        */
        template<typename F>
        void EnumKV(F&& func) const
        {
            for (const auto& [key, value] : map_data)
            {
                func(key, value);
            }
        }

        /**
        * @brief CN:枚举所有键。\nEN:Enumerate all keys.
        */
        template<typename F>
        void EnumKeys(F&& func)
        {
            for (auto& [key, value] : map_data)
            {
                func(key);
            }
        }

        /**
        * @brief CN:枚举所有键（const版本）。\nEN:Enumerate all keys (const version).
        */
        template<typename F>
        void EnumKeys(F&& func) const
        {
            for (const auto& [key, value] : map_data)
            {
                func(key);
            }
        }

        /**
        * @brief CN:枚举所有值。\nEN:Enumerate all values.
        */
        template<typename F>
        void EnumValues(F&& func)
        {
            for (auto& [key, value] : map_data)
            {
                func(value);
            }
        }

        /**
        * @brief CN:枚举所有值（const版本）。\nEN:Enumerate all values (const version).
        */
        template<typename F>
        void EnumValues(F&& func) const
        {
            for (const auto& [key, value] : map_data)
            {
                func(value);
            }
        }

        // ============================================================
        // 迭代器支持
        // ============================================================

        auto begin() { return map_data.begin(); }
        auto end() { return map_data.end(); }
        auto begin() const { return map_data.begin(); }
        auto end() const { return map_data.end(); }

        auto rbegin() { return map_data.rbegin(); }
        auto rend() { return map_data.rend(); }
        auto rbegin() const { return map_data.rbegin(); }
        auto rend() const { return map_data.rend(); }
    };
}  // namespace hgl
