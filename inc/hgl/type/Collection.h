#pragma once

#include<hgl/type/MemoryBlock.h>
namespace hgl
{
    class Collection;

    struct CheckElement
    {
        virtual         void Update(const void *){}
        virtual const   bool Check(const void *)const=0;
    };//struct CheckElement

    struct CheckElementMemcmp:public CheckElement
    {
        const void *value;
        size_t size;

    public:

        CheckElementMemcmp(const size_t s)
        {
            value=nullptr;
            size=s;
        }

        CheckElementMemcmp(const void *v,const size_t s)
        {
            value=v;
            size=s;
        }

        void Update(const void *v)override
        {
            value=v;
        }

        const bool Check(const void *v) const override
        {
            return !memcmp(v,value,size);
        }
    };//struct CheckElementMemcmp:public CheckElement

    template<typename T>
    struct CheckElementEqual:public CheckElement
    {
        T value;

    public:

        CheckElementEqual(){}

        CheckElementEqual(const T &v)
        {
            value=v;
        }

        void Update(const void *v)override
        {
            value=*(const T *)v;
        }

        const bool Check(const void *v) const override
        {
            return (*(const T *)v)==value;
        }
    };//struct CheckElementEqual:public CheckElement<T>

    /**
     * 数据合集类
     */
    class Collection
    {
    protected:

        MemoryBlock *memory_block;

        uint32 unit_bytes;      //单个数据字节数
        uint64 data_count;
        uint64 total_bytes;     //总字节数

        uchar *tmp_buffer;

    public: //属性

        MemoryBlock *   GetMemoryBlock  ()const{return memory_block;}                                                               ///<获取整个合集所用的内存块

        template<typename T>
                T *     ToArray         ()const{return memory_block?(T *)(memory_block->Get()):nullptr;}                            ///<按C阵列方式访问数据

                void *  begin           ()const{return memory_block?memory_block->Get():nullptr;}
                void *  end             ()const{return memory_block?((uchar *)memory_block->Get())+total_bytes:nullptr;}

        const   bool    IsEmpty         ()const{return !data_count;}                                                                ///<当前合集是否为空
        const   uint64  GetCount        ()const{return data_count;}                                                                 ///<获取合集内的数据个数
        const   uint64  GetAllocCount   ()const{return memory_block?memory_block->GetAllocSize()/unit_bytes:0;}                     ///<获取合集实际分配空间

        const   uint32  GetUnitBytes    ()const{return unit_bytes;}                                                                 ///<取得单项数据长度字节数
        const   uint64  GetTotalBytes   ()const{return total_bytes;}                                                                ///<取得所有数据总长度字节数

    public:

        Collection(const uint32 ub=1,MemoryBlock *mb=new MemoryBlock);
        virtual ~Collection();

        bool Reserve(const uint64 count);                                                     ///<预分配空间

    public:

        bool Add(const void *element);                                                      ///<增加一个数据到合集

        template<typename T>
        bool AddValue(const T &value)
        {
            return Add(&value);
        }

        bool AddCollection(const Collection &c);                                            ///<增加一整个合集的数据
        bool Get(const uint64 index,void *element);                                         ///<获取一个数据
        bool Set(const uint64 index,const void *element);                                   ///<设置一个数据
        bool Insert(const uint64 offset,const void *element);                               ///<插入一个数据到合集
        bool Exchange(uint64 target,uint64 source);                                         ///<交换两个数据的位置

        void Clear();                                                                       ///<清空整个合集(并不释放内存)
        void *Map(const uint64 start,const uint64 range);                                   ///<映射一段数据区供访问
        void Free();                                                                        ///<清空整个合集并释放内存

        bool RemoveAt(const uint64 offset);                                                 ///<移除指定位置的数据
        bool RemoveAt(const uint64 start,const uint64 remove_count);                        ///<移除指定数量的连续

        int64 indexOfCondition(CheckElement *condition) const;                              ///<获取数据在合集中的索引

        int64 indexOf(const void *value) const                                              ///<获取数据在合集中的索引
        {
            CheckElementMemcmp cem(value,unit_bytes);

            return indexOfCondition(&cem);
        }

        template<typename T>
        int64 indexOfValue(const T &value) const                                            ///<获取数据在合集中的索引
        {
            CheckElementEqual<T> cee(value);

            return indexOfCondition(&cee);
        }

        bool Contains(const void *value) const{return indexOf(value)!=-1;}                  ///<判断当前数据是否是其成员

        template<typename T>
        bool  ContainsValue(const T &value)const{return indexOfValue<T>(value)!=-1;}        ///<判断当前数据是否是其成员

        int64 RemoveCondition(CheckElement *condition,int max_count=1);                     ///<按条件移除

        /**
         * 移除指定数据
         * @param value 要移除的数据
         * @param max_count 最大移除个数
         * @return 移除的数据总量
         */
        int64 Remove(const void *value,int max_count=1)
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
        int64 RemoveValue(const T &value,int max_count=1)
        {
            CheckElementEqual<T> cee(value);

            return RemoveCondition(&cee,max_count);
        }

        int64 RemoveCollection(const Collection &coll,CheckElement *ce,int64 max_count=-1); ///<从合集中移除指定的数据
    };//class Collection

    /**
     * 合集数据枚举器
     */
    template<typename T> class ElementEnumerator
    {
        const Collection *coll;

    public:

        ElementEnumerator(const Collection *c)
        {
            coll=c;
        }

        const size_t size()const
        {
            return (T *)(coll->end())
                  -(T *)(coll->begin());
        }

        const T *begin()const{return (T *)(coll->begin());}
        const T *end()const{return (T *)(coll->end());}
    };//template<typename T> class ElementEnumerator
}//namespace hgl
