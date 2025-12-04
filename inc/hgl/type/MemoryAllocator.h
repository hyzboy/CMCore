#pragma once

#include<hgl/type/StrChar.h>
#include<hgl/type/MemoryAlloc.h>
#include<hgl/Comparator.h>
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

        virtual         bool    AllocMemory     ()=0;                               ///<分配指定空间的数据

    public:

        virtual const   uint64  GetSize         ()const{return data_size;}          ///<获取所需求的内存大小
        virtual const   uint64  GetAllocUnitSize()const{return alloc_unit_size;}    ///<获取分配单元大小
        virtual const   uint64  GetAllocSize    ()const{return alloc_size;}         ///<获取实际分配的内存大小

        virtual const   bool    CanRealloc      ()const=0;                          ///<是否可重新分配

        virtual         void *  Get             (){return memory_block;}            ///<取得内存块地址
        virtual         void *  Get             (const uint64 offset)
        {
            return (uint8 *)memory_block+offset;
        }

    public:

        /**
         * 设置分配单元长度
         */
        virtual void SetAllocUnitSize(const uint64 size);

        /**
         * 计算需要分配的内存容量
         */
        virtual uint64 ComputeAllocSize(const uint64 size);

    public:

        AbstractMemoryAllocator();
        virtual ~AbstractMemoryAllocator()=default;

                bool Reserve(const uint64);                     ///<分配指定长度的空间
        virtual void Free   ()=0;                               ///<释放数据空间
        virtual void Clear  (){data_size=0;}
        virtual bool Write  (const void *source,const uint64 offset,const uint64 size)=0;
    };//class AbstractMemoryAllocator

    class MemoryAllocator:public AbstractMemoryAllocator
    {
    protected:

        virtual bool AllocMemory() override;

    public:

        virtual const bool CanRealloc()const override{return true;}

    public:

        using AbstractMemoryAllocator::AbstractMemoryAllocator;
        virtual ~MemoryAllocator();
        virtual void Free() override;

        virtual bool Write(const void *source,const uint64 offset,const uint64 size) override
        {
            if(!source||size==0)return(false);

            if(offset+size>data_size)
                return(false);

            memcpy((uint8 *)memory_block+offset,source,size);
            return(true);
        }
    };//class MemoryAllocator:public AbstractMemoryAllocator
}//namespace hgl
