#pragma once

#include<hgl/type/ArrayList.h>
#include<hgl/type/Queue.h>
#include<hgl/type/LifecycleTraits.h>
#include<type_traits>
namespace hgl
{
    // 检测 Active 容器是否提供 GetArray() （例如 ArrayList）
    template<typename C>
    class has_get_array
    {
        template<typename X> static auto test(int)->decltype(std::declval<X>().GetArray(), std::true_type());
        template<typename>   static auto test(...)->std::false_type;
    public:
        static constexpr bool value=decltype(test<C>(0))::value;
    };

    template<typename T,typename AT,typename IT> class PoolTemplate
    {
    protected:
        AT Active;
        IT Idle;
        int max_active_count;
        int history_max;

        void UpdateHistoryMax()
        {
            int cur=Active.GetCount()+Idle.GetCount();
            if(cur>history_max) history_max=cur;
        }

        // 仅在 Active 支持 GetArray() 时批量访问
        template<typename C>
        static auto active_data_ptr(C &c,int) -> decltype(c.GetArray().GetData()) { return c.GetArray().GetData(); }
        template<typename C>
        static T *active_data_ptr(C &,...) { return nullptr; }

        template<typename C>
        static auto active_data_count(C &c,int) -> decltype(c.GetArray().GetCount()) { return c.GetArray().GetCount(); }
        template<typename C>
        static int active_data_count(C &,...) { return 0; }

    public:
        int GetActiveCount()    const { return Active.GetCount(); }
        int GetIdleCount()      const { return Idle.GetCount(); }
        int GetHistoryMaxCount()const { return history_max; }

        DataArray<T> &      GetActiveArray()       { return Active.GetArray(); }
        const DataArray<T> &GetActiveArray() const { return Active.GetArray(); }

        bool IsActive(const T &v) const { return Active.Contains(v); }
        bool IsIdle  (const T &v) const { return Idle.Contains(v); }
        bool IsFull() const { return max_active_count>0 && Active.GetCount()>=max_active_count; }

        PoolTemplate():max_active_count(0),history_max(0){}
        virtual ~PoolTemplate(){ Clear(); }

        void Reserve(int count,bool set_to_max=false)
        {
            Active.Reserve(count);
            Idle.Reserve(count);
            if(set_to_max) max_active_count=count;
        }

        void SetMaxActiveCount(int mc){ max_active_count=mc; }

        bool Create(T &value)
        {
            if(IsFull()) return false;
            bool ok;
            if constexpr(std::is_pointer_v<T>)
                ok=LifecycleTraitsOwningPtr<typename std::remove_pointer<T>::type>::create(&value);
            else
                ok=LifecycleTraits<T>::create(&value);
            if(!ok) return false;
            Active.Add(value); UpdateHistoryMax(); return true;
        }

        bool GetOrCreate(T &value)
        {
            if(!Idle.Pop(value))
            {
                if(IsFull()) return false;
                bool ok;
                if constexpr(std::is_pointer_v<T>)
                    ok=LifecycleTraitsOwningPtr<typename std::remove_pointer<T>::type>::create(&value);
                else
                    ok=LifecycleTraits<T>::create(&value);
                if(!ok) return false;
            }
            else if constexpr(!std::is_pointer_v<T>)
            {
                LifecycleTraits<T>::on_active(&value);
            }
            Active.Add(value); UpdateHistoryMax(); return true;
        }

        bool Get(T &value)
        {
            if(!Idle.Pop(value)) return false;
            if constexpr(!std::is_pointer_v<T>) LifecycleTraits<T>::on_active(&value);
            Active.Add(value); return true;
        }

        bool AppendToActive(T value)
        { if(IsFull()) return false; Active.Add(value); UpdateHistoryMax(); return true; }

        bool AppendToIdle(T value)
        { return Idle.Push(value); }

        bool Release(T value)
        {
            // 仅支持 Active 有 Find/Delete (ArrayList)。若不是此类容器，忽略操作
            if constexpr(has_get_array<AT>::value)
            {
                int idx=Active.Find(value);
                if(idx<0) return false;
                Active.Delete(idx);
                if(!Idle.Push(value)) return false;
                if constexpr(!std::is_pointer_v<T>) LifecycleTraits<T>::on_idle(&value);
                return true;
            }
            else
                return false;
        }

        int Release(T *arr,int count)
        { if(!arr||count<=0) return 0; int r=0; for(int i=0;i<count;i++) if(Release(arr[i])) ++r; return r; }

        void ReleaseActive()
        {
            if constexpr(has_get_array<AT>::value)
            {
                T *ptr=active_data_ptr(Active,0);
                int cnt=active_data_count(Active,0);
                if(ptr && cnt>0)
                {
                    if constexpr(!std::is_pointer_v<T>) LifecycleTraits<T>::on_idle(ptr,cnt);
                    Idle.Push(ptr,cnt);
                }
                Active.Clear();
            }
            else
            {
                // 逐个转移 (Active 视为队列)
                T v; // 假定 Active 提供 Pop
                while(Active.Pop(v))
                {
                    if constexpr(!std::is_pointer_v<T>) LifecycleTraits<T>::on_idle(&v,1);
                    Idle.Push(v);
                }
            }
        }

        void ClearActive()
        {
            if constexpr(has_get_array<AT>::value)
            {
                T *ptr=active_data_ptr(Active,0);
                int cnt=active_data_count(Active,0);
                if(ptr && cnt>0)
                {
                    if constexpr(std::is_pointer_v<T>)
                        LifecycleTraitsOwningPtr<typename std::remove_pointer<T>::type>::destroy(ptr,cnt);
                    else
                        LifecycleTraits<T>::destroy(ptr,cnt);
                }
                Active.Clear();
            }
            else
            {
                T v; while(Active.Pop(v))
                {
                    if constexpr(std::is_pointer_v<T>)
                        LifecycleTraitsOwningPtr<typename std::remove_pointer<T>::type>::destroy(&v,1);
                    else
                        LifecycleTraits<T>::destroy(&v,1);
                }
            }
        }

        void ClearIdle()
        { Idle.Clear(); }

        void Clear()
        { ClearActive(); ClearIdle(); }
    };

    template<typename T> using Pool       = PoolTemplate<T,   ArrayList<T>,   Queue<T>>;
    template<typename T> using ObjectPool = PoolTemplate<T *, ArrayList<T *>, Queue<T *>>;
}//namespace hgl
