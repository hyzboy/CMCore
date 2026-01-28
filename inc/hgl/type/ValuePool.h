/**
* @file ValuePool.h
* @brief CN:对象池模板类，支持活跃与闲置对象管理。\nEN:Object pool template class, supports active and idle object management.
*/
#pragma once

#include <hgl/type/ValueArray.h>
#include <hgl/type/Queue.h>
#include <hgl/type/has_get_array.h>
#include <tsl/robin_map.h>

namespace hgl
{
    /**
    * @brief CN:对象池模板类。\nEN:Object pool template class.
    * @tparam T CN:对象类型。EN:Object type.
    * @note CN:仅支持平凡可销毁类型或指针类型。复杂对象使用ManagedPool。EN:Only supports trivially destructible or pointer types. Use ManagedPool for complex objects.
    * @warning CN:指针类型不会自动释放内存，需要手动管理生命周期。EN:Pointer types are not automatically freed, lifecycle must be managed manually.
    */
    template<typename T>
    class ValuePool
    {
        // CN:确保 T 是平凡可销毁类型。EN:Ensure T is trivially destructible type.
        static_assert(
            std::is_trivially_destructible_v<T>,
            "ValuePool<T> only supports trivially destructible types (int, float, POD struct, pointers). "
            "For objects that require complex cleanup (RAII types, STL containers, etc.), "
            "use ManagedPool<T> instead. "
            "Note: Pointers stored in ValuePool<T*> are NOT automatically deleted."
        );

    public:

        // ============================================================
        // 统计信息结构
        // ============================================================

        struct Stats
        {
            int total_creates   = 0;            // CN:总创建次数。EN:Total create count.
            int total_reuses    = 0;            // CN:总重用次数。EN:Total reuse count.
            int total_releases  = 0;            // CN:总释放次数。EN:Total release count.
            int peak_active     = 0;            // CN:峰值活跃数量。EN:Peak active count.

            float GetHitRate() const { int total = total_creates + total_reuses; return total > 0 ? (float)total_reuses / total : 0.0f; }
        };

    protected:

        // ============================================================
        // 成员变量
        // ============================================================

        ValueArray<T>                   Active;                 // CN:活跃对象容器。EN:Active object container.
        Queue<T>                        Idle;                   // CN:闲置对象容器。EN:Idle object container.
        tsl::robin_map<const T*, int>   ActiveIndex;            // CN:活跃对象指针索引映射（内存高效）。EN:Active object pointer index map (memory efficient).
        int                             max_active_count;       // CN:最大活跃对象数量。EN:Maximum active object count.
        int                             min_idle_count;         // CN:最小闲置对象数量。EN:Minimum idle object count.
        int                             history_max;            // CN:历史最大对象数量。EN:History maximum object count.
        Stats                           stats;                  // CN:统计信息。EN:Statistics.

        // ============================================================
        // 内部辅助函数
        // ============================================================

        void UpdateHistoryMax()
        {
            int cur = Active.GetCount() + Idle.GetCount();
            if (cur > history_max)
                history_max = cur;
        }

        void UpdatePeakActive()
        {
            int active = Active.GetCount();
            if (active > stats.peak_active)
                stats.peak_active = active;
        }

        void MaintainMinIdle()                  // CN:维持最小闲置数量。EN:Maintain minimum idle count.
        {
            while (Idle.GetCount() < min_idle_count && !IsFull())
            {
                T value = T();
                Idle.Push(value);
            }
        }

    public:

        // ============================================================
        // 获取类方法
        // ============================================================

        int                     GetActiveCount()            const { return Active.GetCount();                   }   // CN:获取活跃对象数量。EN:Get active object count.
        int                     GetIdleCount()              const { return Idle.GetCount();                     }   // CN:获取闲置对象数量。EN:Get idle object count.
        int                     GetTotalCount()             const { return Active.GetCount() + Idle.GetCount(); }   // CN:获取总对象数量。EN:Get total object count.
        int                     GetHistoryMaxCount()        const { return history_max;                         }   // CN:获取历史最大对象数量。EN:Get history maximum object count.
        const Stats             &GetStats()                 const { return stats;                               }   // CN:获取统计信息。EN:Get statistics.
        ValueBuffer<T>          &GetActiveView()                  { return Active.GetArray();                   }   // CN:获取活跃对象数组。EN:Get active object array.
        const ValueBuffer<T>    &GetActiveView()            const { return Active.GetArray();                   }   // CN:获取活跃对象数组（常量）。EN:Get active object array (const).

        // ============================================================
        // 状态检查
        // ============================================================

