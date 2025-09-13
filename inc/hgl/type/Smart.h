#pragma once

#include<hgl/TypeFunc.h>
#include<hgl/thread/Atomic.h>
#include <memory>        // for std::unique_ptr
#include <vector>        // for AutoDeleteObjectArray
#include <algorithm>     // for std::fill, std::for_each
#include <cstddef>
#include <cstdint>

namespace hgl
{
    /**
     * RefCount
     * ------------------------------------------------------------
     * 强/弱引用控制块（非数组对象使用）。
     * 设计目标：提供类似 std::shared_ptr/std::weak_ptr 的最小核心语义：
     *  1. count 为强引用计数(>0 表示对象尚未析构)。
     */
    struct RefCount
    {
        atom_int count{1};   // strong 引用数，初始 1 表示创建即拥有一个 SharedPtr
        atom_int weak{0};    // weak 引用数，不含强引用自身

    public:

        RefCount()=default;
        virtual ~RefCount()=default;

        /** Delete(): 强引用计数从 1 -> 0 时调用一次，用于销毁真实对象。 */
        virtual void Delete()=0;

        /** 增加强引用（假定调用者保证对象未过期） */
        int inc_ref()
        {
            return count.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        /** 释放一个强引用；返回剩余强引用数量 */
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
                return 0;
            }
            return old-1;
        }
    };//struct RefCount

    struct ArrayRefCount
    {
        atom_int count{1};
        atom_int weak{0};

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
                return 0;
            }
            return old-1;
        }
    };//struct ArrayRefCount

    template<typename T> struct SmartData:public RefCount
    {
        T *data{nullptr};

    public:

        SmartData(T *ptr)
        {
            data=ptr;
        }

        ~SmartData() override = default;

        void Delete() override
        {
            SAFE_CLEAR(data);
        }

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

        bool valid() const
        {
            return data!=nullptr;
        }
    };

    template<typename T> struct SmartArrayData:public ArrayRefCount
    {
        T *data{nullptr};

    public:

        SmartArrayData(T *ptr)
        {
            data=ptr;
        }

        ~SmartArrayData() override = default;

        void Delete() override
        {
            SAFE_CLEAR_ARRAY(data);
        }

        const T &operator *() const {return data[0];}
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

        bool valid() const
        {
            return data!=nullptr;
        }
    };

    template<typename SD,typename T> class _Smart
    {
    protected:

        using SelfClass=_Smart<SD,T>; SD *sd;

    public:

        _Smart()
        {
            sd=nullptr;
        }

        _Smart(T *ptr)
        {
            if(ptr)
                sd=new SD(ptr);
            else
                sd=nullptr;
        }

        _Smart(const SelfClass &st)
        {
            sd=nullptr;
            set(st.get());
        }

        virtual ~_Smart()=default;

        T *set(T *ptr)
        {
            if(sd)
                sd->unref();

            if(!ptr)
            {
                sd=nullptr;
                return nullptr;
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
                sd=nullptr;
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
                sd=nullptr;
            }
        }

        bool try_lock_from_weak(const SelfClass &weak_sc)
        {
            if(sd==weak_sc.sd)
            {
                if(sd && !sd->data)
                { unref(); return false; }
                return sd!=nullptr;
            }

            unref();

            if(!weak_sc.sd) { sd=nullptr; return false; }

            if(weak_sc.sd->lock())
            {
                sd=weak_sc.sd;
                return true;
            }

            sd=nullptr;
            return false;
        }

        // 基础访问接口 -------------------------------------------------
                T *     get         ()const{return sd?sd->data:nullptr;}          ///< 返回裸指针（可能为空）
          const T *     const_get   ()const{return sd?sd->data:nullptr;}          ///< const 版 get
        virtual bool    valid       ()const{return sd && sd->valid();}         ///< 是否指向一个尚未析构的对象
                std::size_t use_count   ()const{return sd? static_cast<std::size_t>(sd->count.load(std::memory_order_acquire)) : 0;}  ///< 强引用计数（空时返回 0）
                bool    only        ()const{return sd?sd->count.load(std::memory_order_acquire)==1:true;} ///< 是否唯一拥有者

    public:

        // 运算符（兼容性） -------------------------------------------------
        const   T &     operator *  ()const{return *(sd->data);}             ///< 解引用（调用方需保证 valid==true）
        const   bool    operator !  ()const{return !(sd && sd->valid());}       ///< 与 !valid 语义一致

                        operator T *()const{return(sd?sd->data:nullptr);}          ///< 隐式转换成裸指针（不建议新代码使用）
                T *     operator -> ()const{return(sd?sd->data:nullptr);}          ///< 访问成员

                bool    operator == (const SelfClass & rp)const{return(get()==rp.get());}
                bool    operator == (const T *         rp)const{return(get()==rp);}        ///< 与裸指针比较

                bool    operator != (const SelfClass & rp)const{return !(operator==(rp));}
                bool    operator != (const T *         rp)const{return !(operator==(rp));}
    };

    template<typename T> class WeakPtr;   // 前置声明

    template<typename T> class SharedPtr:public _Smart<SmartData<T>,T>
    {
        friend class WeakPtr<T>;

    public:

        typedef _Smart<SmartData<T>,T> SuperClass;
        typedef SharedPtr<T> SelfClass;

    public:

        SharedPtr():SuperClass(){}
        SharedPtr(T *ptr):SuperClass(ptr){}
        SharedPtr(const SelfClass &sp):SuperClass(){ operator=(sp); }
        SharedPtr(const WeakPtr<T> &wp):SuperClass(){ operator=(wp); }

        ~SharedPtr(){ SuperClass::unref(); }

        operator T *(){ return SuperClass::get(); }
        operator const T *()const{ return SuperClass::get(); }

        T *operator =(T *ptr){ return SuperClass::set(ptr); }

        SelfClass &operator =(const SelfClass &sp)
        {
            SuperClass::inc_ref(sp);
            return *this;
        }

        SelfClass &operator =(const WeakPtr<T> &wp)
        {
            SuperClass::try_lock_from_weak(wp);
            return *this;
        }

        bool valid()const override{return SuperClass::valid();}
    };

    template<typename T> class WeakArray; // 前置声明

    template<typename T> class SharedArray:public _Smart<SmartArrayData<T>,T>
    {
        friend class WeakArray<T>;

    public:

        typedef _Smart<SmartArrayData<T>,T> SuperClass;
        typedef SharedArray<T> SelfClass;

    public:

        SharedArray():SuperClass(){}
        SharedArray(T *ptr):SuperClass(ptr){}
        SharedArray(const SelfClass &sa):SuperClass(){ operator=(sa); }
        SharedArray(const WeakArray<T> &wa):SuperClass(){ operator=(wa); }

        ~SharedArray(){ SuperClass::unref(); }

        operator T *(){ return SuperClass::get(); }
        operator const T *()const{ return SuperClass::get(); }

        T &operator [](std::size_t n){ return SuperClass::sd->data[n]; }

        SelfClass &operator =(const SelfClass &sap)
        {
            SuperClass::inc_ref(sap);
            return *this;
        }

        SelfClass &operator =(const WeakPtr<T> &wp)
        {
            SuperClass::try_lock_from_weak(wp);
            return *this;
        }
    };

    template<typename T> class WeakPtr:public _Smart<SmartData<T>,T>
    {
        friend class SharedPtr<T>;

    public:

        typedef _Smart<SmartData<T>,T> SuperClass;
        typedef WeakPtr<T> SelfClass;

    public:

        WeakPtr():SuperClass(){}
        WeakPtr(const SharedPtr<T> &sp):SuperClass(){ operator=(sp); }
        WeakPtr(const SelfClass &wp):SuperClass(){ operator=(wp); }

        ~WeakPtr(){ SuperClass::unref_weak(); }

        operator T *(){ return SuperClass::get(); }
        operator const T *()const{ return SuperClass::get(); }

        virtual SuperClass &operator =(const SharedPtr<T> &sp)
        {
            SuperClass::inc_ref_weak(sp);
            return *this;
        }

        virtual SelfClass &operator =(const SelfClass &wp)
        {
            SuperClass::inc_ref_weak(wp);
            return *this;
        }

        SharedPtr<T> lock() const
        {
            SharedPtr<T> r;
            if(!this->sd) return r;
            if(this->sd->lock())
                r.SuperClass::sd=this->sd;
            return r;
        }
    };

    template<typename T> class WeakArray:public _Smart<SmartArrayData<T>,T>
    {
        friend class SharedArray<T>;

    public:

        typedef _Smart<SmartArrayData<T>,T> SuperClass;
        typedef WeakArray<T> SelfClass;

    public:

        WeakArray():SuperClass(){}
        WeakArray(const SharedArray<T> &sap):SuperClass(){ operator=(sap); }
        WeakArray(const WeakArray<T> &wap):SuperClass(){ operator=(wap); }

        ~WeakArray(){ SuperClass::unref_weak(); }

        operator T *(){ return SuperClass::get(); }
        operator const T *()const{ return SuperClass::get(); }

        virtual SuperClass &operator =(const SharedArray<T> &sap)
        {
            SuperClass::inc_ref_weak(sap);
            return *this;
        }

        virtual SelfClass &operator =(const SelfClass &wap)
        {
            SuperClass::inc_ref_weak(wap);
            return *this;
        }

        SharedArray<T> lock() const
        {
            SharedArray<T> r;
            if(!this->sd) return r;
            if(this->sd->lock())
                r.SuperClass::sd=this->sd;
            return r;
        }
    };

    template<typename T> class AutoDelete
    {
        T *obj;

    public:

        AutoDelete(){ obj=nullptr; }
        AutoDelete(T *o){ obj=o; }
        ~AutoDelete(){ if(obj) delete obj; }

        void operator = (T *o)
        {
            if(obj) delete obj;
            obj=o;
        }

        T *operator -> (){return obj;}

        operator T *(){return obj;}
        operator const T *()const{return obj;}
        const bool operator !()const{return !obj;}

        void Discard(){ obj=nullptr; }

        T *Finish()
        {
            T *reuslt=obj;
            obj=nullptr;
            return reuslt;
        }
    };

    template<typename T> class AutoDeleteArray
    {
        std::unique_ptr<T[]> obj;   // managed array
        std::size_t size = 0;

    public:
        AutoDeleteArray()=default;

        explicit AutoDeleteArray(const std::size_t count)
        {
            if(count>0)
            {
                obj = std::unique_ptr<T[]>(new T[count]);
                size = count;
            }
        }

        AutoDeleteArray(T *o,const std::size_t count)
        {
            obj.reset(o);
            size = count;
        }

        AutoDeleteArray(const AutoDeleteArray &) = delete;
        AutoDeleteArray &operator=(const AutoDeleteArray &) = delete;
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

        void set(T *o,const std::size_t count)
        {
            obj.reset(o);
            size = count;
        }

        T *alloc(const std::size_t count)
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

        T *data(){return obj.get();
        }
        operator T *(){return obj.get();}
        operator const T *()const{return obj.get();}
        const bool operator !()const{return !obj;}

        T *begin(){return obj.get();
        }
        T *end(){return obj.get()?obj.get()+size:nullptr;}

        void zero()
        {
            if(obj)
                std::fill(begin(), end(), T());
        }

                T &operator[](std::size_t index){return obj[index];}
        const   T &operator[](std::size_t index)const{return obj[index];}

        void Discard()
        {
            obj.reset();
            size=0;
        }

        std::size_t length() const { return size; }
    };

    template<typename T> class AutoDeleteObjectArray
    {
        std::vector<T*> items;   // each element owned and deleted in destructor

    public:
        AutoDeleteObjectArray()=default;
        explicit AutoDeleteObjectArray(std::size_t c)
        {
            if(c>0)
            {
                items.resize(c,nullptr);
            }
        }
        AutoDeleteObjectArray(T **o,std::size_t c)
        {
            if(o && c>0)
            {
                items.assign(o,o+c);
            }
        }

        AutoDeleteObjectArray(const AutoDeleteObjectArray&) = delete;
        AutoDeleteObjectArray &operator=(const AutoDeleteObjectArray&) = delete;
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

                T *&operator[](std::size_t index){return items[index];}
        const   T *operator[](std::size_t index)const{return items[index];}

        void Discard()
        {
            for(auto &p:items){ delete p; p=nullptr; }
            items.clear();
        }

        void DiscardObject() { Discard(); }

        std::size_t count() const { return items.size(); }
        void resize(std::size_t c){ items.resize(c,nullptr); }
        T **begin(){ return items.data(); }
        T **end(){ return items.data()+items.size(); }
    };

    template<typename T> class AutoFree
    {
        T *obj;

    public:

        AutoFree(){ obj=nullptr; }
        explicit AutoFree(T *o){ obj=o; }
        ~AutoFree(){ if(obj) free(obj); }        

        void operator = (T *o)
        {
            if(obj) free(obj);
            obj=o;
        }

        T *data(){return obj;}
        T *operator -> (){return obj;}

        operator T *(){return obj;}
        operator const T *()const{return obj;}
        const bool operator !()const{return !obj;}

        void Discard(){ obj=nullptr; }

        T *Finish()
        {
            T *reuslt=obj;
            obj=nullptr;
            return reuslt;
        }
    };
}//namespace hgl
