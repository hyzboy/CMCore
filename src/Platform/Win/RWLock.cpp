#include<hgl/thread/RWLock.h>

#pragma warning(disable:4800)            // BOOL -> bool 性能损失警告
namespace hgl
{
    RWLock::RWLock()
    {
        lock = new SRWLOCK;

        InitializeSRWLock(lock);
    }

    RWLock::~RWLock()
    {
        delete lock;
    }

    bool RWLock::TryReadLock(){ return TryAcquireSRWLockShared(lock); }
    bool RWLock::ReadLock   (){ AcquireSRWLockShared(lock); return(true); }
    bool RWLock::ReadUnlock (){ ReleaseSRWLockShared(lock); return(true); }

    bool RWLock::TryWriteLock   (){ return TryAcquireSRWLockExclusive(lock); }
    bool RWLock::WriteLock      (){ AcquireSRWLockExclusive(lock); return(true); }
    bool RWLock::WriteUnlock    (){ ReleaseSRWLockExclusive(lock); return(true); }
}//namespace hgl
