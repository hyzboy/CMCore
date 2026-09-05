#pragma once

#include<hgl/type/BlockPool.h>
#include<hgl/type/Stack.h>
#include<type_traits>

namespace hgl
{
    /**
     * 带类型块分配器
     *
     * 绑定一个 C++ 结构 T（必须为 POD/trivially copyable），按 block_size（默认 16
     * 字节）切分。每个对象占 slot_blocks = ceil(sizeof(T)/block_size) 个连续块，
     * 块号即对象句柄，0 表示空/错误。GPU shader 中直接用 block_index * block_size 寻址。
     *
     * 空闲块在本类型内复用（不归还上级 BlockPool），保证同类型对象在块池中局部性良好；
     * 仅在空闲列表耗尽时才向上级批量申请。
     *
     * 每次向上级批量申请得到的连续段起始块号会被记录；调用 ReleaseAll()（或析构）可
     * 把这些连续段一次性归还 BlockPool 做批量回收。若 BlockPool 先于本类型销毁，先调用
     * Detach() 放弃所有权以避免悬空指针。
     */
    template<typename T>
    class TypedBlockAllocator
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "TypedBlockAllocator<T> requires T to be trivially copyable (POD).");

        BlockPool *pool;                ///< 上级块池（非拥有）
        uint32     slot_blocks;         ///< 每个对象占用块数 = ceil(sizeof(T)/block_size)
        uint32     batch_slots;         ///< 每批向上级申请的槽数

        Stack<uint32> free_slots;       ///< 空闲槽起始块号（LIFO，缓存友好）
        Stack<uint32> batch_starts;     ///< 每次向上级申请的连续段起始块号（用于整体回收）

#ifdef _DEBUG

        /**
         * 块号是否属于本类型申请过的批量段（防越权释放污染其它类型）
         */
        bool IsOwnedBlock(const uint32 idx) const
        {
            const uint32 seg_blocks=batch_slots*slot_blocks;
            const uint32 *starts=batch_starts.GetData();
            const int count=batch_starts.GetCount();

            for(int i=0;i<count;i++)
                if(idx>=starts[i]&&idx<starts[i]+seg_blocks)
                    return true;

            return false;
        }

        /**
         * 块号是否已在空闲表中（防 double-release 造成双份借出）
         */
        bool IsInFreeList(const uint32 idx) const
        {
            const uint32 *slots=free_slots.GetData();
            const int count=free_slots.GetCount();

            for(int i=0;i<count;i++)
                if(slots[i]==idx)
                    return true;

            return false;
        }
#endif//#ifdef _DEBUG

    public:

        TypedBlockAllocator():pool(nullptr),slot_blocks(0),batch_slots(0){}
        ~TypedBlockAllocator(){ReleaseAll();}

        /**
         * 绑定上级块池
         * @param p 上级 BlockPool（共享同一块号空间）
         * @param in_batch_slots 每批向上级申请的槽数
         */
        bool Init(BlockPool *p,const uint32 in_batch_slots=16)
        {
            if(!p)
                return false;

            pool=p;

            const uint32 block_size=pool->GetBlockSize();

            slot_blocks=((uint32)sizeof(T)+block_size-1)/block_size;
            if(slot_blocks==0)
                slot_blocks=1;

            batch_slots=in_batch_slots?in_batch_slots:1;

            free_slots.Clear();
            batch_starts.Clear();

            return true;
        }

        /**
         * 申请一个对象
         * @return 对象起始块号；失败返回 0
         */
        uint32 Acquire()
        {
            if(!pool)
                return 0;

            if(!free_slots.IsEmpty())
            {
                uint32 idx;
                free_slots.Pop(idx);
                return idx;
            }

            const uint32 start=pool->Acquire(batch_slots*slot_blocks);

            if(!start)
                return 0;

            batch_starts.Push(start);   // 记录连续段起始块号，供整体回收

            // 第 0 个槽直接返回，其余槽压入空闲列表
            for(uint32 k=1;k<batch_slots;k++)
                free_slots.Push(start+k*slot_blocks);

            return start;
        }

        /**
         * 释放对象（块号归还本类型空闲列表，不归还上级）
         *
         * debug 构造下校验：块号必须属于本类型的批量段，且不在空闲表中
         * （double-release 会产生两份借出记录，GPU 侧表现为静默数据串扰）
         */
        bool Release(const uint32 idx)
        {
            if(!pool||idx==0)
                return false;

#ifdef _DEBUG
            if(!IsOwnedBlock(idx))
                return false;

            if(IsInFreeList(idx))
                return false;
#endif//#ifdef _DEBUG

            free_slots.Push(idx);
            return true;
        }

        /**
         * 整体释放：把从上级申请的所有连续段一次性归还 BlockPool，
         * 并清空空闲列表。之后本分配器仍可继续 Acquire（会重新向上级批量申请）。
         *
         * @note 本类型非拥有 BlockPool，调用方须保证 BlockPool 生命周期长于本类型；
         *       析构函数会自动调用本方法，除非你显式调用了 Detach()。
         */
        void ReleaseAll()
        {
            uint32 start;

            if(pool)
                while(batch_starts.Pop(start))
                    pool->Release(start);

            free_slots.Clear();
        }

        /**
         * 放弃所有权：清空内部状态但不归还上级块池。
         * 用于本类型析构时上级 BlockPool 已先销毁（此时不可再调用 ReleaseAll）。
         */
        void Detach()
        {
            pool=nullptr;
            free_slots.Clear();
            batch_starts.Clear();
        }

        /**
         * 取得对象块号对应的 T 指针（CPU 侧直读直写）
         */
        T *GetData(const uint32 idx)
        {
            if(!pool||idx==0)
                return nullptr;

            return pool->GetData<T>(idx);
        }

        const uint32 GetSlotBlocks()const{return slot_blocks;}
        const uint32 GetBatchSlots()const{return batch_slots;}
        const int    GetFreeCount ()const{return free_slots.GetCount();}
        const int    GetBatchCount()const{return batch_starts.GetCount();}   ///< 已向上级申请的连续段数量
    };//class TypedBlockAllocator
}//namespace hgl
