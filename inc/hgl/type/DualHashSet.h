/**
 * @file FlatUnorderedSet_DualHash.h
 * @brief CN:双哈希表渐进式重建无序集合 - DualHashSet
 */
#pragma once

#include <type_traits>
#include <vector>
#include <algorithm>
#include <hgl/util/hash/QuickHash.h>
#include <hgl/type/ActiveDataManager.h>
#include <ankerl/unordered_dense.h>

namespace hgl
{
    /**
     * 【双哈希表集合 DualHashSet】
     *
     * 【原理】
     * 采用双哈希表设计实现渐进式垃圾回收。维护两个哈希表：
     *  - hash_map_active: 活跃数据表
     *  - hash_map_rebuilding: 重建过程中的临时表
     *
     * 当删除计数超过阈值(默认1024)时，启动增量重建：
     *  1. 在StartRebuild()时创建重建表
     *  2. 每次Add/Delete时调用StepRebuild(batch)进行增量处理，默认批次128个元素
     *  3. 无需等待GC完成，可继续Add/Delete操作
     *  4. 重建完成后，合并旧表剩余数据到新表，然后交换
     *
     * 【特点】
     *  • 无停顿垃圾回收 - GC不会导致性能突变
     *  • 删除密集操作友好 - 平滑处理大量删除
     *  • 内存占用稳定 - 二次哈希表占用可预测
     *
     * 【适用场景】✓
     *  • 高频删除操作（删除率 > 10%）
     *  • 需要稳定延迟的实时系统
     *  • 缓存/会话管理（频繁超期清理）
     *  • 事件队列（消息处理后删除）
     *  • 不适合：内存极度受限的场景
     *
     * 【性能特征】
     *  • Insert: O(1) avg, O(n) worst
     *  • Delete: O(1) avg, O(n) worst
     *  • Lookup: O(1) avg, O(n) worst
     *  • GC代价: 分摊O(1)，无峰值延迟
     *
     * 【配置】
     *  • SetRebuildThreshold(n): 何时触发重建（删除计数达到n）
     *  • SetRebuildBatch(n): 每步处理元素数（越大GC越快，可能有延迟峰值）
     */
    template<typename T>
    class DualHashSet
    {
    protected:
        using HashMap = ankerl::unordered_dense::map<uint64, std::vector<int>>;

        ActiveDataManager<T> data_manager;
        HashMap hash_map_active;
        HashMap hash_map_rebuilding;

        bool is_rebuilding = false;
        int rebuild_progress = 0;
        int rebuild_batch = 128;

        int deleted_count = 0;
        int rebuild_threshold = 1024;

        int FindIDInMap(const HashMap &map, const T &value) const
        {
            uint64 hash = ComputeOptimalHash(value);
            auto it = map.find(hash);
            if (it == map.end())
                return -1;

            for (int id : it->second)
            {
                if (!data_manager.IsActive(id))
                    continue;
                T existing;
                if (data_manager.GetData(existing, id) && existing == value)
                    return id;
            }
            return -1;
        }

        int FindID(const T &value) const
        {
            int id = FindIDInMap(hash_map_active, value);
            if (id != -1)
                return id;

            if (is_rebuilding)
                return FindIDInMap(hash_map_rebuilding, value);

            return -1;
        }

        void StartRebuild()
        {
            is_rebuilding = true;
            rebuild_progress = 0;
            hash_map_rebuilding.clear();
        }

        void StepRebuild(int batch)
        {
            if (!is_rebuilding)
                return;

            auto active_ids = data_manager.GetActiveView();
            const int total = (int)active_ids.size();
            if (total == 0)
            {
                hash_map_active.clear();
                hash_map_rebuilding.clear();
                is_rebuilding = false;
                rebuild_progress = 0;
                return;
            }

            int end = rebuild_progress + batch;
            if (end > total)
                end = total;

            for (int i = rebuild_progress; i < end; i++)
            {
                int id = active_ids[i];
                if (!data_manager.IsActive(id))
                    continue;
                T *ptr = data_manager.At(id);
                if (!ptr)
                    continue;
                uint64 hash = ComputeOptimalHash(*ptr);
                hash_map_rebuilding[hash].push_back(id);
            }

            rebuild_progress = end;
            if (rebuild_progress >= total)
            {
                // 重建完成：清理旧表中的失效ID，然后交换
                for (auto it = hash_map_active.begin(); it != hash_map_active.end(); )
                {
                    auto &id_list = it->second;
                    id_list.erase(
                        std::remove_if(id_list.begin(), id_list.end(),
                            [this](int id) { return !data_manager.IsActive(id); }),
                        id_list.end()
                    );
                    if (id_list.empty())
                        it = hash_map_active.erase(it);
                    else
                        ++it;
                }

                // 合并：把旧表中剩余的有效ID添加到新表
                for (auto &pair : hash_map_active)
                {
                    for (int id : pair.second)
                        hash_map_rebuilding[pair.first].push_back(id);
                }

                hash_map_active = std::move(hash_map_rebuilding);
                hash_map_rebuilding.clear();
                is_rebuilding = false;
                rebuild_progress = 0;
                deleted_count = 0;
            }
        }

