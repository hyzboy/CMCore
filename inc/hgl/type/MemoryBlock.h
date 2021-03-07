#ifndef HGL_MEMORY_BLOCK_INCLUDE
#define HGL_MEMORY_BLOCK_INCLUDE

#include<hgl/type/MemoryAllocator.h>
#include<string.h>
namespace hgl
{
    /**
     * 内存数据块
     */
    class MemoryBlock
    {
    protected:

        AbstractMemoryAllocator *memory_allocator;

        void *temp_block;
        uint64 temp_size;

    public:

        MemoryBlock(AbstractMemoryAllocator *ama);
        virtual ~MemoryBlock();

        virtual const   uint64  GetSize ()const                 {return memory_allocator->GetSize();}
        virtual         void *  Get     ()                      {return memory_allocator->Get();}
        virtual         void *  Get     (const uint64 offset)   {return memory_allocator->Get(offset);}

        virtual         void Clear      ();
        virtual         bool Alloc      (const uint64 size,const uint64 uint_size=0);
        virtual         bool Write      (const uint64 target,const void *source,const uint64 size);
        virtual         bool Write      (const uint64 target,MemoryBlock *source,const uint64 offset,const uint64 size);
        virtual         bool Exchange   (const uint64 target,const uint64 source,const uint64 size);
        virtual         void Move       (const uint64 target,const uint64 source,const uint64 size);
    };//class MemoryBlock:public AbstractDataBlock
}//namespace hgl
#endif//HGL_MEMORY_BLOCK_INCLUDE
