#include<hgl/thread/RWLock.h>
#include<pthread.h>

namespace hgl
{
    void GetWaitTime(struct timespec &,double);

    RWLock::RWLock()
    {
        lock=new pthread_rwlock_t;

        pthread_rwlock_init(lock,nullptr);
    }

    RWLock::~RWLock()
    {
        pthread_rwlock_destroy(lock);
    }

    bool RWLock::TryReadLock()  {return !pthread_rwlock_tryrdlock(lock);}

#if HGL_OS != HGL_OS_macOS
    bool RWLock::WaitReadLock(double t)
    {
        struct timespec abstime;

        GetWaitTime(abstime,t);

        return !pthread_rwlock_timedrdlock(lock,&abstime);
    }
#endif//HGL_OS != HGL_OS_macOS

    bool RWLock::ReadLock()     {return !pthread_rwlock_rdlock(lock);}
    bool RWLock::ReadUnlock()   {return !pthread_rwlock_unlock(lock);}

    bool RWLock::TryWriteLock() {return !pthread_rwlock_trywrlock(lock);}

#if HGL_OS != HGL_OS_macOS
    bool RWLock::WaitWriteLock(double t)
    {
        struct timespec abstime;

        GetWaitTime(abstime,t);

        return !pthread_rwlock_timedwrlock(lock,&abstime);
    }
#endif//HGL_OS != HGL_OS_macOS

    bool RWLock::WriteLock()    {return !pthread_rwlock_wrlock(lock);}
    bool RWLock::WriteUnlock()  {return !pthread_rwlock_unlock(lock);}
}//namespace hgl
