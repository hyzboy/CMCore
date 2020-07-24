#ifndef HGL_POOL_CPP
#define HGL_POOL_CPP

#include<hgl/type/DataType.h>
#include<hgl/type/Pool.h>
namespace hgl
{
    template<typename T>
    void Pool<T>::PreMalloc(int num,bool set_to_max_count)
    {
        if(num<=0)return;

        for(int i=0;i<num;i++)
            Inactive.Push(Create());

        alloc_count+=num;
        if(alloc_count>history_max)
            history_max=alloc_count;

        if(set_to_max_count)
            max_count=alloc_count;
    }
    
    template<typename T>
    int Pool<T>::SetMaxCount(int mc)
    {
        if(mc<0)
            return(mc);

        if(mc==0)
        {
            max_count=0;
            return 0;
        }

        max_count=mc;
        return mc;
    }

    template<typename T>
    bool Pool<T>::Acquire(T &value)
    {
        if(!Inactive.Pop(value))
        {
            if(max_count>0&&alloc_count>=max_count)
                return(false);

            value=Create();

            alloc_count++;

            if(alloc_count>history_max)
                history_max=alloc_count;
        }

        Active.Add(value);
        return(true);
    }

    template<typename T>
    bool Pool<T>::Get(T &value)
    {
        if(!Inactive.Pop(value))
            return(false);

        Active.Add(value);
        return(true);
    }

    template<typename T>
    void Pool<T>::Append(T value)
    {
        T result;

        if(!Inactive.Pop(result))
        {
            alloc_count++;

            if(alloc_count>history_max)
                history_max=alloc_count;
        }

        Active.Add(result);

        result=value;
    }

    template<typename T>
    bool Pool<T>::Release(T value)
    {
        int index=Active.Find(value);

        if(index!=-1)
        {
            Active.Delete(index);

            Inactive.Push(value);

            return(true);
        }

        return(false);
    }

    template<typename T>
    int Pool<T>::Release(T *vl,int alloc_count)
    {
        int total=0;

        for(int i=0;i<alloc_count;i++)
        {
            if(Release(*vl))
                ++total;

            ++vl;
        }

        return(total);
    }

    template<typename T>
    int Pool<T>::ReleaseAll()
    {
        int alloc_count=Active.GetCount();

        Inactive.Push(Active.GetData(),alloc_count);

        Active.ClearData();
        return(alloc_count);
    }

    template<typename T>
    void Pool<T>::Clear(T *dp,int dc)
    {
        for(int i=0;i<dc;i++)
        {
            Clear(*dp);
            ++dp;
        }
    }

    template<typename T>
    void Pool<T>::ClearActive()
    {
        T *p=Active.GetData();
        int ac=Active.GetCount();

        Inactive.Push(p,ac);

        Active.ClearData();
    }

    template<typename T>
    void Pool<T>::ClearInactive()
    {
        T *p=Inactive.GetData();
        int ic=Inactive.GetCount();

        alloc_count-=ic;

        Clear(p,ic);

        Inactive.ClearData();
    }

    template<typename T>
    void Pool<T>::ClearAll()
    {
        Clear(Inactive.GetData(),Inactive.GetCount());
        Inactive.ClearData();

        Clear(Active.GetData(),Active.GetCount());
        Active.ClearData();

        alloc_count=0;
    }
}//namespace hgl
#endif//HGL_POOL_CPP
