#include<hgl/sync/ProcessMutex.h>

namespace hgl
{
    ProcessMutex::ProcessMutex()
    {
        lock = nullptr;
    }

    bool ProcessMutex::Create(const os_char *name)
    {
        if (lock != nullptr)return(false);

        lock = CreateMutexW(NULL, FALSE, name);

        if(GetLastError()==ERROR_ALREADY_EXISTS)
        {
            CloseHandle(lock);
            lock=nullptr;
        }

        return lock;
    }

    void ProcessMutex::Clear()
    {
        if (lock == nullptr)return;

        CloseHandle(lock);

        lock = nullptr;
    }

    bool ProcessMutex::Lock()
    {
        if (lock == nullptr)return(false);

        DWORD rv = WaitForSingleObject(lock, INFINITE);

        if (rv == WAIT_OBJECT_0 || rv == WAIT_ABANDONED)
            return(true);

        return(false);
    }

    bool ProcessMutex::TryLock()
    {
        if (lock == nullptr)return(false);

        DWORD rv = WaitForSingleObject(lock, 0);

        if (rv == WAIT_OBJECT_0 || rv == WAIT_ABANDONED)
            return(true);

        return(false);
    }

    bool ProcessMutex::Unlock()
    {
        if (lock == nullptr)return(false);

        return(!ReleaseMutex(lock));
    }
}//namespace hgl
