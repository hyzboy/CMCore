#pragma once
#include<hgl/type/FlatOrderedSet.h>
#include<random>
#include<type_traits>
#include<vector>

// SeriesPool 安全检测配置
// 定义 SERIES_POOL_NO_TRACKING 可以完全禁用跟踪（极致性能场景）
// 默认：DEBUG 和 Release 模式都启用（推荐）
#ifndef SERIES_POOL_NO_TRACKING
    #define SERIES_POOL_ENABLE_TRACKING
#endif

#ifdef SERIES_POOL_ENABLE_TRACKING
#include<ankerl/unordered_dense.h>
#endif

namespace hgl
{
    /**
     * 序号池<br>
     * 没什么用，就是一个序号堆栈而已。
     *
     * @note 默认启用双重释放检测（使用高效的 hash set）
     *       如需禁用以获得极致性能，定义 SERIES_POOL_NO_TRACKING
     */
    template<typename T> class SeriesPool
    {
        static_assert(std::is_integral<T>::value, "SeriesPool requires integral type");
        //一定要用带BOM头的UTF8存哦！"量"字在一行的最后，会导致编译器报错，认不出来换行符。

        T max_count;                        ///<最大数量
        T *series_data;                     ///<序列数据
        T *end;                             ///<结束指针
        T *access;                          ///<访问指针

#ifdef SERIES_POOL_ENABLE_TRACKING
        ankerl::unordered_dense::set<T> allocated_set;  ///<跟踪已分配的序号（防止双重释放）
#endif

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

#ifdef SERIES_POOL_ENABLE_TRACKING
            allocated_set.clear();
            allocated_set.reserve(max_count);
#endif

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
         * @param min_value 最小值（默认0）
         * @param max_value 最大值（默认 max_count-1）
         * @note 如果范围不足，将使用 Fisher-Yates shuffle 算法
         */
        void InitRandomSeries(T min_value=0,T max_value=0)
        {
            if(min_value==0&&max_value==0)
                max_value=max_count-1;

            T range = max_value - min_value + 1;

            // 如果范围不足以容纳所需数量，回退到顺序初始化
            if(range < max_count)
            {
                T *p=series_data;
                for(T i=0;i<max_count;i++)
                {
                    *p=i;
                    ++p;
                }
                return;
            }

            // 使用 Fisher-Yates shuffle 避免重试
            std::vector<T> indices;
            indices.reserve(range);
            for(T i=min_value; i<=max_value; i++)
            {
                indices.push_back(i);
            }

            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(indices.begin(), indices.end(), gen);

            // 取前 max_count 个
            T *p=series_data;
            for(T i=0; i<max_count; i++)
            {
                *p=indices[i];
                ++p;
            }
        }

        bool Acquire(T *sp)
        {
            if(!sp)return(false);
            if(!series_data)return(false);

            if(access<=series_data)
                return(false);

            *sp=*(--access);

#ifdef SERIES_POOL_ENABLE_TRACKING
            allocated_set.insert(*sp);
#endif

            return(true);
        }

        bool Release(const T &s)
        {
            if(!series_data)return(false);

            if(access>=end)
                return(false);

            // 验证序号范围
            if(s<0 || s>=max_count)
                return(false);

#ifdef SERIES_POOL_ENABLE_TRACKING
            // 使用 set 检测双重释放和无效释放 (O(1))
            if(allocated_set.find(s) == allocated_set.end())
                return(false);  // 序号未被分配或已释放

            allocated_set.erase(s);
#endif

            *access=s;
            ++access;

            return(true);
        }

        /**
         * 检查序号是否在空闲列表中（用于诊断）
         * @param s 要检查的序号
         * @return 如果在空闲列表中返回 true
         */
        bool IsInFreeList(const T &s) const
        {
            if(!series_data)return(false);

#ifdef SERIES_POOL_ENABLE_TRACKING
            // 启用跟踪：使用 set 查询 (O(1))
            return allocated_set.find(s) == allocated_set.end();
#else
            // 禁用跟踪：使用遍历 (O(n))
            for(const T *p=access; p<end; ++p)
            {
                if(*p==s)
                    return(true);
            }
            return(false);
#endif
        }

#ifdef SERIES_POOL_ENABLE_TRACKING
        /**
         * 获取当前已分配的序号数量
         * @return 已分配但未释放的序号数量
         */
        size_t GetAllocatedCount() const
        {
            return allocated_set.size();
        }

        /**
         * 检查是否有内存泄漏
         * @return 如果所有序号都已释放返回 true
         */
        bool IsFullyReleased() const
        {
            return allocated_set.empty();
        }
#endif
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
