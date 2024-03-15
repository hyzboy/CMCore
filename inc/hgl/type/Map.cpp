#ifndef HGL_MAP_CPP
#define HGL_MAP_CPP

namespace hgl
{
    /**
    * 查找数据是否存在
    * @param flag 数据标识
    * @return 数据所在索引，-1表示不存在
    */
    template<typename K,typename V,typename KVData>
    int _Map<K,V,KVData>::Find(const K &flag)const
    {
        int left=0,right=data_list.GetCount()-1;                //使用left,right而不使用min,max是为了让代码能够更好的阅读。
        int mid;

        KVData **data_array=data_list.GetData();

        while(left<=right)
        {
            if(data_array[left ]->key==flag)return(left);
            if(data_array[right]->key==flag)return(right);

            mid=(right+left)>>1;

            if(data_array[mid]->key==flag)return(mid);

            if(data_array[mid]->key>flag)
            {
                ++left;
                right=mid-1;
            }
            else
            {
                --right;
                left=mid+1;
            }
        }

//      LOG_PROBLEM(OS_TEXT("Map::Find,no result."));
        return(-1);
    }

    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::FindPos(const K &flag,int &pos)const
    {
        int left=0,right=data_list.GetCount()-1;
        int mid;

        KVData **data_array=data_list.GetData();

        while(left<=right)
        {
            if(data_array[left ]->key>flag)
            {
                pos=left;
                return(false);
            }
            else
            if(data_array[left ]->key==flag)
            {
                pos=left;
                return(true);
            }

            if(data_array[right]->key<flag)
            {
                pos=right+1;
                return(false);
            }
            else
            if(data_array[right]->key==flag)
            {
                pos=right;
                return(true);
            }

            mid=(right+left)>>1;

            if(data_array[mid]->key==flag)
            {
                pos=mid;
                return(true);
            }

            if(data_array[mid]->key>flag)
            {
                if(data_array[mid-1]->key<flag)
                {
                    pos=mid;
                    return(false);
                }
                else
                if(data_array[mid-1]->key==flag)
                {
                    pos=mid-1;
                    return(true);
                }

                ++left;
                right=mid-1;
            }
            else
            {
                if(data_array[mid+1]->key>flag)
                {
                    pos=mid+1;
                    return(false);
                }
                else
                if(data_array[mid+1]->key==flag)
                {
                    pos=mid+1;
                    return(true);
                }

                --right;
                left=mid+1;
            }
        }

//      LOG_PROBLEM(OS_TEXT("Map::FindPos,no result."));
        pos=-1;
        return(false);
    }

    template<typename K,typename V,typename KVData>
    int _Map<K,V,KVData>::FindByValue(const V &data)const
    {
        const int count=data_list.GetCount();

        KVData **data_array=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            //if((*data_array)->value==data)
            if(hgl_cmp((*data_array)->value,data)==0)
                return(i);

            ++data_array;
        }