        bool IsFull()               const { return max_active_count > 0 && Active.GetCount() >= max_active_count; }  // CN:判断池是否已满。EN:Check if pool is full.
        bool IsEmpty()              const { return Active.GetCount() == 0 && Idle.GetCount() == 0;               }  // CN:判断池是否为空。EN:Check if pool is empty.
        bool IsActive(const T &v)   const   // CN:判断对象是否活跃（O(n)）。EN:Check if object is active (O(n)).
        {
            const T *arr = Active.GetData();
            int count = Active.GetCount();
            for (int i = 0; i < count; ++i)
            {
                if (arr[i] == v)
                    return true;
            }
            return false;
        }
        bool IsIdle(const T &v)     const { return Idle.Contains(v);                                             }  // CN:判断对象是否闲置。EN:Check if object is idle.

        // ============================================================
        // 构造与析构
        // ============================================================

        ValuePool() : max_active_count(0), min_idle_count(0), history_max(0) { }   // CN:构造函数。EN:Constructor.

        virtual ~ValuePool() { Clear(); }   // CN:析构函数。EN:Destructor.

        // ============================================================
        // 初始化操作
        // ============================================================

        void Reserve(int count, bool set_to_max = false)   // CN:预分配容量。EN:Reserve capacity.
        {
            Active.Reserve(count);
            Idle.Reserve(count);
            ActiveIndex.reserve(count);
            if (set_to_max)
                max_active_count = count;
        }

        void Warmup(int count)   // CN:预热：预创建指定数量的闲置对象。EN:Warmup: pre-create idle objects.
        {
            for (int i = 0; i < count && !IsFull(); ++i)
            {
                T value = T();
                Idle.Push(value);
            }
            UpdateHistoryMax();
        }

        void SetMaxActiveCount(int mc)      { max_active_count = mc;  }   // CN:设置最大活跃对象数量。EN:Set maximum active object count.
        void SetMinIdleCount(int mc)        { min_idle_count = mc; MaintainMinIdle(); }   // CN:设置最小闲置对象数量。EN:Set minimum idle object count.

        // ============================================================
        // 对象获取与创建
        // ============================================================

        bool CreateNew(T &value)   // CN:强制创建新对象。EN:Force create new object.
        {
            if (IsFull())
                return false;

            value = T();

            int idx = Active.GetCount();
            Active.Add(value);
            ActiveIndex[&Active[idx]] = idx;

            ++stats.total_creates;
            UpdateHistoryMax();
            UpdatePeakActive();
            MaintainMinIdle();
            return true;
        }

        bool GetOrCreate(T &value)   // CN:获取闲置对象或创建新对象。EN:Get idle object or create new one.
        {
            if (Idle.Pop(value))
            {
                ++stats.total_reuses;
            }
            else
            {
                if (IsFull())
                    return false;

                value = T();
                ++stats.total_creates;
            }

            int idx = Active.GetCount();
            Active.Add(value);
            ActiveIndex[&Active[idx]] = idx;

            UpdateHistoryMax();
            UpdatePeakActive();
            return true;
        }

        template<typename ResetFunc>
        bool GetOrCreate(T &value, ResetFunc reset_fn)   // CN:获取并重置对象。EN:Get object and reset it.
        {
            if (Idle.Pop(value))
            {
                reset_fn(value);
                ++stats.total_reuses;
            }
            else
            {
                if (IsFull())
                    return false;

                value = T();
                ++stats.total_creates;
            }

            int idx = Active.GetCount();
            Active.Add(value);
            ActiveIndex[&Active[idx]] = idx;

            UpdateHistoryMax();
            UpdatePeakActive();
            return true;
        }

        bool GetIfAvailable(T &value)   // CN:仅从闲置池获取，不创建。EN:Get from idle pool only, don't create.
        {
            if (!Idle.Pop(value))
                return false;

            int idx = Active.GetCount();
            Active.Add(value);
            ActiveIndex[&Active[idx]] = idx;

            ++stats.total_reuses;
            UpdatePeakActive();
            return true;
        }

        int GetBatch(T *out, int max_count)   // CN:批量获取对象。EN:Batch get objects.
        {
            if (!out || max_count <= 0)
                return 0;

            int count = 0;
            for (int i = 0; i < max_count; ++i)
            {
                if (!GetOrCreate(out[i]))
                    break;
                ++count;
            }
            return count;
        }

        int CreateBatch(T *out, int count)   // CN:批量创建对象。EN:Batch create objects.
        {
            if (!out || count <= 0)
                return 0;

            int created = 0;
            for (int i = 0; i < count; ++i)
            {
                if (!CreateNew(out[i]))
                    break;
                ++created;
            }
            return created;
        }

        // ============================================================
        // 对象添加
        // ============================================================

        bool AppendToActive(const T &value)   // CN:直接添加对象到活跃池。EN:Append object to active pool directly.
        {
            if (IsFull())
                return false;

            int idx = Active.GetCount();
            Active.Add(value);
            ActiveIndex[&Active[idx]] = idx;

            UpdateHistoryMax();
            UpdatePeakActive();
            return true;
        }

        bool AppendToIdle(const T &value)   // CN:直接添加对象到闲置池。EN:Append object to idle pool directly.
        {
            bool result = Idle.Push(value);
            if (result)
                UpdateHistoryMax();
            return result;
        }

