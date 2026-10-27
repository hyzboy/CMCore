#include<hgl/sync/ProcessMutex.h>
#include<sys/fcntl.h>
#include<sys/stat.h>
#include<errno.h>

namespace hgl
{
    ProcessMutex::ProcessMutex()
    {
        lock = nullptr;
    }

    bool ProcessMutex::Create(const os_char *name)
    {
        if (lock != nullptr)return(false);

        lock = sem_open(name, O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);

        if(lock!=SEM_FAILED)
            return(true);

        lock = nullptr;
        return(false);
    }

    void ProcessMutex::Clear()
    {
        if (lock == nullptr)return;

        sem_close(lock);

        lock = nullptr;
    }

    bool ProcessMutex::Lock()
    {
        if (lock == nullptr)return(false);

        int err;
        do
        {
            err = sem_wait(lock);
        }
        while (err && errno == EINTR);

        return !err;
    }

    bool ProcessMutex::TryLock()
    {
        if (lock == nullptr)return(false);

        return(sem_trywait(lock)==0);
    }

    bool ProcessMutex::Unlock()
    {
        if (lock == nullptr)return(false);

        return(sem_post(lock)==0);
    }
}//namespace hgl
