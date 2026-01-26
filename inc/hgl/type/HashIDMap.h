#pragma once

#include<hgl/type/ValueKVMap.h>
#include<vector>
#include<algorithm>

namespace hgl
{
    // ==================== 通用哈希函数（FNV-1a） ====================
    /**
     * 计算数据的哈希值（使用 FNV-1a 算法）
     * @tparam T 数据类型
     * @param value 要计算哈希的数据
     * @return 64位哈希值
     */
    template<typename T>
    inline uint64 ComputeFNV1aHash(const T& value)
    {
        // 对于基础类型，使用指针转换
        const uint8* bytes = (const uint8*)&value;
        const int size = sizeof(T);

        uint64 hash = 14695981039346656037ULL;

        for(int i = 0; i < size; i++)
        {
            hash ^= (uint64)bytes[i];
            hash *= 1099511628211ULL;
        }

        return hash;
    }

    // ==================== 哈希到ID的映射管理器 ====================
    // MAX_COLLISION 现作为初始容量提示，不再是硬性上限
    template<int MAX_COLLISION = 4>
    class HashIDMap
    {
    private:
        // ==================== 动态碰撞槽位（内部类） ====================
        // 改进：使用 std::vector 替代固定数组，允许任意数量的冲突ID
        struct CollisionSlot
        {
            std::vector<int> ids;       // 动态ID数组

            CollisionSlot()
            {
                ids.reserve(MAX_COLLISION);  // 使用 MAX_COLLISION 作为初始容量提示
            }

            // IsFull() 现在总是返回 false（不再有硬性上限）
            // 保留此方法以维持向后兼容性
            bool IsFull() const { return false; }
            bool IsEmpty() const { return ids.empty(); }
            int GetCount() const { return static_cast<int>(ids.size()); }

            // Add 现在总是成功（除非内存分配失败）
            bool Add(int id)
            {
                ids.push_back(id);
                return true;
            }

            // 移除指定ID，返回是否成功
            bool Remove(int id)
            {
                auto it = std::find(ids.begin(), ids.end(), id);
                if(it != ids.end())
                {
                    ids.erase(it);
                    return true;
                }
                return false;
            }

            // 更新ID：将 old_id 替换为 new_id
            bool Update(int old_id, int new_id)
            {
                auto it = std::find(ids.begin(), ids.end(), old_id);
                if(it != ids.end())
                {
                    *it = new_id;
                    return true;
                }
                return false;
            }

            template<typename VerifyFunc>
            int Find(VerifyFunc verify_func) const
            {
                for(int id : ids)
                {
                    if(verify_func(id))
                        return id;
                }
                return -1;
            }

            // 获取第一个ID（用于提升到 quick_map）
            int GetFirstID() const
            {
                return ids.empty() ? -1 : ids[0];
            }

            // 移除第一个ID并返回（用于提升）
            int PopFirstID()
            {
                if(ids.empty())
                    return -1;
                int first_id = ids[0];
                ids.erase(ids.begin());
                return first_id;
            }

            // 迭代器支持
            const int* begin() const { return ids.data(); }
            const int* end() const { return ids.data() + ids.size(); }
        };

        // 快速查询：哈希值 → ID（99% 情况）
        ValueKVMap<uint64, int> quick_map;

        // 碰撞处理：哈希值 → 固定大小的ID槽位（哈希碰撞时使用）
        ValueKVMap<uint64, CollisionSlot> collision_map;

    public:

        HashIDMap() = default;
        ~HashIDMap() = default;

        HashIDMap(const HashIDMap&) = delete;
        HashIDMap& operator=(const HashIDMap&) = delete;

        HashIDMap(HashIDMap&&) = default;
        HashIDMap& operator=(HashIDMap&&) = default;

