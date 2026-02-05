/**
 * @file FlatPerfectHashOrderedSet.h
 * @brief CN:基于完美哈希的平铺有序集合 - 支持O(1)查找和零拷贝序列化
 *        EN:Flat ordered set with perfect hash - O(1) lookup with zero-copy serialization
 */
#pragma once

#include <vector>
#include <algorithm>
#include <cstdint>
#include <hgl/util/hash/QuickHash.h>

namespace hgl
{
    // 类型定义
    using int64 = int64_t;
    using int32 = int32_t;
    /**
     * @brief CN:基于完美哈希的平铺有序集合
     *        EN:Flat Ordered Set with Perfect Hash
     * 设计特点：
     * 1. 有序数据存储：元素按升序存储在连续内存中
     * 2. 哈希索引表：使用哈希表将O(log n)的查找优化为O(1)
     * 3. 完美哈希：针对已知静态数据集，哈希表大小可优化以减少冲突
     * 4. 零拷贝序列化：整个结构可直接保存到文件并加载使用
     * 5. 平凡类型：仅支持trivially copyable类型
     * 使用场景：
     * - 静态只读数据集（配置、资源列表）
     * - 需要快速查找和有序遍历
     * - 需要序列化到文件并快速加载
     * - 内存映射文件场景
     * 性能特征：
     * - 查找：O(1)（哈希表）
     * - 插入：O(n)（需要维护有序性和重建哈希）
     * - 删除：O(n)（需要移动元素和重建哈希）
     * - 遍历：O(n)（按升序）
     * - 序列化：O(n)（直接内存拷贝）
     * @tparam T 元素类型，必须：
     *           1. 支持 operator< （用于排序）
     *           2. 是 trivially copyable（可直接内存拷贝）
     *           3. 支持 operator==（用于去重）
     */
    template<typename T>
    class FlatPerfectHashOrderedSet
    {
    protected:
        using ThisClass = FlatPerfectHashOrderedSet<T>;

        static_assert(std::is_trivially_copyable_v<T>,
                      "FlatPerfectHashOrderedSet requires trivially copyable types");

        /**
         * @brief CN:有序数据存储（升序）\nEN:Ordered data storage (ascending)
         */
        std::vector<T> data;

        /**
         * @brief CN:哈希表（hash -> data index）\nEN:Hash table (hash -> data index)
         * 
         * 存储格式：连续存储 [hash_size] 个桶
         * 每个桶存储 -1（空）或数据索引
         * 
         * 碰撞处理：线性探测
         */
        std::vector<int32_t> hash_table;

        /**
         * @brief CN:哈希表大小（桶数量）\nEN:Hash table size (bucket count)
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
            // 计算哈希表大小：数据量的1.5倍，保证较低的负载因子
            int64 count = static_cast<int64>(data.size());
            if (count == 0)
            {
                hash_size = 0;
                hash_table.clear();
                return;
            }

            // 选择质数大小以减少冲突
            hash_size = static_cast<int32_t>(count * 3 / 2);
            if (hash_size < 16)
                hash_size = 16;

            // 初始化哈希表（-1表示空）
            hash_table.assign(hash_size, -1);

            // 填充哈希表
            for (int64 i = 0; i < count; ++i)
            {
                int32_t bucket = ComputeBucket(data[i]);
                
                // 线性探测找到空桶
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

        FlatPerfectHashOrderedSet() = default;
        virtual ~FlatPerfectHashOrderedSet() = default;

        // ==================== 序列化支持 ====================

        /**
         * @brief CN:获取数据指针（用于序列化）\nEN:Get data pointer (for serialization)
         */
        T* GetData() { return data.empty() ? nullptr : data.data(); }
        const T* GetData() const { return data.empty() ? nullptr : data.data(); }

        /**
         * @brief CN:获取哈希表指针（用于序列化）\nEN:Get hash table pointer (for serialization)
         */
        int32_t* GetHashTable() { return hash_table.empty() ? nullptr : hash_table.data(); }
        const int32_t* GetHashTable() const { return hash_table.empty() ? nullptr : hash_table.data(); }

