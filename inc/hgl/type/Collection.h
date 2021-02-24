#ifndef HGL_COLLECTION_INCLUDE
#define HGL_COLLECTION_INCLUDE

#include<hgl/type/DataType.h>
namespace hgl
{
    /**
     * 抽像内存分配器
     */
    class AbstractMemoryAllocator
    {
    protected:

        void *memory_block;                 ///<内存地址

        uint64 data_size;                   ///<数据长度

        uint64 alloc_unit_size;             ///<分配单位长度(分配长度必须为此值的整倍数)
        uint64 alloc_size;                  ///<实际分配数据长度
        
    public:

        virtual const   uint64  GetSize         ()const{return data_size;}          ///<获取所需求的内存大小
        virtual const   uint64  GetAllocUnitSize()const{return alloc_unit_size;}    ///<获取分配单元大小
        virtual const   uint64  GetAllocSize    ()const{return alloc_size;}         ///<获取实际分配的内存大小

        virtual         void *  Get             (){return memory_block;}            ///<取得内存块地址
        virtual         void *  Get             (const uint64 offset)
        {
            return (uint8 *)memory_block+offset;
        }

    public:

        /**
         * 设置分配单元长度
         */
        virtual void SetAllocUnitSize(const uint64 size)
        {
            alloc_unit_size=size;
        }

        /**
         * 计算需要分配的内存容量
         */
        virtual uint64 ComputeAllocSize(const uint64 size)
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

    public:

        AbstractMemoryAllocator()
        {
            memory_block=nullptr;

            data_size=0;
            alloc_unit_size=0;
            alloc_size=0;
        }

        virtual ~AbstractMemoryAllocator()=default;

        virtual bool Alloc  (const uint64 size)=0;              ///<分配指定空间的数据
        virtual void Free   ()=0;                               ///<释放数据空间
        virtual void Clear  (){data_size=0;}
    };//class AbstractMemoryAllocator

    class MemoryAllocator:public AbstractMemoryAllocator
    {
    public:

        using AbstractMemoryAllocator::AbstractMemoryAllocator;

        virtual ~MemoryAllocator()
        {
            Free();
        }

        bool Alloc(const uint64 size) override
        {
            if(size<=0)return(false);
            if(size<=data_size)return(true);

            if(size<alloc_size)
                return(true);

            alloc_size=ComputeAllocSize(size);

            memory_block=hgl_align_realloc(memory_block,alloc_size,alloc_unit_size);       //hgl_align_realloc支持传入空的memory_block，所以无需判断malloc/realloc

            return memory_block;
        }

        void Free() override
        {
            if(memory_block)
            {
                hgl_free(memory_block);
                memory_block=nullptr;
            }

            data_size=0;
            alloc_size=0;
        }
    };//class MemoryAllocator:public AbstractMemoryAllocator

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

        MemoryBlock(AbstractMemoryAllocator *ama)
        {
            memory_allocator=ama;

            temp_block=nullptr;
            temp_size=0;
        }

        virtual ~MemoryBlock()
        {
            hgl_free(temp_block);
            SAFE_CLEAR(memory_allocator);       //会同步释放内存
        }

        virtual uint64  GetSize ()const                 {return memory_allocator->GetSize();}
        virtual void *  Get     ()                      {return memory_allocator->Get();}
        virtual void *  Get     (const uint64 offset)   {return memory_allocator->Get(offset);}

        virtual void Clear()
        {
            memory_allocator->Clear();
        }

        virtual bool Alloc(const uint64 size,const uint64 uint_size=0)
        {
            memory_allocator->SetAllocUnitSize(uint_size);
            memory_allocator->Alloc(size);
        }

        virtual bool Write(const uint64 target,const void *source,const uint64 size) 
        {
            if(target+size>memory_allocator->GetSize())return(false);
            if(!source||!size)return(false);

            memcpy(Get(target),source,size);
            return(true);
        }

        virtual bool Write(const uint64 target,MemoryBlock *source,const uint64 offset,const uint64 size)
        {
            if(offset+size>source->GetSize())return(false);

            return Write(target,source->Get(offset),size);
        }

        virtual bool Exchange   (const uint64 target,const uint64 source,const uint64 size) 
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

        void Move       (const uint64 target,const uint64 source,const uint64 size)
        {
            memmove(Get(target),
                    Get(source),
                    size);
        }
    };//class MemoryBlock:public AbstractDataBlock

    /**
     * 数据合集
     */
    template<typename T> class Collection
    {
    protected:

        MemoryBlock *memory_block;

        uint64 data_count;

    public:

        Collection(AbstractMemoryBlock *adb)
        {
            memory_block=adb;
            data_count=0;
        }

        virtual ~Collection()
        {
            SAFE_CLEAR(memory_block);
        }

        virtual MemoryBlock *   GetMemoryBlock()const{return memory_block;}

        virtual const uint64    GetCount()const{return data_count;}
        virtual const bool      IsEmpty()const{return !data_count;}

        virtual bool Add(const T &obj)
        {
            if(!memory_block->Alloc((data_count+1)*sizeof(T)))
                return(false);

            if(!memory_block->Write(data_count*sizeof(T),&obj,sizeof(T)))
                return(false);

            ++data_count;
            return(true);
        }

        virtual bool Add(const Collection<T> &c)
        {
            const uint64 source_size=c.GetCount();

            if(!source_size)return(true);

            if(!memory_block->Alloc((data_count+source_size)*sizeof(T)))
                return(false);

            return memory_block->Write(data_count*sizeof(T),GetMemoryBlock());
        }

        virtual void Clear()
        {
            data_count=0;
            if(memory_block)memory_block->Clear();
        }
    };//class Collection
}//namespace hgl
#endif//HGL_COLLECTION_INCLUDE