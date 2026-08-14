#pragma once

#include<hgl/type/BlockAllocator.h>
#include<hgl/type/MemoryAllocator.h>
#include<hgl/type/UnorderedMap.h>

namespace hgl
{
    /**
     * 块内存池
     *
     * 在 BlockAllocator（只管理块号链）之上，对外统一暴露 uint32 块号接口。
     * 底层字节内存通过 AbstractMemoryAllocator（纯虚接口）注入：既可以是堆内存
     * （MemoryAllocator / hgl_align_realloc），也可以是显存映射（如 vkMapMemory）。
     * 所有子分配器与 GPU shader 都只与块号打交道，实际字节地址 = base + index * block_size。
     *
     * 0 号块被永久保留，作为“空/错误”标记：Acquire 失败返回 0，Release(0) 恒失败。
     * 因此本池必须初始化至少 2 块。
     *
     * @note BlockPool 不拥有注入的内存分配器，调用方需保证其生命周期不短于本池。
     */
    class BlockPool
    {
        BlockAllocator          block_alloc;        ///< 块号分配器（只管理块号链）
        AbstractMemoryAllocator *memory_allocator;  ///< 底层字节内存（注入，非拥有）

        uint32 block_size;                          ///< 每块字节数（默认 16）
        uint32 max_count;                           ///< 总块数

        UnorderedMap<uint32,BlockAllocator::UserNode *> node_by_start;  ///< 起始块号 -> 占用节点（反查用）

    public:

        BlockPool():memory_allocator(nullptr),block_size(0),max_count(0){}
        ~BlockPool()=default;

        BlockPool(const BlockPool &)=delete;            ///< 禁止拷贝
        BlockPool &operator=(const BlockPool &)=delete;

        /**
         * 初始化块池
         * @param ma            底层字节内存（AbstractMemoryAllocator 纯虚接口实现）
         * @param block_count   总块数（>=2，其中 0 号块保留）
         * @param in_block_size 每块字节数（默认 16，需 >= alignof(T)）
         * @return 成功返回 true
         */
        bool Init(AbstractMemoryAllocator *ma,const uint32 block_count,const uint32 in_block_size=16)
        {
            if(!ma||in_block_size==0||block_count<2)
                return false;

            if(!block_alloc.Init((int)block_count))
                return false;

            memory_allocator=ma;
            memory_allocator->SetAllocUnitSize(in_block_size);

            if(!memory_allocator->Reserve((uint64)block_count*in_block_size))
                return false;

            block_size=in_block_size;
            max_count=block_count;

            Acquire(1);             // 永久占用 0 号块，此后 Acquire 返回的块号恒 >=1

            return true;
        }

        /**
         * 申请 count 个连续块
         * @return 起始块号；失败或参数非法返回 0
         */
        uint32 Acquire(const uint32 count)
        {
            if(count==0)
                return 0;

            BlockAllocator::UserNode *ud=block_alloc.Acquire((int)count);

            if(!ud)
                return 0;

            const uint32 start=(uint32)ud->GetStart();

            node_by_start.Add(start,ud);

            return start;
        }

        /**
         * 释放起始块号为 start 的连续块（内部反查占用节点）
         * @return 成功返回 true；start==0 或非本池块号返回 false
         */
        bool Release(const uint32 start)
        {
            if(start==0)            // 0 号块保留，不可释放
                return false;

            BlockAllocator::UserNode *ud;

            if(!node_by_start.Get(start,ud))
                return false;

            node_by_start.DeleteByKey(start);

            return block_alloc.Release(ud);
        }

        /**
         * 取得块号对应的字节地址（CPU 侧直读直写）
         */
        void *GetData(const uint32 index)
        {
            return memory_allocator->Get((uint64)index*block_size);
        }

        template<typename T>
        T *GetData(const uint32 index)
        {
            return (T *)GetData(index);
        }

        /**
         * 取得底层内存基址（如需直接做显存映射/偏移计算）
         */
        void *GetBase(){return memory_allocator->Get();}

        const uint32 GetBlockSize()const{return block_size;}
        const uint32 GetMaxCount ()const{return max_count;}
        const uint32 GetFreeCount()const{return (uint32)block_alloc.GetFreeCount();}
    };//class BlockPool
}//namespace hgl
