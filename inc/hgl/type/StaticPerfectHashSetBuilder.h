/**
 * @file StaticPerfectHashSetBuilder.h
 * @brief 静态完美哈希集合构建器 - 编辑器工具
 * 
 * 实现简化的CHD算法，用于构建完美哈希集合。
 */
#pragma once

#include <hgl/type/StaticPerfectHashSet.h>
#include <vector>
#include <algorithm>
#include <set>
#include <cmath>
#include <fstream>
#include <chrono>

namespace hgl
{
    /**
     * @brief 构建统计信息
     */
    struct BuildStatsSet
    {
        int retry_count = 0;        // 重试次数 (0表示第一次成功)
        int num_buckets = 0;        // 最终桶数量
        int num_keys = 0;           // 键数量
        float space_overhead = 0.0f; // 空间开销百分比
        uint64_t build_time_us = 0;  // 构建时间(微秒)
    };

    /**
     * @brief 静态完美哈希集合构建器
     * 
     * 使用CHD算法构建零碰撞的完美哈希集合。
     * 
     * @tparam K Key类型
     */
    template<typename K>
    class StaticPerfectHashSetBuilder
    {
    private:
        std::vector<K> keys;                    // 输入键
        std::vector<uint16_t> displacement_table;  // 位移表
        std::vector<K> sorted_keys;             // 排列后的键
        uint32_t num_buckets;                   // 桶数量
        BuildStatsSet build_stats;              // 构建统计

        // 哈希函数（与运行时容器相同）
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
        StaticPerfectHashSetBuilder() = default;

        /**
         * @brief 添加键
         */
        void Add(const K& key)
        {
            keys.push_back(key);
        }

        /**
         * @brief 清空所有键
         */
        void Clear()
        {
            keys.clear();
            displacement_table.clear();
            sorted_keys.clear();
        }

        /**
         * @brief 获取键数量
         */
        size_t GetCount() const
        {
            return keys.size();
        }

