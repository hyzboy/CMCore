/**
 * @file FlatPerfectHashUnorderedSet.h
 * @brief CN:基于完美哈希的平铺无序集合 - 支持O(1)查找和零拷贝序列化
 *        EN:Flat unordered set with perfect hash - O(1) lookup with zero-copy serialization
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
     * @brief CN:基于完美哈希的平铺无序集合
     *        EN:Flat Unordered Set with Perfect Hash
     * 设计特点：
     * 1. 无序存储：元素按插入顺序存储在连续内存中
     * 2. 哈希索引：使用哈希表实现O(1)查找
     * 3. 完美哈希：针对静态数据集优化，负载因子可调
     * 4. 零拷贝序列化：整个结构可直接保存/加载
     * 5. 平凡类型：仅支持trivially copyable类型
     * 使用场景：
     * - 静态只读数据集
     * - 需要快速查找但不关心顺序
     * - 需要序列化到文件并快速加载
     * - 内存映射文件场景
     * 性能特征：
     * - 查找：O(1)（哈希表）
     * - 插入：O(1) 平均，最坏O(n)（需要重建哈希）
     * - 删除：O(1) 平均，最坏O(n)（需要移动最后元素和重建哈希）
     * - 遍历：O(n)（按插入顺序）
     * - 序列化：O(n)（直接内存拷贝）
     * @tparam T 元素类型，必须：
     *           1. 是 trivially copyable（可直接内存拷贝）
     *           2. 支持 operator==（用于去重）
          */
    template<typename T>
    class FlatPerfectHashUnorderedSet
    {
    protected:
        using ThisClass = FlatPerfectHashUnorderedSet<T>;

        static_assert(std::is_trivially_copyable_v<T>,
                      "FlatPerfectHashUnorderedSet requires trivially copyable types");

        /**
         * @brief CN:数据存储（插入顺序）\nEN:Data storage (insertion order)
         */
        std::vector<T> data;

        /**
         * @brief CN:哈希表（hash -> data index）\nEN:Hash table (hash -> data index)
         * -1 表示空桶
         */
        std::vector<int32_t> hash_table;

        /**
         * @brief CN:哈希表大小\nEN:Hash table size
         */
        int32_t hash_size = 0;

        /**
         * @brief CN:计算哈希桶索引\nEN:Compute hash bucket index
         */
        int32_t ComputeBucket(const T& value) const
        {
            if (hash_size == 0)
                return -1;
            
            uint64_t hash = ComputeOptimalHash(value);
            return static_cast<int32_t>(hash % hash_size);
        }

        /**
         * @brief CN:重建哈希表\nEN:Rebuild hash table
         */
        void RebuildHashTable()
        {
            int64 count = static_cast<int64>(data.size());
            if (count == 0)
            {
                hash_size = 0;
                hash_table.clear();
                return;
            }

            // 使用2倍大小以保证良好性能
            hash_size = static_cast<int32_t>(count * 2);
            if (hash_size < 16)
                hash_size = 16;

            hash_table.assign(hash_size, -1);

            for (int64 i = 0; i < count; ++i)
            {
                int32_t bucket = ComputeBucket(data[i]);
                
                // 线性探测
                while (hash_table[bucket] != -1)
                {
                    bucket = (bucket + 1) % hash_size;
                }
                
                hash_table[bucket] = static_cast<int32_t>(i);
            }
        }

    public:
        using iterator = typename std::vector<T>::iterator;
        using const_iterator = typename std::vector<T>::const_iterator;

        FlatPerfectHashUnorderedSet() = default;
        virtual ~FlatPerfectHashUnorderedSet() = default;

        // ==================== 序列化支持 ====================

        /**
         * @brief CN:获取数据指针\nEN:Get data pointer
         */
        T* GetData() { return data.empty() ? nullptr : data.data(); }
        const T* GetData() const { return data.empty() ? nullptr : data.data(); }

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
         * @param data_buffer CN:数据缓冲区\nEN:Data buffer
         * @param hash_buffer CN:哈希表缓冲区\nEN:Hash table buffer
         * @param count CN:元素数量\nEN:Element count
         * @param hash_table_size CN:哈希表大小\nEN:Hash table size
         */
        void LoadFromBuffers(const T* data_buffer, const int32_t* hash_buffer, 
                            int64 count, int32_t hash_table_size)
        {
            if (!data_buffer || !hash_buffer || count <= 0 || hash_table_size <= 0)
                return;

            // 直接加载数据
            data.assign(data_buffer, data_buffer + count);
            
            // 直接加载哈希表
            hash_size = hash_table_size;
            hash_table.assign(hash_buffer, hash_buffer + hash_table_size);
        }

        /**
         * @brief CN:获取原始数据大小（字节）\nEN:Get raw data size (bytes)
         */
        int64 GetRawDataSize() const
        {
            return static_cast<int64>(data.size() * sizeof(T) + 
                                     hash_table.size() * sizeof(int32_t));
        }

        // ==================== 容量管理 ====================

        /**
         * @brief CN:获取元素数量\nEN:Get element count
         */
        int64 GetCount() const { return static_cast<int64>(data.size()); }

        /**
         * @brief CN:是否为空\nEN:Check if empty
         */
        bool IsEmpty() const { return data.empty(); }

        /**
         * @brief CN:预留容量\nEN:Reserve capacity
         */
        void Reserve(int64 count)
        {
            if (count > 0)
            {
                data.reserve(count);
            }
        }

        /**
         * @brief CN:清空\nEN:Clear
         */
        void Clear()
        {
            data.clear();
            hash_table.clear();
            hash_size = 0;
        }

        // ==================== 查找（O(1)）====================

        /**
         * @brief CN:检查元素是否存在（O(1)）\nEN:Check if element exists (O(1))
         */
        bool Contains(const T& value) const
        {
            if (hash_size == 0)
                return false;

            int32_t bucket = ComputeBucket(value);
            int32_t start_bucket = bucket;

            while (hash_table[bucket] != -1)
            {
                int32_t index = hash_table[bucket];
                if (index >= 0 && index < static_cast<int32_t>(data.size()) && 
                    data[index] == value)
                {
                    return true;
                }

                bucket = (bucket + 1) % hash_size;
                if (bucket == start_bucket)
                    break;
            }

            return false;
        }

        /**
         * @brief CN:查找元素索引（O(1)）\nEN:Find element index (O(1))
         */
        int64 Find(const T& value) const
        {
            if (hash_size == 0)
                return -1;

            int32_t bucket = ComputeBucket(value);
            int32_t start_bucket = bucket;

            while (hash_table[bucket] != -1)
            {
                int32_t index = hash_table[bucket];
                if (index >= 0 && index < static_cast<int32_t>(data.size()) && 
                    data[index] == value)
                {
                    return static_cast<int64>(index);
                }

                bucket = (bucket + 1) % hash_size;
                if (bucket == start_bucket)
                    break;
            }

            return -1;
        }

        // ==================== 添加 ====================

        /**
         * @brief CN:添加元素\nEN:Add element
         * @return CN:成功返回索引，已存在返回-1\nEN:Returns index if added, -1 if exists
         */
        int64 Add(const T& value)
        {
            // 检查是否已存在
            if (Contains(value))
                return -1;

            // 检查是否需要重建哈希表
            if (hash_size == 0 || data.size() * 2 >= static_cast<size_t>(hash_size))
            {
                data.push_back(value);
                RebuildHashTable();
                return static_cast<int64>(data.size() - 1);
            }

            // 直接添加
            int64 new_index = static_cast<int64>(data.size());
            data.push_back(value);

            // 更新哈希表
            int32_t bucket = ComputeBucket(value);
            while (hash_table[bucket] != -1)
            {
                bucket = (bucket + 1) % hash_size;
            }
            hash_table[bucket] = static_cast<int32_t>(new_index);

            return new_index;
        }

        /**
         * @brief CN:添加元素（移动语义）\nEN:Add element (move semantics)
         */
        int64 Add(T&& value)
        {
            if (Contains(value))
                return -1;

            if (hash_size == 0 || data.size() * 2 >= static_cast<size_t>(hash_size))
            {
                data.push_back(std::move(value));
                RebuildHashTable();
                return static_cast<int64>(data.size() - 1);
            }

            int64 new_index = static_cast<int64>(data.size());
            data.push_back(std::move(value));

            int32_t bucket = ComputeBucket(data.back());
            while (hash_table[bucket] != -1)
            {
                bucket = (bucket + 1) % hash_size;
            }
            hash_table[bucket] = static_cast<int32_t>(new_index);

            return new_index;
        }

        /**
         * @brief CN:批量添加\nEN:Add multiple elements
         */
        int64 Add(const T* values, int64 count)
        {
            if (!values || count <= 0)
                return 0;

            int64 added = 0;
            for (int64 i = 0; i < count; ++i)
            {
                if (Add(values[i]) >= 0)
                    ++added;
            }

            return added;
        }

        // ==================== 删除 ====================

        /**
         * @brief CN:删除元素（swap-and-pop策略）\nEN:Delete element (swap-and-pop)
         * 
         * 注意：删除会改变最后一个元素的索引位置
         */
        bool Delete(const T& value)
        {
            int64 index = Find(value);
            if (index < 0)
                return false;

            // Swap with last element and pop
            if (index != static_cast<int64>(data.size() - 1))
            {
                data[index] = data.back();
            }
            data.pop_back();

            // 重建哈希表
            RebuildHashTable();

            return true;
        }

        /**
         * @brief CN:按索引删除\nEN:Delete by index
         */
        bool DeleteAt(int64 index)
        {
            if (index < 0 || index >= static_cast<int64>(data.size()))
                return false;

            // Swap with last and pop
            if (index != static_cast<int64>(data.size() - 1))
            {
                data[index] = data.back();
            }
            data.pop_back();

            RebuildHashTable();

            return true;
        }

        // ==================== 访问 ====================

        /**
         * @brief CN:按索引获取元素\nEN:Get element by index
         */
        bool Get(int64 index, T& value) const
        {
            if (index < 0 || index >= static_cast<int64>(data.size()))
                return false;

            value = data[index];
            return true;
        }

        /**
         * @brief CN:按索引访问\nEN:Access by index
         */
        const T& operator[](int64 index) const { return data[index]; }

        /**
         * @brief CN:获取第一个元素\nEN:Get first element
         */
        bool GetFirst(T& value) const
        {
            if (data.empty())
                return false;
            value = data.front();
            return true;
        }

        /**
         * @brief CN:获取最后一个元素\nEN:Get last element
         */
        bool GetLast(T& value) const
        {
            if (data.empty())
                return false;
            value = data.back();
            return true;
        }

        // ==================== 迭代器 ====================

        iterator begin() { return data.begin(); }
        const_iterator begin() const { return data.begin(); }
        const_iterator cbegin() const { return data.cbegin(); }

        iterator end() { return data.end(); }
        const_iterator end() const { return data.end(); }
        const_iterator cend() const { return data.cend(); }

        // ==================== 运算符 ====================

        bool operator==(const ThisClass& other) const
        {
            if (data.size() != other.data.size())
                return false;

            // 检查所有元素是否都存在
            for (const auto& val : data)
            {
                if (!other.Contains(val))
                    return false;
            }

            return true;
        }

        bool operator!=(const ThisClass& other) const
        {
            return !(*this == other);
        }
    };

} // namespace hgl