        return -1;
    }

    /**
    * 添加一个数据
    * @param flag 数据标识
    * @param data 数据
    * @return 新创建好的数据结构
    */
    template<typename K,typename V,typename KVData>
    KVData *_Map<K,V,KVData>::Add(const K &flag,const V &data)
    {
        KVData *dp=new KVData;
        
        dp->key=flag;
        dp->value=data;

        if(!data_pool.AppendToActive(dp))
            return(nullptr);

        int pos;

        if(FindPos(flag,pos))
            return(nullptr);

        data_list.Insert(pos,dp);

        return(dp);
    }

    /**
    * 添加一个数据
    * @param obj 数据
    */
    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::Add(KVData *obj)
    {
        data_list.Insert(FindPos(obj->key),obj);
    }

    /**
    * 根据索引取得指定数据的指针<Br>
    * 比如定义了Map<int ID,struct DATA>，直接返回DATA需要复制会消耗一些时间，直接返回DATA *会更好一些
    */
    template<typename K,typename V,typename KVData>
    V *_Map<K,V,KVData>::GetPointer(const K &key)const
    {
        int index=Find(key);

        if(index==-1)
            return(nullptr);

        KVData *obj=GetListObject(data_list,index);

        if(!obj)return(nullptr);
        return &(obj->value);
    }

    /**
    * 根据索引取得数据与序号
    * @param flag 数据索引
    * @param data 数据存放处
    * @return 数据序号,<0表示失败
    */
    template<typename K,typename V,typename KVData>
    int _Map<K,V,KVData>::GetValueAndSerial(const K &flag,V &data) const
    {
        int index=Find(flag);

        KVData *obj=GetListObject(data_list,index);

        if(!obj)
            return(-1);

        data=obj->value;

        return(index);
    }

    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::Check(const K &key,const V &value) const
    {
        int index=Find(key);

        KVData *obj=GetListObject(data_list,index);

        if(!obj)
            return(false);

        return (value==obj->value);
    }

    /**
    * 根据序号取得数据
    * @param index 序号
    * @param f 数据索引存放处
    * @param t 数据存放处
    * @return 是否取得成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::GetBySerial(int index,K &f,V &t) const
    {
        if(index<0||index>=data_list.GetCount())return(false);
        
        KVData *ds;

        if(!data_list.Get(index,ds))
            return(false);

        f=ds->key;
        t=ds->value;

        return(true);
    }

    /**
    * 根据序号取得索引
    * @param index 序号
    * @param f 数据索引存放处
    * @return 是否取得成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::GetKey(int index,K &f)
    {
        if(index<0||index>=data_list.GetCount())return(false);
        
        KVData *ds;

        if(!data_list.Get(index,ds))
            return(false);

        f=ds->key;

        return(true);
    }

    /**
    * 根据序号取得数据
    * @param index 序号
    * @param t 数据存放处
    * @return 是否取得成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::GetValue(int index,V &t)
    {
        if(index<0||index>=data_list.GetCount())return(false);
        
        KVData *ds;

        if(!data_list.Get(index,ds))
            return(false);

        t=ds->value;

        return(true);
    }

    /**
     * 根据序号设置数据
     * @param index 数据序号
     * @param t 数据
     */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::SetValueBySerial(int index,V &t)
    {
        if(index<0||index>=data_list.GetCount())return(false);

        data_list[index]->value=t;

        return(true);
    }

    /**
    * 将指定数据从列表中移除同时取得这个数据
    * @param flag 数据标识
    * @param data 数据存放位处
    * @return 是否成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::Delete(const K &flag,V &data)
    {
        int index=Find(flag);

        KVData *dp=GetListObject(data_list,index);

        if(!dp)
            return(false);

        data=dp->value;

        data_pool.Release(dp);
        data_list.DeleteMove(index);

        return(true);
    }

    /**
    * 根据索引将指定数据从列表中移除
    * @param flag 索引
    * @return 是否成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::DeleteByKey(const K &flag)
    {
        return DeleteAt(Find(flag));
    }

    /**
    * 根据索引将指定数据从列表中批量移除
    * @param fp 索引列表
    * @param count 索引数量
    * @return 是否成功
    */
    template<typename K,typename V,typename KVData>
    int _Map<K,V,KVData>::DeleteByKey(const K *fp,const int count)
    {
        if(!fp||count<=0)return(0);

        int total=0;

        for(int i=0;i<count;i++)
        {
            if(DeleteAt(Find(*fp)))
                ++total;

            ++fp;
        }

        return(total);
    }


    /**
    * 根据数据将指定数据从列表中移除
    * @param data 数据
    * @return 是否成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::DeleteByValue(const V &data)
    {
        return DeleteAt(FindByValue(data));
    }

    /**
    * 根据序号将指定数据从列表中移除
    * @param index 序号
    * @return 是否成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::DeleteAt(int index)
    {
        if(index<0
         ||index>=data_list.GetCount())return(false);

        data_pool.Release(GetListObject(data_list,index));
        data_list.DeleteMove(index);

        return(true);
    }

    /**
     * 根据序号将指定数据从列表中移除
     * @param start 起始序号
     * @param number 数量
     * @return 是否成功
     */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::DeleteAt(int start,int number)
    {
        KVData **dp=data_list.GetData()+start;

        for(int i=0;i<number;i++)
        {
            data_pool.Release(*dp);
            ++dp;
        }

        return data_list.Delete(start,number);
    }

    /**
     * 更新一个数据，如果这个数据不存在，就添加一个新的
     * @param flag 数据标识
     * @param data 新的数据内容
     */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::ChangeOrAdd(const K &flag,const V &data)
    {
        int result;
        KVData *dp;

        if(FindPos(flag,result))
        {
            dp=GetListObject(data_list,result);

            if(dp)
            {
                dp->value=data;
                return(true);
            }
        }
        else
        {
            if(!data_pool.GetOrCreate(dp))
            {
                dp=new KVData;

                data_pool.AppendToActive(dp);
            }

            dp->key=flag;
            dp->value=data;

            data_list.Insert(result,dp);
            return(true);
        }

        return(false);
    }

    /**
    * 更改数据,这个更改和Set不同,它要求指定标识的数据必须存在，则否就会更改失败
    * @param flag 数据标识
    * @param data 新的数据内容
    * @param return 是否更改成功
    */
    template<typename K,typename V,typename KVData>
    bool _Map<K,V,KVData>::Change(const K &flag,const V &data)
    {
        KVData *dp=GetListObject(data_list,Find(flag));

        if(!dp)
            return(false);

        dp->value=data;
        return(true);
    }

    /**
    * 清除所有数据
    */
    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::Free()
    {
        data_pool.Clear();
        data_list.Free();
    }

    /**
    * 清除所有数据，但不释放内存
    */
    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::Clear()
    {
        data_pool.ReleaseActive();
        data_list.Clear();
    }

    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::operator=(const ThisClass &ftd)
    {
        Free();

        data_pool.Clear();
        data_list.Clear();

        const int count=ftd.data_list.GetCount();

        if(count<=0)
            return;

        KVData **obj=ftd.data_list.GetData();
        KVData *new_obj;

        for(int i=0;i<count;i++)
        {
            if(!data_pool.Acquire(new_obj))
                break;

            new_obj->key=(*obj)->key;
            new_obj->value=(*obj)->value;

            data_list.Add(new_obj);

            ++obj;
        }
    }

    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::Enum(void (*enum_func)(const K &,V &))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        KVData **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            enum_func((*idp)->key,(*idp)->value);

            ++idp;
        }
    }

    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::EnumKey(void (*enum_func)(const K &))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        KVData **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            enum_func((*idp)->key);

            ++idp;
        }
    }

    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::EnumAllValue(void (*enum_func)(V &))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        KVData **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            enum_func((*idp)->value);

            ++idp;
        }
    }

    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::EnumValue(bool (*enum_func)(V &))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        KVData **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            if(!enum_func((*idp)->value))
                return;

            ++idp;
        }
    }

    /**
     * 统计出所有在in_list中出现的数据，产生的结果写入with_list
     */
    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::WithList(_Map<K,V,KVData>::KVDataList &with_list,const List<K> &in_list)
    {
        with_list.Clear();
        const int count=this->GetCount();

        if(count<=0)return;

        with_list.PreAlloc(count);

        const KVData *sp=this->GetDataList();

        for(int i=0;i<count;i++)
        {
            if(in_list.IsExist(*sp))
                with_list.Add(*sp);

            ++sp;
        }
    }
    
    /**
     * 统计出所有没有出现在in_list中的数据，产生的结果写入without_list
     */
    template<typename K,typename V,typename KVData>
    void _Map<K,V,KVData>::WithoutList(_Map<K,V,KVData>::KVDataList &without_list,const List<K> &in_list)
    {
        without_list.Clear();
        const int count=this->GetCount();

        if(count<=0)return;

        without_list.PreAlloc(count);

        const KVData *sp=this->GetDataList();

        for(int i=0;i<count;i++)
        {
            if(!in_list.IsExist(*sp))
                without_list.Add(*sp);

            ++sp;
        }
    }
}//namespace hgl
#endif//HGL_MAP_CPP
