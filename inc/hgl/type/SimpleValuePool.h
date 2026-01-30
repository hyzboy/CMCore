#pragma once

#include<vector>
#include<hgl/type/SeriesPool.h>

namespace hgl
{
    /**
     * 数据堆栈池(一种简单的数据池结构，不支持动态调整大小)<br>
     * 仅用于一些只有申请和释放操作的情况，感觉没什么用处，但又经常需要。
     */
    template<typename T> class SimpleValuePool
    {
    protected:

        T *data_array;                      ///<数据区
        T *end;                             ///<结束指针
        SeriesInt series;                   ///<序号池

    public:

        T *GetRawData()const{return data_array;}                    ///<取得原始数据指针
        SeriesInt *GetSeries(){return &series;}                     ///<取得序号池

    public:

        SimpleValuePool()
        {
            data_array=nullptr;
            end=nullptr;
        }

        bool Init(const int max_count)
        {
            data_array=zero_new<T>(max_count);

            end=data_array+max_count;

            return series.Init(max_count);
        }

        ~SimpleValuePool()
        {
            delete[] data_array;
        }

        T *Acquire()                       ///<请求一个数据
        {
            int pos;

            if(!series.Acquire(&pos))
                return nullptr;

            return data_array+pos;
        }

        bool Release(T *obj)                ///<释放一个数据
        {
            if(!obj)
                return(false);

            if(obj<data_array||obj>=end)
                return(false);

            const int pos=obj-data_array;

            return series.Release(pos);
        }
    };//template<typename T> class SimpleValuePool
}//namespace
