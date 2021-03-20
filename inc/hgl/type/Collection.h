#ifndef HGL_COLLECTION_INCLUDE
#define HGL_COLLECTION_INCLUDE

#include<hgl/type/MemoryBlock.h>
namespace hgl
{
    template<typename T> struct CheckElement
    {
        virtual const bool Check(const T &)=0;
    };//template<typename T> struct CheckElement

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
        virtual const uint64    GetAllocCount   ()const{return memory_block?memory_block->GetAllocSize()/sizeof(T):0;}                      ///<获取合集实际分配空间

        virtual const uint64    TotalBytes      ()const{return data_count*sizeof(T);}
        
        //begin/end用于兼容for(T obj:Collection<T>)
                        T *     begin           ()const{return memory_block?(T *)(memory_block->Get()):nullptr;}
                        T *     end             ()const{return memory_block?(T *)(memory_block->Get())+data_count:nullptr;}

    public:

        Collection(MemoryBlock *mb=new MemoryBlock)
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
         * @param element 要增加的元素
         */
        virtual bool Add(const T &element)
        {
            if(!memory_block)
                return(false);

            if(!memory_block->Alloc((data_count+1)*sizeof(T)))
                return(false);

            if(!memory_block->Write(data_count*sizeof(T),&element,sizeof(T)))
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

            if(!memory_block->Write(data_count*sizeof(T),GetMemoryBlock(),0,source_size))
                return(false);

            ++data_count;
            return(true);
        }

        /**
         * 获取一个数据
         * @param index 数据索引
         * @param element 数据存放地址
         * @return 是否成功
         */
        virtual bool Get(const uint64 index,T *element)
        {
            if(index>=data_count)return(false);
            if(!element)return(false);
            if(!memory_block)return(false);

            memcpy(element,memory_block->Get(index*sizeof(T)),sizeof(T));

            return(true);
        }

        /**
         * 设置一个数据
         * @param index 要设置的位置索引(如索引不可用会失败)
         * @param element 要设置的数据
         * @return 是否成功
         */
        virtual bool Set(const uint64 index,const T &element)
        {
            if(index>=data_count)return(false);
            if(!memory_block)return(false);

            memcpy(memory_block->Get(index*sizeof(T)),&element,sizeof(T));

            return(true);
        }

        /**
         * 插入一个数据到合集
         * @param offset 要插入的偏移地址
         * @param element 要插入的元素
         */
        virtual bool Insert(const uint64 offset,const T &element)
        {
            if(data_count==0)
                return Add(element);

            if(!memory_block->Alloc((data_count+1)*sizeof(T)))
                return(false);

            memory_block->Move( sizeof(T)*(offset+1),
                                sizeof(T)* offset,
                                sizeof(T)*(data_count-offset));

            if(!memory_block->Write(offset*sizeof(T),&element,sizeof(T)))
                return(false);

            ++data_count;
            return(true);
        }

        /**
         * 交换两个数据的位置
         */
        virtual bool Exchange(uint64 target,uint64 source)
        {
            if(data_count<2)return(false);
            if(target==source)return(true);

            T tmp;

            target*=sizeof(T);
            source*=sizeof(T);

            memcpy(&tmp,                        memory_block->Get(target),  sizeof(T));
            memcpy(memory_block->Get(target),   memory_block->Get(source),  sizeof(T));
            memcpy(memory_block->Get(source),   &tmp,                       sizeof(T));

            return(true);
        }

        /**
         * 清空整个合集(并不释放内存)
         */
        virtual void Clear()
        {
            data_count=0;
        }

        /**
         * 清空整个合集并释放内存 
         */
        virtual void Free()
        {
            data_count=0;

            if(memory_block)
                memory_block->Free();
        }

        /**
         * 移除指定位置的数据
         * @param offset 要删除的数据偏移
         */
        virtual bool RemoveAt(const uint64 offset)
        {
            if(!memory_block)
                return(false);

            if(offset>data_count)
                return(false);

            //将最后一个数据移到这个位置
            memory_block->Copy( sizeof(T)*offset,
                                sizeof(T)*(data_count-1),
                                sizeof(T));

            --data_count;
            return(true);
        }

        /**
         * 移除指定数量的连续
         * @param start 起始索引
         * @param count 数据个数
         */
        virtual bool RemoveAt(const uint64 start,const uint64 count)
        {
            if(count==1)
                return RemoveAt(start);

            if(!memory_block)
                return(false);

            if(start+count>data_count)
                return(false);

            const uint64 end_count=data_count-(start+count);            //删除的数据段后面的数据个数

            if(end_count>0)
            {
                if(end_count<=count)                                    //后面比较少，那直接复制过来就行了
                {
                    //后面剩的和前面删掉的一样多，复制过来    
                    //[------------][***********]
                    //      ^             v
                    //      |             |
                    //      +-------------+

                    memory_block->Copy( sizeof(T)*start,
                                        sizeof(T)*(start+count),
                                        sizeof(T)*end_count);
                }
                else
                {
                    //后面剩的比前面空的多，将最后一段等长的复制过来
                    //[---][**********][***]
                    //  ^                v
                    //  |                |
                    //  +----------------+

                    memory_block->Copy( sizeof(T)*start,
                                        sizeof(T)*(data_count-count),
                                        sizeof(T)*count);
                }
            }
            //else{后面剩的数据个数为0，那就什么都不用干}

            data_count-=count;
            return(true);
        }

        /**
         * 获取数据在合集中的索引
         */
        virtual int64 indexOf(const T &value) const
        {
            T *p=begin();
            T *ep=end();

            do
            {
                if(*p==value)
                    return(p-begin());
            }
            while(++p<ep);

            return -1;
        }

        virtual bool isMember(const T &value) const
        {
            for(const T &v:*this)
                if(v==value)return(true);

            return(false);
        }
        
        /**
         * 按条件移除
         * @param condition 条件判断对象
         */
        virtual uint64 RemoveCondition(CheckElement<T> *condition)
        {
            uint64 origin_count=data_count;

            T *p=end();
            T *ep;
            T *bp=begin();

            ep=--p;

            do
            {
                if(condition->Check(*p))
                {
                    if(*p==*ep)     //就是最后一个不管了
                    {
                    }
                    else
                    {
                        *p=*ep;
                        --ep;
                    }

                    --data_count;
                }
            }
            while(--p>=bp);

            return origin_count-data_count;
        }

        struct CheckElementEqual:public CheckElement<T>
        {
            T value;

        public:

            CheckElementEqual(const T &v)
            {
                value=v;
            }

            const bool Check(const T &v) override
            {
                return v==value;
            }
        };//struct CheckElementEqual:public CheckElement<T>

        /**
         * 移除指定数据
         * @param value 要移除的数据
         * @return 移除的数据总量
         */
        virtual uint64 Remove(const T &value)
        {
            return RemoveCondition(&CheckElementEqual(value));
        }

        struct CheckElementInCollection:public CheckElement<T>
        {
            const Collection<T> *coll;

        public:

            CheckElementInCollection(const Collection<T> *c)
            {
                coll=c;
            }

            const bool Check(const T &v) override
            {
                return coll->isMember(v);
            }
        };//struct CheckElementInCollection:public CheckElement<T>

        /**
         * 从合集中移除指定的数据
         * @param coll 要移除的数据合集
         * @return 移除的数据总量
         */
        virtual uint64 Remove(const Collection<T> &coll)
        {
            return RemoveCondition(&CheckElementInCollection(&coll));
        }
    };//class Collection
}//namespace hgl
#endif//HGL_COLLECTION_INCLUDE
