#include<hgl/thread/Thread.h>
#include<hgl/sync/Mutex.h>

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
        // CN: 与 Start 并发时，先经过同一把锁，确保创建线程所需的句柄及对象状态
        //     已由启动方完整发布，再进入派生类回调。
        // EN: When racing with Start, use the same lock so the startup handle and object
        //     state are fully published before entering derived callbacks.
        tc->thread_lock.Lock();
        tc->thread_lock.Unlock();

        // CN: ProcStartThread 可拒绝启动；只有接受后才运行循环并调用结束回调。
        // EN: ProcStartThread may reject startup; only an accepted start runs the loop and end callback.
        if(tc->ProcStartThread())
        {
            // CN: Execute 决定自然结束；stop_requested 由外部请求，用于协作式停止。
            // EN: Execute controls natural exit; stop_requested is an external cooperative-stop request.
            while(tc->Execute()&&!tc->IsStopRequested())
            {
            }

            tc->ProcEndThread();
        }

        // CN: 线程工作体到此结束；平台线程句柄仍须由 Join 回收。
        // EN: The thread body has ended; Join must still reclaim the platform thread handle.
        tc->SetThreadRunning(false);

        return(0);
    }
}//namespace hgl
