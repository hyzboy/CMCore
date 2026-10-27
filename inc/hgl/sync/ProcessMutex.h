#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    /**
     * @brief CN: 操作系统命名的跨进程互斥对象。
     *        EN: OS-named inter-process mutex.
     * @details CN: 与 hgl::Mutex 不同，此锁可由多个进程通过相同名称打开并协调访问。
     *              Create 用于创建/打开命名对象；成功后才可 Lock、TryLock 或 Unlock。
     *              Clear 关闭当前对象持有的系统句柄，不会替代 Unlock；命名规则与
     *              对象共享范围由具体平台实现决定。
     *          EN: Unlike hgl::Mutex, multiple processes can open this mutex by the same
     *              name to coordinate access. Create creates or opens the named object;
     *              Lock, TryLock and Unlock are valid after successful creation. Clear
     *              closes this object's OS handle and is not a substitute for Unlock.
     *              Naming and sharing scope depend on the platform implementation.
     */
    class ProcessMutex                                                                              ///< @brief CN: 跨进程命名互斥对象。 EN: Named inter-process mutex.
    {
        proc_mutex_ptr lock;

    public:

        ProcessMutex();
        ~ProcessMutex(){Clear();}

        bool Create(const os_char *);                                                               ///< @brief CN: 按操作系统名称创建或打开跨进程互斥对象。 EN: Create or open a named inter-process mutex.
        void Clear();                                                                               ///< @brief CN: 关闭当前进程持有的原生句柄。 EN: Close the native handle held by this process.

        bool Lock();                                                                                ///< @brief CN: 阻塞获取互斥锁。 EN: Acquire the mutex, blocking if necessary.
        bool TryLock();                                                                             ///< @brief CN: 非阻塞尝试获取互斥锁。 EN: Try to acquire the mutex without blocking.

        bool Unlock();                                                                              ///< @brief CN: 释放当前持有的互斥锁。 EN: Release the mutex currently held by this process.
    };//class ProcessMutex
}//namespace hgl
