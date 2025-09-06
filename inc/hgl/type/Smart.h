#pragma once

#include<hgl/TypeFunc.h>
#include<hgl/thread/Atomic.h>
#include <atomic>        // added for std::atomic used by ArrayRefCount and RefCount
#include <memory>        // for std::unique_ptr
#include <vector>        // for AutoDeleteObjectArray
#include <algorithm>     // for std::fill, std::for_each

namespace hgl
{
    /**
     * 基础引用计数控制块（非数组），提供strong/weak计数。
     * NOTE: 已统一使用std::atomic确保与数组版本一致；不再允许weak升级“复活”已销毁对象。
     */
    struct RefCount
    {
        std::atomic<int> count{1};   // strong 引用数
        std::atomic<int> weak{0};    // weak 引用数

    public:

        RefCount()=default;
        virtual ~RefCount()=default;

        virtual void Delete()=0;     // 仅在 strong 计数从1->0 时调用一次；实现需将 data 置空防止复活

        int inc_ref()
        {
            // 强引用复制：调用者应保证对象未过期；这里不再做额外检查以保持开销最小
            return count.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        virtual int unref()
        {
            int old=count.fetch_sub(1,std::memory_order_acq_rel);
            if(old==1)                      // 变为0
            {
                Delete();                   // 销毁对象本体，并将data置空
                if(weak.load(std::memory_order_acquire)==0)
                    delete this;            // 无weak，释放控制块
                return 0;
            }
            return old-1;
        }

        int inc_ref_weak()
        {
            return weak.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        int unref_weak()
        {
            int old=weak.fetch_sub(1,std::memory_order_acq_rel);
            if(old==1) // 变为0
            {
                if(count.load(std::memory_order_acquire)==0)
                {
                    delete this;            // strong也为0 -> 释放控制块
                    return 0;
                }
                return 0;                   // strong仍存活，控制块继续存在
            }
            return old-1;
        }
    };//struct RefCount

    // New control block using std::atomic specifically for array shared/weak pointers (保持，与RefCount实现方式统一)
    struct ArrayRefCount
    {
        std::atomic<int> count{1};
        std::atomic<int> weak{0};

        ArrayRefCount()=default;
        virtual ~ArrayRefCount()=default;
        virtual void Delete()=0;

        int inc_ref()
        {
            return count.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        virtual int unref()
        {
            int old=count.fetch_sub(1,std::memory_order_acq_rel);
            if(old==1)
            {
                Delete();
                if(weak.load(std::memory_order_acquire)==0)
                    delete this;
                return 0;
            }
            return old-1;
        }

        int inc_ref_weak()
        {
            return weak.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        int unref_weak()
        {
            int old=weak.fetch_sub(1,std::memory_order_acq_rel);
            if(old==1)
            {
                if(count.load(std::memory_order_acquire)==0)
                {
                    delete this;
                    return 0;
                }
                return 0; // weak count now zero, but strong still exist
            }
            return old-1;
        }
    };//struct ArrayRefCount

    template<typename T> struct SmartData:public RefCount
    {
        T *data;

    public:

        SmartData(T *ptr)
        {
            data=ptr;
        }

        // 控制块析构时对象早已释放（或为空），不要再调用 Delete 以免二次 delete
        ~SmartData() override = default;

        void Delete() override
        {
            SAFE_CLEAR(data);   // 将data置空防止“复活”
        }

        // 尝试从 weak 升级：只有在 strong count>0 且 data 非空时才成功
        bool lock()
        {
            for(;;)
            {
                int cur=count.load(std::memory_order_acquire);
                if(cur<=0||!data) return false;                // 已过期
                if(count.compare_exchange_weak(cur,cur+1,std::memory_order_acq_rel))
                    return true;                               // 成功+1
            }
        }

        bool expired() const { return data==nullptr; }
    };//struct template<typename T> struct SmartData

    template<typename T> struct SmartArrayData:public ArrayRefCount
    {
        T *data;

    public:

        SmartArrayData(T *ptr)
        {
            data=ptr;
        }

        ~SmartArrayData() override = default;      // 不再在析构里重复 Delete

        void Delete() override
        {
            SAFE_CLEAR_ARRAY(data);                // 将data置空
        }

        const T &operator *() const {return data;}
        const bool operator!() const{return !data;}

        bool lock()
        {
            for(;;)
            {
                int cur=count.load(std::memory_order_acquire);
                if(cur<=0||!data) return false;
                if(count.compare_exchange_weak(cur,cur+1,std::memory_order_acq_rel))
                    return true;
            }
        }

        bool expired() const { return data==nullptr; }
    };//struct template<typename T> struct SmartArrayData

    template<typename SD,typename T> class _Smart
    {
    protected:

        using SelfClass=_Smart<SD,T>;

        SD *sd;

    public:

        _Smart()
        {
            sd=0;
        }

        _Smart(T *ptr)
        {
            if(ptr)
                sd=new SD(ptr);
            else
                sd=0;
        }

        _Smart(const SelfClass &st)
        {
            sd=0;

            set(st);
        }

        virtual ~_Smart()=default;

        T *set(T *ptr)
        {
            if(sd)
                sd->unref();

            if(!ptr)
            {
                sd=0;
                return(0);
            }

            sd=new SD(ptr);

            return ptr;
        }

        void inc_ref(const SelfClass &sc)
        {
            if(sd==sc.sd)return;

            unref();

            sd=sc.sd;

            if(sd)
                sd->inc_ref();
        }

        void unref()
        {
            if(sd)
            {
                sd->unref();
                sd=0;
            }
        }

        void inc_ref_weak(const SelfClass &sc)
        {
            if(sd==sc.sd)return;

            unref();

            sd=sc.sd;

            if(sd)
                sd->inc_ref_weak();
        }

        void unref_weak()
        {
            if(sd)
            {
                sd->unref_weak();
                sd=0;
            }
        }

        // 从 weak 尝试升级；成功则返回 true。
        bool try_lock_from_weak(const SelfClass &weak_sc)
        {
            if(sd==weak_sc.sd)
            {
                // 已经指向同一个；若对象已过期，置为空
                if(sd && sd->expired())
                {
                    unref(); // 强引用释放
                    return false;
                }
                return sd!=nullptr;
            }

            unref();

            if(!weak_sc.sd) { sd=nullptr; return false; }

            if(weak_sc.sd->lock())
            {
                sd=weak_sc.sd; // 已经加过 strong count
                return true;
            }

            sd=nullptr;
            return false;
        }

                T *     get         ()const{return sd?sd->data:0;}
          const T *     const_get   ()const{return sd?sd->data:0;}
        virtual bool    valid       ()const{return sd && sd->data;}        // 统一语义：对象存在
                bool    expired     ()const{return !(sd && sd->data);}      // 便于 WeakPtr 使用
                int     use_count   ()const{return sd?sd->count.load(std::memory_order_acquire):-1;}
                bool    only        ()const{return sd?sd->count.load(std::memory_order_acquire)==1:true;}

    public:

        const   T &     operator *  ()const{return *(sd->data);}
        const   bool    operator !  ()const{return !(sd && sd->data);} // 与valid一致

                        operator T *()const{return(sd?sd->data:0);}    // 保持兼容（可能考虑移除）
                T *     operator -> ()const{return(sd?sd->data:0);}    

                bool    operator == (const SelfClass & rp)const{return(get()==rp.get());   }
                bool    operator == (const T *         rp)const{return(get()==rp);         }

                bool    operator != (const SelfClass & rp)const{return !(operator==(rp));  }
                bool    operator != (const T *         rp)const{return !(operator==(rp));  }
    };//template <typename T> class _Smart

    template<typename T> class WeakPtr;

    /**
    * 共享指针数据类<br>
    * 用于自动释放超出作用域的指针
    */
    template<typename T> class SharedPtr:public _Smart<SmartData<T>,T>                              ///共享指针数据类
    {
        friend class WeakPtr<T>;

    public:

        typedef _Smart<SmartData<T>,T> SuperClass;
        typedef SharedPtr<T> SelfClass;

    public:

        SharedPtr():SuperClass(){}
        SharedPtr(T *ptr):SuperClass(ptr){}
        SharedPtr(const SelfClass &sp):SuperClass()
        {
            operator=(sp);
        }

        SharedPtr(const WeakPtr<T> &wp):SuperClass()
        {
            operator=(wp);      // 使用 lock 语义
        }

        ~SharedPtr()
        {
            SuperClass::unref();
        }

        operator T *()
        {
            return SuperClass::get();
        }

        operator const T *()const
        {
            return SuperClass::get();
        }

        T *operator =(T *ptr)
        {
            return SuperClass::set(ptr);
        }

        SelfClass &operator =(const SelfClass &sp)
        {
            SuperClass::inc_ref(sp);

            return(*this);
        }

        // 从 WeakPtr 升级，如失败则结果为空 SharedPtr
        SelfClass &operator =(const WeakPtr<T> &wp)
        {
            SuperClass::try_lock_from_weak(wp);
            return *this;
        }

        bool valid()const override{return SuperClass::valid();}
    };//template <typename T> class SharedPtr

    template<typename T> class WeakArray;

    /**
    * 共享阵列数据类，它在SharedPtr的基础上增加了[]操作符访问，以及在删除时使用delete[]
    */
    template<typename T> class SharedArray:public _Smart<SmartArrayData<T>,T>                       ///共享阵列数据类
    {
        friend class WeakArray<T>;

    public:

        typedef _Smart<SmartArrayData<T>,T> SuperClass;
        typedef SharedArray<T> SelfClass;

    public:

        SharedArray():SuperClass(){}
        SharedArray(T *ptr):SuperClass(ptr){}
        SharedArray(const SelfClass &sa):SuperClass()
        {
            operator=(sa);
        }

        SharedArray(const WeakArray<T> &wa):SuperClass()
        {
            operator=(wa);
        }

        ~SharedArray()
        {
            SuperClass::unref();
        }

        operator T *()
        {
            return SuperClass::get();
        }

        operator const T *()const
        {
            return SuperClass::get();
        }

        T &operator [](int n)
        {
            return SuperClass::sd->data[n];
        }

        SelfClass &operator =(const SelfClass &sap)
        {
            SuperClass::inc_ref(sap);

            return(*this);
        }

        // 从 WeakArray 升级
        SelfClass &operator =(const WeakPtr<T> &wp)
        {
            SuperClass::try_lock_from_weak(wp);

            return(*this);
        }
    };//template <typename T> class SharedArray

    template<typename T> class WeakPtr:public _Smart<SmartData<T>,T>
    {
        friend class SharedPtr<T>;

    public:

        typedef _Smart<SmartData<T>,T> SuperClass;
        typedef WeakPtr<T> SelfClass;

    public:

        WeakPtr():SuperClass(){}
        WeakPtr(const SharedPtr<T> &sp):SuperClass()
        {
            operator=(sp);
        }

        WeakPtr(const SelfClass &wp):SuperClass()
        {
            operator=(wp);
        }

        ~WeakPtr()
        {
            SuperClass::unref_weak();
        }

        operator T *()
        {
            return SuperClass::get();
        }

        operator const T *()const
        {
            return SuperClass::get();
        }

        virtual SuperClass &operator =(const SharedPtr<T> &sp)
        {
            SuperClass::inc_ref_weak(sp);

            return(*this);
        }

        virtual SelfClass &operator =(const SelfClass &wp)
        {
            SuperClass::inc_ref_weak(wp);

            return(*this);
        }

        bool expired() const { return !SuperClass::valid(); }

        // 获取一个 SharedPtr（若对象已销毁则返回空）
        SharedPtr<T> lock() const
        {
            SharedPtr<T> r;
            if(!this->sd) return r;
            if(this->sd->lock())
                r.SuperClass::sd=this->sd;   // 已增加strong计数
            return r;
        }
    };//template<typename T> class WeakPtr

    template<typename T> class WeakArray:public _Smart<SmartArrayData<T>,T>
    {
        friend class SharedArray<T>;

    public:

        typedef _Smart<SmartArrayData<T>,T> SuperClass;
        typedef WeakArray<T> SelfClass;

    public:

        WeakArray():SuperClass(){}
        WeakArray(const SharedArray<T> &sap):SuperClass()
        {
            operator=(sap);
        }

        WeakArray(const WeakArray<T> &wap):SuperClass()
        {
            operator=(wap);
        }

        ~WeakArray()
        {
            SuperClass::unref_weak();
        }

        operator T *()
        {
            return SuperClass::get();
        }

        operator const T *()const
        {
            return SuperClass::get();
        }

        virtual SuperClass &operator =(const SharedArray<T> &sap)
        {
            SuperClass::inc_ref_weak(sap);

            return(*this);
        }

        virtual SelfClass &operator =(const SelfClass &wap)
        {
            SuperClass::inc_ref_weak(wap);

            return(*this);
        }

        bool expired() const { return !SuperClass::valid(); }

        SharedArray<T> lock() const
        {
            SharedArray<T> r;
            if(!this->sd) return r;
            if(this->sd->lock())
                r.SuperClass::sd=this->sd;
            return r;
        }
    };//template<typename T> class WeakArray

    template<typename T> class AutoDelete
    {
        T *obj;

    public:

        AutoDelete()
        {
            obj=nullptr;
        }

        AutoDelete(T *o)
        {
            obj=o;
        }

        ~AutoDelete()
        {
            if(obj)
                delete obj;
        }

        void operator = (T *o)
        {
            if(obj)
                delete obj;

            obj=o;
        }

        T *operator -> (){return obj;}

        operator T *(){return obj;}
        operator const T *()const{return obj;}
        const bool operator !()const{return !obj;}

        void Discard()
        {
            obj=nullptr;
        }

        T *Finish()
        {
            T *reuslt=obj;
            
            obj=nullptr;

            return reuslt;
        }
    };//template<typename T> class AutoDelete

    // Refactored AutoDeleteArray to use std::unique_ptr<T[]>
    template<typename T> class AutoDeleteArray
    {
        std::unique_ptr<T[]> obj;   // managed array
        size_t size = 0;

    public:
        AutoDeleteArray()=default;

        explicit AutoDeleteArray(const size_t count)
        {
            if(count>0)
            {
                obj = std::unique_ptr<T[]>(new T[count]);
                size = count;
            }
        }

        AutoDeleteArray(T *o,const size_t count)
        {
            obj.reset(o);
            size = count;
        }

        // non-copyable
        AutoDeleteArray(const AutoDeleteArray &) = delete;
        AutoDeleteArray &operator=(const AutoDeleteArray &) = delete;
        // movable
        AutoDeleteArray(AutoDeleteArray &&other) noexcept
        {
            obj = std::move(other.obj);
            size = other.size;
            other.size = 0;
        }
        AutoDeleteArray &operator=(AutoDeleteArray &&other) noexcept
        {
            if(this!=&other)
            {
                obj = std::move(other.obj);
                size = other.size;
                other.size = 0;
            }
            return *this;
        }

        ~AutoDeleteArray()=default;

        void set(T *o,const size_t count)
        {
            obj.reset(o);
            size = count;
        }

        T *alloc(const size_t count)
        {
            if(count==0)
            {
                obj.reset();
                size=0;
                return nullptr;
            }
            obj.reset(new T[count]);
            size=count;
            return obj.get();
        }

        T *operator -> (){return obj.get();}

        T *data(){return obj.get();}
        operator T *(){return obj.get();}
        operator const T *()const{return obj.get();}
        const bool operator !()const{return !obj;}

        T *begin(){return obj.get();}
        T *end(){return obj.get()?obj.get()+size:nullptr;}

        void zero()
        {
            if(obj)
                std::fill(begin(), end(), T());
        }

                T &operator[](int index){return obj[index];}
        const   T &operator[](int index)const{return obj[index];}

        void Discard()
        {
            obj.reset();
            size=0;
        }

        size_t length() const { return size; }
    };//template<typename T> class AutoDeleteArray
    
    // Refactored AutoDeleteObjectArray to use std::vector<T*> with RAII cleanup
    template<typename T> class AutoDeleteObjectArray
    {
        std::vector<T*> items;   // each element owned and deleted in destructor

    public:
        AutoDeleteObjectArray()=default;
        explicit AutoDeleteObjectArray(uint c)
        {
            if(c>0)
            {
                items.resize(c,nullptr);
            }
        }
        AutoDeleteObjectArray(T **o,uint c)
        {
            if(o && c>0)
            {
                items.assign(o,o+c); // copy pointers, take ownership
                // caller should not delete *o elements afterwards
            }
        }

        // non-copyable (ownership unique)
        AutoDeleteObjectArray(const AutoDeleteObjectArray&) = delete;
        AutoDeleteObjectArray &operator=(const AutoDeleteObjectArray&) = delete;
        // movable
        AutoDeleteObjectArray(AutoDeleteObjectArray &&other) noexcept
        {
            items.swap(other.items);
        }
        AutoDeleteObjectArray &operator=(AutoDeleteObjectArray &&other) noexcept
        {
            if(this!=&other)
                items.swap(other.items);
            return *this;
        }

        ~AutoDeleteObjectArray()
        {
            for(auto &p:items){ delete p; p=nullptr; }
        }

        T **operator -> (){return items.data();}

        T **data(){return items.data();}
        operator T **(){return items.data();}
        operator const T * const *()const{return items.data();}
        const bool operator !()const{return items.empty();}

                T *&operator[](int index){return items[index];}
        const   T *operator[](int index)const{return items[index];}

        void Discard()
        {
            for(auto &p:items){ delete p; p=nullptr; }
            items.clear();
        }

        void DiscardObject()
        {
            Discard();
        }

        uint count() const { return static_cast<uint>(items.size()); }
        void resize(uint c){ items.resize(c,nullptr); }
        T **begin(){ return items.data(); }
        T **end(){ return items.data()+items.size(); }
    };//template<typename T> class AutoDeleteObjectArray

    template<typename T> class AutoFree
    {
        T *obj;

    public:

        AutoFree()
        {
            obj=nullptr;
        }

        AutoFree(T *o)
        {
            obj=o;
        }

        ~AutoFree()
        {
            if(obj)
                free(obj);
        }        

        void operator = (T *o)
        {
            if(obj)
                free(obj);

            obj=o;
        }

        T *data(){return obj;}
        T *operator -> (){return obj;}

        operator T *(){return obj;}
        operator const T *()const{return obj;}
        const bool operator !()const{return !obj;}

        void Discard()
        {
            obj=nullptr;
        }

        T *Finish()
        {
            T *reuslt=obj;
            
            obj=nullptr;

            return reuslt;
        }
    };//class AutoFree
}//namespace hgl
