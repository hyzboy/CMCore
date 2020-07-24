#ifndef HGL_STACK_CPP
#define HGL_STACK_CPP

#include<hgl/log/LogInfo.h>
namespace hgl
{
    /**
    * 本类构造函数
    * @param m 如果m的值不为0，则表示使用固定的堆栈大小。固定大小的堆栈会在一开始即分配好内存。
    */
    template<typename T>
    Stack<T>::Stack(int m)
    {
        cur_count=0;

        if(m)
        {
            max_count=m;

            items=hgl_aligned_malloc<T>(max_count);
        }
        else
            max_count=0;

        alloc_count=max_count;
    }

    template<typename T>
    Stack<T>::~Stack()
    {
        if(cur_count||max_count)hgl_free(items);
    }

    /**
    * 修改堆栈的最大值
    */
    template<typename T>
    void Stack<T>::SetMaxCount(int m)
    {
        if(m<=0)return;

        if(alloc_count==0)
        {
            alloc_count=power_to_2(m);
            items=hgl_aligned_malloc<T>(alloc_count);
        }
        else
        if(alloc_count<m)
        {
            alloc_count=power_to_2(m);
            items=(T *)hgl_realloc(items,alloc_count*sizeof(T));
        }

        max_count=m;

        if(cur_count>=max_count)cur_count=max_count-1;
    }

    template<typename T>
    bool Stack<T>::SetCount(int c)
    {
        if(c<0)return(false);

        if(c>max_count)
            return(false);

        cur_count=c;
        return(true);
    }

    /**
    * 清除堆栈中的所有数据
    */
    template<typename T>
    void Stack<T>::Clear()
    {
        if(max_count==0)
            if(cur_count)
            {
                hgl_free(items);
                alloc_count=0;
            }

        cur_count=0;
    }

    template<typename T>
    bool Stack<T>::GetItem(int n,T &data)
    {
        if(n<0||n>=cur_count)
        {
            LOG_ERROR(OS_TEXT("从堆栈中按索引<") + OSString(n) + OS_TEXT(">取数据，超出正常范围<")+OSString(cur_count) + OS_TEXT(">"));

            return(false);
        }

        data=items[n];
        return(true);
    }

    /**
    * 访问堆栈中的一个数据，但不清除它
    * @param t 取出的数据保存地
    * @return 是否成功取出数据
    */
    template<typename T>
    bool Stack<T>::Peek(T &t)
    {
        if(cur_count)
        {
            memcpy(&t,items+(cur_count-1),sizeof(T));
//          t=items[cur_count-1];
            return(true);
        }
        else
            return(false);
    }

    /**
    * 从堆栈中取出一个数据
    * @param t 取出的数据保存地
    * @return 是否成功取出数据
    */
    template<typename T>
    bool Stack<T>::Pop(T &t)
    {
        if(cur_count)
        {
//          t=items[--cur_count];
            cur_count--;
            memcpy(&t,items+cur_count,sizeof(T));

            if(max_count==0)
            {
                if(cur_count==0)
                {
                    hgl_free(items);

                    alloc_count=0;
                }
                //else
                    //items=(T *)hgl_realloc(items,cur_count*sizeof(T));
            }

            return(true);
        }
        else
            return(false);
    }

    /**
    * 向堆栈中放入一个数据
    * @param data 要放入的数据指针
    * @return true 放入数据成功
    * @return false 放入数据失败
    */
    template<typename T>
    bool Stack<T>::Push(T &data)
    {
        if(max_count)
        {
            if(cur_count>=max_count)return(false);
        }
        else
        {
            if(cur_count)
            {
                if(cur_count+1>alloc_count)
                {
                    alloc_count=power_to_2(cur_count+1);

                    items=(T *)hgl_realloc(items,alloc_count*sizeof(T));
                }
            }
            else
            {
                items=hgl_aligned_malloc<T>(1);

                alloc_count=1;
            }
        }

//      items[cur_count++]=data;
        memcpy(items+cur_count,&data,sizeof(T));
        cur_count++;

        return(true);
    }

    /**
    * 向堆栈中放入多个数据
    * @param data 要放入的数据
    * @param data_count 要放入的数据个数
    * @return true 放入数据成功
    * @return false 放入数据失败
    */
    template<typename T>
    bool Stack<T>::Push(T *data,int data_count)
    {
        if(max_count)
        {
            if(cur_count>=max_count)return(false);
        }
        else
        {
            if(cur_count)
            {
                if(cur_count+data_count>alloc_count)
                {
                    alloc_count=power_to_2(cur_count+data_count);

                    items=(T *)hgl_realloc(items,alloc_count*sizeof(T));
                }
            }
            else
            {
                items=hgl_aligned_malloc<T>(data_count);

                alloc_count=data_count;
            }
        }

        memcpy(items+cur_count,data,data_count*sizeof(T));
        cur_count+=data_count;

        return(true);
    }

    template<typename T>
    void Stack<T>::operator =(const Stack<T> &ori)
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
}//namespace hgl

namespace hgl
{
    template<typename T>
    void StackObject<T>::Clear()
    {
        for(int i=0;i<this->cur_count;i++)
            delete this->items[i];

        Stack<T *>::Clear();
    }
}//namespace hgl
#endif//HGL_STACK_CPP
