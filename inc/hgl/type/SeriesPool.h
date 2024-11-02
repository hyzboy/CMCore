#pragma once
#include<hgl/type/SortedSet.h>
#include<random>

namespace hgl
{
    /**
     * 序号池<br>
     * 没什么用，就是一个序号堆栈而已。
     */
    template<typename T> class SeriesPool
    {
        //一定要用带BOM头的UTF8存哦！“量”字在一行的最后，会导致编译器报错，认不出来换行符。

        T max_count;                        ///<最大数量
        T *series_data;                     ///<序列数据
        T *end;                             ///<结束指针
        T *access;                          ///<访问指针

    public:

              T *   GetRawData  ()const{return series_data;}                    ///<取得原始数据指针

        const T     GetMaxCount ()const{return max_count;}                      ///<取得最大数量
        const T     GetFreeCount()const{return access-series_data;}             ///<取得空闲数量
        const T     GetUseCount ()const{return end-access;}                     ///<取得使用数量

    public:

        SeriesPool()
        {
            max_count=0;
            series_data=nullptr;
        }

        SeriesPool(const T &count)
        {
            series_data=nullptr;
            Init(count);
        }

        bool Init(const T &count)
        {
            if(series_data)
                return(false);

            max_count=count;
            series_data=new T[max_count];

            if(!series_data)
                return(false);

            end=series_data+max_count;
            access=end;

            {
                T *p=series_data;

                for(T i=0;i<max_count;i++)
                {
                    *p=i;
                    ++p;
                }
            }

            return(true);
        }

        virtual ~SeriesPool()
        {
            delete[] series_data;
        }

        /**
         * 初始化随机序号
         */
        void InitRandomSeries(T min_value=0,T max_value=0)
        {
            if(min_value==0&&max_value==0)
                max_value=max_count-1;

            SortedSet<T> ss;           //使用排序集合用来储存随机序号
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<T> dis(min_value,max_value);

            T *p=series_data;
            T value;

            do
            {
                value=dis(gen);

                if(ss.Add(value)>=0)                                    //加入随机序号，因为使用的是集合，所以不会重复
                {
                    *p=value;                                           //加入到序号池中
                    ++p;
                }
            }while(ss.GetCount()<max_count);                            //如果满了就退出循环
        }

        bool Acquire(T *sp)
        {
            if(!sp)return(false);
            if(!series_data)return(false);

            if(access<=series_data)
                return(false);

            *sp=*(--access);
            return(true);
        }

        bool Release(const T &s)
        {
            if(!series_data)return(false);

            if(access>=end)
                return(false);

            *access=s;
            ++access;

            return(true);
        }
    };//template<typename T> class SeriesPool
    
    using SeriesSize=SeriesPool<size_t>;

    using SeriesInt =SeriesPool<int>;
    using SeriesUInt=SeriesPool<uint>;

    using SeriesI8  =SeriesPool<int8>;
    using SeriesU8  =SeriesPool<uint8>;

    using SeriesI16 =SeriesPool<int16>;
    using SeriesU16 =SeriesPool<uint16>;

    using SeriesI32 =SeriesPool<int32>;
    using SeriesU32 =SeriesPool<uint32>;

    using SeriesI64 =SeriesPool<int64>;
    using SeriesU64 =SeriesPool<uint64>;
}//namespace hgl