        // ==================== 添加映射 ====================
        // 返回值：true = 新增成功（现在总是成功，除非内存分配失败）
        // 改进：不再因为碰撞槽溢出而返回 false
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
                // 第一次碰撞，创建动态碰撞槽位
                CollisionSlot new_slot;
                new_slot.Add(*p_existing);  // 旧的 ID
                new_slot.Add(id);           // 新的 ID（现在总是成功）
                collision_map.Add(hash, new_slot);
                return true;
            }

            // 已有碰撞槽位，追加新ID（现在总是成功）
            return collision_slot->Add(id);
        }

        // ==================== 删除映射 ====================
        // 从映射中移除指定 hash 下的 id
        // 返回值：true = 成功移除，false = 未找到该ID
        // 注意：当从 quick_map 移除时，会从 collision_slot 提升一个ID到 quick_map
        bool Remove(uint64 hash, int id)
        {
            int* p_quick_id = quick_map.GetValuePointer(hash);
            if(!p_quick_id)
                return false;  // 该哈希值不存在

            // 检查是否在 quick_map 中
            if(*p_quick_id == id)
            {
                // 需要移除 quick_map 中的ID
                auto* collision_slot = collision_map.GetValuePointer(hash);
                if(collision_slot && !collision_slot->IsEmpty())
                {
                    // 有碰撞槽位，提升第一个ID到 quick_map
                    int promoted_id = collision_slot->PopFirstID();
                    quick_map.Change(hash, promoted_id);
                    
                    // 如果碰撞槽位现在为空，删除它
                    if(collision_slot->IsEmpty())
                    {
                        collision_map.DeleteByKey(hash);
                    }
                }
                else
                {
                    // 没有碰撞槽位，直接删除 quick_map 条目
                    quick_map.DeleteByKey(hash);
                }
                return true;
            }

            // 不在 quick_map 中，检查碰撞槽位
            auto* collision_slot = collision_map.GetValuePointer(hash);
            if(collision_slot)
            {
                bool removed = collision_slot->Remove(id);
                if(removed && collision_slot->IsEmpty())
                {
                    // 碰撞槽位现在为空，删除它
                    collision_map.DeleteByKey(hash);
                }
                return removed;
            }

            return false;  // ID不存在
        }

        // ==================== 更新映射 ====================
        // 将指定 hash 下的 old_id 替换为 new_id
        // 返回值：true = 成功更新，false = 未找到 old_id
        // 用途：主要用于 UnorderedMap 使用尾部替换删除时更新索引
        bool Update(uint64 hash, int old_id, int new_id)
        {
            int* p_quick_id = quick_map.GetValuePointer(hash);
            if(!p_quick_id)
                return false;  // 该哈希值不存在

            // 检查是否在 quick_map 中
            if(*p_quick_id == old_id)
            {
                quick_map.Change(hash, new_id);
                return true;
            }

            // 不在 quick_map 中，检查碰撞槽位
            auto* collision_slot = collision_map.GetValuePointer(hash);
            if(collision_slot)
            {
                return collision_slot->Update(old_id, new_id);
            }

            return false;  // old_id 不存在
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

        void Free()
        {
            quick_map.Free();
            collision_map.Free();
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
                total_collision_ids += kv.value.GetCount();
            }
            return (float)total_collision_ids / collision_map.GetCount();
        }

        // 获取碰撞槽位溢出次数（性能监控）
        // 注意：由于使用动态vector，此函数现在返回碰撞链长度超过 MAX_COLLISION 的数量
        // 而不是真正的"溢出"（因为不再有硬性上限）
        // 返回值为 0 表示所有碰撞链都在初始容量提示内
        int GetCollisionOverflowCount() const {
            int overflow_count = 0;
            for(const auto& kv : collision_map) {
                if(kv.value.GetCount() > MAX_COLLISION) {
                    overflow_count++;
                }
            }
            return overflow_count;
        }

        // 获取最大碰撞槽位大小（配置参数）
        // 注意：现在这是初始容量提示，不再是硬性上限
        static constexpr int GetMaxCollisionPerHash() {
            return MAX_COLLISION;
        }
    };
}//namespace hgl
