/**
* @file FlatOrderedMap.h
* @brief CN:平铺有序映射 - 当Key/Value为trivially copyable时，支持零拷贝序列化
*        EN:Flat ordered map - When Key/Value are trivially copyable, supports zero-copy serialization
*/
#pragma once

#include<vector>
#include<algorithm>
#include<hgl/type/DataType.h>

namespace hgl
{
    /**
    * @brief CN:平铺有序映射（平凡类型专用版本）
    *        EN:Flat Ordered Map (Specialized for trivially copyable types)
    *
    * 设计特点：
    * 1. 分离式存储：Key和Value分别存储在独立的std::vector中，保持有序
    * 2. 对齐关系：Key[i] 对应 Value[i]
    * 3. 连续内存：支持零拷贝序列化/反序列化
    * 4. 有序维护：所有操作都维护按Key升序排列的不变量
    * 5. 限制条件：Key和Value都必须是trivially copyable类型
    *
    * 使用场景：
    * - 需要序列化/反序列化整个容器（直接SAVE/LOAD两个数组）
    * - 大量查询操作，偶尔修改
    * - 配置文件、资源索引、静态数据表
    *
    * 性能特征：
    * - 查找：O(log n)（二分搜索）
    * - 插入：O(n)（需要维护有序性）
    * - 删除：O(n)（需要移动元素）
    * - 序列化：O(n)，支持直接内存拷贝
    *
    * @tparam K Key 类型，必须：
    *           1. 支持 operator<（用于排序）
    *           2. 是 trivially copyable（可直接内存拷贝）
    * @tparam V Value 类型，必须：
    *           1. 是 trivially copyable（可直接内存拷贝）
    *
    * @example
    * ```cpp
    * // 支持的用法
    * FlatOrderedMap<int, float> map;
    * map.Add(1, 1.5f);
    * map.Add(2, 2.5f);
    *
    * // 序列化：直接保存两个数组
    * auto keys = map.GetKeyData();
    * auto values = map.GetValueData();
    * int count = map.GetCount();
    * // ... 写入文件 ...
    *
    * // 反序列化：直接加载
    * map.LoadFromBuffers(keys_buffer, values_buffer, count);
    * ```
    *
    * @see OrderedMap 通用版本（支持非平凡类型）
    * @see FlatOrderedSet 单值集合版本
    */
    template<typename K, typename V>
    class FlatOrderedMap
    {
    protected:
        using ThisClass = FlatOrderedMap<K, V>;

        static_assert(std::is_trivially_copyable_v<K>,
                      "FlatOrderedMap requires trivially copyable Key type. For non-trivial Key (with custom copy/move semantics, virtual functions, dynamic memory), use OrderedMap instead.");
        static_assert(std::is_trivially_copyable_v<V>,
                      "FlatOrderedMap requires trivially copyable Value type. For non-trivial Value (with custom copy/move semantics, virtual functions, dynamic memory), use OrderedMap instead.");

        /**
        * @brief CN:Key数组（保持升序）\nEN:Key array (maintained in ascending order)
        */
        std::vector<K> keys;

        /**
        * @brief CN:Value数组（与Key数组对齐，keys[i]对应values[i]）\nEN:Value array (aligned with Key array, keys[i] corresponds to values[i])
        */
        std::vector<V> values;

    public:
        using key_type = K;
        using value_type = V;
        using size_type = int64;

        FlatOrderedMap() = default;
        virtual ~FlatOrderedMap() = default;

        // ============================================================
        // 序列化支持
        // ============================================================

        /**
        * @brief CN:获取Key数据指针（用于序列化）\nEN:Get Key data pointer (for serialization)
        * @return CN:Key数组首指针或nullptr。EN:Key array pointer or nullptr.
        */
        K* GetKeyData() { return keys.empty() ? nullptr : keys.data(); }
        const K* GetKeyData() const { return keys.empty() ? nullptr : keys.data(); }

