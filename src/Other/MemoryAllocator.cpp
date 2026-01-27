#include<hgl/type/MemoryAllocator.h>

namespace hgl
{
    AbstractMemoryAllocator::AbstractMemoryAllocator()
    {
        memory_block=nullptr;

        data_size=0;
        alloc_unit_size=0;
        alloc_size=0;
    }

    void AbstractMemoryAllocator::SetAllocUnitSize(const uint64 size)
    {
        alloc_unit_size=size;
    }

    /**
        * 计算需要分配的内存容量
        */
    uint64 AbstractMemoryAllocator::ComputeAllocSize(const uint64 size)
    {
        uint64 result;

        if(alloc_unit_size==0)
        {
            result=power_to_2(size);
        }
        else
        if(alloc_unit_size==1)
        {
            result=size;
        }
        else
        {
            result=(size+alloc_unit_size-1)/alloc_unit_size;
            result*=alloc_unit_size;
        }

        return result;
    }

    bool AbstractMemoryAllocator::Reserve(const uint64 size)
    {
        if(size<=0)return(false);

        if(memory_block)
        {
            if(size<=data_size)return(true);

            if(size<=alloc_size)
            {
                data_size=size;
                return(true);
            }
        }

        alloc_size=ComputeAllocSize(size);

        if(!AllocMemory())
            return(false);

        data_size=size;
        return(true);
    }

    MemoryAllocator::~MemoryAllocator()
    {
        Free();
    }

    bool MemoryAllocator::AllocMemory()
    {
        memory_block=hgl_align_realloc(memory_block,alloc_size,alloc_unit_size);       //hgl_align_realloc支持传入空的memory_block，所以无需判断malloc/realloc

        return memory_block;
    }

    void MemoryAllocator::Free()
    {
        if(memory_block)
        {
            hgl_free(memory_block);
            memory_block=nullptr;
        }

        data_size=0;
        alloc_size=0;
    }
}//namespace hgl
