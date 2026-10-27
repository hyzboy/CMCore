#pragma once

#include<hgl/platform/Platform.h>
namespace hgl
{
    class Mutex;
    class RWLock;

    /**
     * @brief CN: 条件变量：在互斥锁保护下等待共享状态发生变化。
     *        EN: Condition variable for waiting on shared state protected by a mutex.
     * @details CN: Wait 会原子地释放传入的锁并进入等待；被唤醒返回前重新获取该锁。
     *              Signal/Broadcast 只是通知，不代表等待条件必然成立，因此调用方应
     *              在持锁状态下使用 while 循环重复检查谓词。超时单位为秒，非正数表示
     *              无限等待。通知可能发生在等待开始前，条件状态本身必须由调用方保存。
     *          EN: Wait atomically releases the supplied lock while waiting and
     *              reacquires it before returning. Signal/Broadcast only notify and do
     *              not guarantee the predicate is true; callers must check the
     *              predicate in a while loop while holding the mutex. Timeouts are in
     *              seconds; non-positive values wait indefinitely. The caller must
     *              store the condition because notifications may precede the wait.
     */
    class CondVar
    {
        cond_var_ptr *cond_var;

    public:

        CondVar();
        virtual ~CondVar();

        bool Wait(Mutex *,double time=0);                                                           ///< @brief CN: 原子释放锁并等待，返回前重新持锁；返回值表示等待结果。 EN: Atomically unlock while waiting and relock before return; the result indicates the wait outcome.

#if HGL_OS == HGL_OS_Windows
        bool Wait(RWLock *,double time=0,bool read=false);                                          ///< @brief CN: Windows 专用：临时释放并重新获取指定读写锁模式。 EN: Windows only; temporarily release and reacquire the selected read/write lock mode.
#endif//HGL_OS == HGL_OS_Windows

        void Signal();                                                                              ///< @brief CN: 通知一个等待线程；不等于条件谓词已经满足。 EN: Notify one waiter; this does not imply its predicate is true.
        void Broadcast();                                                                           ///< @brief CN: 通知所有等待线程；醒来的线程仍须重新获取锁。 EN: Notify all waiters; each awakened thread must still reacquire the lock.
    };//class CondVar
}//namespace hgl
