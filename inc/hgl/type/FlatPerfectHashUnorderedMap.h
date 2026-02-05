/**
 * @file FlatPerfectHashUnorderedMap.h
 * @brief CN:基于完美哈希的平铺无序映射 - 支持O(1)查找和零拷贝序列化
 *        EN:Flat unordered map with perfect hash - O(1) lookup with zero-copy serialization
 */
#pragma once

#include <vector>
#include <cstdint>
#include <hgl/util/hash/QuickHash.h>

namespace hgl
{
    // 类型定义
    using int64 = int64_t;
    using int32 = int32_t;
    /**
     * @brief CN:基于完美哈希的平铺无序映射
     *        EN:Flat Unordered Map with Perfect Hash
     * 设计特点：
     * 1. 无序存储：Key和Value按插入顺序存储，Key[i]对应Value[i]
     * 2. 哈希索引：使用哈希表实现O(1)查找
     * 3. 完美哈希：针对静态数据集优化
     * 4. 零拷贝序列化：可直接保存/加载整个结构
     * 5. 平凡类型：Key和Value必须是trivially copyable
     * 性能特征：
     * - 查找：O(1)（哈希表）
     * - 插入：O(1) 平均
     * - 删除：O(1) 平均（swap-and-pop）
     * - 遍历：O(n)（按插入顺序）
     * - 序列化：O(n)（直接内存拷贝）
     * @tparam K Key类型，必须是trivially copyable
     * @tparam V Value类型，必须是trivially copyable
          */
    template<typename K, typename V>
    class FlatPerfectHashUnorderedMap
    {
    protected:
        using ThisClass = FlatPerfectHashUnorderedMap<K, V>;

        static_assert(std::is_trivially_copyable_v<K>,
                      "FlatPerfectHashUnorderedMap requires trivially copyable Key type");
        static_assert(std::is_trivially_copyable_v<V>,
                      "FlatPerfectHashUnorderedMap requires trivially copyable Value type");

        /**
         * @brief CN:Key数组（插入顺序）\nEN:Key array (insertion order)
         */
        std::vector<K> keys;

        /**
         * @brief CN:Value数组（与Key对齐）\nEN:Value array (aligned with keys)
         */
        std::vector<V> values;

        /**
         * @brief CN:哈希表（hash -> key index）\nEN:Hash table (hash -> key index)
         */
        std::vector<int32_t> hash_table;

        /**
         * @brief CN:哈希表大小\nEN:Hash table size
         */
        int32_t hash_size = 0;

        /**
         * @brief CN:计算哈希桶索引\nEN:Compute hash bucket index
         */
        int32_t ComputeBucket(const K& key) const
        {
            if (hash_size == 0)
                return -1;
            
            uint64_t hash = ComputeOptimalHash(key);
            return static_cast<int32_t>(hash % hash_size);
        }

        /**
         * @brief CN:重建哈希表\nEN:Rebuild hash table
         */
        void RebuildHashTable()
        {
            int64 count = static_cast<int64>(keys.size());
            if (count == 0)
            {
                hash_size = 0;
                hash_table.clear();
                return;
            }

            hash_size = static_cast<int32_t>(count * 2);
            if (hash_size < 16)
                hash_size = 16;

            hash_table.assign(hash_size, -1);

            for (int64 i = 0; i < count; ++i)
            {
                int32_t bucket = ComputeBucket(keys[i]);
                
                while (hash_table[bucket] != -1)
                {
                    bucket = (bucket + 1) % hash_size;
                }
                
                hash_table[bucket] = static_cast<int32_t>(i);
            }
        }

    public:
        using key_type = K;
        using value_type = V;
        using size_type = int64;

        FlatPerfectHashUnorderedMap() = default;
        virtual ~FlatPerfectHashUnorderedMap() = default;

        // ==================== 序列化支持 ====================

        /**
         * @brief CN:获取Key数据指针\nEN:Get key data pointer
         */
        K* GetKeyData() { return keys.empty() ? nullptr : keys.data(); }
        const K* GetKeyData() const { return keys.empty() ? nullptr : keys.data(); }

        /**
         * @brief CN:获取Value数据指针\nEN:Get value data pointer
         */
        V* GetValueData() { return values.empty() ? nullptr : values.data(); }
        const V* GetValueData() const { return values.empty() ? nullptr : values.data(); }

