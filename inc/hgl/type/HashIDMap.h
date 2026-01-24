#pragma once

#include<hgl/type/ValueKVMap.h>

namespace hgl
{
    // ==================== 固定大小的碰撞槽位 ====================
    template<int MAX_COLLISION = 4>
    struct CollisionSlot
    {
        int ids[MAX_COLLISION];     // 固定大小的ID数组
        int count;                  // 当前存储的ID数量

        CollisionSlot() : count(0) 
        {
            for(int i = 0; i < MAX_COLLISION; i++)
                ids[i] = -1;
        }

        bool IsFull() const { return count >= MAX_COLLISION; }
        bool IsEmpty() const { return count == 0; }

        bool Add(int id)
        {
            if(IsFull())
                return false;
            ids[count++] = id;
            return true;
        }

        int Find(const auto& verify_func) const
        {
            for(int i = 0; i < count; i++)
            {
                if(verify_func(ids[i]))
                    return ids[i];
            }
            return -1;
        }

        // 迭代器支持
        const int* begin() const { return ids; }
        const int* end() const { return ids + count; }
    };

    // ==================== 哈希到ID的映射管理器 ====================
    template<int MAX_COLLISION = 4>
    class HashIDMap
    {
    private:
        // 快速查询：哈希值 → ID（99% 情况）
        ValueKVMap<uint64, int> quick_map;

        // 碰撞处理：哈希值 → 固定大小的ID槽位（哈希碰撞时使用）
        ValueKVMap<uint64, CollisionSlot<MAX_COLLISION>> collision_map;

    public:

        HashIDMap() = default;
        ~HashIDMap() = default;

        HashIDMap(const HashIDMap&) = delete;
        HashIDMap& operator=(const HashIDMap&) = delete;

        HashIDMap(HashIDMap&&) = default;
        HashIDMap& operator=(HashIDMap&&) = default;

        // ==================== 添加映射 ====================
        // 返回值：true = 新增成功，false = 槽位已满（但仍可查询）
        bool Add(uint64 hash, int id)
        {
            int* p_existing = quick_map.GetValuePointer(hash);
            if(!p_existing) {
                // 首次遇见此哈希值，直接添加
                quick_map.Add(hash, id);
                return true;
            }

            // 发生哈希碰撞，转移到碰撞槽位
            auto* collision_slot = collision_map.GetValuePointer(hash);
            if(!collision_slot) {
                // 第一次碰撞，创建固定大小的碰撞槽位
                CollisionSlot<MAX_COLLISION> new_slot;
                new_slot.Add(*p_existing);  // 旧的 ID
                bool success = new_slot.Add(id);  // 新的 ID
                collision_map.Add(hash, new_slot);
                return success;
            }

            // 已有碰撞槽位，尝试追加
            return collision_slot->Add(id);
        }

        // ==================== 查找ID ====================
        // 使用 lambda 验证函数查找匹配的 ID
        // verify_func 签名: bool(int id)
        template<typename VerifyFunc>
        int Find(uint64 hash, VerifyFunc verify_func) const
        {
            // 快路径：直接查询（99% 情况，O(1)）
            int* p_id = quick_map.GetValuePointer(hash);
            if(p_id) {
                if(verify_func(*p_id))
                    return *p_id;  // 找到！

                // 碰撞处理（1% 情况，O(1) 固定时间）
                const auto* collision_slot = collision_map.GetValuePointer(hash);
                if(collision_slot) {
                    return collision_slot->Find(verify_func);
                }
            }

            return -1;  // 未找到
        }

        // ==================== 管理接口 ====================

        void Clear()
        {
            quick_map.Clear();
            collision_map.Clear();
        }

        bool IsEmpty() const
        {
            return quick_map.IsEmpty();
        }

        // ==================== 统计接口（性能分析） ====================

        // 获取哈希碰撞次数
        int GetCollisionCount() const {
            return collision_map.GetCount();
        }

        // 获取快速映射表大小
        int GetQuickMapCount() const {
            return quick_map.GetCount();
        }

        // 获取哈希表负载因子（用于性能分析）
        float GetLoadFactor(int total_ids) const {
            int total_entries = quick_map.GetCount();
            return total_entries > 0 ?
                   (float)total_ids / total_entries :
                   0.0f;
        }

        // 获取平均碰撞链长度（用于性能分析）
        float GetAverageCollisionChainLength() const {
            if(collision_map.GetCount() == 0)
                return 0.0f;

            int total_collision_ids = 0;
            for(const auto& kv : collision_map) {
                total_collision_ids += kv.value.count;
            }
            return (float)total_collision_ids / collision_map.GetCount();
        }

        // 获取碰撞槽位溢出次数（性能监控）
        int GetCollisionOverflowCount() const {
            int overflow_count = 0;
            for(const auto& kv : collision_map) {
                if(kv.value.IsFull()) {
                    overflow_count++;
                }
            }
            return overflow_count;
        }

        // 获取最大碰撞槽位大小（配置参数）
        static constexpr int GetMaxCollisionPerHash() {
            return MAX_COLLISION;
        }
    };
}//namespace hgl
