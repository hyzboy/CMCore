#ifndef HGL_LIST_CPP
#define HGL_LIST_CPP

#ifdef _WIN32
    #include <time.h>
#endif

#include<hgl/log/LogInfo.h>
#include<hgl/type/String.h>

//--------------------------------------------------------------------------------------------------
// 代码中的部分memcpy可替换为memmove,但这样会引起CodeGuard/Valgrind之类的内存调试器报错
//--------------------------------------------------------------------------------------------------
namespace hgl
{
    template<typename T>
    bool List<T>::Get(int index,T &ti)const
    {
        if(!items||index<0||index>=count)
            return(false);

        hgl_cpy<T>(ti,items[index]);
        return(true);
    }

    template<typename T>
    bool List<T>::First(T &ti)const
    {
        if(!items||count<=0)
            return(false);

        hgl_cpy<T>(ti,*items);
        return(true);
    }

    template<typename T>
    bool List<T>::Last(T &ti)const
    {
        if(!items||count<=0)
            return(false);

        hgl_cpy<T>(ti,items[count-1]);
        return(true);
    }

    template<typename T>
    bool List<T>::Set(int index,const T &val)
    {
        if(!items||index<0||index>=count)
            return(false);

        hgl_cpy<T>(items[index],val);//items[index]=val;

        return(true);
    }

    /**
     * 向列表中添加一个空数据
     * @return 这个数据的指针
     */
    template<typename T>
    T *List<T>::Add()
    {
        if(!PreAlloc(count+1))
            return(nullptr);

        ++count;
        return(items+(count-1));
    }

    /**
    * 向列表中添加一个数据对象
    * @param data 要添加的数据对象
    * @return 这个数据的索引号
    */
    template<typename T>
    int List<T>::Add(const T &data)
    {
        if(!PreAlloc(count+1))
            return(-1);

        hgl_cpy<T>(items[count],data);//items[count]=data;
        return(count++);
    }

    /**
    * 重复向列表中添加一个数据对象
    * @param data 要添加的数据对象
    * @param n 要添加的数据个数
    * @return 这个数据的索引号
    * @return -1 出错
    */
    template<typename T>
    int List<T>::RepeatAdd(const T &data,int n)
    {
        if(n<=0)return(-1);

        if(!PreAlloc(count+n))
            return(-2);

        T *p=items;
        int result=count;

        for(int i=0;i<n;i++)
        {
            hgl_cpy<T>(*p,data);//items[count]=data;
            ++p;
        }

        count+=n;
        return(result);
    }

    /**
    * 向列表中添加一批数据对象
    * @param data 要添加的数据对象
    * @param n 要添加的数据数量
    * @return 起始数据的索引号
    */
    template<typename T>
    int List<T>::Add(const T *data,int n)
    {
        if(!data||n<=0)
            return(-1);

        if(!PreAlloc(count+n))
            return(-1);

        hgl_cpy<T>(items+count,data,n);

        int r=count;

        count+=n;
        return(r);
    }

    /**
    * 清除整个列表
    */
    template<typename T>
    void List<T>::Clear()
    {
        count=0;
        alloc_count=0;

        if(items)
        {
            hgl_free(items);
            items=0;
        }
    }

    /**
    * 清除整个列表，但不清除缓冲区
    */
    template<typename T>
    void List<T>::ClearData()
    {
        count=0;
    }

    template<typename T>
    void List<T>::DeleteClear()
    {
        if(count)
        {
            T *p=items;

            while(count--)
            {
                delete *p;
                ++p;
            }

            count=0;
        }
    }

    /**
    * 在列表中查找指定的数据项
    * @param data 要查找的数据项
    * @return 数据在列表中的位置
    */
    template<typename T>
    int List<T>::Find(const T &data)const
    {
        int n=count;

        while(n--)
//          if(items[n]==data)return(n);
            if(memcmp(items+n,&data,sizeof(T))==0)
                return(n);

        return(-1);
    }

    /**
    * 删除列表中的指定项(关心顺序，删除中间数据后，会将后面的数据整体前移)
    * @param start 要删除的数据项的索引值
    * @return 是否成功
    */
    template<typename T>
    bool List<T>::DeleteMove(int start,int delete_count)
    {
        if(start>=count)return(false);

        if(start<0)
        {
            delete_count+=start;
            start=0;
        }

        if(start+delete_count>count)
            delete_count=count-start;

        if(delete_count<=0)return(false);

        const int end_count=count-(start+delete_count);

        if(end_count>0)
            hgl_cpy<T>(items+start,items+start+delete_count,end_count);

        count-=delete_count;

        return(true);
    }