        /**
         * @brief CN:获取哈希表指针\nEN:Get hash table pointer
         */
        int32_t* GetHashTable() { return hash_table.empty() ? nullptr : hash_table.data(); }
        const int32_t* GetHashTable() const { return hash_table.empty() ? nullptr : hash_table.data(); }

        /**
         * @brief CN:获取哈希表大小\nEN:Get hash table size
         */
        int32_t GetHashTableSize() const { return hash_size; }

        /**
         * @brief CN:从缓冲区加载\nEN:Load from buffers
         */
        void LoadFromBuffers(const K* key_buffer, const V* value_buffer, 
                            const int32_t* hash_buffer, int64 count, 
                            int32_t hash_table_size)
        {
            if (!key_buffer || !value_buffer || !hash_buffer || count <= 0 || hash_table_size <= 0)
                return;

            keys.assign(key_buffer, key_buffer + count);
            values.assign(value_buffer, value_buffer + count);
            
            hash_size = hash_table_size;
            hash_table.assign(hash_buffer, hash_buffer + hash_table_size);
        }

        /**
         * @brief CN:获取原始数据大小（字节）\nEN:Get raw data size (bytes)
         */
        int64 GetRawDataSize() const
        {
            return static_cast<int64>(keys.size() * sizeof(K) + 
                                     values.size() * sizeof(V) +
                                     hash_table.size() * sizeof(int32_t));
        }

        // ==================== 容量管理 ====================

        /**
         * @brief CN:获取元素数量\nEN:Get element count
         */
        int64 GetCount() const { return static_cast<int64>(keys.size()); }

        /**
         * @brief CN:是否为空\nEN:Check if empty
         */
        bool IsEmpty() const { return keys.empty(); }

        /**
         * @brief CN:预留容量\nEN:Reserve capacity
         */
        void Reserve(int64 count)
        {
            if (count > 0)
            {
                keys.reserve(count);
                values.reserve(count);
            }
        }

        /**
         * @brief CN:清空\nEN:Clear
         */
        void Clear()
        {
            keys.clear();
            values.clear();
            hash_table.clear();
            hash_size = 0;
        }

        // ==================== 查找（O(1)）====================

        /**
         * @brief CN:检查键是否存在（O(1)）\nEN:Check if key exists (O(1))
         */
        bool ContainsKey(const K& key) const
        {
            return Find(key) != -1;
        }

        /**
         * @brief CN:查找键的索引（O(1)）\nEN:Find key index (O(1))
         */
        virtual int64 Find(const K& key) const
        {
            if (hash_size == 0)
                return -1;

            int32_t bucket = ComputeBucket(key);
            int32_t start_bucket = bucket;

            while (hash_table[bucket] != -1)
            {
                int32_t index = hash_table[bucket];
                if (index >= 0 && index < static_cast<int32_t>(keys.size()) && 
                    keys[index] == key)
                {
                    return static_cast<int64>(index);
                }

                bucket = (bucket + 1) % hash_size;
                if (bucket == start_bucket)
                    break;
            }

            return -1;
        }

        /**
         * @brief CN:获取值（O(1)）\nEN:Get value (O(1))
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
         * @brief CN:获取值指针（O(1)）\nEN:Get value pointer (O(1))
         */
        virtual V* GetValuePointer(const K& key)
        {
            int64 index = Find(key);
            return index >= 0 ? &values[index] : nullptr;
        }

        virtual const V* GetValuePointer(const K& key) const
        {
            int64 index = Find(key);
            return index >= 0 ? &values[index] : nullptr;
        }

        /**
         * @brief CN:获取值引用（O(1)）\nEN:Get value reference (O(1))
         */
        virtual V& GetValueRef(const K& key)
        {
            int64 index = Find(key);
            return values[index];
        }

        virtual const V& GetValueRef(const K& key) const
        {
            int64 index = Find(key);
            return values[index];
        }

        // ==================== 按索引访问 ====================

        /**
         * @brief CN:按索引获取键\nEN:Get key by index
         */
        K GetKeyAt(int64 index) const
        {
            if (index < 0 || index >= static_cast<int64>(keys.size()))
                return K();
            return keys[index];
        }

        /**
         * @brief CN:按索引获取值\nEN:Get value by index
         */
        V GetValueAt(int64 index) const
        {
            if (index < 0 || index >= static_cast<int64>(values.size()))
                return V();
            return values[index];
        }

        /**
         * @brief CN:按索引获取值指针\nEN:Get value pointer by index
         */
        V* GetValuePointerAt(int64 index)
        {
            if (index < 0 || index >= static_cast<int64>(values.size()))
                return nullptr;
            return &values[index];
        }

