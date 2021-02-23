#ifndef HGL_COLLECTION_INCLUDE
#define HGL_COLLECTION_INCLUDE

#include<hgl/type/DataType.h>
namespace hgl
{
    /**
     * 抽像数据块
     */
    class AbstractDataBlock
    {
    protected:

        uint64 data_size;                   ///<数据长度

        uint64 alloc_unit_size;             ///<分配单位长度(分配长度必须为此值的整倍数)
        uint64 alloc_size;                  ///<实际分配数据长度

    public:

        virtual void SetAllocUnitSize(const uint64 size)
        {
            alloc_unit_size=size;
        }

        /**
         * 计算需要分配的内存容量
         */
        virtual uint64 ComputeAllocSize(const uint64 size)
        {
            if(alloc_unit_size==0)
            {
                alloc_size=power_to_2(size);
            }
            else
            if(alloc_unit_size==1)
            {
                alloc_size=size;
            }
            else
            {
                alloc_size=(size+alloc_unit_size-1)/alloc_unit_size;
                alloc_size*=alloc_unit_size;
            }

            return alloc_size;
        }

    public:

        AbstractDataBlock()
        {
            data_size=0;
            alloc_unit_size=0;
            alloc_size=0;
        }

        virtual bool Alloc  (const uint64 size)=0;              ///<分配指定空间的数据
        virtual void Free   ()=0;                               ///<释放数据空间
        virtual void Clear  ()=0;                               ///<清空数据(但不释放空间)

        virtual const uint64 GetSize()const {return data_size;}

        virtual bool Exchange   (const uint64 target,const uint64 source,const uint64 size)=0;          ///<交换两个数据空间
        virtual void Move       (const uint64 target,const uint64 source,const uint64 size)=0;          ///<移动一块数据

        virtual bool Write      (const uint64 target,const void *source,const uint64 size)=0;           ///<写入一块数据

        virtual bool CopyFrom   (const uint64 target,AbstractDataBlock *source,const uint64 offset,const uint64 size)=0;
        virtual bool CopyFrom   (const uint64 target,AbstractDataBlock *source)
        {
            if(!source)return(false);

            return CopyFrom(target,source,0,source->GetSize());
        }
    };//class AbstractDataBlock

    class MemoryBlock:public AbstractDataBlock
    {
    protected:

        void *memory_block;

        void *temp_block;
        uint64 temp_size;

    public:

        MemoryBlock()
        {
            memory_block=nullptr;
            temp_block=nullptr;
            temp_size=0;
        }

        virtual ~MemoryBlock()
        {
            Free();
            hgl_free(temp_block);
        }

        bool Alloc(const uint64 size) override
        {
            if(size<=0)return(false);
            if(size<=data_size)return(true);

            if(size<alloc_size)
                return(true);

            alloc_size=ComputeAllocSize(size);

            hgl_align_realloc(memory_block,alloc_size,alloc_unit_size);       //hgl_align_realloc支持传入空的memory_block，所以无需判断malloc/realloc

            return memory_block;
        }

        void Free() override
        {
            if(memory_block)
                hgl_free(memory_block);

            data_size=0;
            alloc_size=0;
        }

        void Clear() override
        {
            data_size=0;
        }

        bool Write(const uint64 target,const void *source,const uint64 size) override
        {
            if(target>data_size)return(false);
            if(!source||!size)return(false);

            memcpy((uint8 *)memory_block+target,source,size);
            return(true);
        }

        bool Exchange   (const uint64 target,const uint64 source,const uint64 size) override
        {
            if(size<=0)return(false);
            if(target==source)return(true);

            if(size>temp_size)
            {
                temp_size=ComputeAllocSize(size);
                temp_block=hgl_align_realloc(temp_block,temp_size,alloc_unit_size);
            }

            memcpy( temp_block,
                    (uint8 *)memory_block+target,
                    size);

            memcpy( (uint8 *)memory_block+target,
                    (uint8 *)memory_block+source,
                    size);

            memcpy( (uint8 *)memory_block+source,
                    temp_block,
                    size);

            return(true);
        }

        void Move       (const uint64 target,const uint64 source,const uint64 size) override
        {
            memmove((uint8 *)memory_block+target,
                    (uint8 *)memory_block+source,
                    size);
        }

        bool CopyFrom   (const uint64 target,AbstractDataBlock *source,const uint64 offset,const uint64 size) override
        {
        }

        bool CopyFrom   (const uint64 target,AbstractDataBlock *source) override
        {
        }
    };//class MemoryBlock:public AbstractDataBlock

    /**
     * 抽像合集
     */
    template<typename T> class AbstractCollection
    {
    protected:

        AbstractDataBlock *data_block;

        uint64 data_count;

    public:

        AbstractCollection(AbstractDataBlock *adb)
        {
            data_block=adb;
            data_count=0;
        }

        virtual ~AbstractCollection()
        {
            SAFE_CLEAR(data_block);
        }

        virtual AbstractDataBlock *getDataBlock()const{return data_block;}

        virtual const uint64 GetCount()const{return data_count;}
        virtual const bool isEmpty()const{return !data_count;}

        virtual bool add(const T &obj)
        {
            if(!data_block->Alloc((data_count+1)*sizeof(T)))
                return(false);

            if(!data_block->Write(data_count*sizeof(T),&obj,sizeof(T)))
                return(false);

            ++data_count;
            return(true);
        }

        virtual bool addAll(const AbstractCollection<T> &c)
        {
            const uint64 source_size=c.GetCount();

            if(!source_size)return(true);

            if(!data_block->Alloc((data_count+source_size)*sizeof(T)))
                return(false);

            return data_block->CopyFrom(data_count*sizeof(T),getDataBlock());
        }

        virtual void clear()
        {
            data_count=0;
            if(data_block)data_block->Clear();
        }
    };//

    /**
     * 数据合集模板
     */
    template<typename T> class Collection:public AbstractCollection
    {
    public:

        Collection():AbstractCollection(new MemoryBlock){}

        virtual bool add(const T &obj);
        virtual bool addAll(const Collection<T> &c);

        virtual void clear();
    };//
}//namespace hgl
#endif//HGL_COLLECTION_INCLUDE