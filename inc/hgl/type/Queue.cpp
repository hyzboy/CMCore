#ifndef HGL_QUEUE_CPP
#define HGL_QUEUE_CPP

namespace hgl
{
    /**
    * 本类构造函数
    * @param m 如果m的值不为0，则表示使用固定的队列大小。固定大小的队列会在一开始即分配好内存。
    */
    template<typename T>
    Queue<T>::Queue(int m)
    {
        cur_count=0;

        if(m)
        {
            max_count=m;

            items=hgl_aligned_malloc<T>(max_count);
        }
        else max_count=0;

        alloc_count=max_count;
    }

    template<typename T>
    Queue<T>::~Queue()
    {
        if(cur_count||max_count)hgl_free(items);
    }
    
    template<typename T>
    bool Queue<T>::AllocMemory(int count)
    {
        if(max_count)
            if(count>max_count)return(false);

        if(cur_count)
        {
            if(count>alloc_count)
            {
                alloc_count=power_to_2(count);

                items=(T *)hgl_realloc(items,alloc_count*sizeof(T));
            }
        }
        else
        {
            alloc_count=power_to_2(count);
            
            items=hgl_aligned_malloc<T>(alloc_count);
        }

        return(true);
    }

    /**
    * 修改队列的最大值
    */
    template<typename T>
    bool Queue<T>::SetMaxCount(int m)
    {
        if(max_count)
        {
            if(max_count==m)return(true);
        }
        
        if(cur_count)
        {
            if(cur_count>m)
                return(false);
        }

        max_count=m;
        return(true);
    }

    /**
    * 清除队列中的所有数据
    */
    template<typename T>
    void Queue<T>::Clear()
    {
        if(max_count==0)
            if(cur_count)
            {
                hgl_free(items);
                alloc_count=0;
            }

        cur_count=0;
    }

    /**
    * 清除队列中的所有数据，但不释放内存
    */
    template<typename T>
    void Queue<T>::ClearData()
    {
        cur_count=0;
    }

    /**
    * 访问队列中的一个数据，但不清除它
    * @param t 取出的数据保存地
    * @return 是否成功取出数据
    */
    template<typename T>
    bool Queue<T>::Peek(T &t)
    {
        if(cur_count)
        {
//          t=items[0];
            memcpy(&t,items,sizeof(T));
            return(true);
        }
        else
            return(false);
    }

    /**
     * 删除队列中的指定数据
     * @param index 索引
     */
    template<typename T>
    bool Queue<T>::Delete(int index)
    {
        if(index<0||index>=cur_count)
            return(false);

        cur_count--;

        if(cur_count)
        {
            if(index<cur_count)
                memmove(items+index,items+index+1,(cur_count-index)*sizeof(T));
        }

        return(true);
    }

    /**
    * 从队列中取出一个数据
    * @param t 取出的数据保存地
    * @return 是否成功取出数据
    */
    template<typename T>
    bool Queue<T>::Pop(T &t)
    {
        if(cur_count)
        {
//          t=items[0];
            memcpy(&t,items,sizeof(T));

            cur_count--;

            if(max_count==0)
            {
                if(cur_count)
                {
                    //memcpy(items,items+1,count*sizeof(T));
                    memmove(items,items+1,cur_count*sizeof(T));
//                    items=(T *)hgl_realloc(items,count*sizeof(T));
                }
            }
            else
            {
                memcpy(items,items+1,cur_count*sizeof(T));
            }

            return(true);
        }
        else
            return(false);
    }

    /**
    * 向队列中放入一个数据
    * @param data 要放入的数据指针
    * @return true 放入数据成功
    * @return false 放入数据失败
    */
    template<typename T>
    bool Queue<T>::Push(const T &data)
    {
        if(!AllocMemory(cur_count+1))
            return(false);

//      items[count++]=data;
        memcpy(items+cur_count,&data,sizeof(T));
        cur_count++;

        return(true);
    }

    /**
     * 向队列中压入一堆数据
     * @param dp 要放入的数据指针
     * @param dc 要放入的数据数量
     */
    template<typename T>
    bool Queue<T>::Push(T *dp,const int dc)
    {
        if(!AllocMemory(cur_count+dc))
            return(false);

        memcpy(items+cur_count,dp,sizeof(T)*dc);
        cur_count+=dc;

        return(true);
    }

    template<typename T>
    int Queue<T>::Find(const T &data)
    {
        if(cur_count<=0)
            return(-1);

        T *p=items;
        for(int i=0;i<cur_count;i++)
        {
            if(*p==data)
                return(i);

            ++p;
        }

        return(-1);
    }

    template<typename T>
    void Queue<T>::operator =(const Queue<T> &ori)
    {
        if(ori.cur_count==0)return;

        Clear();

        max_count=ori.cur_count;
        cur_count=ori.cur_count;

        if(max_count==0)
            alloc_count=cur_count;
        else
            alloc_count=max_count;

        items=hgl_aligned_malloc<T>(alloc_count);

        memcpy(items,ori.items,alloc_count*sizeof(T));
    }
}

namespace hgl
{
    template<typename T>
    void QueueObject<T>::Clear()
    {
        int n=Queue<T *>::cur_count;

        while(n--)
            delete Queue<T *>::items[n];

        Queue<T *>::Clear();
    }
}
#endif//HGL_QUEUE_CPP
