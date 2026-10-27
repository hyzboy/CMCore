#include<hgl/sync/Mutex.h>
#include<pthread.h>
#include<sys/time.h>

namespace hgl
{
    void GetWaitTime(struct timespec &,double);

    Mutex::Mutex()
    {
        pthread_mutexattr_t attr;

        pthread_mutexattr_init(&attr);

        pthread_mutex_init(&ptr,&attr);

        pthread_mutexattr_destroy(&attr);
    }

    Mutex::~Mutex()
    {
        pthread_mutex_destroy(&ptr);
    }

    /**
    * 取得控制权
    * 如果对象处于排斥状态，则等待
    */
    void Mutex::Lock()
    {
        pthread_mutex_lock(&ptr);
    }

    /**
    * 尝试取得控制权
    * @return 是否成功取得控制权
    */
    bool Mutex::TryLock()
    {
        return(!pthread_mutex_trylock(&ptr));
    }

#if !defined(__APPLE__)&&!defined(__ANDROID__)
    /**
    * 等待并取得控制权
    * @param time 等待的最大时间,时间为0表示尝试
    * @return 是否取得控制权
    */
    bool Mutex::WaitLock(double t)
    {
        if(t<=0)
            return TryLock();

        struct timespec abstime;

        GetWaitTime(abstime,t);

        return !pthread_mutex_timedlock(&ptr, &abstime);
    }
#endif//__APPLE__

    /**
    * 放弃控制权
    */
    void Mutex::Unlock()
    {
        pthread_mutex_unlock(&ptr);
    }
}//namespace hgl
