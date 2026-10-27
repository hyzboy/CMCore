#include<hgl/thread/Thread.h>
#include<hgl/thread/ThreadMutex.h>

namespace hgl
{
    /**
     *  tips:   PTHREAD_CREATE_DETACHED 方式创建的线程，在退出时，自动清除线程。无法使用pthread_join函数获取运行状态,pthread_join会返回22号错误
     *         PTHREAD_CREATE_JOINABLE 方式创建的线程，在退出时，不会清除线程，必使使用pthread_join函数获取。或是在退出时使用pthread_detach(pthread_self())。
     */

    /**
     * 线程具体执行函数，会依操作系统的不同而有所差别
     */
    THREAD_FUNC ThreadFunc(Thread *tc)
    {
        tc->thread_lock.Lock();
        tc->thread_lock.Unlock();

        if(tc->ProcStartThread())
        {
            while(!tc->IsStopRequested()&&tc->Execute())
            {
            }

            tc->ProcEndThread();
        }

        tc->SetThreadRunning(false);

        return(0);
    }
}//namespace hgl
