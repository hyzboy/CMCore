#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/Queue.h>
#include<hgl/type/SortedSet.h>
#include<compare>

namespace hgl
{
    /**
    * 活跃ID管理器
    *
    * 设计说明：
    * - 管理一个ID池，支持创建、获取、释放操作
    * - 活跃ID存储在 SortedSet 中，支持快速查询
    * - 闲置ID存储在 Queue 中（FIFO），确保公平复用，避免ID碎片化
    * - 提供完整的验证和统计功能
    */
    class ActiveIDManager
    {
        struct IntView
        {
            const int *data;
            int64 count;
        };

        SortedSet<int> active_list;        ///<活跃ID列表
        Queue<int> idle_list;               ///<闲置ID列表（FIFO队列，确保公平复用）

        int id_count;                       ///<创建过的最大ID值+1
        int released_count;                 ///<已释放过的ID总数（用于统计）

    private:

        bool Create(int *id_list,const int count);

    public:

        ActiveIDManager()
            : id_count(0), released_count(0)
        {
        }

        ~ActiveIDManager()=default;

        // ==================== 禁用拷贝，允许移动 ====================

        ActiveIDManager(const ActiveIDManager&) = delete;
        ActiveIDManager& operator=(const ActiveIDManager&) = delete;

        ActiveIDManager(ActiveIDManager&&) = default;
        ActiveIDManager& operator=(ActiveIDManager&&) = default;

        // ==================== 查询接口 ====================

        int GetActiveCount  ()const{return active_list.GetCount();}
        int GetIdleCount    ()const{return idle_list.GetCount();}
        int GetTotalCount   ()const{return active_list.GetCount()+idle_list.GetCount();}
        int GetHistoryMaxId ()const{return id_count;}
        int GetReleasedCount()const{return released_count;}        ///<已释放过的ID总数

        [[deprecated("Use GetActiveView for read-only access to maintain invariants")]]
        const DataArray<int> &GetActiveArray()const{return active_list;}
        [[deprecated("Use GetIdleView for read-only access to maintain invariants - Queue not directly compatible")]]
        const Queue<int> &GetIdleArray()const{return idle_list;}

        IntView GetActiveView() const
        {
            return {active_list.GetData(), active_list.GetCount()};
        }

        /**
         * 获取闲置ID队列的只读视图（FIFO队列）
         * @return IntView 结构体，包含指向底层数组的指针和元素计数
         *
         * 注：由于Queue内部实现使用双缓冲（data_array[2]），GetIdleView返回的指针和数据可能不完全连续
         */
        IntView GetIdleView() const
        {
            return {idle_list.GetArray().GetData(), idle_list.GetCount()};
        }

        // ==================== 创建接口 ====================
        // 返回值: 成功创建并写入的数量；count<=0 返回0且不改动状态；部分成功则返回实际成功数。

        [[nodiscard]] int CreateActive(int *id,const int count=1);              ///<创建若干ID并置于Active；不从Idle复用；部分成功返回实际成功数
        [[nodiscard]] int CreateIdle  (int *idp=nullptr,const int count=1);     ///<创建若干ID并置于Idle；忽略已存在；部分成功返回实际成功数
        [[nodiscard]] int CreateIdle  (const int count=1);                      ///<同上（忽略输出）；部分成功返回实际成功数

        // ==================== 获取/激活接口 ====================
        // 返回值: 是否“全部成功”。count<=0 直接返回false且不改动；部分成功时返回false，调用方需检查输出数量。

        [[nodiscard]] bool Get        (int *id,const int count=1);              ///<优先Idle，不创建新ID；部分成功返回false
        [[nodiscard]] bool GetOrCreate(int *id,const int count=1);              ///<先取Idle，不足则创建；部分成功返回false

        int GetIdle()                                                           ///<从闲置ID列表中获取一个ID，并将其从闲置列表中删除(注：不会创建新的ID)
        {
            int id;

            if(!Get(&id))
                return(-1);

            return id;
        }

        // ==================== 释放接口 ====================
        // 返回值: 成功释放的数量；count<=0 返回0；部分成功则返回已释放数量。

        [[nodiscard]] int Release     (const int *id,int count=1);              ///<释放指定数量；部分成功返回已释放数量
        [[nodiscard]] int ReleaseAllActive();                                   ///<释放所有活跃ID；返回释放的数量

        // ==================== 查询和验证接口 ====================

        bool IsActive(const int id)const{return active_list.Contains(id);}  ///<确认指定ID是否处于活跃状态
        bool IsIdle  (const int id)const{return idle_list.Contains(id);}    ///<确认指定ID是否处于闲置状态
        bool IsValid (const int id)const{return id>=0 && id<id_count;}      ///<确认指定ID是否曾被创建过

        // ==================== 内存管理接口 ====================

        void Reserve(int c);                                                    ///<预分配容量

        /**
         * 清空所有ID列表，并可选地重置ID生成器
         * @param reset_counter 是否重置ID计数器（默认false，保留历史信息）
         *
         * 用法示例：
         * idm.Clear();           // 清空但保留历史ID计数
         * idm.Clear(true);       // 完全重置，id_count从0开始
         */
        void Clear(bool reset_counter=false)
        {
            active_list.Clear();
            idle_list.Clear();

            if(reset_counter)
            {
                id_count = 0;
                released_count = 0;
            }
        }

        // ==================== 统计接口 ====================

        /**
         * 获取总的已分配ID数
         * = active_list.GetCount() + idle_list.GetCount()
         */
        int GetAllocatedCount() const { return GetTotalCount(); }

        /**
         * 获取已使用的ID百分比
         * @return 0.0 ~ 1.0 之间的浮点数
         */
        double GetUtilizationRatio() const
        {
            int total = GetTotalCount();
            return total > 0 ? (double)GetActiveCount() / total : 0.0;
        }

        /**
         * 检查是否有闲置ID可复用
         */
        bool HasIdleID() const { return idle_list.GetCount() > 0; }

        // ==================== 比较运算符 ====================

        /**
         * 相等比较：检查两个管理器的状态是否完全相同
         * 
         * 注：Queue类型不支持==比较，仅比较计数器和active_list
         */
        bool operator==(const ActiveIDManager &other) const
        {
            return id_count == other.id_count
                && released_count == other.released_count
                && GetIdleCount() == other.GetIdleCount()
                && active_list == other.active_list;
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