        /**
        * @brief CN:获取Value数据指针（用于序列化）\nEN:Get Value data pointer (for serialization)
        * @return CN:Value数组首指针或nullptr。EN:Value array pointer or nullptr.
        */
        V* GetValueData() { return values.empty() ? nullptr : values.data(); }
        const V* GetValueData() const { return values.empty() ? nullptr : values.data(); }

        /**
        * @brief CN:从缓冲区加载（直接反序列化）\nEN:Load from buffers (direct deserialization)
        * @param key_buffer CN:Key缓冲区。EN:Key buffer.
        * @param value_buffer CN:Value缓冲区。EN:Value buffer.
        * @param count CN:元素数量。EN:Element count.
        * @param is_sorted CN:数据是否已排序。EN:Whether data is already sorted.
        *
        * @note CN:如果数据不是有序的，会进行排序和验证（确保不重复）。\n
        *       EN:If data is not sorted, it will be sorted and verified (no duplicates).
        */
        void LoadFromBuffers(const K* key_buffer, const V* value_buffer, int64 count, bool is_sorted = true)
        {
            if (!key_buffer || !value_buffer || count <= 0)
                return;

            keys.assign(key_buffer, key_buffer + count);
            values.assign(value_buffer, value_buffer + count);

            if (!is_sorted)
            {
                // 按Key排序，同时维持Value对齐
                std::vector<int64> indices(count);
                std::iota(indices.begin(), indices.end(), 0);

                std::sort(indices.begin(), indices.end(),
                    [this](int64 i, int64 j) { return keys[i] < keys[j]; });

                // 应用排列
                std::vector<K> sorted_keys(count);
                std::vector<V> sorted_values(count);
                for (int64 i = 0; i < count; ++i)
                {
                    sorted_keys[i] = keys[indices[i]];
                    sorted_values[i] = values[indices[i]];
                }

                keys = std::move(sorted_keys);
                values = std::move(sorted_values);

                // 去重
                int64 write_pos = 0;
                for (int64 i = 1; i < count; ++i)
                {
                    if (!(keys[i] < keys[write_pos]) && !(keys[write_pos] < keys[i]))  // keys[i] == keys[write_pos]
                    {
                        // 重复，跳过
                        continue;
                    }
                    ++write_pos;
                    keys[write_pos] = keys[i];
                    values[write_pos] = values[i];
                }
                keys.erase(keys.begin() + write_pos + 1, keys.end());
                values.erase(values.begin() + write_pos + 1, values.end());
            }
        }

        /**
        * @brief CN:获取原始数据大小（字节）\nEN:Get raw data size (bytes)
        * @return CN:Key和Value的总字节数。EN:Total bytes for Keys and Values.
        */
        int64 GetRawDataSize() const
        {
            return (int64)(keys.size() * sizeof(K) + values.size() * sizeof(V));
        }

        // ============================================================
        // 容量管理
        // ============================================================

        /**
        * @brief CN:获取元素数量\nEN:Get element count
        * @return CN:元素数量。EN:Number of elements.
        */
        int64 GetCount() const
        {
            return static_cast<int64>(keys.size());
        }

        /**
        * @brief CN:获取已分配容量\nEN:Get allocated capacity
        * @return CN:Key数组容量。EN:Key array capacity.
        */
        int64 GetAllocCount() const
        {
            return static_cast<int64>(keys.capacity());
        }

        /**
        * @brief CN:检查是否为空\nEN:Check if empty
        * @return CN:为空返回true。EN:Return true if empty.
        */
        bool IsEmpty() const
        {
            return keys.empty();
        }

        /**
        * @brief CN:预留容量\nEN:Reserve capacity
        * @param count CN:预期元素数量。EN:Expected element count.
        */
        void Reserve(int64 count)
        {
            if (count > 0)
            {
                keys.reserve(count);
                values.reserve(count);
            }
        }

        // ============================================================
        // 基础查询
        // ============================================================

        /**
        * @brief CN:检查键是否存在\nEN:Check if key exists
        * @param key CN:键。EN:Key.
        * @return CN:存在返回true。EN:Return true if key exists.
        */
        bool ContainsKey(const K& key) const
        {
            return Find(key) != -1;
        }

        // ============================================================
        // 查找元素
        // ============================================================