        const V* GetValuePointerAt(int64 index) const
        {
            if (index < 0 || index >= static_cast<int64>(values.size()))
                return nullptr;
            return &values[index];
        }

        /**
         * @brief CN:按序列号获取键值对\nEN:Get key-value pair by serial
         */
        virtual bool GetBySerial(int64 serial, K& key, V& value) const
        {
            if (serial < 0 || serial >= static_cast<int64>(keys.size()))
                return false;

            key = keys[serial];
            value = values[serial];
            return true;
        }

        // ==================== 添加 ====================

        /**
         * @brief CN:添加键值对\nEN:Add key-value pair
         */
        virtual bool Add(const K& key, const V& value)
        {
            if (ContainsKey(key))
                return false;

            // 检查是否需要重建哈希表
            if (hash_size == 0 || keys.size() * 2 >= static_cast<size_t>(hash_size))
            {
                keys.push_back(key);
                values.push_back(value);
                RebuildHashTable();
                return true;
            }

            // 直接添加
            int64 new_index = static_cast<int64>(keys.size());
            keys.push_back(key);
            values.push_back(value);

            // 更新哈希表
            int32_t bucket = ComputeBucket(key);
            while (hash_table[bucket] != -1)
            {
                bucket = (bucket + 1) % hash_size;
            }
            hash_table[bucket] = static_cast<int32_t>(new_index);

            return true;
        }

        virtual bool Add(K&& key, V&& value)
        {
            if (ContainsKey(key))
                return false;

            if (hash_size == 0 || keys.size() * 2 >= static_cast<size_t>(hash_size))
            {
                keys.push_back(std::move(key));
                values.push_back(std::move(value));
                RebuildHashTable();
                return true;
            }

            int64 new_index = static_cast<int64>(keys.size());
            keys.push_back(std::move(key));
            values.push_back(std::move(value));

            int32_t bucket = ComputeBucket(keys.back());
            while (hash_table[bucket] != -1)
            {
                bucket = (bucket + 1) % hash_size;
            }
            hash_table[bucket] = static_cast<int32_t>(new_index);

            return true;
        }

        /**
         * @brief CN:添加或更新\nEN:Add or update
         */
        virtual bool AddOrUpdate(const K& key, const V& value)
        {
            int64 index = Find(key);
            if (index >= 0)
            {
                values[index] = value;
                return false;
            }

            Add(key, value);
            return true;
        }

        /**
         * @brief CN:修改值\nEN:Change value
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

        // ==================== 删除 ====================

        /**
         * @brief CN:按键删除（swap-and-pop）\nEN:Delete by key (swap-and-pop)
         */
        virtual bool DeleteByKey(const K& key)
        {
            int64 index = Find(key);
            if (index < 0)
                return false;

            // Swap with last element and pop
            if (index != static_cast<int64>(keys.size() - 1))
            {
                keys[index] = keys.back();
                values[index] = values.back();
            }
            keys.pop_back();
            values.pop_back();

            RebuildHashTable();

            return true;
        }

        /**
         * @brief CN:按索引删除\nEN:Delete by index
         */
        virtual bool DeleteAt(int64 index)
        {
            if (index < 0 || index >= static_cast<int64>(keys.size()))
                return false;

            if (index != static_cast<int64>(keys.size() - 1))
            {
                keys[index] = keys.back();
                values[index] = values.back();
            }
            keys.pop_back();
            values.pop_back();

            RebuildHashTable();

            return true;
        }

        // ==================== 枚举 ====================

        /**
         * @brief CN:枚举所有键值对\nEN:Enumerate all key-value pairs
         */
        template<typename F>
        void EnumKV(F&& func)
        {
            for (int64 i = 0; i < static_cast<int64>(keys.size()); ++i)
            {
                func(keys[i], values[i]);
            }
        }

        template<typename F>
        void EnumKV(F&& func) const
        {
            for (int64 i = 0; i < static_cast<int64>(keys.size()); ++i)
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
        void EnumValues(F&& func) const
        {
            for (const auto& value : values)
            {
                func(value);
            }
        }

        // ==================== 迭代器（遍历Key）====================

        auto begin() { return keys.begin(); }
        auto end() { return keys.end(); }
        auto begin() const { return keys.begin(); }
        auto end() const { return keys.end(); }
    };

} // namespace hgl