    private:
        /**
         * @brief 尝试用指定桶数量构建
         * @param target_buckets 目标桶数量
         * @return 成功返回true
         */
        bool TryBuildWithBuckets(uint32_t target_buckets)
        {
            uint32_t n = static_cast<uint32_t>(keys.size());
            num_buckets = target_buckets;
            if (num_buckets == 0)
                num_buckets = 1;

            // 步骤1：将键分配到桶中
            std::vector<std::vector<K>> buckets(num_buckets);
            for (const K& key : keys)
            {
                uint32_t bucket = Hash1(key) % num_buckets;
                buckets[bucket].push_back(key);
            }

            // 步骤2：按桶大小排序（大桶优先处理）
            std::vector<uint32_t> bucket_order(num_buckets);
            for (uint32_t i = 0; i < num_buckets; ++i)
                bucket_order[i] = i;
            
            std::sort(bucket_order.begin(), bucket_order.end(),
                [&buckets](uint32_t a, uint32_t b) {
                    return buckets[a].size() > buckets[b].size();
                });

            // 步骤3：为每个桶计算位移值
            displacement_table.resize(num_buckets, 0);
            sorted_keys.resize(n);
            std::vector<bool> occupied(n, false);

            for (uint32_t b : bucket_order)
            {
                const auto& bucket = buckets[b];
                if (bucket.empty())
                    continue;

                // 尝试不同的位移值，直到找到无碰撞的
                bool found = false;
                uint32_t max_tries = 65536;  // 尝试所有uint16_t值
                for (uint32_t disp = 0; disp < max_tries; ++disp)
                {
                    std::vector<uint32_t> positions;
                    bool collision = false;

                    // 检查这个位移值是否导致碰撞
                    for (const K& key : bucket)
                    {
                        uint64_t h1 = Hash1(key);
                        uint64_t h2 = Hash2(key);
                        uint32_t pos = (h1 + disp * h2) % n;

                        // 检查是否与已占用的位置冲突
                        if (occupied[pos])
                        {
                            collision = true;
                            break;
                        }
                        
                        // 检查是否与同桶内其他键冲突
                        for (uint32_t p : positions)
                        {
                            if (p == pos)
                            {
                                collision = true;
                                break;
                            }
                        }
                        if (collision)
                            break;
                        
                        positions.push_back(pos);
                    }

                    // 找到无碰撞的位移值
                    if (!collision)
                    {
                        displacement_table[b] = static_cast<uint16_t>(disp % 65536);
                        
                        // 标记这些位置并存储键
                        for (size_t i = 0; i < bucket.size(); ++i)
                        {
                            uint32_t pos = positions[i];
                            occupied[pos] = true;
                            sorted_keys[pos] = bucket[i];
                        }
                        
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    // 无法找到合适的位移值（罕见情况）
                    return false;
                }
            }

            // 验证所有键都已放置
            uint32_t placed_count = 0;
            for (bool occ : occupied)
            {
                if (occ)
                    placed_count++;
            }
            
            if (placed_count != n)
                return false;

            return true;
        }

    public:
        /**
         * @brief 构建完美哈希集合（带自动重试机制）
         * 
         * 使用自动重试策略：
         * - Round 1: num_buckets = N (原始)
         * - Round 2: num_buckets = N * 1.5 (增加50%)
         * - Round 3: num_buckets = N * 2.0 (双倍)
         * 
         * @return 成功返回true
         */
        bool Build()
        {
            if (keys.empty())
                return false;

            uint32_t n = static_cast<uint32_t>(keys.size());
            
            // 开始计时
            auto start = std::chrono::high_resolution_clock::now();
            
            // 尝试3轮，逐步增加桶数量
            const float multipliers[] = {1.0f, 1.5f, 2.0f};
            
            for (int round = 0; round < 3; round++)
            {
                uint32_t target_buckets = static_cast<uint32_t>(n * multipliers[round]);
                if (target_buckets < n)
                    target_buckets = n;
                
                if (TryBuildWithBuckets(target_buckets))
                {
                    // 构建成功，记录统计信息
                    auto end = std::chrono::high_resolution_clock::now();
                    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
                    
                    build_stats.retry_count = round;
                    build_stats.num_buckets = num_buckets;
                    build_stats.num_keys = n;
                    build_stats.space_overhead = ((float)(num_buckets - n) / n) * 100.0f;
                    build_stats.build_time_us = duration.count();
                    
                    return true;
                }
            }
            
            // 所有重试都失败（极罕见）
            return false;
        }

        /**
         * @brief 获取构建统计信息
         * @return 构建统计
         */
        const BuildStatsSet& GetBuildStats() const
        {
            return build_stats;
        }

        /**
         * @brief 保存到内存缓冲区
         * @param buffer 输出缓冲区
         * @return 缓冲区大小
         */
        size_t SaveToMemory(std::vector<uint8_t>& buffer) const
        {
            if (sorted_keys.empty())
                return 0;

            typename StaticPerfectHashSet<K>::FileHeader header;
            std::memcpy(header.magic, "SPHS", 4);
            header.version = 1;
            header.num_keys = static_cast<uint32_t>(sorted_keys.size());
            header.num_buckets = num_buckets;
            header.key_size = sizeof(K);
            header.checksum = header.num_keys + header.num_buckets;
            header.reserved[0] = 0;
            header.reserved[1] = 0;

            size_t total_size = sizeof(header) + 
                              num_buckets * sizeof(uint16_t) +
                              header.num_keys * sizeof(K);

            buffer.resize(total_size);
            uint8_t* ptr = buffer.data();

            // 写入文件头
            std::memcpy(ptr, &header, sizeof(header));
            ptr += sizeof(header);

            // 写入位移表
            std::memcpy(ptr, displacement_table.data(), 
                       num_buckets * sizeof(uint16_t));
            ptr += num_buckets * sizeof(uint16_t);

            // 写入键数组
            std::memcpy(ptr, sorted_keys.data(), 
                       header.num_keys * sizeof(K));

            return total_size;
        }

        /**
         * @brief 保存到文件
         * @param filename 文件路径
         * @return 成功返回true
         */
        bool SaveToFile(const char* filename) const
        {
            std::vector<uint8_t> buffer;
            if (SaveToMemory(buffer) == 0)
                return false;

            std::ofstream file(filename, std::ios::binary);
            if (!file.is_open())
                return false;

            file.write(reinterpret_cast<const char*>(buffer.data()), buffer.size());
            file.close();
            return true;
        }

        /**
         * @brief 获取统计信息
         */
        struct Stats
        {
            uint32_t num_keys;
            uint32_t num_buckets;
            size_t total_size;
            double space_overhead;  // 相对于原始数据的开销百分比
        };

        Stats GetStats() const
        {
            Stats stats;
            stats.num_keys = static_cast<uint32_t>(sorted_keys.size());
            stats.num_buckets = num_buckets;
            stats.total_size = sizeof(typename StaticPerfectHashSet<K>::FileHeader) +
                              num_buckets * sizeof(uint16_t) +
                              stats.num_keys * sizeof(K);
            
            size_t raw_data_size = stats.num_keys * sizeof(K);
            stats.space_overhead = 100.0 * (stats.total_size - raw_data_size) / raw_data_size;
            
            return stats;
        }

        /**
         * @brief 获取排序后的键（用于调试）
         */
        const std::vector<K>& GetSortedKeys() const
        {
            return sorted_keys;
        }
    };

} // namespace hgl