        /**
        * @brief CN:查找键的索引\nEN:Find index of key
        * @param key CN:键。EN:Key.
        * @return CN:索引或-1。EN:Index or -1.
        */
        virtual int64 Find(const K& key) const
        {
            auto it = std::lower_bound(keys.begin(), keys.end(), key);
            if (it != keys.end() && !(*it < key) && !(key < *it))  // *it == key
            {
                return std::distance(keys.begin(), it);
            }
            return -1;
        }

        /**
        * @brief CN:获取值（通过键）\nEN:Get value by key
        * @param key CN:键。EN:Key.
        * @param value CN:输出值。EN:Output value.
        * @return CN:找到返回true。EN:Return true if found.
        */
        virtual bool Get(const K& key, V& value) const
        {
            int64 index = Find(key);
            if (index >= 0)
            {
                value = values[index];
                return true;
            }
            return false;
        }

        /**
        * @brief CN:获取值指针（通过键）\nEN:Get value pointer by key
        * @param key CN:键。EN:Key.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        virtual V* GetValuePointer(const K& key)
        {
            int64 index = Find(key);
            return index >= 0 ? &values[index] : nullptr;
        }

        /**
        * @brief CN:获取值指针（const版本）\nEN:Get value pointer (const version)
        * @param key CN:键。EN:Key.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        virtual const V* GetValuePointer(const K& key) const
        {
            int64 index = Find(key);
            return index >= 0 ? &values[index] : nullptr;
        }

        /**
        * @brief CN:按索引获取键\nEN:Get key by index
        * @param index CN:索引。EN:Index.
        * @return CN:键或默认值。EN:Key or default value.
        */
        K GetKeyAt(int64 index) const
        {
            if (index < 0 || index >= (int64)keys.size())
                return K();
            return keys[index];
        }

        /**
        * @brief CN:按索引获取值\nEN:Get value by index
        * @param index CN:索引。EN:Index.
        * @return CN:值或默认值。EN:Value or default value.
        */
        V GetValueAt(int64 index) const
        {
            if (index < 0 || index >= (int64)values.size())
                return V();
            return values[index];
        }

        /**
        * @brief CN:按索引获取值指针\nEN:Get value pointer by index
        * @param index CN:索引。EN:Index.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        V* GetValuePointerAt(int64 index)
        {
            if (index < 0 || index >= (int64)values.size())
                return nullptr;
            return &values[index];
        }

        /**
        * @brief CN:按索引获取值指针（const版本）\nEN:Get value pointer by index (const version)
        * @param index CN:索引。EN:Index.
        * @return CN:值指针或nullptr。EN:Value pointer or nullptr.
        */
        const V* GetValuePointerAt(int64 index) const
        {
            if (index < 0 || index >= (int64)values.size())
                return nullptr;
            return &values[index];
        }

        /**
        * @brief CN:按序列号（索引）获取键值对\nEN:Get key-value pair by serial number (index)
        * @param serial CN:序列号（索引）。EN:Serial number (index).
        * @param key CN:输出键。EN:Output key.
        * @param value CN:输出值。EN:Output value.
        * @return CN:成功返回true。EN:Return true if successful.
        */
        virtual bool GetBySerial(int64 serial, K& key, V& value) const
        {
            if (serial < 0 || serial >= (int64)keys.size())
                return false;

            key = keys[serial];
            value = values[serial];
            return true;
        }

        /**
        * @brief CN:按值查找\nEN:Find by value
        * @param value CN:值。EN:Value.
        * @return CN:索引或-1。EN:Index or -1.
        */
        virtual int64 FindByValue(const V& value) const
        {
            for (int64 i = 0; i < (int64)values.size(); ++i)
            {
                if (values[i] == value)
                    return i;
            }
            return -1;
        }

        /**
        * @brief CN:检查值是否存在\nEN:Check if value exists
        * @param value CN:值。EN:Value.
        * @return CN:存在返回true。EN:Return true if exists.
        */
        bool ContainsValue(const V& value) const
        {
            return FindByValue(value) >= 0;
        }