        // ============================================================
        // 对象释放
        // ============================================================

        bool Release(const T &value)   // CN:释放对象到闲置池，O(n)查找+O(1)删除。EN:Release object to idle pool, O(n) search + O(1) deletion.
        {
            // CN:先在Active数组中找到值的位置。EN:First find the value's position in Active array.
            T *arr = Active.GetData();
            int count = Active.GetCount();
            int idx = -1;
            
            for (int i = 0; i < count; ++i)
            {
                if (arr[i] == value)
                {
                    idx = i;
                    break;
                }
            }
            
            if (idx == -1)
                return false;

            int last_idx = count - 1;

            // CN:从ActiveIndex中删除当前元素（使用当前位置的地址）。EN:Remove current element from ActiveIndex (use current position's address).
            ActiveIndex.erase(&arr[idx]);

            // CN:与最后一个元素交换（避免O(n)删除）。EN:Swap with last element (avoid O(n) deletion).
            if (idx != last_idx)
            {
                // CN:先删除最后一个元素的索引。EN:First remove last element's index.
                ActiveIndex.erase(&arr[last_idx]);
                
                // CN:复制最后一个元素到当前位置。EN:Copy last element to current position.
                Active[idx] = arr[last_idx];
                
                // CN:重新添加索引（使用新位置的地址）。EN:Re-add index (use new position's address).
                ActiveIndex[&Active[idx]] = idx;
            }

            Active.Delete(last_idx, 1);

            if (!Idle.Push(value))
                return false;

            ++stats.total_releases;
            MaintainMinIdle();
            return true;
        }

        int ReleaseBatch(const T *arr, int count)   // CN:批量释放对象到闲置池。EN:Release objects to idle pool in batch.
        {
            if (!arr || count <= 0)
                return 0;

            int released = 0;
            for (int i = 0; i < count; ++i)
            {
                if (Release(arr[i]))
                    ++released;
            }
            return released;
        }

        // ============================================================
        // 清空操作
        // ============================================================

        void ReleaseActive()   // CN:释放所有活跃对象到闲置池。EN:Release all active objects to idle pool.
        {
            T *ptr = Active.GetData();
            int cnt = Active.GetCount();
            if (ptr && cnt > 0)
            {
                Idle.Push(ptr, cnt);
                stats.total_releases += cnt;
            }
            ActiveIndex.clear();
            Active.Clear();
            MaintainMinIdle();
        }

        void ClearActive()   // CN:清空所有活跃对象。EN:Clear all active objects.
        {
            Active.Clear();
            ActiveIndex.clear();
        }

        void ClearIdle()     { Idle.Clear();   }   // CN:清空所有闲置对象。EN:Clear all idle objects.

        void Clear()   // CN:清空池中所有对象。EN:Clear all objects in pool.
        {
            ClearActive();
            ClearIdle();
        }

        void Shrink()   // CN:缩减容量到实际使用大小。EN:Shrink capacity to actual size.
        {
            Active.Shrink();
            Idle.Shrink();
        }

        void ShrinkIdle(int target_count)   // CN:缩减闲置池到目标大小。EN:Shrink idle pool to target size.
        {
            while (Idle.GetCount() > target_count)
            {
                T dummy;
                Idle.Pop(dummy);
            }
        }

        void ResetStats()   { stats = Stats(); }   // CN:重置统计信息。EN:Reset statistics.

        // ============================================================
        // 迭代器支持
        // ============================================================

        auto begin()        { return Active.begin(); }   // CN:开始迭代器。EN:Begin iterator.
        auto end()          { return Active.end();   }   // CN:结束迭代器。EN:End iterator.
        auto begin() const  { return Active.begin(); }   // CN:常量开始迭代器。EN:Const begin iterator.
        auto end()   const  { return Active.end();   }   // CN:常量结束迭代器。EN:Const end iterator.

        template<typename Func>
        void ForEachActive(Func fn)   // CN:遍历所有活跃对象。EN:Iterate all active objects.
        {
            T *ptr = Active.GetData();
            int cnt = Active.GetCount();
            for (int i = 0; i < cnt; ++i)
                fn(ptr[i]);
        }

        // ============================================================
        // 验证与调试
        // ============================================================

        bool Validate() const   // CN:验证池的完整性。EN:Validate pool integrity.
        {
            // CN:检查活跃对象指针索引一致性。EN:Check active object pointer index consistency.
            if (ActiveIndex.size() != (size_t)Active.GetCount())
                return false;

            T *ptr = Active.GetData();
            int cnt = Active.GetCount();
            for (int i = 0; i < cnt; ++i)
            {
                auto it = ActiveIndex.find(&ptr[i]);
                if (it == ActiveIndex.end() || it->second != i)
                    return false;
            }

            // CN:检查总数不超过限制。EN:Check total count doesn't exceed limit.
            if (max_active_count > 0 && Active.GetCount() > max_active_count)
                return false;

            return true;
        }
    };
} // namespace hgl
