#include<hgl/type/Collection.h>

namespace hgl
{
    Collection::Collection(const size_t ub,MemoryBlock *mb,ElementOperator *eo)
    {
        unit_bytes=ub;
        memory_block=mb;            
        data_count=0;
        total_bytes=0;

        if(ub>0)
            tmp_buffer=new uchar[unit_bytes];
        else
            tmp_buffer=nullptr;

        if(!eo)
        {
            eo_default=new ElementOperatorMemcmp(unit_bytes);
            element_operator=eo_default;
        }
        else
        {
            eo_default=nullptr;
            element_operator=eo;
        }
    }

    Collection::~Collection()
    {
        SAFE_CLEAR(eo_default);
        SAFE_CLEAR(tmp_buffer);
        SAFE_CLEAR(memory_block);
    }

    /**
     * Ԥ����ռ�
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
     * ����һ�����ݵ��ϼ�
     * @param element Ҫ���ӵ�Ԫ���ڴ�ָ��
     */
    bool Collection::Add(const void *element)
    {
        if(!memory_block)
            return(false);

        const uint64 write_offset=total_bytes;

        if(!memory_block->Alloc(total_bytes+unit_bytes))
            return(false);

        if(!memory_block->Write(total_bytes,&element,unit_bytes))
            return(false);

        ++data_count;
        total_bytes+=unit_bytes;
        return(true);
    }

    /**
     * ����һ�����ϼ�������
     */
    bool Collection::Add(const Collection &c)
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
     * ��ȡһ������
     * @param index ��������
     * @param element ���ݴ��ָ��
     * @return �Ƿ�ɹ�
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
     * ����һ������
     * @param index Ҫ���õ�λ������(�����������û�ʧ��)
     * @param element Ҫ���õ�����
     * @return �Ƿ�ɹ�
     */
    bool Collection::Set(const uint64 index,const void *element)
    {
        if(index>=data_count)return(false);
        if(!memory_block)return(false);

        memcpy(memory_block->Get(index*unit_bytes),&element,unit_bytes);

        return(true);
    }

    /**
     * ����һ�����ݵ��ϼ�
     * @param offset Ҫ�����ƫ�Ƶ�ַ
     * @param element Ҫ�����Ԫ��
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

        if(!memory_block->Write(offset*unit_bytes,&element,unit_bytes))
            return(false);

        ++data_count;
        total_bytes+=unit_bytes;
        return(true);
    }

    /**
     * �����������ݵ�λ��
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
     * ��������ϼ�(�����ͷ��ڴ�)
     */
    void Collection::Clear()
    {
        data_count=0;
        total_bytes=0;
    }

    /**
     * ӳ��һ��������������
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
     * ��������ϼ����ͷ��ڴ� 
     */
    void Collection::Free()
    {
        data_count=0;
        total_bytes=0;

        if(memory_block)
            memory_block->Free();
    }

    /**
     * �Ƴ�ָ��λ�õ�����
     * @param offset Ҫɾ��������ƫ��
     */
    bool Collection::RemoveAt(const uint64 offset)
    {
        if(!memory_block)
            return(false);

        if(offset>data_count)
            return(false);
                
        --data_count;
        total_bytes-=unit_bytes;

        //�����һ�������Ƶ����λ��
        memory_block->Copy( unit_bytes*offset,
                            unit_bytes*data_count,
                            unit_bytes);

        return(true);
    }

    /**
     * �Ƴ�ָ������������
     * @param start ��ʼ����
     * @param remove_count Ҫ�Ƴ������ݸ���
     */
    bool Collection::RemoveAt(const uint64 start,const uint64 remove_count)
    {
        if(remove_count==1)
            return RemoveAt(start);

        if(!memory_block)
            return(false);

        if(start+remove_count>data_count)
            return(false);

        const uint64 end_count=data_count-(start+remove_count);            //ɾ�������ݶκ�������ݸ���

        if(end_count>0)
        {
            if(end_count<=remove_count)                                    //����Ƚ��٣���ֱ�Ӹ��ƹ���������
            {
                //����ʣ�ĺ�ǰ��ɾ����һ���࣬���ƹ���    
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
                //����ʣ�ı�ǰ��յĶ࣬�����һ�εȳ��ĸ��ƹ���
                //[---][**********][***]
                //  ^                v
                //  |                |
                //  +----------------+

                memory_block->Copy( unit_bytes* start,
                                    unit_bytes*(data_count-remove_count),
                                    unit_bytes*remove_count);
            }
        }
        //else{����ʣ�����ݸ���Ϊ0���Ǿ�ʲô�����ø�}

        data_count-=remove_count;
        total_bytes-=remove_count*unit_bytes;
        return(true);
    }

    /**
     * ��ȡ�����ںϼ��е�����
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
     * �������Ƴ�
     * @param condition �����ж϶���
     * @param max_count ����Ƴ�����
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
                if(p==ep)     //�������һ��������
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

    /**
     * �Ӻϼ����Ƴ�ָ��������
     * @param coll Ҫ�Ƴ������ݺϼ�
     * @param max_count ����Ƴ�����
     * @return �Ƴ�����������
     */
    int64 Collection::Remove(const Collection &coll,const CheckElement *check,int64 max_count)
    {
        if(coll.GetCount()<=0)return 0;
        if(coll.GetUnitBytes()!=unit_bytes)return -1;

        CheckElementInCollection cec(&coll,check);

        return RemoveCondition(&cec,max_count);
    }
}//namespace hgl
