#pragma once

#include <hgl/type/ValueArray.h>
#include <hgl/type/Queue.h>
#include <hgl/type/has_get_array.h>

namespace hgl
{
    /**
    * @brief CN:对象指针池模板类。\nEN:Object pointer pool template class.
    * @tparam T CN:对象类型。EN:Object type.
    * @note CN:ManagedPool 负责管理指针对象的生命周期，自动 delete。对于简单值类型，请使用 ValuePool。\nEN:ManagedPool manages pointer object lifecycle with automatic deletion. For simple value types, use ValuePool instead.
    */
    template<typename T>
    class ManagedPool
    {
        // CN:确保 T 不是平凡可销毁类型，否则应使用 ValuePool<T> 而非 ManagedPool<T>。\nEN:Ensure T is not trivially destructible, otherwise use ValuePool<T> instead of ManagedPool<T>.
        static_assert(
            !std::is_trivially_destructible_v<T>,
            "ManagedPool<T> is for complex objects requiring explicit cleanup (RAII types, custom classes). "
            "For trivially destructible types (int, float, POD struct, pointers), "
            "use ValuePool<T> instead, which is more efficient and doesn't perform unnecessary heap allocations. "
            "Example: Use ValuePool<int> not ManagedPool<int>; Use ValuePool<MyStruct*> for pointer management."
        );

    protected:

        /**
        * @brief CN:活跃对象容器。\nEN:Active object container.
        */
        ValueArray<T *> Active;

        /**
        * @brief CN:闲置对象容器。\nEN:Idle object container.
        */
        Queue<T *> Idle;

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
        ValueBuffer<T *> &GetActiveView()
        {
            return Active.GetArray();
        }

        /**
        * @brief CN:获取活跃对象数组（常量）。\nEN:Get active object array (const).
        */
        const ValueBuffer<T *> &GetActiveView() const
        {
            return Active.GetArray();
        }

        /**
        * @brief CN:判断对象是否活跃。\nEN:Check if object is active.
        */
        bool IsActive(T *v) const
        {
            return Active.Contains(v);
        }

        /**
        * @brief CN:判断对象是否闲置。\nEN:Check if object is idle.
        */
        bool IsIdle(T *v) const
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

        ManagedPool() : max_active_count(0), history_max(0)
        {
        }

        virtual ~ManagedPool()
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
        bool Create(T *&value)
        {
            if (IsFull())
                return false;

            value = new T();
            if (!value)
                return false;

            Active.Add(value);
            UpdateHistoryMax();
            return true;
        }

        /**
        * @brief CN:获取或创建对象并加入活跃池。\nEN:Get or create object and add to active pool.
        */
        bool GetOrCreate(T *&value)
        {
            if (!Idle.Pop(value))
            {
                if (IsFull())
                    return false;

                value = new T();
                if (!value)
                    return false;
            }

            Active.Add(value);
            UpdateHistoryMax();
            return true;
        }

        /**
        * @brief CN:从闲置池获取对象并加入活跃池。\nEN:Get object from idle pool and add to active pool.
        */
        bool Get(T *&value)
        {
            if (!Idle.Pop(value))
                return false;

            Active.Add(value);
            return true;
        }

        /**
        * @brief CN:直接添加对象到活跃池。\nEN:Append object to active pool directly.
        */
        bool AppendToActive(T *value)
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
        bool AppendToIdle(T *value)
        {
            return Idle.Push(value);
        }

        /**
        * @brief CN:释放对象到闲置池。\nEN:Release object to idle pool.
        * @complexity CN:时间复杂度 O(n)，使用线性查找。\nEN:O(n) time complexity with linear search.
        */
        bool Release(T *value)
        {
            int idx = Active.Find(value);
            if (idx < 0)
                return false;
            Active.Delete(idx);
            if (!Idle.Push(value))
                return false;
            return true;
        }

        /**
        * @brief CN:批量释放对象到闲置池。\nEN:Release objects to idle pool in batch.
        */
        int Release(T **arr, int count)
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
            T **ptr = Active.GetData();
            int cnt = Active.GetCount();
            if (ptr && cnt > 0)
            {
                Idle.Push(ptr, cnt);
            }
            Active.Clear();
        }

        /**
        * @brief CN:清空所有活跃对象并释放资源。\nEN:Clear all active objects and release resources.
        */
        void ClearActive()
        {
            T **ptr = Active.GetData();
            int cnt = Active.GetCount();
            if (ptr && cnt > 0)
            {
                for (int i = 0; i < cnt; ++i)
                {
                    if (ptr[i])
                        delete ptr[i];
                }
            }
            Active.Clear();
        }

        /**
        * @brief CN:清空所有闲置对象。\nEN:Clear all idle objects.
        */
        void ClearIdle()
        {
            T *v;
            while (Idle.Pop(v))
            {
                if (v)
                    delete v;
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
} // namespace hgl
