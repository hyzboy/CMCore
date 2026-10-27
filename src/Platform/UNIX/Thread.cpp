#include<hgl/thread/Thread.h>
#include<hgl/sync/CondVar.h>
#include<hgl/log/LogInfo.h>
#include<hgl/time/Time.h>
#include<signal.h>
#include<errno.h>
#include<hgl/type/StrChar.h>

namespace hgl
{
    extern THREAD_FUNC ThreadFunc(Thread *tc);

    /**
    * (线程外部调用)执行当前线程
    * @return 是否创建线程成功
    */
    bool Thread::Start()
    {
        thread_lock.Lock();

        if(tp)
        {
            thread_lock.Unlock();
            LOG_ERROR(OS_TEXT("Thread::Start() error,tp!=nullptr."));
            return(false);
        }

        pthread_attr_t attr;

        pthread_attr_init(&attr);

        pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);

        stop_requested.store(false);
        thread_running.store(true);

        if(pthread_create(&tp,&attr,(void *(*)(void *))ThreadFunc,this))        //返回0表示正常
        {
            thread_running.store(false);
            tp=0;
            thread_lock.Unlock();

            pthread_attr_destroy(&attr);
            LOG_ERROR(OS_TEXT("Create Thread (pthread_create) failed.errno:")+OSString::valueOf(errno));
            return(false);
        }

        pthread_attr_destroy(&attr);
        thread_lock.Unlock();
        return(true);
    }

    /**
    * 是否是当前线程
    */
    bool Thread::IsCurThread()
    {
        if(!tp)
        {
            LOG_ERROR(OS_TEXT("Thread::IsCurThread() error,tp=nullptr."));
            return(true);
        }

        return pthread_equal(pthread_self(),tp);        //返回非0表示一致
    }

    void GetWaitTime(struct timespec &abstime,double t);

    bool Thread::StopAndJoin(const double &time_out)
    {
        RequestStop();
        return Join(time_out);
    }

    bool Thread::Join(const double &time_out)
    {
        join_lock.Lock();
        thread_lock.Lock();

        if(!tp)
        {
            thread_lock.Unlock();
            join_lock.Unlock();
            return(true);
        }

        const thread_ptr joining_thread=tp;
        thread_lock.Unlock();

        if(pthread_equal(pthread_self(),joining_thread))
        {
            join_lock.Unlock();
            return(false);
        }

        bool completed=false;

        if(time_out>0)
        {
#if defined(__linux__)&&!defined(__ANDROID__)&&!defined(HGL_OS_BSD)
            struct timespec ts;

            GetWaitTime(ts,time_out);

            void *res=nullptr;
            const int retval=pthread_timedjoin_np(joining_thread,&res,&ts);
            completed=(retval==0);
#else
            const double start=GetUptimeSec();

            while(thread_running.load())
            {
                if(GetUptimeSec()-start>=time_out)
                {
                    join_lock.Unlock();
                    return(false);
                }

                SleepSecond(0.001);
            }

            void *res=nullptr;
            completed=(pthread_join(joining_thread,&res)==0);
#endif//__ANDROID__&&BSD
        }
        else
        {
            void *res=nullptr;
            completed=(pthread_join(joining_thread,&res)==0);
        }

#ifdef _DEBUG

        if(!completed)
            LOG_INFO(U8_TEXT("pthread_timedjoin_np/pthread_join did not complete"));
#endif//_DEBUG

        if(completed)
        {
            thread_lock.Lock();
            tp=0;
            thread_lock.Unlock();
        }

        join_lock.Unlock();
        return(completed);
    }

    bool WaitThreadExit(thread_ptr tp,const double &time_out)
    {
        if(!tp)
            return(true);

        if(time_out>0)
        {
#if defined(__linux__)&&!defined(__ANDROID__)&&!defined(HGL_OS_BSD)
            struct timespec ts;
            GetWaitTime(ts,time_out);
            void *res=nullptr;
            return(pthread_timedjoin_np(tp,&res,&ts)==0);
#else
            return(false);
#endif
        }

        void *res=nullptr;
        return(pthread_join(tp,&res)==0);
    }
}//namespace hgl
