/**
 * @file FlatUnorderedSet_Sharded.h
 * @brief CN:分片无序集合（按哈希高位分片）- ShardedSet
 */
#pragma once

#include <type_traits>
#include <vector>
#include <array>
#include <hgl/util/hash/QuickHash.h>
#include <hgl/type/ActiveDataManager.h>
#include <ankerl/unordered_dense.h>

namespace hgl
{
    // AI NOTE: Hash set sharded into SHARD_COUNT partitions (power of two).
    // Each shard has its own ActiveDataManager and hash->id map.
    // Operations hash value, pick shard, then act locally.
    /**
     * 【分片集合 ShardedSet】
     *
     * 【原理】
     * 将数据按哈希值高位分割为K个独立分片(默认16个)：
     *  • 哈希值的第4位决定分片编号: (hash >> 60) & 0xF
     *  • 每个分片独立维护ActiveDataManager和HashMap
     *  • 每个分片完全隔离，可独立操作和扩展
     *
     * 架构：
     *  Shard[0]  ┌─ data_manager ┐  Shard[1]  ┌─ data_manager ┐
     *            ├─ hash_map    ┤             ├─ hash_map    ┤
     *            └─ deleted_cnt ┘             └─ deleted_cnt ┘
     *  .......
     *  Shard[15] ┌─ data_manager ┐
     *            ├─ hash_map    ┤
     *            └─ deleted_cnt ┘
     *
     * 【特点】
     *  • 低争用 - 分片间无锁竞争
     *  • 可并行化 - 可为每个分片创建线程
     *  • 大规模友好 - 百万级元素可保持性能
     *  • 可预测性能 - 单分片大小恒定(n/16)
     *  • 平衡设计 - 平衡了内存和访问速度
     *
     * 【适用场景】✓
     *  • 大规模数据集(> 100k元素)
     *  • 多线程环境(可并行操作各分片)
     *  • 高并发读写
     *  • 需要可预测性能的系统
     *  • 分布式缓存(可为每分片配置)
     *  • 不适合：小数据集(16个分片开销大)
     *
     * 【性能特征】
     *  • Insert: O(1) avg, O(n/16) worst
     *  • Delete: O(1) avg, O(n/16) worst
     *  • Lookup: O(1) avg, O(n/16) worst
     *  • 内存占用: 16倍ActiveDataManager (固定)
     *  • 并行度: 16个独立分片
     *
     * 【配置】
     *  • SHARD_COUNT模板参数: 分片数(必须是2的幂)
     *  • Reserve(capacity): 均分容量到各分片
     *  • RefreshHashMap(): 全局重建所有分片哈希表
     *
     * 【使用建议】
     *  • 元素数量预期 > 10000 时开始考虑
     *  • 有锁或并发需求时强烈推荐
     *  • 可通过reduce RefreshHashMap()调用次数优化
     */
    template<typename T, int SHARD_COUNT = 16>
    class ShardedSet
    {
        static_assert((SHARD_COUNT & (SHARD_COUNT - 1)) == 0, "SHARD_COUNT must be power of two.");

    protected:
        using HashMap = ankerl::unordered_dense::map<uint64, std::vector<int>>;

        struct Shard
        {
            ActiveDataManager<T> data_manager;
            HashMap hash_map;
            int deleted_count = 0;
        };

        std::array<Shard, SHARD_COUNT> shards;

        int GetShardIndex(uint64 hash) const
        {
            return (int)((hash >> 60) & (SHARD_COUNT - 1));
        }

        int FindID(const Shard &shard, const T &value) const
        {
            uint64 hash = ComputeOptimalHash(value);
            auto it = shard.hash_map.find(hash);
            if (it == shard.hash_map.end())
                return -1;

            for (int id : it->second)
            {
                if (!shard.data_manager.IsActive(id))
                    continue;
                T existing;
                if (shard.data_manager.GetData(existing, id) && existing == value)
                    return id;
            }
            return -1;
        }

    public:
        ShardedSet() = default;
        virtual ~ShardedSet() = default;

        void Reserve(int capacity)
        {
            int per = capacity / SHARD_COUNT + 1;
            for (auto &shard : shards)
                shard.data_manager.Reserve(per);
        }

        int GetCount() const
        {
            int total = 0;
            for (const auto &shard : shards)
                total += shard.data_manager.GetActiveCount();
            return total;
        }

        bool IsEmpty() const
        {
            return GetCount() == 0;
        }

        bool Add(const T &value)
        {
            static_assert(std::is_trivially_copyable_v<T>, "Add() requires trivially copyable types.");
            uint64 hash = ComputeOptimalHash(value);
            int shard_index = GetShardIndex(hash);
            auto &shard = shards[shard_index];

            if (FindID(shard, value) != -1)
                return false;

            int new_id;
            if (!shard.data_manager.GetOrCreate(&new_id, 1))
                return false;

            if (!shard.data_manager.WriteData(value, new_id))
            {
                shard.data_manager.Release(&new_id, 1);
                return false;
            }

            shard.hash_map[hash].push_back(new_id);
            return true;
        }

        bool Delete(const T &value)
        {
            uint64 hash = ComputeOptimalHash(value);
            int shard_index = GetShardIndex(hash);
            auto &shard = shards[shard_index];

            int id = FindID(shard, value);
            if (id == -1)
                return false;

            if (shard.data_manager.Release(&id, 1) > 0)
            {
                ++shard.deleted_count;
                return true;
            }

            return false;
        }

        bool Contains(const T &value) const
        {
            uint64 hash = ComputeOptimalHash(value);
            int shard_index = GetShardIndex(hash);
            const auto &shard = shards[shard_index];
            return FindID(shard, value) != -1;
        }

        void Clear()
        {
            for (auto &shard : shards)
            {
                shard.data_manager.Clear();
                shard.hash_map.clear();
                shard.deleted_count = 0;
            }
        }

        void Free()
        {
            for (auto &shard : shards)
            {
                shard.data_manager.Free();
                shard.hash_map.clear();
                shard.deleted_count = 0;
            }
        }

        template<typename F>
        void Enum(F &&func) const
        {
            for (const auto &shard : shards)
            {
                auto active_ids = shard.data_manager.GetActiveView();
                const int count = (int)active_ids.size();

                for (int i = 0; i < count; i++)
                {
                    int id = active_ids[i];
                    T value;
                    if (shard.data_manager.GetData(value, id))
                        func(value);
                }
            }
        }

        void RefreshHashMap()
        {
            for (auto &shard : shards)
            {
                shard.hash_map.clear();
                auto active_ids = shard.data_manager.GetActiveView();
                const int count = (int)active_ids.size();

                for (int i = 0; i < count; i++)
                {
                    int id = active_ids[i];
                    if (!shard.data_manager.IsActive(id))
                        continue;
                    T *ptr = shard.data_manager.At(id);
                    if (!ptr)
                        continue;
                    uint64 hash = ComputeOptimalHash(*ptr);
                    shard.hash_map[hash].push_back(id);
                }
            }
        }
    };
}
