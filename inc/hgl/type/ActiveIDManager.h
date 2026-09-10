#pragma once

#include<absl/container/btree_set.h>
#include<climits>
#include<deque>
#include<vector>

namespace hgl
{
    /**
    * 活跃ID管理器
    *
    * 设计说明：
    * - 管理一个ID池，支持创建、获取、释放操作
    * - 活跃ID存储在 absl::btree_set 中，支持快速有序查询 O(log n)
    * - 闲置ID存储在 std::deque 中（FIFO），确保公平复用，避免ID碎片化；
    *   idle_list 只保存复用次序，成员查询由 idle_lookup 承担 O(log n)
    * - 创建接口为“全有或全无”：容量不足时不产生任何副作用
    * - 提供完整的验证和查询功能
    */
    class ActiveIDManager
    {
        absl::btree_set<int> active_list;   ///<活跃ID列表（有序集合，O(log n)查询+有序遍历）
        std::deque<int> idle_list;          ///<闲置ID列表（FIFO复用次序真源）
        absl::btree_set<int> idle_lookup;   ///<闲置ID成员索引（与 idle_list 严格同步）

        int id_count;                       ///<创建过的最大ID值+1（ID水位，只增不减）

    private:

        bool Create(int *id_list,const int count);

        ///<容量是否足够一次性创建 count 个ID（“全有或全无”的预检条件）
        bool CanCreate(const int count) const noexcept
        {
            return count > 0 && count <= GetRemainingCapacity();
        }

        // idle_list 与 idle_lookup 的唯一维护入口：任何对闲置集合的改动都必须
        // 经过这三个函数，避免两份状态失去同步。
        void PushIdle(const int id);
        void PopIdle();
        void ClearIdle();

    public:

        ActiveIDManager()
            : id_count(0)
        {
        }

        ~ActiveIDManager()=default;

        // ==================== 禁用拷贝，允许移动 ====================

        ActiveIDManager(const ActiveIDManager&) = delete;
        ActiveIDManager& operator=(const ActiveIDManager&) = delete;

        ActiveIDManager(ActiveIDManager&&) = default;
        ActiveIDManager& operator=(ActiveIDManager&&) = default;

        // ==================== 查询接口 ====================

        int GetActiveCount  ()const{return static_cast<int>(active_list.size());}
        int GetIdleCount    ()const{return static_cast<int>(idle_list.size());}
        int GetTotalCount   ()const{return GetActiveCount() + GetIdleCount();}
        int GetHistoryMaxId ()const{return id_count;}

        /**
         * @brief CN:获取活跃ID集合的只读访问\nEN:Get read-only access to active ID set
         * @return btree_set 的 const 引用，支持有序遍历
         */
        const absl::btree_set<int>& GetActiveSet() const
        {
            return active_list;
        }

        /**
         * @brief CN:获取活跃ID列表快照\nEN:Get snapshot of active ID list
         * @return 包含所有活跃ID的 vector
         */
        std::vector<int> GetActiveView() const
        {
            return std::vector<int>(active_list.begin(), active_list.end());
        }

        /**
         * @brief CN:获取闲置ID队列的快照\nEN:Get snapshot of idle ID queue
         * @return 包含所有闲置ID的 vector
         */
        std::vector<int> GetIdleView() const
        {
            return std::vector<int>(idle_list.begin(), idle_list.end());
        }

        // ==================== 创建接口 ====================
        // 返回值: 成功创建并写入的数量；count<=0 或 ID 容量不足时返回0，且不改动任何状态。
        // 语义为“全有或全无”：不存在部分成功——容量不足时一个 ID 都不创建。

        int CreateActive(int *id,const int count=1);              ///<创建若干ID并置于Active；不从Idle复用；容量不足返回0
        int CreateIdle  (int *idp=nullptr,const int count=1);     ///<创建若干ID并置于Idle（分配全新编号，不与已有编号重复）；容量不足返回0
        int CreateIdle  (const int count=1);                      ///<同上（忽略输出）；容量不足返回0

        // ==================== 获取/激活接口 ====================
        // 返回值: 是否“全部成功”。全有或全无：失败时返回false、输出数组不写入、
        // 内部状态也无任何改动，调用方无需检查输出数量。

        bool Get        (int *id,const int count=1);              ///<优先Idle，不创建新ID；闲置不足返回false且无副作用
        bool GetOrCreate(int *id,const int count=1);              ///<先取Idle，不足则创建；容量不足返回false且无副作用

