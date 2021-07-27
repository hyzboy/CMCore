#include<hgl/type/Collection.h>

namespace hgl
{
    Collection::Collection(const uint32 ub,MemoryBlock *mb)
    {
        unit_bytes=ub;
        memory_block=mb;
        data_count=0;
        total_bytes=0;

        if(ub>0)
            tmp_buffer=new uchar[unit_bytes];
        else
            tmp_buffer=nullptr;
    }

    Collection::~Collection()
    {
        delete[] tmp_buffer;            //"delete[] nullptr" isn't a bug.
        SAFE_CLEAR(memory_block);
    }

    /**
     * 预分配空间
     */
    bool Collection::Alloc(const uint64 count)
    {
        if(!memory_block)
            return(false);

        uint64 cur_count=GetAllocCount();

        if(count<=cur_count)
            return(true);

        total_bytes=count*unit_bytes;

        if(!memory_block->Alloc(total_bytes))
            return(false);

        return(true);
    }

    /**
     * 增加一个数据到合集
     * @param element 要增加的元素内存指针
     */
    bool Collection::Add(const void *element)
    {
        if(!memory_block)
            return(false);

        const uint64 write_offset=total_bytes;

        if(!memory_block->Alloc(total_bytes+unit_bytes))
            return(false);

        if(!memory_block->Write(total_bytes,element,unit_bytes))
            return(false);

        ++data_count;
        total_bytes+=unit_bytes;
        return(true);
    }

    /**
     * 增加一整个合集的数据
     */
    bool Collection::AddCollection(const Collection &c)
    {
        if(!memory_block)
            return(false);

        const uint64 source_bytes=c.GetTotalBytes();

        if(!source_bytes)return(true);

        if(!memory_block->Alloc(total_bytes+source_bytes))
            return(false);

        if(!memory_block->Write(total_bytes,c.GetMemoryBlock(),0,source_bytes))
            return(false);

        ++data_count;
        total_bytes+=source_bytes;
        return(true);
    }

    /**
     * 获取一个数据
     * @param index 数据索引
     * @param element 数据存放指针
     * @return 是否成功
     */
    bool Collection::Get(const uint64 index,void *element)
    {
        if(index>=data_count)return(false);
        if(!element)return(false);
        if(!memory_block)return(false);

        memcpy(element,memory_block->Get(index*unit_bytes),unit_bytes);

        return(true);
    }

    /**
     * 设置一个数据
     * @param index 要设置的位置索引(如索引不可用会失败)
     * @param element 要设置的数据
     * @return 是否成功
     */
    bool Collection::Set(const uint64 index,const void *element)
    {
        if(index>=data_count)return(false);
        if(!memory_block)return(false);

        memcpy(memory_block->Get(index*unit_bytes),element,unit_bytes);

        return(true);
    }

    /**
     * 插入一个数据到合集
     * @param offset 要插入的偏移地址
     * @param element 要插入的元素
     */
    bool Collection::Insert(const uint64 offset,const void *element)
    {
        if(data_count==0)
            return Add(element);

        if(!memory_block->Alloc((data_count+1)*unit_bytes))
            return(false);

        memory_block->Move( unit_bytes*(offset+1),
                            unit_bytes* offset,
                            unit_bytes*(data_count-offset));

        if(!memory_block->Write(offset*unit_bytes,element,unit_bytes))
            return(false);

        ++data_count;
        total_bytes+=unit_bytes;
        return(true);
    }

    /**
     * 交换两个数据的位置
     */
    bool Collection::Exchange(uint64 target,uint64 source)
    {
        if(data_count<2)return(false);
        if(target==source)return(true);

        target*=unit_bytes;
        source*=unit_bytes;

        memcpy(tmp_buffer,                  memory_block->Get(target),  unit_bytes);
        memcpy(memory_block->Get(target),   memory_block->Get(source),  unit_bytes);
        memcpy(memory_block->Get(source),   tmp_buffer,                 unit_bytes);

        return(true);
    }

    /**
     * 清空整个合集(并不释放内存)
     */
    void Collection::Clear()
    {
        data_count=0;
        total_bytes=0;
    }

    /**
     * 映射一段数据区供访问
     */
    void *Collection::Map(const uint64 start,const uint64 range)
    {
        if(range<=0)return(nullptr);
        if(!memory_block)return(nullptr);

        if(start+range>data_count)
        {
            if(!memory_block->Alloc((start+range)*unit_bytes))
                return(nullptr);

            data_count=start+range;
            total_bytes=data_count*unit_bytes;
        }

        return memory_block->Get(start*unit_bytes);
    }

