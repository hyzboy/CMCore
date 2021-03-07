#ifndef HGL_COLLECTION_INCLUDE
#define HGL_COLLECTION_INCLUDE

#include<hgl/type/MemoryBlock.h>
namespace hgl
{
    /**
     * 数据合集
     */
    template<typename T> class Collection
    {
    protected:

        MemoryBlock *memory_block;

        uint64 data_count;

    public:

        Collection(MemoryBlock *mb)
        {
            memory_block=mb;
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

            return memory_block->Write(data_count*sizeof(T),GetMemoryBlock(),0,source_size);
        }

        virtual void Clear()
        {
            data_count=0;
            if(memory_block)memory_block->Clear();
        }
    };//class Collection
}//namespace hgl
#endif//HGL_COLLECTION_INCLUDE