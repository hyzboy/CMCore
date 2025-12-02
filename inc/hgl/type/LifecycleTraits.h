#pragma once

#include<hgl/TypeFunc.h>
#include<type_traits>
namespace hgl
{
    /**
    * LifecycleTraits: 统一的生命周期策略，编译期可特化。
    * 缺省规则：
    *  - create: 返回true，不做构造(容器可先分配后手动init)
    *  - destroy: 非平凡析构则显式逐个调用析构
    *  - init:  可平凡类型用 zero， 否则placement new 默认构造
    *  - copy:  可平凡复制用 hgl_cpy，否则逐个赋值
    *  - on_active/on_idle: 缺省返回 true
    */
    template<typename T>
    struct LifecycleTraits
    {
        static bool create(T *ptr){ return true; }

        static void destroy(T *ptr){ if constexpr(!std::is_trivially_destructible_v<T>) if(ptr) ptr->~T(); }

        static void destroy(T *ptr,int count)
        {
            if(!ptr||count<=0) return;
            if constexpr(!std::is_trivially_destructible_v<T>)
                for(int i=0;i<count;i++) (ptr+i)->~T();
        }

        static void init(T *ptr,int count)
        {
            if(!ptr||count<=0) return;
            if constexpr(std::is_trivially_copyable_v<T>)
                hgl_zero(ptr,count);
            else
                for(int i=0;i<count;i++) new(ptr+i) T();
        }

        static void copy(T *dst,const T *src,int count)
        {
            if(!dst||!src||count<=0) return;
            if constexpr(std::is_trivially_copyable_v<T>)
                hgl_cpy(dst,src,count);
            else
                for(int i=0;i<count;i++) dst[i]=src[i];
        }

        static bool on_active(T *,int =1){ return true; }
        static bool on_idle  (T *,int =1){ return true; }
    };

    /** 指针拥有型特化: LifecycleTraitsOwningPtr<T>
     *  用于容器元素类型是 T* 且需要由容器负责 new/delete 的情形。
     */
    template<typename T>
    struct LifecycleTraitsOwningPtr
    {
        static bool create(T **p){ if(!p) return false; *p=new T; return true; }
        static void destroy(T **p){ if(p && *p){ delete *p; *p=nullptr; } }
        static void destroy(T **p,int count)
        {
            if(!p||count<=0) return;
            for(int i=0;i<count;i++)
            {
                if(p[i]){ delete p[i]; p[i]=nullptr; }
            }
        }
        static void init(T **p,int count){ if(!p||count<=0) return; hgl_zero(p,count); }
        static void copy(T **dst,T *const *src,int count){ if(!dst||!src||count<=0)return; hgl_cpy(dst,src,count); }
        static bool on_active(T **,int =1){ return true; }
        static bool on_idle  (T **,int =1){ return true; }
    };
}
