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
     * @file
     * @brief
     * CN: 智能指针与自动释放工具集。
     * EN: Smart pointer and auto-release utilities.
     */

    /**
     * @brief RefCount
     * ------------------------------------------------------------
     * CN: 强/弱引用控制块（非数组对象使用）。
     *     设计目标：提供类似 std::shared_ptr/std::weak_ptr 的最小核心语义。
     * EN: Strong/weak reference control block (for non-array objects).
     *     This provides minimal semantics similar to std::shared_ptr/weak_ptr.
     */
    struct RefCount
    {
        atom_int count{1};   // strong 引用数，初始 1 表示创建即拥有一个 SharedPtr
        atom_int weak{0};    // weak 引用数，不含强引用自身

    public:

        RefCount()=default;
        virtual ~RefCount()=default;

        /**
         * @brief CN: 当强引用计数从 1 -> 0 时调用，用于销毁真实对象。
         *        EN: Called once when strong count drops 1 -> 0 to destroy the real
         *            object.
         */
        virtual void Delete()=0;

        /**
         * @brief CN: 增加强引用（假定调用者保证对象未过期）。
         *        EN: Increase strong reference (caller guarantees object not expired).
         * @return 新的强引用计数。
         */
        int inc_ref()
        {
            return count.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        /**
         * @brief CN: 释放一个强引用；返回剩余强引用数量。
         *        EN: Release one strong reference; return remaining strong count.
         */
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

        /**
         * @brief CN: 增加弱引用计数。
         *        EN: Increase weak reference count.
         * @return 新的弱引用计数。
         */
        int inc_ref_weak()
        {
            return weak.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        /**
         * @brief CN: 释放一个弱引用；当弱引用和强引用都为 0 时删除控制块。
         *        EN: Release one weak reference; delete control block when both
         *            weak and strong counts reach zero.
         * @return 剩余弱引用计数。
         */
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


    /**
     * @brief ArrayRefCount
     * CN: 数组对象的强/弱引用控制块（与 RefCount 语义类似但命名区分）。
     * EN: Reference block for array objects (similar semantics to RefCount).
     */
    struct ArrayRefCount
    {
        atom_int count{1};
        atom_int weak{0};

    public:

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


    /**
     * @brief SmartData
     * @tparam T 被管理对象类型。
     *
     * CN: 为单个对象提供引用计数控制块并保存裸指针。Delete()
     *     会释放对象。提供 lock()/valid() 用于弱引用升级语义。
     * EN: Reference-counted control block holding a raw pointer for a single
     *     object. Delete() clears the pointer. lock()/valid() support weak->strong
     *     upgrade semantics.
     */
    template<typename T> struct SmartData:public RefCount
    {
        T *data{nullptr};

    public:

        /**
         * @brief CN: 构造并保存裸指针。
         *        EN: Construct and store the raw pointer.
         */
        SmartData(T *ptr)
        {
            data=ptr;
        }

        ~SmartData() override = default;

        /**
         * @brief CN: 当强引用计数降为 0 时调用以删除对象。
         *        EN: Called when strong count reaches zero to delete the object.
         */
        void Delete() override
        {
            SAFE_CLEAR(data);
        }

        /**
         * @brief CN: 从弱引用尝试升级为强引用（原子检查并增加 count）。
         *        EN: Try to upgrade from weak to strong reference (atomic check
         *            and increment).
         * @return 成功返回 true，否则 false。
         */
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

        /**
         * @brief CN: 检查当前是否指向有效对象（裸指针非空）。
         *        EN: Check whether the underlying pointer is non-null.
         */
        bool valid() const
        {
            return data!=nullptr;
        }
    };


    /**
     * @brief SmartArrayData
     * @tparam T 数组元素类型。
     *
     * CN: 数组版本的引用计数控制块，Delete() 会释放数组。
     * EN: Array-version reference-counted block; Delete() frees the array.
     */
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


    /**
     * @brief _Smart: 智能指针基础模板类。
     * @tparam SD 控制块类型（SmartData 或 SmartArrayData）。
     * @tparam T 目标对象类型。
     *
     * CN: 封装对控制块的引用与基本操作（inc/ref/unref/weak 等），并
     *     提供统一的访问接口 get()/valid()/use_count() 等。
     * EN: Encapsulates handling of the control block and basic operations
     *     (inc/ref/unref/weak) and provides common accessors get()/valid()
     *     use_count(), etc.
     */
    template<typename SD,typename T> class _Smart
    {
    protected:

        using SelfClass=_Smart<SD,T>;

        SD *sd;

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

        /**
         * @brief CN: 设定新的裸指针，替换原有引用块（若存在则 unref）。
         *        EN: Set a new raw pointer, replacing any existing control block
         *            (unref if present).
         * @return 返回传入裸指针。
         */
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

        /**
         * @brief CN: 从另一个 Smart 对象增加强引用并指向同一控制块。
         *        EN: Increase strong reference from another Smart object and point
         *            to the same control block.
         */
        void inc_ref(const SelfClass &sc)
        {
            if(sd==sc.sd)return;
            unref();
            sd=sc.sd;
            if(sd)
                sd->inc_ref();
        }

        /**
         * @brief CN: 释放当前强引用（若存在）。
         *        EN: Release current strong reference (if any).
         */
        void unref()
        {
            if(sd)
            {
                sd->unref();
                sd=nullptr;
            }
        }

        /**
         * @brief CN: 将自身设为对另一个对象的弱引用（增加弱引用计数）。
         *        EN: Become a weak reference to another object (increase weak count).
         */
        void inc_ref_weak(const SelfClass &sc)
        {
            if(sd==sc.sd)return;
            unref();
            sd=sc.sd;
            if(sd)
                sd->inc_ref_weak();
        }

        /**
         * @brief CN: 释放当前弱引用（若存在）。
         *        EN: Release current weak reference (if any).
         */
        void unref_weak()
        {
            if(sd)
            {
                sd->unref_weak();
                sd=nullptr;
            }
        }

        /**
         * @brief CN: 尝试从弱引用升级到强引用（若控制块仍然有效）。
         *        EN: Try to lock a weak reference and obtain a strong reference if
         *            the control block is still valid.
         * @return 成功返回 true 并将 sd 指向控制块，失败返回 false。
         */
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
                T *         get         ()const{return sd?sd->data:nullptr;}            ///< 返回裸指针（可能为空）
          const T *         const_get   ()const{return sd?sd->data:nullptr;}            ///< const 版 get
        virtual bool        valid       ()const{return sd && sd->valid();}              ///< 是否指向一个尚未析构的对象
                std::size_t use_count   ()const{return sd? static_cast<std::size_t>(sd->count.load(std::memory_order_acquire)) : 0;}  ///< 强引用计数（空时返回 0）
                bool        only        ()const{return sd?sd->count.load(std::memory_order_acquire)==1:true;} ///< 是否唯一拥有者

    public:

        // 运算符（兼容性） -------------------------------------------------
        const   T &     operator *  ()const{return *(sd->data);}                    ///< 解引用（调用方需保证 valid==true）
        const   bool    operator !  ()const{return !(sd && sd->valid());}           ///< 与 !valid 语义一致

                        operator T *()const{return(sd?sd->data:nullptr);}           ///< 隐式转换成裸指针（不建议新代码使用）
                T *     operator -> ()const{return(sd?sd->data:nullptr);}           ///< 访问成员

                bool    operator == (const SelfClass & rp)const{return(get()==rp.get());}
                bool    operator == (const T *         rp)const{return(get()==rp);}        ///< 与裸指针比较

                bool    operator != (const SelfClass & rp)const{return !(operator==(rp));}
                bool    operator != (const T *         rp)const{return !(operator==(rp));}
    };

    template<typename T> class WeakPtr;   // 前置声明

    /**
     * @brief SharedPtr
     * @tparam T 被管理对象类型。
     *
     * CN: 强引用智能指针，语义类似 std::shared_ptr。可由 WeakPtr 升级。
     * EN: Strong reference smart pointer, similar to std::shared_ptr. Can be
     *     obtained from WeakPtr via lock().
     */
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

    /**
     * @brief SharedArray
     * @tparam T 数组元素类型。
     *
     * CN: 管理数组的强引用智能指针，支持下标访问与 WeakArray 升级。
     * EN: Strong smart pointer managing arrays; supports operator[] and can be
     *     upgraded from WeakArray.
     */
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

    /**
     * @brief WeakPtr
     * @tparam T 被引用对象类型。
     *
     * CN: 弱引用智能指针，持有弱计数，不影响对象生存周期。可通过
     *     lock() 升级为 SharedPtr。
     * EN: Weak smart pointer holding weak count. Does not affect object lifetime.
     *     Can be upgraded to SharedPtr via lock().
     */
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


    /**
     * @brief AutoDelete
     * @tparam T 被管理对象类型。
     *
     * CN: 简单 RAII 保持裸指针并在析构时 delete。
     * EN: Simple RAII holder that deletes the raw pointer on destruction.
     */
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


    /**
     * @brief AutoDeleteArray
     * @tparam T 数组元素类型。
     *
     * CN: 用 std::unique_ptr<T[]> 管理数组生命周期，并提供若干便捷
     *     接口。保留移动语义，删除拷贝语义。
     * EN: Manages array lifetime with std::unique_ptr<T[]> and provides
     *     convenience methods. Movable but not copyable.
     */
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


    /**
     * @brief AutoDeleteObjectArray
     * @tparam T 对象指针类型。
     *
     * CN: 使用 std::vector<T*> 存储对象指针，在析构或 Discard 时删除
     *     每个对象。移动语义保留，拷贝语义删除。
     * EN: Stores object pointers in std::vector<T*> and deletes each element
     *     on destruction or Discard(). Movable, not copyable.
     */
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


    /**
     * @brief AutoFree
     * @tparam T 被管理指针类型。
     *
     * CN: 使用 free() 释放内存的简单 RAII 工具（适用于 malloc 分配的内存）。
     * EN: Simple RAII tool that frees memory with free() (for malloc'd memory).
     */
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
} // namespace hgl
