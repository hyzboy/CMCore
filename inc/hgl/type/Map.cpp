#ifndef HGL_MAP_CPP
#define HGL_MAP_CPP

namespace hgl
{
    /**
    * 查找数据是否存在
    * @param flag 数据标识
    * @return 数据所在索引，-1表示不存在
    */
    template<typename K,typename V,typename DataPair>
    int _Map<K,V,DataPair>::Find(const K &flag)const
    {
        int left=0,right=data_list.GetCount()-1;                //使用left,right而不使用min,max是为了让代码能够更好的阅读。
        int mid;

        DataPair **data_array=data_list.GetData();

        while(left<=right)
        {
            if(data_array[left ]->left==flag)return(left);
            if(data_array[right]->left==flag)return(right);

            mid=(right+left)>>1;

            if(data_array[mid]->left==flag)return(mid);

            if(data_array[mid]->left>flag)
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

    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::FindPos(const K &flag,int &pos)const
    {
        int left=0,right=data_list.GetCount()-1;
        int mid;

        DataPair **data_array=data_list.GetData();

        while(left<=right)
        {
            if(data_array[left ]->left>flag)
            {
                pos=left;
                return(false);
            }
            else
            if(data_array[left ]->left==flag)
            {
                pos=left;
                return(true);
            }

            if(data_array[right]->left<flag)
            {
                pos=right+1;
                return(false);
            }
            else
            if(data_array[right]->left==flag)
            {
                pos=right;
                return(true);
            }

            mid=(right+left)>>1;

            if(data_array[mid]->left==flag)
            {
                pos=mid;
                return(true);
            }

            if(data_array[mid]->left>flag)
            {
                if(data_array[mid-1]->left<flag)
                {
                    pos=mid;
                    return(false);
                }
                else
                if(data_array[mid-1]->left==flag)
                {
                    pos=mid-1;
                    return(true);
                }

                ++left;
                right=mid-1;
            }
            else
            {
                if(data_array[mid+1]->left>flag)
                {
                    pos=mid+1;
                    return(false);
                }
                else
                if(data_array[mid+1]->left==flag)
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

    template<typename K,typename V,typename DataPair>
    int _Map<K,V,DataPair>::FindByValue(const V &data)const
    {
        const int count=data_list.GetCount();

        DataPair **data_array=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            if((*data_array)->right==data)
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
    template<typename K,typename V,typename DataPair>
    DataPair *_Map<K,V,DataPair>::Add(const K &flag,const V &data)
    {
        DataPair *dp;
        
        if(!data_pool.Acquire(dp))
            return(nullptr);

        dp->left=flag;
        dp->right=data;

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
    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::Add(DataPair *obj)
    {
        data_list.Insert(FindPos(obj->left),obj);
    }

    /**
    * 根据索引取得数据与序号
    * @param flag 数据索引
    * @param data 数据存放处
    * @return 数据序号,<0表示失败
    */
    template<typename K,typename V,typename DataPair>
    int _Map<K,V,DataPair>::GetValueAndSerial(const K &flag,V &data) const
    {
        int index=Find(flag);

        DataPair *obj=GetListObject(data_list,index);

        if(!obj)
            return(-1);

        data=obj->right;

        return(index);
    }

    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::Check(const K &key,const V &value) const
    {
        int index=Find(key);

        DataPair *obj=GetListObject(data_list,index);

        if(!obj)
            return(false);

        return (value==obj->right);
    }

    /**
    * 根据序号取得数据
    * @param index 序号
    * @param f 数据索引存放处
    * @param t 数据存放处
    * @return 是否取得成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::GetBySerial(int index,K &f,V &t) const
    {
        if(index<0||index>=data_list.GetCount())return(false);

        DataPair *ds=data_list[index];

        f=ds->left;
        t=ds->right;

        return(true);
    }

    /**
    * 根据序号取得索引
    * @param index 序号
    * @param f 数据索引存放处
    * @return 是否取得成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::GetKey(int index,K &f)
    {
        if(index<0||index>=data_list.GetCount())return(false);

        DataPair *ds=data_list[index];

        f=ds->left;

        return(true);
    }

    /**
    * 根据序号取得数据
    * @param index 序号
    * @param t 数据存放处
    * @return 是否取得成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::GetValue(int index,V &t)
    {
        if(index<0||index>=data_list.GetCount())return(false);

        DataPair *ds=data_list[index];

        if(!ds)return(false);

        t=ds->right;

        return(true);
    }

    /**
     * 根据序号设置数据
     * @param index 数据序号
     * @param t 数据
     */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::SetValueBySerial(int index,V &t)
    {
        if(index<0||index>=data_list.GetCount())return(false);

        data_list[index]->right=t;

        return(true);
    }

    /**
    * 将指定数据从列表中移除同时取得这个数据
    * @param flag 数据标识
    * @param data 数据存放位处
    * @return 是否成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::Delete(const K &flag,V &data)
    {
        int index=Find(flag);

        DataPair *dp=GetListObject(data_list,index);

        if(!dp)
            return(false);

        data=dp->right;

        data_pool.Release(dp);
        data_list.DeleteMove(index);

        return(true);
    }

    /**
    * 根据索引将指定数据从列表中移除
    * @param flag 索引
    * @return 是否成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::DeleteByKey(const K &flag)
    {
        return DeleteBySerial(Find(flag));
    }

    /**
    * 根据索引将指定数据从列表中批量移除
    * @param fp 索引列表
    * @param count 索引数量
    * @return 是否成功
    */
    template<typename K,typename V,typename DataPair>
    int _Map<K,V,DataPair>::DeleteByKey(const K *fp,const int count)
    {
        if(!fp||count<=0)return(0);

        int total=0;

        for(int i=0;i<count;i++)
        {
            if(DeleteBySerial(Find(*fp)))
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
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::DeleteByValue(const V &data)
    {
        return DeleteBySerial(FindByValue(data));
    }

    /**
    * 根据序号将指定数据从列表中移除
    * @param index 序号
    * @return 是否成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::DeleteBySerial(int index)
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
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::DeleteBySerial(int start,int number)
    {
        DataPair **dp=data_list.GetData()+start;

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
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::Update(const K &flag,const V &data)
    {
        int result;
        DataPair *dp;

        if(FindPos(flag,result))
        {
            dp=GetListObject(data_list,result);

            if(dp)
            {
                dp->right=data;
                return(true);
            }
        }
        else
        {
            if(data_pool.Acquire(dp))
            {
                dp->left=flag;
                dp->right=data;

                data_list.Insert(result,dp);
                return(true);
            }
        }

        return(false);
    }

    /**
    * 更改数据,这个更改和Set不同,它要求指定标识的数据必须存在，则否就会更改失败
    * @param flag 数据标识
    * @param data 新的数据内容
    * @param return 是否更改成功
    */
    template<typename K,typename V,typename DataPair>
    bool _Map<K,V,DataPair>::Change(const K &flag,const V &data)
    {
        DataPair *dp=GetListObject(data_list,Find(flag));

        if(!dp)
            return(false);

        dp->right=data;
        return(true);
    }

    /**
    * 清除所有数据
    */
    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::Clear()
    {
        data_pool.ClearAll();
        data_list.Clear();
    }

    /**
    * 清除所有数据，但不释放内存
    */
    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::ClearData()
    {
        data_pool.ReleaseAll();
        data_list.ClearData();
    }

    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::operator=(const ThisClass &ftd)
    {
        Clear();

        data_pool.ClearAll();
        data_list.ClearData();

        const int count=ftd.data_list.GetCount();

        if(count<=0)
            return;

        DataPair **obj=ftd.data_list.GetData();
        DataPair *new_obj;

        for(int i=0;i<count;i++)
        {
            if(!data_pool.Acquire(new_obj))
                break;

            new_obj->left=(*obj)->left;
            new_obj->right=(*obj)->right;

            data_list.Add(new_obj);

            ++obj;
        }
    }

    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::Enum(void (*enum_func)(const K &,V))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        DataPair **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            enum_func((*idp)->left,(*idp)->right);

            ++idp;
        }
    }

    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::EnumKey(void (*enum_func)(const K &))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        DataPair **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            enum_func((*idp)->left);

            ++idp;
        }
    }

    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::EnumAllValue(void (*enum_func)(V))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        DataPair **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            enum_func((*idp)->right);

            ++idp;
        }
    }

    template<typename K,typename V,typename DataPair>
    void _Map<K,V,DataPair>::EnumValue(bool (*enum_func)(V))
    {
        const int count=data_list.GetCount();

        if(count<=0)
            return;

        DataPair **idp=data_list.GetData();

        for(int i=0;i<count;i++)
        {
            if(!enum_func((*idp)->right))
                return;

            ++idp;
        }
    }
}//namespace hgl
#endif//HGL_MAP_CPP
