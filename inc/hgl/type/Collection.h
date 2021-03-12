#ifndef HGL_COLLECTION_INCLUDE
#define HGL_COLLECTION_INCLUDE

#include<hgl/type/MemoryBlock.h>
namespace hgl
{
    template<typename T> struct Iterable
    {
        virtual bool Equal(T &target,const T &source)
        {
            target=source;
        }

        virtual void Copy(T *target,const T *source,const uint64 count)
        {
            memcpy(target,source,count*sizeof(T));
        }

        virtual int Comp(const T &a,const T &b)=0;
    };

    /**
     * 数值
     */
    template<typename T> struct NumberIterable:public Iterable<T>
    {    
        int Comp(const T &a,const T &b) override
        {
            return a-b;
        }
    };

    template<typename T> struct ObjectIterable:public Iterable<T>
    {
        void Copy(T *target,const T *source,const uint64 count)
        {
            for(uint64 i=0;i<count;i++)
            {
                *target=*source;
                ++target;
                ++source;
            }
        }
    };

    /**
     * 数据合集
     */
    template<typename T> class Collection
    {
    protected:

        MemoryBlock *memory_block;

        uint64 data_count;

    public: //属性
            
        virtual MemoryBlock *   GetMemoryBlock  ()const{return memory_block;}                                                               ///<获取整个合集所用的内存块
        virtual T *             ToArray         ()const{return memory_block?(T *)(memory_block->Get()):nullptr;}                            ///<按C阵列方式访问数据
        
        virtual const bool      IsEmpty         ()const{return !data_count;}                                                                ///<当前合集是否为空
        virtual const uint64    GetCount        ()const{return data_count;}                                                                 ///<获取合集内的数据个数
        virtual const uint64    GetAllocCount   ()const{return memory_block?memory_block->GetAllocSize()/sizeof(T):nullptr;}                ///<获取合集实际分配空间

        virtual const uint64    TotalBytes      ()const{return data_count*sizeof(T);}
        
        //begin/end用于兼容for(T obj:Collection<T>)
                        T *     begin           ()const{return memory_block?memory_block->Get():nullptr;}
                        T *     end             ()const{return memory_block?memory_block->Get()+data_count:nullptr;}

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

    public:

        /**
         * 增加一个数据到合集
         */
        virtual bool Add(const T &obj)
        {
            if(!memory_block)
                return(false);

            if(!memory_block->Alloc((data_count+1)*sizeof(T)))
                return(false);

            if(!memory_block->Write(data_count*sizeof(T),&obj,sizeof(T)))
                return(false);

            ++data_count;
            return(true);
        }

        /**
         * 增加一整个合集的数据
         */
        virtual bool Add(const Collection<T> &c)
        {
            if(!memory_block)
                return(false);

            const uint64 source_size=c.GetCount();

            if(!source_size)return(true);

            if(!memory_block->Alloc((data_count+source_size)*sizeof(T)))
                return(false);

            return memory_block->Write(data_count*sizeof(T),GetMemoryBlock(),0,source_size);
        }

        /**
         * 清空整个合集(并不释放内存)
         */
        virtual void Clear()
        {
            data_count=0;
        }

        /**
         * 删除指定个连续数据
         * @param start 起始索引
         * @param count 数据个数
         */
        virtual bool Delete(const uint64 start,const uint64 count)
        {
            if(!memory_block)
                return(false);

            if(start+count>data_count)
                return(false);

            if(start+count<data_count)  
                memory_block->Move( sizeof(T)*start,
                                    sizeof(T)*(start+count),
                                    sizeof(T)*(data_count-start-count));        //将最后一段数据移到前面来
            //else 
            //{
            //    如果start_count==data_count代表要移除的数据本就在最后，那就不用move处理了
            //}

            data_count-=count;
            return(true);
        }
    };//class Collection
}//namespace hgl
#endif//HGL_COLLECTION_INCLUDE
