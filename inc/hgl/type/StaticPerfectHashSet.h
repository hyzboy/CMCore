/**
 * @file StaticPerfectHashSet.h
 * @brief 静态完美哈希集合 - 运行时容器
 * 
 * 核心特性：
 * - 零碰撞查找（O(1)最坏情况）
 * - 编辑器预构建
 * - 运行时直接加载
 * - 纯静态数据
 * - 只存储键，不存储值
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <vector>
#include <fstream>

namespace hgl
{
    /**
     * @brief 静态完美哈希集合运行时容器
     * 
     * 使用CHD算法预构建的完美哈希集合。
     * 运行时直接加载，无需重建索引。
     * 
     * @tparam K Key类型（必须是trivially copyable）
     */
    template<typename K>
    class StaticPerfectHashSet
    {
    public:
        static_assert(std::is_trivially_copyable_v<K>, "Key type must be trivially copyable");

        /**
         * @brief 文件头结构
         */
        struct FileHeader
        {
            char magic[4];              // "SPHS" (Static Perfect Hash Set)
            uint32_t version;           // 版本号（当前为1）
            uint32_t num_keys;          // 键数量
            uint32_t num_buckets;       // 桶数量
            uint32_t key_size;          // sizeof(K)
            uint32_t checksum;          // 简单校验和（键数量+桶数量）
            uint32_t reserved[2];       // 保留字段
        };

    private:
        // 运行时数据
        FileHeader header;
        std::vector<uint16_t> displacement_table;  // 位移表
        std::vector<K> keys;                       // 键数组（按哈希排列）

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
        StaticPerfectHashSet() = default;
        ~StaticPerfectHashSet() = default;

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
            if (std::memcmp(header.magic, "SPHS", 4) != 0)
                return false;

            // 验证类型大小
            if (header.key_size != sizeof(K))
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

            return true;
        }

        /**
         * @brief 检查键是否存在（O(1)最坏情况）
         * @param key 要查找的键
         * @return 键存在返回true
         */
        bool Contains(const K& key) const
        {
            if (keys.empty())
                return false;

            // CHD查找算法
            uint64_t h1 = Hash1(key);
            uint64_t h2 = Hash2(key);
            
            uint32_t bucket = h1 % header.num_buckets;
            uint16_t disp = displacement_table[bucket];
            
            // 计算最终位置（完美哈希，零碰撞）
            uint32_t index = (h1 + disp * h2) % header.num_keys;

            // 验证键是否匹配（防止查询不存在的键）
            return std::memcmp(&keys[index], &key, sizeof(K)) == 0;
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
         * @brief 从文件加载MPH数据
         * @param filename 文件路径
         * @return 成功返回true
         */
        bool LoadFromFile(const char* filename)
        {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open())
                return false;

            // 获取文件大小
            file.seekg(0, std::ios::end);
            size_t file_size = file.tellg();
            file.seekg(0, std::ios::beg);

            // 读取到内存
            std::vector<uint8_t> buffer(file_size);
            file.read(reinterpret_cast<char*>(buffer.data()), file_size);
            file.close();

            // 从内存加载
            return LoadFromMemory(buffer.data(), buffer.size());
        }

        /**
         * @brief 清空集合
         */
        void Clear()
        {
            displacement_table.clear();
            keys.clear();
            std::memset(&header, 0, sizeof(header));
        }

        /**
         * @brief 检查集合是否为空
         */
        bool IsEmpty() const
        {
            return keys.empty();
        }
    };

} // namespace hgl
