#include<hgl/sync/CondVar.h>
#include<hgl/sync/Mutex.h>
#include<pthread.h>

namespace hgl
{
    void GetWaitTime(struct timespec &,double);

    CondVar::CondVar()
    {
        cond_var=new pthread_cond_t;

        pthread_cond_init(cond_var,nullptr);
    }

    CondVar::~CondVar()
    {
        pthread_cond_destroy(cond_var);
        delete cond_var;
    }

    bool CondVar::Wait(Mutex *tm,double t)
    {
        // CN: pthread 条件等待要求调用方已持有 mutex；返回时仍重新持有该锁。
        // EN: pthread condition waits require the caller to hold mutex and return with it reacquired.
        if(t>0)
        {
            struct timespec abstime;

            GetWaitTime(abstime,t);

            return(!pthread_cond_timedwait(cond_var,tm->GetNativeHandle(),&abstime));
        }
        else
        {
            return(!pthread_cond_wait(cond_var,tm->GetNativeHandle()));
        }
    }

    void CondVar::Signal()
    {
        pthread_cond_signal(cond_var);
    }

    void CondVar::Broadcast()
    {
        pthread_cond_broadcast(cond_var);
    }
}//namespace hgl
