#include<hgl/thread/CondVar.h>
#include<hgl/thread/ThreadMutex.h>
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

    bool CondVar::Wait(ThreadMutex *tm,double t)
    {
        if(t>0)
        {
            struct timespec abstime;

            GetWaitTime(abstime,t);

            return(!pthread_cond_timedwait(cond_var,tm->GetThreadMutex(),&abstime));
        }
        else
        {
            return(!pthread_cond_wait(cond_var,tm->GetThreadMutex()));
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
