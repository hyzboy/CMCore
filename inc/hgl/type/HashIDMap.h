#pragma once

#include<hgl/type/ValueKVMap.h>
#include<vector>

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
    /**
     * HashIDMap: 哈希到ID的映射管理器，使用动态碰撞槽位
     * 
     * 功能特性：
     * - 快速查询路径：quick_map 存储每个哈希的首次插入ID
     * - 碰撞处理：collision_map 使用动态 std::vector 存储同一哈希的多个ID
     * - MAX_COLLISION 作为软阈值，用于监控溢出
     * - 支持 Remove/Update 操作，实现高效的索引管理
     * 
     * 测试建议：
     * - 测试无碰撞场景（Add/Find）
     * - 测试碰撞场景（多个ID相同哈希）
     * - 测试 Remove 操作及提升逻辑
     * - 测试 Update 操作及重复ID检测
     * - 测试溢出计数器（GetCollisionOverflowCount）
     * - 测试 Clear/Free 是否正确重置 overflow_count_
     */
    template<int MAX_COLLISION = 4>
    class HashIDMap
    {
    private:
        // ==================== 动态大小的碰撞槽位（内部类） ====================
        struct CollisionSlot
        {
            std::vector<int> ids;       // 动态大小的ID数组

            CollisionSlot() = default;

            bool IsEmpty() const { return ids.empty(); }

            bool Add(int id)
            {
                // 检查是否已存在（防止重复）
                for(int existing_id : ids)
                {
                    if(existing_id == id)
                        return false;  // 重复ID，拒绝添加
                }
                ids.push_back(id);
                return true;
            }

            bool Remove(int id)
            {
                for(auto it = ids.begin(); it != ids.end(); ++it)
                {
                    if(*it == id)
                    {
                        ids.erase(it);
                        return true;
                    }
                }
                return false;
            }

            bool Update(int old_id, int new_id)
            {
                // 检查 new_id 是否已存在
                for(int existing_id : ids)
                {
                    if(existing_id == new_id)
                        return false;
                }

                // 查找并替换 old_id
                for(int& id : ids)
                {
                    if(id == old_id)
                    {
                        id = new_id;
                        return true;
                    }
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

            int GetFirstID() const
            {
                return ids.empty() ? -1 : ids[0];
            }

            // 迭代器支持
            auto begin() const { return ids.begin(); }
            auto end() const { return ids.end(); }
        };

        // 快速查询：哈希值 → ID（99% 情况）
        ValueKVMap<uint64, int> quick_map;

        // 碰撞处理：哈希值 → 动态大小的ID槽位（哈希碰撞时使用）
        ValueKVMap<uint64, CollisionSlot> collision_map;

        // 溢出计数器：记录超过 MAX_COLLISION 的槽位数量
        int overflow_count_;

    public:

        HashIDMap() : overflow_count_(0) {}
        ~HashIDMap() = default;

        HashIDMap(const HashIDMap&) = delete;
        HashIDMap& operator=(const HashIDMap&) = delete;

        HashIDMap(HashIDMap&&) = default;
        HashIDMap& operator=(HashIDMap&&) = default;

        // ==================== 添加映射 ====================
        /**
         * 添加哈希到ID的映射
         * @param hash 哈希值
         * @param id ID值
         * @return true = 新增成功，false = ID已存在（重复）
         * 
         * 行为说明：
         * - 首次添加：直接添加到 quick_map
         * - 碰撞时：将 quick_map 中的原ID移动到 collision_slot（保留首次插入的ID在 quick_map）
         *   并将新ID也添加到 collision_slot
         * - 拒绝重复ID：如果ID已存在于该哈希下，返回 false
         * - 无容量限制：碰撞槽位动态增长，不会失败（除非重复ID）
         */
        bool Add(uint64 hash, int id)
        {
            int* p_existing = quick_map.GetValuePointer(hash);
            if(!p_existing) {
                // 首次遇见此哈希值，直接添加
                quick_map.Add(hash, id);
                return true;
            }

            // 检查 quick_map 中的ID是否与新ID重复
            if(*p_existing == id)
                return false;  // 重复ID

            // 发生哈希碰撞，转移到碰撞槽位
            auto* collision_slot = collision_map.GetValuePointer(hash);
            if(!collision_slot) {
                // 第一次碰撞，创建动态碰撞槽位
                CollisionSlot new_slot;
                new_slot.Add(*p_existing);  // 旧的 ID（quick_map中的）
                if(!new_slot.Add(id)) {     // 新的 ID
                    return false;  // 重复ID（理论上不会发生）
                }
                collision_map.Add(hash, new_slot);
                
                // 检查是否需要增加溢出计数
                if(new_slot.ids.size() > (size_t)MAX_COLLISION)
                {
                    overflow_count_++;
                }
                return true;
            }

            // 已有碰撞槽位，尝试追加
            size_t old_size = collision_slot->ids.size();
            if(!collision_slot->Add(id)) {
                return false;  // 重复ID
            }
            
            // 检查是否刚好超过阈值（只在首次超过时增加计数）
            size_t new_size = collision_slot->ids.size();
            if(old_size == (size_t)MAX_COLLISION && new_size > (size_t)MAX_COLLISION)
            {
                overflow_count_++;
            }
            
            return true;
        }

        // ==================== 移除映射 ====================
        /**
         * 移除指定哈希下的指定ID
         * @param hash 哈希值
         * @param id 要移除的ID
         * @return true = 移除成功，false = ID不存在
         * 
         * 行为说明：
         * - 如果移除的是 quick_map 的代表ID，且碰撞槽位存在，则提升碰撞槽位中
         *   最早插入的ID（第一个）到 quick_map
         * - 如果碰撞槽位变空，则删除该槽位
         */
        bool Remove(uint64 hash, int id)
        {
            int* p_quick = quick_map.GetValuePointer(hash);
            if(!p_quick)
                return false;  // 哈希不存在

            // 检查是否是 quick_map 中的ID
            if(*p_quick == id)
            {
                // 检查是否有碰撞槽位
                auto* collision_slot = collision_map.GetValuePointer(hash);
                if(collision_slot && !collision_slot->IsEmpty())
                {
                    // 提升碰撞槽位中的第一个ID到 quick_map
                    int promoted_id = collision_slot->GetFirstID();
                    *p_quick = promoted_id;
                    collision_slot->ids.erase(collision_slot->ids.begin());
                    
                    // 如果碰撞槽位为空，删除它
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

            // 检查碰撞槽位
            auto* collision_slot = collision_map.GetValuePointer(hash);
            if(collision_slot)
            {
                if(collision_slot->Remove(id))
                {
                    // 如果碰撞槽位为空，删除它
                    if(collision_slot->IsEmpty())
                    {
                        collision_map.DeleteByKey(hash);
                    }
                    return true;
                }
            }

            return false;  // ID不存在
        }

        // ==================== 更新映射 ====================
        /**
         * 更新指定哈希下的ID（将 old_id 替换为 new_id）
         * @param hash 哈希值
         * @param old_id 旧ID
         * @param new_id 新ID
         * @return true = 更新成功，false = old_id不存在或new_id已存在
         * 
         * 行为说明：
         * - 如果 new_id 已存在于该哈希下，返回 false
         * - 如果 old_id 不存在，返回 false
         * - 否则，将 old_id 替换为 new_id
         */
        bool Update(uint64 hash, int old_id, int new_id)
        {
            int* p_quick = quick_map.GetValuePointer(hash);
            if(!p_quick)
                return false;  // 哈希不存在

            // 检查 new_id 是否已存在
            if(*p_quick == new_id)
                return false;  // new_id 已经是 quick_map 的代表

            auto* collision_slot = collision_map.GetValuePointer(hash);
            if(collision_slot)
            {
                for(int existing_id : collision_slot->ids)
                {
                    if(existing_id == new_id)
                        return false;  // new_id 已存在于碰撞槽位
                }
            }

            // 检查并更新 quick_map
            if(*p_quick == old_id)
            {
                *p_quick = new_id;
                return true;
            }

            // 检查并更新碰撞槽位
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
            const int* p_id = quick_map.GetValuePointer(hash);
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
            overflow_count_ = 0;
        }

        void Free()
        {
            quick_map.Free();
            collision_map.Free();
            overflow_count_ = 0;
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
                total_collision_ids += kv.value.ids.size();
            }
            return (float)total_collision_ids / collision_map.GetCount();
        }

        // 获取碰撞槽位溢出次数（性能监控）
        // 返回超过 MAX_COLLISION 的槽位数量（在首次超过时计数）
        int GetCollisionOverflowCount() const {
            return overflow_count_;
        }

        // 获取最大碰撞槽位大小（配置参数，作为软阈值）
        static constexpr int GetMaxCollisionPerHash() {
            return MAX_COLLISION;
        }
    };
}//namespace hgl
