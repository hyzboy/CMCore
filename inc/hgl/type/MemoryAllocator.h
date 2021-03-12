#ifndef HGL_MEMORY_ALLOCATOR_INCLUDE
#define HGL_MEMORY_ALLOCATOR_INCLUDE

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

        virtual bool Alloc  (const uint64 size)=0;              ///<分配指定空间的数据
        virtual void Free   ()=0;                               ///<释放数据空间
        virtual void Clear  (){data_size=0;}
    };//class AbstractMemoryAllocator

    class MemoryAllocator:public AbstractMemoryAllocator
    {
    public:

        virtual const bool CanRealloc()const{return true;}

    public:

        using AbstractMemoryAllocator::AbstractMemoryAllocator;
        virtual ~MemoryAllocator();

        virtual bool Alloc(const uint64 size);
        virtual void Free();
    };//class MemoryAllocator:public AbstractMemoryAllocator
}//namespace hgl
#endif//HGL_MEMORY_ALLOCATOR_INCLUDE
