#pragma once

#include<hgl/type/SeriesPool.h>

namespace hgl
{
    // AI NOTE: Fixed-size pool for trivial types with ID-based allocation.
    // Init allocates once; Acquire/Release use a SeriesInt free list.
    /**
     * 固定大小值池 - 用于 trivial 类型（如 POD 结构体、整数等）
     * 一次分配固定数量的值，值的生存期与池一致，无需手动构造/析构
     * 适用场景：内存密集、高频分配释放、trivial 类型
     *
     * @tparam T 值类型（必须是 trivial 类型，如 int、float、struct Point 等）
     */
    template<typename T> class FixedValuePool
    {
    protected:

        T *data_array;                      ///<数据区
        T *end;                             ///<结束指针
        SeriesInt series;                   ///<序号池

    public:

        T *GetRawData()const{return data_array;}                    ///<取得原始数据指针
        SeriesInt *GetSeries(){return &series;}                     ///<取得序号池

    public:

        FixedValuePool()
        {
            data_array=nullptr;
            end=nullptr;
        }

        /**
         * 初始化对象池
         * @param max_count 最大对象数量
         * @return 成功返回 true，失败返回 false
         */
        bool Init(const int max_count)
        {
            data_array = new T[max_count]();  // 使用普通 new[] 分配并调用构造函数

            end = data_array + max_count;

            return series.Init(max_count);
        }

        ~FixedValuePool()
        {
            delete[] data_array;
        }

        /**
         * 从池中获取一个对象
         * @return 对象指针，如果池已满返回 nullptr
         */
        T *Acquire()
        {
            int pos;

            if(!series.Acquire(&pos))
                return nullptr;

            return data_array+pos;
        }

        /**
         * 将对象归还到池中
         * @param obj 对象指针
         * @return 成功返回 true，失败返回 false
         */
        bool Release(T *obj)
        {
            if(!obj)
                return(false);

            if(obj<data_array||obj>=end)
                return(false);

            const int pos=obj-data_array;

            return series.Release(pos);
        }
    };//template<typename T> class FixedValuePool
}//namespace hgl
