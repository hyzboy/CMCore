/**
* @file Pool.h
* @brief CN:对象池模板类，支持活跃与闲置对象管理。\nEN:Object pool template class, supports active and idle object management.
*/
#pragma once

#include <hgl/type/ArrayList.h>
#include <hgl/type/Queue.h>
#include <type_traits>

namespace hgl
{
    /**
    * @brief CN:检测容器是否提供 GetArray() 方法。\nEN:Detects if container provides GetArray() method.
    * @tparam C CN:容器类型。EN:Container type.
    */
    template<typename C>
    class has_get_array
    {
        template<typename X> static auto test(int) -> decltype(std::declval<X>().GetArray(), std::true_type());
        template<typename>   static auto test(...) -> std::false_type;
    public:

        /**
        * @brief CN:是否支持 GetArray()。\nEN:Whether GetArray() is supported.
        */
        static constexpr bool value = decltype(test<C>(0))::value;

    };

    /**
    * @brief CN:对象池模板类。\nEN:Object pool template class.
    * @tparam T CN:对象类型。EN:Object type.
    * @tparam AT CN:活跃对象容器类型。EN:Active container type.
    * @tparam IT CN:闲置对象容器类型。EN:Idle container type.
    */
    template<typename T, typename AT, typename IT>
    class PoolTemplate
    {
    protected:

        /**
        * @brief CN:活跃对象容器。\nEN:Active object container.
        */
        AT Active;

        /**
        * @brief CN:闲置对象容器。\nEN:Idle object container.
        */
        IT Idle;

        /**
        * @brief CN:最大活跃对象数量。\nEN:Maximum active object count.
        */
        int max_active_count;

        /**
        * @brief CN:历史最大对象数量。\nEN:History maximum object count.
        */
        int history_max;

        /**
        * @brief CN:更新历史最大对象数量。\nEN:Update history maximum object count.
        */
        void UpdateHistoryMax()
        {
            int cur = Active.GetCount() + Idle.GetCount();
            if (cur > history_max)
                history_max = cur;
        }

        /**
        * @brief CN:获取活跃对象数据指针（仅支持 GetArray()）。\nEN:Get active object data pointer (only supports GetArray()).
        */
        template<typename C>
        static auto active_data_ptr(C &c, int) -> decltype(c.GetArray().GetData())
        {
            return c.GetArray().GetData();
        }

        template<typename C>
        static T *active_data_ptr(C &, ...)
        {
            return nullptr;
        }

        /**
        * @brief CN:获取活跃对象数量（仅支持 GetArray()）。\nEN:Get active object count (only supports GetArray()).
        */
        template<typename C>
        static auto active_data_count(C &c, int) -> decltype(c.GetArray().GetCount())
        {
            return c.GetArray().GetCount();
        }

        template<typename C>
        static int active_data_count(C &, ...)
        {
            return 0;
        }

    public:

        /**
        * @brief CN:获取活跃对象数量。\nEN:Get active object count.
        */
        int GetActiveCount() const
        {
            return Active.GetCount();
        }

        /**
        * @brief CN:获取闲置对象数量。\nEN:Get idle object count.
        */
        int GetIdleCount() const
        {
            return Idle.GetCount();
        }

        /**
        * @brief CN:获取历史最大对象数量。\nEN:Get history maximum object count.
        */
        int GetHistoryMaxCount() const
        {
            return history_max;
        }

        /**
        * @brief CN:获取活跃对象数组。\nEN:Get active object array.
        */
        DataArray<T> &GetActiveArray()
        {
            return Active.GetArray();
        }

        /**
        * @brief CN:获取活跃对象数组（常量）。\nEN:Get active object array (const).
        */
        const DataArray<T> &GetActiveArray() const
        {
            return Active.GetArray();
        }

        /**
        * @brief CN:判断对象是否活跃。\nEN:Check if object is active.
        */
        bool IsActive(const T &v) const
        {
            return Active.Contains(v);
        }

        /**
        * @brief CN:判断对象是否闲置。\nEN:Check if object is idle.
        */
        bool IsIdle(const T &v) const
        {
            return Idle.Contains(v);
        }

        /**
        * @brief CN:判断池是否已满。\nEN:Check if pool is full.
        */
        bool IsFull() const
        {
            return max_active_count > 0 && Active.GetCount() >= max_active_count;
        }

        PoolTemplate() : max_active_count(0), history_max(0)
        {
        }

        virtual ~PoolTemplate()
        {
            Clear();
        }

        /**
        * @brief CN:预分配池容量。\nEN:Reserve pool capacity.
        */
        void Reserve(int count, bool set_to_max = false)
        {
            Active.Reserve(count);
            Idle.Reserve(count);
            if (set_to_max)
                max_active_count = count;
        }

        /**
        * @brief CN:设置最大活跃对象数量。\nEN:Set maximum active object count.
        */
        void SetMaxActiveCount(int mc)
        {
            max_active_count = mc;
        }

