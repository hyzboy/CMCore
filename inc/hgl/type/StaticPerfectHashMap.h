/**
 * @file StaticPerfectHashMap.h
 * @brief 静态完美哈希映射 - 运行时容器（最小原型）
 * 
 * 核心特性：
 * - 零碰撞查找（O(1)最坏情况）
 * - 编辑器预构建
 * - 运行时直接加载
 * - 纯静态数据
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>

namespace hgl
{
    /**
     * @brief 静态完美哈希映射运行时容器
     * 
     * 使用CHD算法预构建的完美哈希表。
     * 运行时直接加载，无需重建索引。
     * 
     * @tparam K Key类型（必须是trivially copyable）
     * @tparam V Value类型（必须是trivially copyable）
     */
    template<typename K, typename V>
    class StaticPerfectHashMap
    {
    public:
        static_assert(std::is_trivially_copyable_v<K>, "Key type must be trivially copyable");
        static_assert(std::is_trivially_copyable_v<V>, "Value type must be trivially copyable");

        /**
         * @brief 文件头结构（简化版）
         */
        struct FileHeader
        {
            char magic[4];              // "SPHM" (Static Perfect Hash Map)
            uint32_t version;           // 版本号（当前为1）
            uint32_t num_keys;          // 键数量
            uint32_t num_buckets;       // 桶数量
            uint32_t key_size;          // sizeof(K)
            uint32_t value_size;        // sizeof(V)
            uint32_t checksum;          // 简单校验和（键数量+桶数量）
        };

    private:
        // 运行时数据
        FileHeader header;
        std::vector<uint16_t> displacement_table;  // 位移表
        std::vector<K> keys;                       // 键数组（按哈希排列）
        std::vector<V> values;                     // 值数组（与键对齐）

        // 哈希函数（简化版FNV-1a）
        uint64_t Hash1(const K& key) const
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(&key);
            uint64_t hash = 14695981039346656037ULL;
            for (size_t i = 0; i < sizeof(K); ++i)
            {
                hash ^= data[i];
                hash *= 1099511628211ULL;
            }
            return hash;
        }

        uint64_t Hash2(const K& key) const
        {
            const uint8_t* data = reinterpret_cast<const uint8_t*>(&key);
            uint64_t hash = 2166136261u;
            for (size_t i = 0; i < sizeof(K); ++i)
            {
                hash ^= data[i];
                hash *= 16777619u;
            }
            return hash;
        }

    public:
        StaticPerfectHashMap() = default;
        ~StaticPerfectHashMap() = default;

        /**
         * @brief 从内存缓冲区加载（零拷贝概念验证）
         * @param data 内存数据指针
         * @param size 数据大小
         * @return 成功返回true
         */
        bool LoadFromMemory(const void* data, size_t size)
        {
            if (!data || size < sizeof(FileHeader))
                return false;

            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
            
            // 读取文件头
            std::memcpy(&header, ptr, sizeof(FileHeader));
            ptr += sizeof(FileHeader);

            // 验证魔数
            if (std::memcmp(header.magic, "SPHM", 4) != 0)
                return false;

            // 验证类型大小
            if (header.key_size != sizeof(K) || header.value_size != sizeof(V))
                return false;

            // 验证校验和
            if (header.checksum != (header.num_keys + header.num_buckets))
                return false;

            // 加载位移表
            displacement_table.resize(header.num_buckets);
            std::memcpy(displacement_table.data(), ptr, 
                       header.num_buckets * sizeof(uint16_t));
            ptr += header.num_buckets * sizeof(uint16_t);

            // 加载键数组
            keys.resize(header.num_keys);
            std::memcpy(keys.data(), ptr, header.num_keys * sizeof(K));
            ptr += header.num_keys * sizeof(K);

            // 加载值数组
            values.resize(header.num_keys);
            std::memcpy(values.data(), ptr, header.num_keys * sizeof(V));

            return true;
        }

        /**
         * @brief 查找键对应的值（O(1)最坏情况）
         * @param key 要查找的键
         * @return 值的指针，如果键不存在返回nullptr
         */
        const V* Get(const K& key) const
        {
            if (keys.empty())
                return nullptr;

            // CHD查找算法
            uint64_t h1 = Hash1(key);
            uint64_t h2 = Hash2(key);
            
            uint32_t bucket = h1 % header.num_buckets;
            uint16_t disp = displacement_table[bucket];
            
            // 计算最终位置（完美哈希，零碰撞）
            uint32_t index = (h1 + disp * h2) % header.num_keys;

            // 验证键是否匹配（防止查询不存在的键）
            if (std::memcmp(&keys[index], &key, sizeof(K)) == 0)
                return &values[index];

            return nullptr;
        }

        /**
         * @brief 检查键是否存在
         */
        bool Contains(const K& key) const
        {
            return Get(key) != nullptr;
        }

        /**
         * @brief 获取键的数量
         */
        uint32_t GetCount() const
        {
            return header.num_keys;
        }

        /**
         * @brief 获取所有键（用于遍历）
         */
        const std::vector<K>& GetKeys() const
        {
            return keys;
        }

        /**
         * @brief 获取所有值（用于遍历）
         */
        const std::vector<V>& GetValues() const
        {
            return values;
        }
    };

} // namespace hgl