    /**
    * 删除列表中的指定项(不关心顺序，如果删除中间的数据，可能会将最后面的数据拿过来填补)
    * @param start 要删除的数据项的索引起始值
    * @param delete_count 要删除的数据项数量
    * @return 是否成功
    */
    template<typename T>
    bool List<T>::Delete(int start,int delete_count)
    {
        if(start>=count)return(false);

        if(start<0)
        {
            delete_count+=start;
            start=0;
        }

        if(start+delete_count>count)
            delete_count=count-start;

        if(delete_count<=0)return(false);

        const int64 end_count=count-(start+delete_count);               //删除的数据段后面的数据个数

        if(end_count>0)
        {
            if(end_count<=delete_count)                                 //后面的数据个数比删除的数据个数少，直接整体移动
            {
                //[------------][***********]
                //      ^             v
                //      ^             v
                //      +<<<<<<<<<<<<<+

                hgl_cpy<T>(items+start,items+start+delete_count,end_count);
            }
            else                                                        //后面的数据个数比删除的数据个数多，那就只移动等长的最后一段数据
            {
                //[---][**********][***]
                //  ^                v
                //  ^                v
                //  +<<<<<<<<<<<<<<<<+

                hgl_cpy<T>(items+start,items+(count-delete_count),delete_count);
            }

            count-=delete_count;
        }
        //else{后面都没数据了，那就啥都不用干了}

        return(true);
    }

    /**
    * 删除列表中的指定项
    * @param data 要删除的数据项
    * @return 是否成功
    */
    template<typename T>
    bool List<T>::DeleteByValue(const T &dat)
    {
        int index=Find(dat);

        if(index!=-1)
        {
            Delete(index);
            return(true);
        }
        else
            return(false);
    }

    /**
    * 删除列表中的指定项
    * @param data 要删除的数据项
    * @param n 要删除的数据个数
    */
    template<typename T>
    void List<T>::DeleteByValue(const T *data,int n)
    {
        while(n--)
        {
            int index=Find(*data);

            ++data;

            if(index!=-1)
                Delete(index);
        }
    }

    /**
    * 交换列表中两个数据的位置
    * @param a 第一个数据的位置
    * @param b 第二个数据的位置
    */
    template<typename T>
    void List<T>::Exchange(int a,int b)
    {
        //T t;
        //char t[sizeof(T)];

//      t=items[a];

//      items[a]=items[b];
//      items[b]=t;

        hgl_swap(items[a],items[b]);
    }

    /**
    * 在列表的指定位置中插入一个数据
    * @param index 插入数据的位置
    * @param data 要插入的数据
    */
    template<typename T>
    bool List<T>::Insert(int index,const T &data)
    {
        if(index<0)index=0;

        if(index<count)
        {
            if(!PreAlloc(count+1))
                return(false);

            hgl_move<T>(items+index+1,items+index,count-index);

            hgl_cpy<T>(items[index],data);//items[index]=data;

            ++count;
        }
        else
            Add(data);

        return(true);
    }

    /**
    * 移动列表中的某一项到新的位置
    * @param index 要移动的数据位置
    * @param newindex 新的位置
    */
    template<typename T>
    void List<T>::Move(int index,int newindex)
    {
        if(index==newindex)return;
        if(index<=0||index>=count)return;
        if(newindex<0)newindex=0;
        if(newindex>=count)newindex=count-1;

        //T t;
        char t[sizeof(T)];

        hgl_cpy<T>(*(T *)&t,items[index]);//t=items[index];

        if(index<newindex)hgl_move<T>(items+index      ,items+index+1  ,newindex-index);
                     else hgl_move<T>(items+newindex+1 ,items+newindex ,index-newindex);

        hgl_cpy<T>(items[newindex],*(T *)&t);//items[newindex]=t;
    }

    template<typename T>
    bool List<T>::PreAlloc(int new_count)
    {
        if(alloc_count>=new_count)return(true);

        alloc_count=power_to_2(new_count);

        if(!items)
        {
            items=hgl_align_malloc<T>(alloc_count);

            return items;
        }
        else
        {
            T *new_items=hgl_align_realloc<T>(items,alloc_count);

            if(!new_items)return(false);

            items=new_items;
            return(true);
        }
    }

    template<typename T>
    bool List<T>::SetCount(int new_count)
    {
        if(count==new_count)return(true);

        if(new_count<=0)
        {
            ClearData();
            return(true);
        }

        if(!PreAlloc(new_count))
            return(false);

        count=new_count;
        return(true);
    }

    /**
    * 复制整个列表
    * @param lt 列表
    */
    template<typename T>
    void List<T>::operator = (const List<T> &lt)
    {
        if(lt.count<=0)
        {
            count=0;
            return;
        }

        SetCount(lt.count);

        hgl_cpy<T>(items,lt.items,count);
    }

    template<typename T>
    void List<T>::operator = (const std::initializer_list<T> &l)
    {
        ClearData();

        SetCount((int)l.size());

        hgl_cpy<T>(items,l.begin(),count);
    }
}//namespace hgl
#endif//HGL_LIST_CPP