    /**
     * 清空整个合集并释放内存
     */
    void Collection::Free()
    {
        data_count=0;
        total_bytes=0;

        if(memory_block)
            memory_block->Free();
    }

    /**
     * 移除指定位置的数据
     * @param offset 要删除的数据偏移
     */
    bool Collection::RemoveAt(const uint64 offset)
    {
        if(!memory_block)
            return(false);

        if(offset>data_count)
            return(false);

        --data_count;
        total_bytes-=unit_bytes;

        //将最后一个数据移到这个位置
        memory_block->Copy( unit_bytes*offset,
                            unit_bytes*data_count,
                            unit_bytes);

        return(true);
    }

    /**
     * 移除指定数量的连续
     * @param start 起始索引
     * @param remove_count 要移除的数据个数
     */
    bool Collection::RemoveAt(const uint64 start,const uint64 remove_count)
    {
        if(remove_count==1)
            return RemoveAt(start);

        if(!memory_block)
            return(false);

        if(start+remove_count>data_count)
            return(false);

        const uint64 end_count=data_count-(start+remove_count);            //删除的数据段后面的数据个数

        if(end_count>0)
        {
            if(end_count<=remove_count)                                    //后面比较少，那直接复制过来就行了
            {
                //后面剩的和前面删掉的一样多，复制过来
                //[------------][***********]
                //      ^             v
                //      |             |
                //      +-------------+

                memory_block->Copy( unit_bytes* start,
                                    unit_bytes*(start+remove_count),
                                    unit_bytes*end_count);
            }
            else
            {
                //后面剩的比前面空的多，将最后一段等长的复制过来
                //[---][**********][***]
                //  ^                v
                //  |                |
                //  +----------------+

                memory_block->Copy( unit_bytes* start,
                                    unit_bytes*(data_count-remove_count),
                                    unit_bytes*remove_count);
            }
        }
        //else{后面剩的数据个数为0，那就什么都不用干}

        data_count-=remove_count;
        total_bytes-=remove_count*unit_bytes;
        return(true);
    }

    /**
     * 获取数据在合集中的索引
     */
    int64 Collection::indexOfCondition(CheckElement *condition) const
    {
        if(data_count<=0)return -1;
        if(!condition)return -2;

        const uchar *start=(uchar *)(memory_block->Get());
        const uchar *p=start;
        const uchar *ep=start+total_bytes;

        do
        {
            if(condition->Check(p))
                return((p-start)/unit_bytes);

            p+=unit_bytes;
        }
        while(p<ep);

        return -1;
    }

    /**
     * 按条件移除
     * @param condition 条件判断对象
     * @param max_count 最大移除个数
     */
    int64 Collection::RemoveCondition(CheckElement *condition,int max_count)
    {
        uint64 origin_count=data_count;

        uchar *p=(uchar *)end();
        uchar *ep;
        uchar *bp=(uchar *)begin();

        p-=unit_bytes;
        ep=p;

        do
        {
            if(condition->Check(p))
            {
                if(p==ep)     //就是最后一个不管了
                {
                }
                else
                {
                    memcpy(p,ep,unit_bytes);
                    ep-=unit_bytes;
                }

                --data_count;
                --max_count;
            }

            p-=unit_bytes;
        }
        while(p>=bp&&max_count);

        total_bytes=data_count*unit_bytes;

        return origin_count-data_count;
    }

    struct CheckElementInCollection:public CheckElement
    {
        const Collection *coll;
        CheckElement *check;

    public:

        CheckElementInCollection(const Collection *c,CheckElement *ce)
        {
            coll=c;
            check=ce;
        }

        const bool Check(const void *v) const override
        {
            check->Update(v);

            return coll->indexOfCondition(check)!=-1;
        }
    };//struct CheckElementInCollection:public CheckElement

    /**
     * 从合集中移除指定的数据
     * @param coll 要移除的数据合集
     * @param max_count 最大移除个数
     * @return 移除的数据总量
     */
    int64 Collection::RemoveCollection(const Collection &coll,CheckElement *ce,int64 max_count)
    {
        if(coll.GetCount()<=0)return 0;
        if(coll.GetUnitBytes()!=unit_bytes)return -1;

        CheckElementInCollection cec(&coll,ce);

        return RemoveCondition(&cec,max_count);
    }
}//namespace hgl
