#include<hgl/thread/ThreadMutex.h>
#include<pthread.h>
#include<sys/time.h>

namespace hgl
{
    void GetWaitTime(struct timespec &,double);

    ThreadMutex::ThreadMutex()
    {
        ptr=OS_UNFAIR_LOCK_INIT;
    }

    ThreadMutex::~ThreadMutex()
    {
        if(ptr!=OS_UNFAIR_LOCK_INIT)
            os_unfair_lock_unlock(&ptr);
    }

    /**
    * 取得控制权
    * 如果对象处于排斥状态，则等待
    */
    void ThreadMutex::Lock()
    {
        os_unfair_lock_lock(&ptr);
    }

    /**
    * 尝试取得控制权
    * @return 是否成功取得控制权
    */
    bool ThreadMutex::TryLock()
    {
        return os_unfair_lock_trylock(&ptr);
    }

#if !defined(__APPLE__)&&!defined(__ANDROID__)
    /**
    * 等待并取得控制权
    * @param time 等待的最大时间,时间为0表示尝试
    * @return 是否取得控制权
    */
    bool ThreadMutex::WaitLock(double t)
    {
        struct timespec abstime;

        GetWaitTime(abstime,t);

        return !pthread_mutex_timedlock(&ptr, &abstime);
    }
#endif//__APPLE__

    /**
    * 放弃控制权
    */
    void ThreadMutex::Unlock()
    {
        os_unfair_lock_unlock(&ptr);
    }
}//namespace hgl
