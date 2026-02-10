/**
 * @file FlatUnorderedSet_OpenAddress.h
 * @brief CN:开放寻址无序集合（线性探测）- LinearProbeSet
 */
#pragma once

#include <type_traits>
#include <vector>
#include <hgl/util/hash/QuickHash.h>
#include <hgl/type/ActiveDataManager.h>

namespace hgl
{
    /**
     * 【线性探测集合 LinearProbeSet】
     *
     * 【原理】
     * 使用单个连续数组实现开放寻址哈希表，采用线性探测冲突处理：
     *  • 插入/查找时，如果目标位置被占据，则依次探测下一个位置
     *  • 删除使用墓碑标记(-2)，不物理移除元素
     *  • 当负载因子超过阈值(默认70%)时重哈希
     *
     * 数据结构：
     *  struct Slot {
     *    int id;      // -1: 空, -2: 墓碑, >=0: 有效元素ID
     *    uint64 hash; // 哈希值
     *  }
     *
     * 【特点】
     *  • 缓存友好 - 单一连续数组，缓存命中率高
     *  • 内存高效 - 无链表/桶额外开销，仅一个数组
     *  • 碰撞处理优秀 - 线性探测在高碰撞数据上表现稳定
     *  • 快速遍历 - 无需追踪链表
     *
     * 【适用场景】✓
     *  • 高碰撞率数据（如连续哈希值）
     *  • CPU缓存敏感的应用
     *  • 内存占用敏感的嵌入式系统
     *  • 小规模集合（缓存命中最优）
     *  • 不适合：频繁删除后再插入的场景（墓碑积累）
     *
     * 【性能特征】
     *  • Insert: O(1) avg, O(n) worst
     *  • Delete: O(1) avg, O(n) worst
     *  • Lookup: O(1) avg with good hash, O(n) worst
     *  • 缓存友好度: 最优（线性内存访问）
     *  • 内存占用: 最小（无额外结构）
     *
     * 【配置】
     *  • SetMaxLoadFactor(f): 重哈希阈值(0.1~0.95)
     *  • RehashToFit(): 手动清理墓碑并压缩
     *
     * 【注意】
     *  • 大量删除后需要定期调用RehashToFit()清理墓碑
     *  • 否则查找性能会因墓碑积累而退化
     */
    template<typename T>
    class LinearProbeSet
    {
    protected:
        struct Slot
        {
            int id = -1;       // -1 空槽位, -2 墓碑
            uint64 hash = 0;
        };

        ActiveDataManager<T> data_manager;
        std::vector<Slot> table;
        int active_slots = 0;
        float max_load_factor = 0.7f;

        int Capacity() const
        {
            return (int)table.size();
        }

        void EnsureCapacity(int desired)
        {
            if (Capacity() >= desired)
                return;
            int new_cap = 8;
            while (new_cap < desired)
                new_cap <<= 1;
            Rehash(new_cap);
        }

        void Rehash(int new_capacity)
        {
            std::vector<Slot> old = std::move(table);
            table.clear();
            table.resize(new_capacity);
            active_slots = 0;

            for (const auto &slot : old)
            {
                if (slot.id >= 0 && data_manager.IsActive(slot.id))
                    InsertSlot(slot.id, slot.hash);
            }
        }

        void MaybeGrow()
        {
            if (Capacity() == 0)
                EnsureCapacity(8);
            if (active_slots + 1 > (int)(Capacity() * max_load_factor))
                Rehash(Capacity() * 2);
        }

        void InsertSlot(int id, uint64 hash)
        {
            int cap = Capacity();
            int pos = (int)(hash & (uint64)(cap - 1));

            while (true)
            {
                Slot &slot = table[pos];
                // 空槽位或墓碑都可以插入
                if (slot.id == -1 || slot.id == -2)
                {
                    slot.id = id;
                    slot.hash = hash;
                    ++active_slots;
                    return;
                }
                pos = (pos + 1) & (cap - 1);
            }
        }

        int FindID(const T &value) const
        {
            if (Capacity() == 0)
                return -1;

            uint64 hash = ComputeOptimalHash(value);
            int cap = Capacity();
            int pos = (int)(hash & (uint64)(cap - 1));
            int start_pos = pos;
            bool first = true;

            while (first || pos != start_pos)
            {
                first = false;
                const Slot &slot = table[pos];

                // 空槽位，说明不存在（线性探测中遇到空槽就可以停止）
                if (slot.id == -1)
                    return -1;

                // 墓碑，跳过继续找
                if (slot.id == -2)
                {
                    pos = (pos + 1) & (cap - 1);
                    continue;
                }

                // 检查是否匹配
                if (slot.hash == hash)
                {
                    T existing;
                    if (data_manager.GetData(existing, slot.id) && existing == value)
                        return slot.id;
                }

                pos = (pos + 1) & (cap - 1);
            }

            return -1;
        }

    public:
        LinearProbeSet() = default;
        virtual ~LinearProbeSet() = default;

        void Reserve(int capacity)
        {
            data_manager.Reserve(capacity);
            EnsureCapacity((int)(capacity / max_load_factor) + 1);
        }

        int GetCount() const
        {
            return data_manager.GetActiveCount();
        }

        bool IsEmpty() const
        {
            return GetCount() == 0;
        }

        void SetMaxLoadFactor(float lf)
        {
            if (lf > 0.1f && lf < 0.95f)
                max_load_factor = lf;
        }

        bool Add(const T &value)
        {
            static_assert(std::is_trivially_copyable_v<T>, "Add() requires trivially copyable types.");
            if (FindID(value) != -1)
                return false;

            MaybeGrow();

            int new_id;
            if (!data_manager.GetOrCreate(&new_id, 1))
                return false;

            if (!data_manager.WriteData(value, new_id))
            {
                data_manager.Release(&new_id, 1);
                return false;
            }

            uint64 hash = ComputeOptimalHash(value);
            InsertSlot(new_id, hash);
            return true;
        }

        bool Delete(const T &value)
        {
            int id = FindID(value);
            if (id == -1)
                return false;

            uint64 hash = ComputeOptimalHash(value);
            int cap = Capacity();
            int pos = (int)(hash & (uint64)(cap - 1));
            int start_pos = pos;
            bool first = true;

            while (first || pos != start_pos)
            {
                first = false;
                Slot &slot = table[pos];

                // 空槽位，不应该发生（FindID已经找到了）
                if (slot.id == -1)
                    return false;

                // 找到对应的槽位
                if (slot.id == id)
                {
                    slot.id = -2; // tombstone
                    --active_slots;
                    return data_manager.Release(&id, 1) > 0;
                }

                pos = (pos + 1) & (cap - 1);
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
            table.clear();
            active_slots = 0;
        }

        void Free()
        {
            data_manager.Free();
            table.clear();
            active_slots = 0;
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

        void RehashToFit()
        {
            int count = GetCount();
            int desired = 8;
            while (desired < (int)(count / max_load_factor) + 1)
                desired <<= 1;
            Rehash(desired);
        }
    };
}
