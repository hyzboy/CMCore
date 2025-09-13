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
     *  2. weak  为弱引用控制块计数(>=0)。
     *  3. 当 count 递减到 0 时：调用派生类的 Delete() 释放真实对象，并将数据指针置空，防止“复活”。
     *  4. 当 count==0 且 weak==0 时：释放控制块自身(delete this)。
     *
     * 线程安全：
     *  - 所有计数操作使用原子，memory_order 采用 acq_rel（增/减）+ acquire（读）保证可见性。
     *  - 不提供 ABA 级别的严格保障（与标准 shared_ptr 一样我们假定整数溢出不可发生）。
     *
     * 注意：
     *  - Delete() 必须只析构对象本体，并清空指针。不要在派生析构函数再重复 Delete()。
     *  - 不支持自定义删除器；如需可在 SmartData 内部封装。
     */
    struct RefCount
    {
        atom_int count{1};   // strong 引用数，初始 1 表示创建即拥有一个 SharedPtr
        atom_int weak{0};    // weak 引用数，不含强引用自身

    public:

        RefCount()=default;
        virtual ~RefCount()=default;

        /**
         * Delete(): 强引用计数从 1 -> 0 时调用一次，用于销毁真实对象。
         * 约定：实现中须将底层 data 指针清零，避免 WeakPtr 升级成功后获得已释放指针。
         */
        virtual void Delete()=0;

        /** 增加强引用（假定调用者保证对象未过期） */
        int inc_ref()
        {
            return count.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        /**
         * 释放一个强引用。
         * 返回剩余强引用数量；若返回 0 表示本次释放后对象已析构（控制块可能仍存活，若还有 weak）。
         */
        virtual int unref()
        {
            int old=count.fetch_sub(1,std::memory_order_acq_rel);
            if(old==1)
            {
                // old==1 -> 递减后为 0，最后一个强引用释放
                Delete();
                if(weak.load(std::memory_order_acquire)==0)
                    delete this;        // 无弱引用，控制块失效
                return 0;
            }
            return old-1;
        }

        /** 增加弱引用 */
        int inc_ref_weak()
        {
            return weak.fetch_add(1,std::memory_order_acq_rel)+1;
        }

        /**
         * 释放一个弱引用。
         * 若释放后 weak==0 且 count==0，销毁控制块。
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
                return 0;   // 仍有强引用，控制块保留
            }
            return old-1;
        }
    };//struct RefCount

    /**
     * ArrayRefCount
     * ------------------------------------------------------------
     * 与 RefCount 语义相同，区别在于 Delete() 期望对数组调用 delete[]。
     */
    struct ArrayRefCount
    {
        atom_int count{1};
        atom_int weak{0};

        ArrayRefCount()=default;
        virtual ~ArrayRefCount()=default;
        virtual void Delete()=0;   // 必须 delete[] 或其他数组释放策略

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
                return 0; // 仍有强引用
            }
            return old-1;
        }
    };//struct ArrayRefCount

    /**
     * SmartData<T>
     * ------------------------------------------------------------
     * 非数组对象的控制块+裸指针包装：
     *  - data 指向单个 T。
     *  - Delete() 负责 delete data，并置 data=nullptr。
     *  - lock(): 尝试从弱引用升级为强引用（CAS 增加 count）。对象已过期或 data==nullptr 则失败。
     */
    template<typename T> struct SmartData:public RefCount
    {
        T *data{
#if defined(_MSC_VER)
            nullptr
#else
            nullptr
#endif
        };

    public:

        SmartData(T *ptr)
        {
            data=ptr;
        }

        ~SmartData() override = default;   // 不在析构中重复 Delete()

        void Delete() override
        {
            SAFE_CLEAR(data);
        }

        /**
         * lock(): 从弱升级为强。
         * 返回 true 表示已成功增加强引用；false 表示对象已销毁或竞争失败（被销毁）。
         */
        bool lock()
        {
            for(;;)
            {
                int cur=count.load(std::memory_order_acquire);
                if(cur<=0||!data) return false;        // 已过期
                if(count.compare_exchange_weak(cur,cur+1,std::memory_order_acq_rel))
                    return true;                       // 成功
            }
        }

        bool valid() const
        {
            return data!=nullptr;
        }
    };//struct template<typename T> struct SmartData

    /**
     * SmartArrayData<T>
     * ------------------------------------------------------------
     * 数组对象的控制块包装，语义同 SmartData，但 Delete 使用 SAFE_CLEAR_ARRAY。
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
            SAFE_CLEAR_ARRAY(data);    // 自定义安全宏，确保指针清零
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
    };//struct template<typename T> struct SmartArrayData

    /**
     * _Smart<SD,T>
     * ------------------------------------------------------------
     * 所有 SharedPtr / WeakPtr / SharedArray / WeakArray 的公共基础。
     * 已移除 expired()，统一使用 !valid() 来判断是否已失效。
     */
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
            set(st.get());              // 复制时建立新的控制块（语义：深复制底层裸指针）
        }

        virtual ~_Smart()=default;

        /**
         * set(): 释放旧控制块强引用，使用新裸指针创建独立控制块。
         * 与 shared_ptr 的 reset(ptr) 类似，但不会沿用旧的控制块。
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

        /** 复制强引用（与 shared_ptr 拷贝同义） */
        void inc_ref(const SelfClass &sc)
        {
            if(sd==sc.sd)return;
            unref();
            sd=sc.sd;
            if(sd)
                sd->inc_ref();
        }

        /** 强引用释放 */
        void unref()
        {
            if(sd)
            {
                sd->unref();
                sd=nullptr;
            }
        }

        /** 复制弱引用 */
        void inc_ref_weak(const SelfClass &sc)
        {
            if(sd==sc.sd)return;
            unref();            // 先释放本对象（按当前是强引用还是弱引用）
            sd=sc.sd;
            if(sd)
                sd->inc_ref_weak();
        }

        /** 释放弱引用 */
        void unref_weak()
        {
            if(sd)
            {
                sd->unref_weak();
                sd=nullptr;
            }
        }

        /**
         * try_lock_from_weak(): 尝试把一个弱引用升级为强引用。
         *  - 失败：sd 置为 nullptr。
         *  - 成功：sd 指向同一控制块，count 已加 1。
         */
        bool try_lock_from_weak(const SelfClass &weak_sc)
        {
            if(sd==weak_sc.sd)
            {
                if(sd && !sd->data)  // 已失效
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
                int     use_count   ()const{return sd?sd->count.load(std::memory_order_acquire):-1;}  ///< 强引用计数（空指针返回 -1）
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
    };//template <typename T> class _Smart

    template<typename T> class WeakPtr;   // 前置声明

    /**
     * SharedPtr<T>
     * ------------------------------------------------------------
     * 强引用智能指针：
     *  - 拷贝/赋值共享所有权；析构时减少强引用计数。
     *  - 可通过 WeakPtr 升级（失败则置空）。
     *  - valid() == true 代表底层对象仍存在。
     *  - 不提供线程级别的获取-使用-释放屏障，需要调用方自行设计更高层同步。
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
        SharedPtr(const WeakPtr<T> &wp):SuperClass(){ operator=(wp); }  // 升级构造

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
    };//template <typename T> class SharedPtr

    template<typename T> class WeakArray; // 前置声明

    /**
     * SharedArray<T>
     * ------------------------------------------------------------
     * 针对数组 (T[]) 的强引用智能指针：
     *  - 语义同 SharedPtr，但 Delete 使用 delete[]。
     *  - 提供 operator[] 访问。
     */
    template<typename T> class SharedArray:public _Smart<SmartArrayData<T>,T>
    {
        friend class WeakArray<T>;

    public:

        typedef _Smart<SmartArrayData<T>,T> SuperClass;
        typedef SharedArray<T> SelfClass;

    public:

        SharedArray():SuperClass(){}
        SharedArray(T *ptr):SuperClass(ptr){}  // 恢复隐式指针构造
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
    };//template <typename T> class SharedArray

    /**
     * WeakPtr<T>
     * ------------------------------------------------------------
     * 弱引用指针：
     *  - 不拥有对象生命周期；不阻止对象析构。
     *  - lock() 尝试获取 SharedPtr；若对象已销毁返回空 SharedPtr。
     *  - expired() == true 表示对象已被销毁或当前本身为空。
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

        /** 尝试获取强引用 */
        SharedPtr<T> lock() const
        {
            SharedPtr<T> r;
            if(!this->sd) return r;
            if(this->sd->lock())
                r.SuperClass::sd=this->sd;   // 已增加 strong 计数
            return r;
        }
    };//template<typename T> class WeakPtr

    /**
     * WeakArray<T>
     * ------------------------------------------------------------
     * 数组版本弱引用，语义同 WeakPtr。
     */
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
    };//template<typename T> class WeakArray

    /**
     * AutoDelete<T>
     * ------------------------------------------------------------
     * 作用域辅助类：
     *  - 拥有一块动态分配的对象（单个 T）。
     *  - 析构时自动 delete。
     *  - 可通过 Finish() 取出并放弃自动删除。
     *  - 非线程安全，仅用于局部临时管理。
     */
    template<typename T> class AutoDelete
    {
        T *obj;

    public:

        AutoDelete(){ obj=nullptr; }
        AutoDelete(T *o){ obj=o; }   // 允许隐式从 T* 构造，兼容旧代码 AutoDelete<T> x=new T;
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

        /** 取出管理权（不再自动删除） */
        T *Finish()
        {
            T *reuslt=obj;
            obj=nullptr;
            return reuslt;
        }
    };//template<typename T> class AutoDelete

    /**
     * AutoDeleteArray<T>
     * ------------------------------------------------------------
     * RAII 管理 new T[] 分配的数组。
     *  - 内部使用 std::unique_ptr<T[]> 保障异常安全。
     *  - alloc()/set() 重新分配或替换。
     *  - zero() 用缺省值填充。
     *  - Finish 语义不需要，可直接通过 release+自定义包装实现（这里未提供）。
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

        T *begin(){return obj.get();}
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
    };//template<typename T> class AutoDeleteArray
    
    /**
     * AutoDeleteObjectArray<T>
     * ------------------------------------------------------------
     * 维护一组裸指针（元素所有权在此容器内）：
     *  - 析构/Discard() 时逐个 delete。
     *  - 使用场景：需要手工批量管理一组已 new 的对象，又暂不适合使用 vector<unique_ptr<T>>。
     *  - 不可复制，可移动。
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
    };//template<typename T> class AutoDeleteObjectArray

    /**
     * AutoFree<T>
     * ------------------------------------------------------------
     * 与 AutoDelete 类似，但使用 free() 释放（适用于 malloc 分配的内存）。
     * Finish(): 取出所有权，避免析构释放。
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
    };//class AutoFree
}//namespace hgl