        // ============================================================
        // 添加元素
        // ============================================================

        /**
        * @brief CN:添加键值对\nEN:Add key-value pair
        * @param key CN:键。EN:Key.
        * @param value CN:值。EN:Value.
        * @return CN:添加成功返回true，键已存在返回false。EN:Return true if added successfully, false if key exists.
        */
        virtual bool Add(const K& key, const V& value)
        {
            auto it = std::lower_bound(keys.begin(), keys.end(), key);

            // 检查是否已存在
            if (it != keys.end() && !(*it < key) && !(key < *it))  // *it == key
            {
                return false;  // 键已存在
            }

            // 插入到正确的有序位置
            int64 index = std::distance(keys.begin(), it);
            keys.insert(it, key);
            values.insert(values.begin() + index, value);
            return true;
        }

        /**
        * @brief CN:添加或更新键值对\nEN:Add or update key-value pair
        * @param key CN:键。EN:Key.
        * @param value CN:值。EN:Value.
        * @return CN:返回是否是新增（true=新增，false=更新）。EN:Return true if added, false if updated.
        */
        virtual bool AddOrUpdate(const K& key, const V& value)
        {
            int64 index = Find(key);
            if (index >= 0)
            {
                values[index] = value;
                return false;  // 更新了现有键
            }

            Add(key, value);
            return true;  // 添加了新键
        }

        /**
        * @brief CN:修改键对应的值\nEN:Change value for a key
        * @param key CN:键。EN:Key.
        * @param value CN:新值。EN:New value.
        * @return CN:成功返回true，键不存在返回false。EN:Return true if successful, false if key doesn't exist.
        */
        virtual bool Change(const K& key, const V& value)
        {
            int64 index = Find(key);
            if (index >= 0)
            {
                values[index] = value;
                return true;
            }
            return false;
        }

        // ============================================================
        // 删除元素
        // ============================================================

        /**
        * @brief CN:按键删除\nEN:Delete by key
        * @param key CN:键。EN:Key.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteByKey(const K& key)
        {
            int64 index = Find(key);
            if (index >= 0)
            {
                keys.erase(keys.begin() + index);
                values.erase(values.begin() + index);
                return true;
            }
            return false;
        }

        /**
        * @brief CN:按索引删除\nEN:Delete by index
        * @param index CN:索引。EN:Index.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteAt(int64 index)
        {
            if (index < 0 || index >= (int64)keys.size())
                return false;

            keys.erase(keys.begin() + index);
            values.erase(values.begin() + index);
            return true;
        }

        /**
        * @brief CN:按索引范围删除\nEN:Delete by index range
        * @param start CN:开始索引。EN:Start index.
        * @param count CN:删除数量。EN:Count.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteAt(int64 start, int64 count)
        {
            if (start < 0 || count <= 0 || start + count > (int64)keys.size())
                return false;

            keys.erase(keys.begin() + start, keys.begin() + start + count);
            values.erase(values.begin() + start, values.begin() + start + count);
            return true;
        }

        /**
        * @brief CN:按值删除\nEN:Delete by value
        * @param value CN:值。EN:Value.
        * @return CN:删除成功返回true。EN:Return true if deleted.
        */
        virtual bool DeleteByValue(const V& value)
        {
            int64 index = FindByValue(value);
            if (index >= 0)
            {
                keys.erase(keys.begin() + index);
                values.erase(values.begin() + index);
                return true;
            }
            return false;
        }

        /**
        * @brief CN:获取并删除\nEN:Get and delete
        * @param key CN:键。EN:Key.
        * @param value CN:输出值。EN:Output value.
        * @return CN:成功返回true。EN:Return true if successful.
        */
        virtual bool GetAndDelete(const K& key, V& value)
        {
            int64 index = Find(key);
            if (index >= 0)
            {
                value = values[index];
                keys.erase(keys.begin() + index);
                values.erase(values.begin() + index);
                return true;
            }
            return false;
        }

        /**
        * @brief CN:清空所有元素\nEN:Clear all elements
        */
        virtual void Clear()
        {
            keys.clear();
            values.clear();
        }