        /**
        * @brief CN:创建新对象并加入活跃池。\nEN:Create new object and add to active pool.
        */
        bool Create(T &value)
        {
            if (IsFull())
                return false;
            
            // 对于指针类型，创建新对象；对于非指针类型，使用默认构造
            if constexpr (std::is_pointer_v<T>)
            {
                value = new typename std::remove_pointer<T>::type();
                if (!value)
                    return false;
            }
            else
            {
                value = T();
            }
            
            Active.Add(value);
            UpdateHistoryMax();
            return true;
        }

        /**
        * @brief CN:获取或创建对象并加入活跃池。\nEN:Get or create object and add to active pool.
        */
        bool GetOrCreate(T &value)
        {
            if (!Idle.Pop(value))
            {
                if (IsFull())
                    return false;
                
                // 对于指针类型，创建新对象；对于非指针类型，使用默认构造
                if constexpr (std::is_pointer_v<T>)
                {
                    value = new typename std::remove_pointer<T>::type();
                    if (!value)
                        return false;
                }
                else
                {
                    value = T();
                }
            }
            
            Active.Add(value);
            UpdateHistoryMax();
            return true;
        }

        /**
        * @brief CN:从闲置池获取对象并加入活跃池。\nEN:Get object from idle pool and add to active pool.
        */
        bool Get(T &value)
        {
            if (!Idle.Pop(value))
                return false;
            
            Active.Add(value);
            return true;
        }

        /**
        * @brief CN:直接添加对象到活跃池。\nEN:Append object to active pool directly.
        */
        bool AppendToActive(T value)
        {
            if (IsFull())
                return false;
            Active.Add(value);
            UpdateHistoryMax();
            return true;
        }

        /**
        * @brief CN:直接添加对象到闲置池。\nEN:Append object to idle pool directly.
        */
        bool AppendToIdle(T value)
        {
            return Idle.Push(value);
        }

        /**
        * @brief CN:释放对象到闲置池。\nEN:Release object to idle pool.
        */
        bool Release(T value)
        {
            // 仅支持 Active 有 Find/Delete (ArrayList)。若不是此类容器，忽略操作
            if constexpr (has_get_array<AT>::value)
            {
                int idx = Active.Find(value);
                if (idx < 0)
                    return false;
                Active.Delete(idx);
                if (!Idle.Push(value))
                    return false;
                return true;
            }
            else
                return false;
        }

        /**
        * @brief CN:批量释放对象到闲置池。\nEN:Release objects to idle pool in batch.
        */
        int Release(T *arr, int count)
        {
            if (!arr || count <= 0)
                return 0;
            int r = 0;
            for (int i = 0; i < count; i++)
                if (Release(arr[i]))
                    ++r;
            return r;
        }

        /**
        * @brief CN:释放所有活跃对象到闲置池。\nEN:Release all active objects to idle pool.
        */
        void ReleaseActive()
        {
            if constexpr (has_get_array<AT>::value)
            {
                T *ptr = active_data_ptr(Active, 0);
                int cnt = active_data_count(Active, 0);
                if (ptr && cnt > 0)
                {
                    Idle.Push(ptr, cnt);
                }
                Active.Clear();
            }
            else
            {
                // 逐个转移 (Active 视为队列)
                T v;
                // 假定 Active 提供 Pop
                while (Active.Pop(v))
                {
                    Idle.Push(v);
                }
            }
        }

        /**
        * @brief CN:清空所有活跃对象并释放资源。\nEN:Clear all active objects and release resources.
        */
        void ClearActive()
        {
            if constexpr (has_get_array<AT>::value)
            {
                T *ptr = active_data_ptr(Active, 0);
                int cnt = active_data_count(Active, 0);
                if (ptr && cnt > 0)
                {
                    // 对于指针类型，手动删除；对于非指针类型，由容器处理
                    if constexpr (std::is_pointer_v<T>)
                    {
                        for (int i = 0; i < cnt; ++i)
                        {
                            if (ptr[i])
                                delete ptr[i];
                        }
                    }
                }
                Active.Clear();
            }
            else
            {
                // 逐个清理
                T v;
                while (Active.Pop(v))
                {
                    if constexpr (std::is_pointer_v<T>)
                    {
                        if (v)
                            delete v;
                    }
                }
            }
        }

        /**
        * @brief CN:清空所有闲置对象。\nEN:Clear all idle objects.
        */
        void ClearIdle()
        {
            // 对于指针类型，需要手动删除
            if constexpr (std::is_pointer_v<T>)
            {
                T v;
                while (Idle.Pop(v))
                {
                    if (v)
                        delete v;
                }
            }
            else
            {
                // 对于非指针类型，容器会自动处理
                Idle.Clear();
            }
        }

        /**
        * @brief CN:清空池中所有对象。\nEN:Clear all objects in pool.
        */
        void Clear()
        {
            ClearActive();
            ClearIdle();
        }
    };

    /**
    * @brief CN:对象池类型定义。\nEN:Object pool type definition.
    */
    template<typename T>
    using Pool = PoolTemplate<T, ArrayList<T>, Queue<T>>;

    /**
    * @brief CN:对象指针池类型定义。\nEN:Object pointer pool type definition.
    */
    template<typename T>
    using ObjectPool = PoolTemplate<T *, ArrayList<T *>, Queue<T *>>;

} // namespace hgl