        void MaybeRebuild()
        {
            if (!is_rebuilding && deleted_count >= rebuild_threshold)
                StartRebuild();

            StepRebuild(rebuild_batch);
        }

    public:
        DualHashSet() = default;
        virtual ~DualHashSet() = default;

        void Reserve(int capacity)
        {
            data_manager.Reserve(capacity);
        }

        int GetCount() const
        {
            return data_manager.GetActiveCount();
        }

        bool IsEmpty() const
        {
            return GetCount() == 0;
        }

        void SetRebuildThreshold(int threshold)
        {
            rebuild_threshold = threshold > 0 ? threshold : 1;
        }

        void SetRebuildBatch(int batch)
        {
            rebuild_batch = batch > 0 ? batch : 1;
        }

        bool Add(const T &value)
        {
            static_assert(std::is_trivially_copyable_v<T>, "Add() requires trivially copyable types.");

            if (FindID(value) != -1)
                return false;

            int new_id;
            if (!data_manager.GetOrCreate(&new_id, 1))
                return false;

            if (!data_manager.WriteData(value, new_id))
            {
                data_manager.Release(&new_id, 1);
                return false;
            }

            uint64 hash = ComputeOptimalHash(value);
            hash_map_active[hash].push_back(new_id);

            // 如果正在重建，也要添加到重建表中
            if (is_rebuilding)
                hash_map_rebuilding[hash].push_back(new_id);

            // 在添加后推进重建
            MaybeRebuild();

            return true;
        }

        bool Delete(const T &value)
        {
            int id = FindID(value);
            if (id == -1)
                return false;

            if (data_manager.Release(&id, 1) > 0)
            {
                ++deleted_count;

                // 从哈希表中移除（避免查找失效的ID）
                // 先从活跃表移除
                uint64 hash = ComputeOptimalHash(value);
                auto it = hash_map_active.find(hash);
                if (it != hash_map_active.end())
                {
                    auto &id_list = it->second;
                    id_list.erase(
                        std::remove(id_list.begin(), id_list.end(), id),
                        id_list.end()
                    );
                    if (id_list.empty())
                        hash_map_active.erase(it);
                }

                // 如果正在重建，也从重建表移除
                if (is_rebuilding)
                {
                    auto it2 = hash_map_rebuilding.find(hash);
                    if (it2 != hash_map_rebuilding.end())
                    {
                        auto &id_list = it2->second;
                        id_list.erase(
                            std::remove(id_list.begin(), id_list.end(), id),
                            id_list.end()
                        );
                        if (id_list.empty())
                            hash_map_rebuilding.erase(it2);
                    }
                }

                // 在删除后推进重建
                MaybeRebuild();

                return true;
            }

            return false;
        }

        bool Contains(const T &value) const
        {
            return FindID(value) != -1;
        }

        void Clear()
        {
            data_manager.Clear();
            hash_map_active.clear();
            hash_map_rebuilding.clear();
            is_rebuilding = false;
            rebuild_progress = 0;
            deleted_count = 0;
        }

        void Free()
        {
            data_manager.Free();
            hash_map_active.clear();
            hash_map_rebuilding.clear();
            is_rebuilding = false;
            rebuild_progress = 0;
            deleted_count = 0;
        }

        template<typename F>
        void Enum(F &&func) const
        {
            auto active_ids = data_manager.GetActiveView();
            const int count = (int)active_ids.size();

            for (int i = 0; i < count; i++)
            {
                int id = active_ids[i];
                T value;
                if (data_manager.GetData(value, id))
                    func(value);
            }
        }

        void RefreshHashMap()
        {
            StartRebuild();
            while (is_rebuilding)
                StepRebuild(rebuild_batch);
        }
    };
}
