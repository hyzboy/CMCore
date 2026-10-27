#include<hgl/thread/Thread.h>

namespace hgl
{
    extern THREAD_FUNC ThreadFunc(Thread *tc);

    /**
    * (线程外部调用)执行当前线程，线程优先级为tplevel
    * @param tplevel 线程优先级
    * @return 是否创建线程成功
    */
    bool Thread::Start()
    {
        unsigned long threadid;

        thread_lock.Lock();

        if(tp)
        {
            thread_lock.Unlock();
            LogWarning(OS_TEXT("Thread::Start() error, thread must be joined before restart."));
            return(false);
        }

        // CN: 复用对象前必须已 Join；新一轮启动重新清除上一次停止请求。
        // EN: Join before reusing the object; a new start clears the previous stop request.
        stop_requested.store(false);
        thread_running.store(true);

        tp=::CreateThread(0,0,(PTHREAD_START_ROUTINE)ThreadFunc,this,0,&threadid);

        if(!tp)
        {
            thread_running.store(false);
            thread_lock.Unlock();
            LogWarning(OS_TEXT("Create Thread failed,Windows ErrorCode: ")+OSString::numberOf((uint)GetLastError()));
            return(false);
        }

        thread_lock.Unlock();
        return(true);
    }

    /**
    * 是否是当前线程
    */
    bool Thread::IsCurThread()
    {
        return(tp&&GetThreadId(tp)==GetCurrentThreadId());
    }

    bool Thread::StopAndJoin(const double &time_out)
    {
        // CN: 停止请求只通知工作体，不终止 OS 线程；Join 负责等待并关闭线程句柄。
        // EN: A stop request only notifies the worker; Join waits for exit and closes the OS thread handle.
        RequestStop();
        return Join(time_out);
    }

    bool Thread::Join(const double &time_out)
    {
        // CN: join_lock 保证多个调用者不会同时等待并重复回收同一个原生句柄。
        // EN: join_lock prevents concurrent callers from waiting on and reclaiming the same native handle.
        join_lock.Lock();
        thread_lock.Lock();

        if(!tp)
        {
            thread_lock.Unlock();
            join_lock.Unlock();
            return(true);
        }

        // CN: 先在 thread_lock 下快照句柄，再释放该锁，避免等待期间阻塞线程收尾。
        // EN: Snapshot the handle under thread_lock, then release it so waiting cannot block thread cleanup.
        const thread_ptr joining_thread=tp;
        thread_lock.Unlock();

        if(GetThreadId(joining_thread)==GetCurrentThreadId())
        {
            join_lock.Unlock();
            return(false);
        }

        DWORD wait_time=INFINITE;

        if(time_out>0)
        {
            const double timeout_ms=time_out*1000.0;
            wait_time=timeout_ms>=double(INFINITE-1)?INFINITE-1:DWORD(timeout_ms);
        }

        const DWORD result=WaitForSingleObject(joining_thread,wait_time);

        if(result==WAIT_OBJECT_0)
        {
            thread_lock.Lock();
            if(tp==joining_thread)
            {
                CloseHandle(tp);
                tp=nullptr;
            }
            thread_lock.Unlock();
            join_lock.Unlock();
            return(true);
        }

        if(result==WAIT_FAILED)
            LogError(OS_TEXT("WaitForSingleObject failed, Windows ErrorCode: ")+OSString::numberOf((uint)GetLastError()));

        join_lock.Unlock();
        return(false);
    }

    bool WaitThreadExit(thread_ptr tp,const double &time_out)
    {
        if(!tp)return(true);

        DWORD wait_time=INFINITE;

        if(time_out>0)
        {
            const double timeout_ms=time_out*1000.0;
            wait_time=timeout_ms>=double(INFINITE-1)?INFINITE-1:DWORD(timeout_ms);
        }

        const DWORD result=WaitForSingleObject(tp,wait_time);

        if(result==WAIT_FAILED)
            GLogError(OS_TEXT("WaitForSingleObject failed, Windows ErrorCode: ")+OSString::numberOf((uint)GetLastError()));

        return(result==WAIT_OBJECT_0);
    }

    /**
    * 等待多个线程中的一个完成
    * @param mt 线程
    * @param count 线程数量
    * @param time_out 等待的时间，如果为0表示等到线程运行结束为止。默认为0
    */
    void WaitThread(Thread **mt,int count,double time_out)
    {
        void **obj=new void *[count];

        for(int i=0;i<count;i++)
            obj[i]=mt[i]->tp;

        WaitForMultipleObjects(count,obj,false,time_out>0?time_out*1000:INFINITE);

        delete[] obj;
    }
}//namespace hgl