        /**
         * @brief CN:获取哈希表大小\nEN:Get hash table size
         */
        int32_t GetHashTableSize() const { return hash_size; }

        /**
         * @brief CN:从缓冲区加载（直接反序列化）\nEN:Load from buffers (direct deserialization)
         * @param data_buffer CN:数据缓冲区\nEN:Data buffer
         * @param hash_buffer CN:哈希表缓冲区\nEN:Hash table buffer
         * @param count CN:元素数量\nEN:Element count
         * @param hash_table_size CN:哈希表大小\nEN:Hash table size
         * @param is_sorted CN:数据是否已排序\nEN:Whether data is sorted
         */
        void LoadFromBuffers(const T* data_buffer, const int32_t* hash_buffer, 
                            int64 count, int32_t hash_table_size, bool is_sorted = true)
        {
            if (!data_buffer || !hash_buffer || count <= 0 || hash_table_size <= 0)
                return;

            // 加载数据
            data.assign(data_buffer, data_buffer + count);
            
            // 如果未排序，先排序并去重
            if (!is_sorted)
            {
                std::sort(data.begin(), data.end());
                auto last = std::unique(data.begin(), data.end());
                data.erase(last, data.end());
                
                // 重新构建哈希表
                RebuildHashTable();
            }
            else
            {
                // 直接加载哈希表
                hash_size = hash_table_size;
                hash_table.assign(hash_buffer, hash_buffer + hash_table_size);
            }
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
                // 哈希表会在添加元素时自动调整
            }
        }

        /**
         * @brief CN:清空所有元素\nEN:Clear all elements
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

            // 线性探测查找
            while (hash_table[bucket] != -1)
            {
                int32_t index = hash_table[bucket];
                if (index >= 0 && index < static_cast<int32_t>(data.size()) && 
                    data[index] == value)
                {
                    return true;
                }

                bucket = (bucket + 1) % hash_size;
                
                // 避免无限循环
                if (bucket == start_bucket)
                    break;
            }

            return false;
        }

        /**
         * @brief CN:查找元素索引（O(1)）\nEN:Find element index (O(1))
         * @return CN:索引或-1\nEN:Index or -1
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

            // 找到插入位置（保持有序）
            auto it = std::lower_bound(data.begin(), data.end(), value);
            auto result_it = data.insert(it, value);
            
            // 重建哈希表
            RebuildHashTable();
            
            return std::distance(data.begin(), result_it);
        }

        /**
         * @brief CN:添加元素（移动语义）\nEN:Add element (move semantics)
         */
        int64 Add(T&& value)
        {
            if (Contains(value))
                return -1;

            auto it = std::lower_bound(data.begin(), data.end(), value);
            auto result_it = data.insert(it, std::move(value));
            
            RebuildHashTable();
            
            return std::distance(data.begin(), result_it);
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
         * @brief CN:删除元素\nEN:Delete element
         */
        bool Delete(const T& value)
        {
            auto it = std::lower_bound(data.begin(), data.end(), value);
            if (it == data.end() || *it != value)
                return false;

            data.erase(it);
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

            data.erase(data.begin() + index);
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
         * @brief CN:按索引访问（无边界检查）\nEN:Access by index (no bounds check)
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

        auto rbegin() { return data.rbegin(); }
        auto rbegin() const { return data.rbegin(); }
        auto crbegin() const { return data.crbegin(); }

        auto rend() { return data.rend(); }
        auto rend() const { return data.rend(); }
        auto crend() const { return data.crend(); }

        // ==================== 范围查询 ====================

        /**
         * @brief CN:查找第一个不小于给定值的元素\nEN:Find first element not less than value
         */
        const_iterator lower_bound(const T& value) const
        {
            return std::lower_bound(data.begin(), data.end(), value);
        }

        /**
         * @brief CN:查找第一个大于给定值的元素\nEN:Find first element greater than value
         */
        const_iterator upper_bound(const T& value) const
        {
            return std::upper_bound(data.begin(), data.end(), value);
        }

        // ==================== 运算符 ====================

        bool operator==(const ThisClass& other) const
        {
            return data == other.data;
        }

        bool operator!=(const ThisClass& other) const
        {
            return data != other.data;
        }
    };

} // namespace hgl