        int GetIdle()                                                           ///<从闲置ID列表中获取一个ID，并将其从闲置列表中删除(注：不会创建新的ID)
        {
            int id;

            if(!Get(&id))
                return(-1);

            return id;
        }

        // ==================== 释放接口 ====================
        // 返回值: 成功释放的数量；count<=0 返回0；部分成功则返回已释放数量。

        int Release     (const int *id,int count=1);              ///<释放指定数量；非活跃ID静默跳过；返回实际释放数量
        int ReleaseAllActive();                                   ///<释放所有活跃ID；返回释放的数量

        // ==================== 查询和验证接口 ====================

        bool IsActive(const int id)const{return active_list.contains(id);}  ///<确认指定ID是否处于活跃状态
        bool IsIdle  (const int id)const{return idle_lookup.contains(id);}
        ///<确认指定ID是否曾被创建过。注：Clear(false) 后 [0,id_count) 内的“幽灵ID”
        ///既非Active也非Idle（永久不可复用），但此函数仍返回 true。
        bool IsValid (const int id)const{return id>=0 && id<id_count;}

        // ==================== 内存管理接口 ====================
        // 注：active_list(btree_set)/idle_list(deque) 均无预分配语义，本类不提供
        //     Reserve——预分配只对 ActiveDataManager 的 data_array 有意义。


        /**
         * 清空所有ID列表，并可选地重置ID水位
         * @param reset_counter 是否重置ID水位（默认false，保留历史水位）
         *
         * 注意：Clear(false) 只是丢弃所有ID——[0,id_count) 范围内的旧编号此后既非
         * Active 也非 Idle，且永久不可复用（ID水位只增不减）。IsValid() 对这些
         * “幽灵ID”仍返回 true，GetTotalCount() != GetHistoryMaxId()；需要完全回到
         * 初始状态请使用 Clear(true)。
         *
         * 用法示例：
         * idm.Clear();           // 清空但保留历史水位
         * idm.Clear(true);       // 完全重置，id_count从0开始
         */
        void Clear(bool reset_counter=false)
        {
            active_list.clear();
            ClearIdle();

            if(reset_counter)
                id_count = 0;
        }

        /**
         * @brief CN:释放所有内存并重置所有计数器\nEN:Free all memory and reset all counters
         *
         * CN:彻底释放内部容器的内存，并将所有计数器归零。\nEN:Completely frees internal container memory and resets all counters to zero.
         * CN:适用于不再需要管理器时的彻底清理。\nEN:Suitable for thorough cleanup when the manager is no longer needed.
         *
         * 用法示例 / Usage example:
         * idm.Free();  // CN:彻底释放所有资源 / EN:Completely free all resources
         */
        void Free()
        {
            active_list.clear();
            ClearIdle();
            idle_list.shrink_to_fit();  // 释放 deque 的内存

            id_count = 0;
        }

        // ==================== 统计接口 ====================

        /**
         * 检查是否有闲置ID可复用
         */
        bool HasIdleID() const { return !idle_list.empty(); }

        /**
         * 获取剩余可分配的ID容量
         * @return 剩余ID数（基于int范围限制）
         *
         * 注：ID使用int类型，最大值为INT_MAX（2,147,483,647）
         * 此方法返回还能分配多少个ID而不会溢出
         */
        int GetRemainingCapacity() const
        {
            // INT_MAX - id_count = 剩余容量
            if(id_count >= INT_MAX) return 0;
            return INT_MAX - id_count;
        }

        /**
         * 检查是否即将达到ID容量上限
         * @param threshold 容量警告阈值（默认1亿）
         * @return true 表示剩余容量小于阈值
         */
        bool IsNearCapacity(int threshold = 100000000) const
        {
            return GetRemainingCapacity() <= threshold;
        }

        // ==================== 比较运算符 ====================

        /**
         * 相等比较：检查两个管理器的ID状态是否完全相同
         *
         * 比较 ID水位 + 活跃集合 + 闲置队列（含FIFO次序——次序决定后续复用结果，
         * 是状态的一部分）。idle_lookup 由 idle_list 派生，不参与比较。
         */
        bool operator==(const ActiveIDManager &other) const
        {
            return id_count == other.id_count
                && active_list == other.active_list
                && idle_list == other.idle_list;
        }

        /**
         * 不相等比较
         */
        bool operator!=(const ActiveIDManager &other) const
        {
            return !(*this == other);
        }

    };//class ActiveIDManager
}//namespace hgl
