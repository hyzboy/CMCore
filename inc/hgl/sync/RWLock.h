#pragma once

#include<hgl/platform/Platform.h>
namespace hgl
{
    /**
     * @brief CN: 读写锁：允许多个读者并行，写者进入时要求独占访问。
     *        EN: Reader-writer lock allowing concurrent readers and exclusive writers.
     * @details CN: 读锁适用于只读临界区，写锁适用于会修改共享状态的临界区。读写锁
     *              不会自动升级/降级锁模式；每次成功加锁都必须使用对应的解锁函数。
     *              对具体平台的公平性、优先级及等待策略不作统一保证。
     *          EN: Use read locks for read-only critical sections and write locks for
     *              shared-state updates. Lock modes are not upgraded or downgraded
     *              automatically; pair each successful acquisition with its matching
     *              unlock. Fairness, priority and wait policy are platform-dependent.
     */
    class RWLock                                                                                    ///< @brief CN: 读写锁。 EN: Reader-writer lock.
    {
        rwlock_ptr *lock;

    public:

        RWLock();
        virtual ~RWLock();

        /** @brief CN: 仅供平台条件变量适配使用；调用方不得释放或替换原生锁。 EN: For platform condition-variable adapters only; callers must not release or replace the native lock. */
        rwlock_ptr *GetRWLock(){ return lock; }                                                     ///< @brief CN: 返回操作系统级锁句柄。 EN: Return the native OS lock handle.

        bool TryReadLock();                                                                         ///< @brief CN: 非阻塞尝试获取读锁。 EN: Try to acquire a read lock without blocking.
        bool ReadLock();                                                                            ///< @brief CN: 获取读锁以进行共享只读访问。 EN: Acquire a read lock for shared read access.
        bool ReadUnlock();                                                                          ///< @brief CN: 释放当前线程持有的读锁。 EN: Release a read lock held by the current thread.

        bool TryWriteLock();                                                                        ///< @brief CN: 非阻塞尝试获取写锁。 EN: Try to acquire a write lock without blocking.
        bool WriteLock();                                                                           ///< @brief CN: 获取写锁以进行独占访问。 EN: Acquire a write lock for exclusive access.
        bool WriteUnlock();                                                                         ///< @brief CN: 释放当前线程持有的写锁。 EN: Release a write lock held by the current thread.

#if (HGL_OS != HGL_OS_Windows)&&(HGL_OS != HGL_OS_macOS)
        bool WaitReadLock(double);                                                                  ///< @brief CN: 限时等待获取读锁，单位秒。 EN: Wait for a read lock with a timeout in seconds.
        bool WaitWriteLock(double);                                                                 ///< @brief CN: 限时等待获取写锁，单位秒。 EN: Wait for a write lock with a timeout in seconds.
#endif//(HGL_OS != HGL_OS_Windows)&&(HGL_OS != HGL_OS_macOS)
    };//class RWLock

    #define HGL_RWLOCK(lock)                bool ReadLock()     {return lock.ReadLock();    }   \
                                            bool WriteLock()    {return lock.WriteLock();   }   \
                                            bool TryReadLock()  {return lock.TryReadLock(); }   \
                                            bool TryWriteLock() {return lock.TryWriteLock();}   \
                                            bool ReadUnlock()   {return lock.ReadUnlock();  }   \
                                            bool WriteUnlock()  {return lock.WriteUnlock(); }

    #define HGL_RWLOCK_PTR(lock)            bool ReadLock()     {return lock->ReadLock();       }   \
                                            bool WriteLock()    {return lock->WriteLock();      }   \
                                            bool TryReadLock()  {return lock->TryReadLock();    }   \
                                            bool TryWriteLock() {return lock->TryWriteLock();   }   \
                                            bool ReadUnlock()   {return lock->ReadUnlock();     }   \
                                            bool WriteUnlock()  {return lock->WriteUnlock();    }

    #define HGL_NULL_RWLOCK                 bool ReadLock()     {return true;}  \
                                            bool WriteLock()    {return true;}  \
                                            bool TryReadLock()  {return true;}  \
                                            bool TryWriteLock() {return true;}  \
                                            bool ReadUnlock()   {return true;}  \
                                            bool WriteUnlock()  {return true;}  \

    #define HGL_VIRTUAL_NULL_RWLOCK virtual bool ReadLock()     {return true;}  \
                                    virtual bool WriteLock()    {return true;}  \
                                    virtual bool TryReadLock()  {return true;}  \
                                    virtual bool TryWriteLock() {return true;}  \
                                    virtual bool ReadUnlock()   {return true;}  \
                                    virtual bool WriteUnlock()  {return true;}  \

    /**
     * @brief CN: 与读写锁关联的指针容器。
     *        EN: Pointer holder associated with a reader-writer lock.
     * @details CN: operator-> 不会自动获取读锁或写锁；访问数据前须由调用方按读写
     *              语义显式加锁。构造时接收的指针由此对象负责 delete。
     *          EN: operator-> does not acquire a read or write lock; callers must
     *              explicitly lock according to their access semantics. The supplied
     *              pointer is owned and deleted by this object.
     */
    template<typename T> class RWLockObject:public RWLock
    {
        T *data;

    public:

        RWLockObject()
        {
            data=new T;
        }

        RWLockObject(T *t)
        {
            data=t;
        }

        virtual ~RWLockObject()
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
    };//class RWLockObject

    /**
     * @brief CN: 读锁作用域守卫，构造时获取共享读锁并在析构或 Unlock 时释放。
     *        EN: Scoped read-lock guard that acquires on construction and releases on destruction or Unlock.
     * @note CN: 守卫不可复制；其关联 RWLock 必须比守卫活得更久。
     *       EN: The guard is non-copyable; its RWLock must outlive the guard.
     */
    class ReadLockGuard
    {
        RWLock *rw;

    public:

        ReadLockGuard(const ReadLockGuard &)=delete;
        ReadLockGuard &operator=(const ReadLockGuard &)=delete;

        ReadLockGuard(RWLock &rw_lock)
        {
            rw=&rw_lock;

            rw->ReadLock();
        }

        ReadLockGuard(RWLock *rw_lock)
        {
            rw=rw_lock;

            rw->ReadLock();
        }

        ~ReadLockGuard()
        {
            Unlock();
        }

        void Unlock()
        {
            if(rw)
            {
                rw->ReadUnlock();
                rw=nullptr;
            }
        }
    };//class ReadLockGuard

    /**
     * @brief CN: 写锁作用域守卫，构造时获取独占写锁并在析构或 Unlock 时释放。
     *        EN: Scoped write-lock guard that acquires on construction and releases on destruction or Unlock.
     * @note CN: 守卫不可复制；不要在同一临界区对同一把非递归读写锁重复加写锁。
     *       EN: The guard is non-copyable; do not recursively acquire the same non-recursive RWLock.
     */
    class WriteLockGuard
    {
        RWLock *rw;

    public:

        WriteLockGuard(const WriteLockGuard &)=delete;
        WriteLockGuard &operator=(const WriteLockGuard &)=delete;

        WriteLockGuard(RWLock &rw_lock)
        {
            rw=&rw_lock;

            rw->WriteLock();
        }

        WriteLockGuard(RWLock *rw_lock)
        {
            rw=rw_lock;

            rw->WriteLock();
        }

        ~WriteLockGuard()
        {
            Unlock();
        }

        void Unlock()
        {
            if(rw)
            {
                rw->WriteUnlock();
                rw=nullptr;
            }
        }
    };//class WriteLockGuard
}//namespace hgl
