#ifndef HGL_COLLECTION_INCLUDE
#define HGL_COLLECTION_INCLUDE

#include<hgl/type/MemoryBlock.h>
namespace hgl
{
    class Collection;

    struct ElementOperator
    {
        virtual const bool equal(const void *,const void *)const{return false;}
        virtual const int comp(const void *,const void *)const{return 0;}
        virtual const bool copy(void *,const void *)const{return false;}
    };//

    struct ElementOperatorMemcmp:public ElementOperator
    {
        size_t size;

    public:

        ElementOperatorMemcmp(const size_t s)
        {
            size=s;
        }

        const bool equal(const void *a,const void *b)const override
        {
            return !memcmp(a,b,size);
        }

        const int comp(const void *a,const void *b)const override
        {
            return memcmp(a,b,size);
        }
    };//struct ElementOperatorMemcmp:public ElementOperator

    template<typename T>
    struct ElementOperatorRawtype:public ElementOperator
    {        
    public:

        const bool equal(const void *a,const void *b)const override
        {
            return (*(T *)a)
                 ==(*(T *)b);
        }

        const int comp(const void *a,const void *b)const override
        {
            return (*(T *)a)
                  -(*(T *)b);
        }
    };//struct ElementOperatorRawtype:public ElementOperator

    struct CheckElement
    {
        virtual const bool Check(const void *)const=0;
    };//struct CheckElement
    
    struct CheckElementMemcmp:public CheckElement
    {
        const void *value;
        size_t size;

    public:

        CheckElementMemcmp(const void *v,const size_t s)
        {
            value=v;
            size=s;
        }

        const bool Check(const void *v) const override
        {
            return !memcmp(v,value,size);
        }
    };//struct CheckElementMemcmp:public CheckElement

    template<typename T>
    struct CheckElementEqual:public CheckElement
    {
        const T value;

    public:

        CheckElementEqual(const T &v)
        {
            value=v;
        }

        const bool Check(const void *v) const override
        {
            return (*(T *)v)==value;
        }
    };//struct CheckElementEqual:public CheckElement<T>

    struct CheckElementInCollection:public CheckElement
    {
        const Collection *coll;
        const CheckElement *check;

    public:

        CheckElementInCollection(const Collection *c,const CheckElement *ce)
        {
            coll=c;
            check=ce;
        }

        const bool Check(const void *v) const override
        {
            return check->Check(v);
        }
    };//struct CheckElementInCollection:public CheckElement

    /**
     * 数据合集类
     */
    class Collection
    {
    protected:

        MemoryBlock *memory_block;

        size_t unit_bytes;      //单个数据字节数
        uint64 data_count;
        uint64 total_bytes;     //总字节数

        uchar *tmp_buffer;

        ElementOperator *eo_default;
        ElementOperator *element_operator;

    public: //属性

                const size_t    GetUnitBytes    ()const{return unit_bytes;}
            
        virtual MemoryBlock *   GetMemoryBlock  ()const{return memory_block;}                                                               ///<获取整个合集所用的内存块

        template<typename T>
                T *             ToArray         ()const{return memory_block?(T *)(memory_block->Get()):nullptr;}                            ///<按C阵列方式访问数据

                      void *    begin           ()const{return memory_block?memory_block->Get():nullptr;}
                      void *    end             ()const{return memory_block?((uchar *)memory_block->Get())+total_bytes:nullptr;}
        
        virtual const bool      IsEmpty         ()const{return !data_count;}                                                                ///<当前合集是否为空
        virtual const uint64    GetCount        ()const{return data_count;}                                                                 ///<获取合集内的数据个数
        virtual const uint64    GetAllocCount   ()const{return memory_block?memory_block->GetAllocSize()/unit_bytes:0;}                      ///<获取合集实际分配空间

        virtual const uint64    GetTotalBytes   ()const{return total_bytes;}

    public:

        Collection(const size_t ub=1,MemoryBlock *mb=new MemoryBlock,ElementOperator *eo=nullptr);
        virtual ~Collection();

        virtual bool Alloc(const uint64 count);                                 ///<预分配空间

    public:

        virtual bool Add(const void *element);                                  ///<增加一个数据到合集
        virtual bool Add(const Collection &c);                                  ///<增加一整个合集的数据
        virtual bool Get(const uint64 index,void *element);                     ///<获取一个数据
        virtual bool Set(const uint64 index,const void *element);               ///<设置一个数据
        virtual bool Insert(const uint64 offset,const void *element);           ///<插入一个数据到合集
        virtual bool Exchange(uint64 target,uint64 source);                     ///<交换两个数据的位置

        virtual void Clear();                                                   ///<清空整个合集(并不释放内存)
        virtual void *Map(const uint64 start,const uint64 range);               ///<映射一段数据区供访问
        virtual void Free();                                                    ///<清空整个合集并释放内存

        virtual bool RemoveAt(const uint64 offset);                             ///<移除指定位置的数据
        virtual bool RemoveAt(const uint64 start,const uint64 remove_count);    ///<移除指定数量的连续

        virtual int64 indexOfCondition(CheckElement *condition) const;          ///<获取数据在合集中的索引

        virtual int64 indexOf(const void *value) const                          ///<获取数据在合集中的索引
        {
            CheckElementMemcmp cem(value,unit_bytes);

            return indexOfCondition(&cem);
        }

        template<typename T>
        int64 indexOf(const T &value)
        {
            CheckElementEqual<T> cee(value);

            return indexOfCondition(&cee);
        }

        virtual bool isMember(const void *value) const{return indexOf(value)!=-1;}                         ///<

        virtual int64 RemoveCondition(CheckElement *condition,int max_count=1);                             ///<按条件移除
        
        /**
         * 移除指定数据
         * @param value 要移除的数据
         * @param max_count 最大移除个数
         * @return 移除的数据总量
         */
        virtual int64 Remove(const void *value,int max_count=1)
        {
            CheckElementMemcmp cem(value,unit_bytes);

            return RemoveCondition(&cem,max_count);
        }

        /**
         * 根据类型移除指定数据
         * @param value 要移除的数据
         * @param max_count 最大移除个数
         * @return 移除的数据总量
         */
        template<typename T>
        int64 Remove(const T &value,int max_count=1)
        {
            CheckElementEqual<T> cee(value);

            return RemoveCondition(&cee,max_count);
        }

        virtual int64 Remove(const Collection &coll,const CheckElement *check,int64 max_count=-1);                          ///<从合集中移除指定的数据
    };//class Collection

    /**
     * 合集数据枚举器.
     */
    template<typename T> class ElementEnumerator
    {
        const Collection *coll;

    public:

        ElementEnumerator(const Collection *c)
        {
            coll=c;
        }

        const T *begin()const{return (T *)(coll->begin());}
        const T *end()const{return (T *)(coll->end());}
    };//template<typename T> class ElementEnumerator
}//namespace hgl
#endif//HGL_COLLECTION_INCLUDE
