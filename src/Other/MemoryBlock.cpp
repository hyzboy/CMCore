#include<hgl/type/MemoryBlock.h>
#include<string.h>
namespace hgl
{
    MemoryBlock::MemoryBlock(AbstractMemoryAllocator *ama)
    {
        memory_allocator=ama;

        temp_block=nullptr;
        temp_size=0;
    }

    MemoryBlock::~MemoryBlock()
    {
        hgl_free(temp_block);
        SAFE_CLEAR(memory_allocator);       //会同步释放内存
    }

    void MemoryBlock::Clear()
    {
        memory_allocator->Clear();
    }

    void MemoryBlock::Free()
    {
        memory_allocator->Free();
    }

    bool MemoryBlock::Reserve(const uint64 size,const uint64 uint_size)
    {
        memory_allocator->SetAllocUnitSize(uint_size);
        return memory_allocator->Reserve(size);
    }

    bool MemoryBlock::Write(const uint64 target,const void *source,const uint64 size)
    {
        if(target+size>memory_allocator->GetSize())return(false);
        if(!source||!size)return(false);

        memcpy(Get(target),source,size);
        return(true);
    }

    bool MemoryBlock::Write(const uint64 target,MemoryBlock *source,const uint64 offset,const uint64 size)
    {
        if(offset+size>source->GetSize())return(false);

        return Write(target,source->Get(offset),size);
    }

    bool MemoryBlock::Exchange   (const uint64 target,const uint64 source,const uint64 size)
    {
        if(size<=0)return(false);
        if(target==source)return(true);

        if(size>temp_size)
        {
            temp_size=memory_allocator->ComputeAllocSize(size);
            temp_block=hgl_align_realloc(temp_block,temp_size,memory_allocator->GetAllocUnitSize());
        }

        memcpy( temp_block,
                Get(target),
                size);

        memcpy( Get(target),
                Get(source),
                size);

        memcpy( Get(source),
                temp_block,
                size);

        return(true);
    }

    void MemoryBlock::Move       (const uint64 target,const uint64 source,const uint64 size)
    {
        memmove(Get(target),
                Get(source),
                size);
    }

    void MemoryBlock::Copy       (const uint64 target,const uint64 source,const uint64 size)
    {
        memcpy( Get(target),
                Get(source),
                size);
    }
}//namespace hgl