        // ============================================================
        // 批量操作
        // ============================================================

        /**
        * @brief CN:获取所有键\nEN:Get all keys
        * @param key_array CN:输出键数组。EN:Output key array.
        */
        template<typename KeyArray>
        void GetKeyArray(KeyArray& key_array) const
        {
            key_array.Clear();
            key_array.SetMaxCount((int)keys.size());

            for (const auto& key : keys)
            {
                key_array.Add(key);
            }
        }

        /**
        * @brief CN:获取所有值\nEN:Get all values
        * @param value_array CN:输出值数组。EN:Output value array.
        */
        template<typename ValueArray>
        void GetValueArray(ValueArray& value_array) const
        {
            value_array.Clear();
            value_array.SetMaxCount((int)values.size());

            for (const auto& value : values)
            {
                value_array.Add(value);
            }
        }

        /**
        * @brief CN:获取所有键值对\nEN:Get all key-value pairs
        * @param key_array CN:输出键数组。EN:Output key array.
        * @param value_array CN:输出值数组。EN:Output value array.
        */
        template<typename KeyArray, typename ValueArray>
        void GetKeyValueArrays(KeyArray& key_array, ValueArray& value_array) const
        {
            key_array.Clear();
            value_array.Clear();
            key_array.SetMaxCount((int)keys.size());
            value_array.SetMaxCount((int)values.size());

            for (const auto& key : keys)
            {
                key_array.Add(key);
            }

            for (const auto& value : values)
            {
                value_array.Add(value);
            }
        }

        // ============================================================
        // 枚举接口
        // ============================================================

        /**
        * @brief CN:枚举所有键值对\nEN:Enumerate all key-value pairs
        */
        template<typename F>
        void EnumKV(F&& func)
        {
            for (int64 i = 0; i < (int64)keys.size(); ++i)
            {
                func(keys[i], values[i]);
            }
        }

        /**
        * @brief CN:枚举所有键值对（const版本）\nEN:Enumerate all key-value pairs (const version)
        */
        template<typename F>
        void EnumKV(F&& func) const
        {
            for (int64 i = 0; i < (int64)keys.size(); ++i)
            {
                func(keys[i], values[i]);
            }
        }

        /**
        * @brief CN:枚举所有键\nEN:Enumerate all keys
        */
        template<typename F>
        void EnumKeys(F&& func) const
        {
            for (const auto& key : keys)
            {
                func(key);
            }
        }

        /**
        * @brief CN:枚举所有值\nEN:Enumerate all values
        */
        template<typename F>
        void EnumValues(F&& func)
        {
            for (auto& value : values)
            {
                func(value);
            }
        }

        /**
        * @brief CN:枚举所有值（const版本）\nEN:Enumerate all values (const version)
        */
        template<typename F>
        void EnumValues(F&& func) const
        {
            for (const auto& value : values)
            {
                func(value);
            }
        }

        // ============================================================
        // 直接访问（低级API）
        // ============================================================

        /**
        * @brief CN:获取Key向量（直接访问）\nEN:Get Key vector (direct access)
        */
        std::vector<K>& GetKeyVector() { return keys; }
        const std::vector<K>& GetKeyVector() const { return keys; }

        /**
        * @brief CN:获取Value向量（直接访问）\nEN:Get Value vector (direct access)
        */
        std::vector<V>& GetValueVector() { return values; }
        const std::vector<V>& GetValueVector() const { return values; }

        // ============================================================
        // 迭代器支持
        // ============================================================

        /**
        * @brief CN:获取开始迭代器（Key向量）\nEN:Get begin iterator (Key vector)
        */
        auto begin() { return keys.begin(); }
        auto end() { return keys.end(); }
        auto begin() const { return keys.begin(); }
        auto end() const { return keys.end(); }

        /**
        * @brief CN:获取反向迭代器\nEN:Get reverse iterator
        */
        auto rbegin() { return keys.rbegin(); }
        auto rend() { return keys.rend(); }
        auto rbegin() const { return keys.rbegin(); }
        auto rend() const { return keys.rend(); }
    };

}  // namespace hgl
