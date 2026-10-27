#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/Macro.h>
namespace hgl
{
    /**
     * @brief CN: 进程内互斥锁：同一时刻仅允许一个线程进入临界区。
     *        EN: In-process mutex that allows only one thread in the critical section at a time.
     * @details CN: Mutex 只提供互斥，不会自动知道或保护某个数据对象；所有访问共享
     *              状态的代码都必须遵守同一把锁的约定。它不是跨进程锁，也不是递归
     *              锁的可移植抽象。通常优先使用 MutexLock 管理锁的获取与释放。
     *          EN: Mutex provides mutual exclusion but does not automatically protect
     *              any particular data object; all shared-state access must follow the
     *              same locking convention. It is neither inter-process nor a portable
     *              recursive mutex. Prefer MutexLock for scoped lock management.
     */
    class Mutex                                                                                     ///< @brief CN: 进程内互斥锁。 EN: In-process mutex.
    {
        thread_mutex_ptr ptr;

    public:

        Mutex();                                                                                    ///< @brief CN: 构造并初始化互斥锁。 EN: Construct and initialize the mutex.
        virtual ~Mutex();                                                                           ///< @brief CN: 析构并释放互斥锁资源。 EN: Destroy the mutex and release its resources.

        /**
         * @brief CN: 返回平台原生锁句柄的地址，供 CondVar 等平台适配代码使用。
         *        EN: Return the address of the native lock handle for adapters such as CondVar.
         * @note CN: 普通调用方不得修改或释放该句柄；句柄使用期间不得销毁 Mutex。
         *       EN: Callers must not modify or release the handle or destroy Mutex while it is in use.
         */
        thread_mutex_ptr *GetNativeHandle(){return &ptr;}

        virtual void    Lock();                                                                     ///< @brief CN: 阻塞直到获取锁；成功后由当前持有者 Unlock。 EN: Block until acquired; the owning thread must call Unlock.
        virtual bool    TryLock();                                                                  ///< @brief CN: 立即尝试加锁；false 表示当前未能获取。 EN: Try to acquire immediately; false means acquisition failed.
#if !defined(__APPLE__)&&!defined(__ANDROID__)
        virtual bool    WaitLock(const double=0);                                                   ///< @brief CN: 限时获取锁，单位秒；非正数表示仅尝试一次。 EN: Acquire with a timeout in seconds; non-positive values perform one immediate attempt.
#endif//__APPLE__
        virtual void    Unlock();                                                                   ///< @brief CN: 释放当前线程持有的锁；未持有时调用属于错误用法。 EN: Release a lock owned by the current thread; calling without ownership is invalid.
    };//class Mutex

    /**
     * @brief CN: 与一把 Mutex 关联的指针容器。
     *        EN: Pointer holder associated with a Mutex.
     * @details CN: operator-> 只返回数据指针，不会自动加锁；同步访问数据时，调用方
     *              仍须显式持有继承来的 Mutex。构造时接收的指针由本对象负责销毁，
     *              因此传入的指针必须可由 delete 释放，且不能被其他所有者重复释放。
     *          EN: operator-> only returns the data pointer and does not lock. Callers
     *              must explicitly hold the inherited Mutex when accessing shared data.
     *              The supplied pointer is owned and deleted by this object; it must
     *              be deletable and must not be released by another owner.
     */
    template<typename T> class MutexObject:public Mutex
    {
        T *data;

    public:

        MutexObject()
        {
            data=new T;
        }

        MutexObject(T *t)
        {
            data=t;
        }

        virtual ~MutexObject()
        {
            SAFE_CLEAR(data);
        }

        void operator = (T *t)
        {
            if(data)
                delete data;

            data=t;
        }

        T *operator ->()
        {
            return data;
        }

        bool valid()const
        {
            return data;
        }

        bool operator !()
        {
            return !data;
        }
    };//class MutexObject

    /**
     * @brief CN: MutexObject 的定长数组封装。
     *        EN: Fixed-size array wrapper for MutexObject.
     * @details CN: 数组只负责创建和销毁各对象，不提供索引边界检查，也不会自动锁定
     *              元素内部数据；并发访问规则与 MutexObject 相同。
     *          EN: The array creates and destroys its elements but performs no bounds
     *              checks and does not lock element data automatically. Concurrency
     *              rules are the same as for MutexObject.
     */
    template<typename T> class MutexObjectArray
    {
    protected:

        MutexObject<T> *items;

    public:

        MutexObjectArray()
        {
            items=nullptr;
        }

        MutexObjectArray(int count)
        {
            if(count<=0)
            {
                items=nullptr;
            }
            else
            {
                items=new MutexObject<T>[count];
            }
        }

        virtual ~MutexObjectArray()
        {
            delete[] items;     //delete nullptr不是个错误
        }

        MutexObject<T> &operator [](int index)
        {
            return items[index];
        }
    };//class MutexObjectArray

    /**
     * @brief CN: Mutex 的作用域锁守卫。
     *        EN: Scoped lock guard for Mutex.
     * @details CN: 构造时若传入非空指针则立即加锁，析构时自动解锁。该类不可复制，
     *              避免多个守卫析构时重复释放同一把锁。Unlock 只释放当前持锁但保留关联；
     *              若需要提前释放并解除关联，应直接调用 Clear，而不是先调用 Unlock。
     *          EN: A non-null mutex is locked on construction and unlocked on destruction.
     *              The guard is non-copyable to prevent duplicate unlocks. Unlock releases
     *              the current lock but retains the association; call Clear directly to
     *              unlock and detach when releasing before scope exit.
     */
    class MutexLock
    {
        Mutex *mutex;

    public:

        MutexLock(const MutexLock &)=delete;
        MutexLock &operator=(const MutexLock &)=delete;

        MutexLock(Mutex *mutex_to_lock)
        {
            mutex=mutex_to_lock;

            if(mutex)
                mutex->Lock();
        }

        ~MutexLock()
        {
            if(mutex)
                mutex->Unlock();
        }

        void Lock()
        {
            if(mutex)
                mutex->Lock();
        }

        void Unlock()
        {
            if(mutex)
                mutex->Unlock();
        }

        void Clear()
        {
            if(mutex)
            {
                mutex->Unlock();
                mutex=nullptr;
            }
        }
    };//class MutexLock
}//namespace hgl
