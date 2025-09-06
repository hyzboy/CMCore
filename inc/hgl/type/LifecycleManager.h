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
    *  - init:  可平凡类型用 zero， 否则用缺省值构造覆盖
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

        static bool on_active(T *,int count=1){ return true; }
        static bool on_idle  (T *,int count=1){ return true; }
    };

    /** 指针拥有型特化: LifecycleTraitsOwningPtr<T>
     *  用于容器元素类型是 T* 且需要由容器负责 new/delete 的情形。
     *  不直接作为 LifecycleTraits<T*> 缺省特化，避免与“只存裸指针不拥有”场景冲突。
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
        static bool on_active(T **,int=1){ return true; }
        static bool on_idle  (T **,int=1){ return true; }
    };

    /**
    * 旧的 DataLifecycleManager：保持接口，但内部委托到 LifecycleTraits
    */
    template<typename T> struct DataLifecycleManager
    {
        virtual bool Create(T *p){ return LifecycleTraits<T>::create(p); }
        virtual void Clear(T *p){ LifecycleTraits<T>::destroy(p); }
        virtual void Clear(T *p,int count){ LifecycleTraits<T>::destroy(p,count); }

        virtual void Copy(T *dst,T *src,int count)
        {
            LifecycleTraits<T>::copy(dst,src,count);
        }

        virtual void Init(T *dst,int count)
        {
            LifecycleTraits<T>::init(dst,count);
        }

    public: //活跃事件

        virtual bool OnActive   (T *p,int count=1){ return LifecycleTraits<T>::on_active(p,count); }         ///<切换到激活
        virtual bool OnIdle     (T *p,int count=1){ return LifecycleTraits<T>::on_idle  (p,count); }         ///<切换到空闲
    };

    /**
    * 对像生命周期处理回调  (保持兼容：默认不负责 new)
    */
    template<typename T> struct ObjectLifecycleManager:public DataLifecycleManager<T *>
    {
        using Elem = T*;
        virtual bool Create(Elem *) override {return false;}   // 不默认分配
        virtual void Clear(Elem *obj) override { if(obj&&*obj) { delete *obj; *obj=nullptr; } }

        virtual void Clear(Elem *obj,int count) override
        {
            if(!obj||count<=0)return;

            for(int i=0;i<count;i++)
            {
                if(obj[i])
                {
                    delete obj[i];
                    obj[i]=nullptr;
                }
            }
        }
    };//struct ObjectLifecycleManager

    /**
    * 缺省对像生命周期处理回调 (拥有指针)
    */
    template<typename T> struct DefaultObjectLifetimeCallback:public ObjectLifecycleManager<T>
    {
        virtual bool Create(T **obj) override { if(!obj) return false; *obj=new T; return true; }
    };//struct DefaultObjectLifetimeCallback
}//namespace hgl
