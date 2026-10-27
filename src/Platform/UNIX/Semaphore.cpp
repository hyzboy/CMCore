#include<hgl/sync/Semaphore.h>
#include<hgl/log/LogInfo.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>

namespace hgl
{
    void GetWaitTime(struct timespec &,double);

    /**
    * @param max_count 最大计数
    */
    Semaphore::Semaphore(int max_count)
    {
        this->max_count=max_count;

        if(max_count<=0)
        {
            LOG_ERROR(OS_TEXT("sem_init error,max_count must be positive."));
            return;
        }

        ptr=new sem_t;

        if(sem_init(ptr,PTHREAD_PROCESS_PRIVATE,0))
        {
            LOG_ERROR(OS_TEXT("sem_init error,max_count=")+OSString::valueOf(max_count));
            delete ptr;
            ptr=nullptr;
        }
    }

    Semaphore::~Semaphore()
    {
        if(!ptr)return;

        sem_destroy(ptr);
        delete ptr;
    }

    /**
    * 发送信号
    * @param n 发送的信号数量
    * @return 是否释放成功
    */
    bool Semaphore::Post(int n)
    {
        // CN: POSIX semaphore 没有跨平台的批量 post，逐个发布；失败时撤销尚未发布的计数。
        // EN: POSIX has no portable multi-post operation; post permits individually and roll back the remaining count on failure.
        if(!ptr||!ReservePost(n))return(false);

        for(int i=0;i<n;i++)
            if(sem_post(ptr))
            {
                UndoPost(n-i);
                return false;
            }

        return true;

        //if(n==1)
        //  return !sem_post(ptr);
        //else
        //  return !sem_post_multiple(ptr,n);  //注：这个函数不是所有os都支持
    }

    /**
    * 尝试获取一个信号
    * @return 是否有取得信号
    */
    bool Semaphore::TryAcquire()
    {
        if(!ptr)return(false);

        int result;
        do
        {
            result=sem_trywait(ptr);
        }while(result&&errno==EINTR);

        if(result)
            return(false);

        Consumed();
        return(true);
    }

    /**
    * 等待并获取一个信号
    * @return 是否等待到了,如果超过最长时间,仍未等到即为超时,返回false
    */
    bool Semaphore::Acquire()
    {
        if(!ptr)return(false);

        int result;
        do
        {
            result=sem_wait(ptr);
        }while(result&&errno==EINTR);

        if(result)
            return(false);

        Consumed();
        return(true);
    }

    /**
    * 等待并获取一个信号
    * @param t 等待的最长时间,使用0表示无限等待.(单位秒)
    * @return 是否等待到了,如果超过最长时间,仍未等到即为超时,返回false
    */
    bool Semaphore::Acquire(double t)
    {
        if(!ptr)return(false);
        if(t<=0)return Acquire();

        struct timespec abstime;

        GetWaitTime(abstime,t);

        int result;
        do
        {
            result=sem_timedwait(ptr,&abstime);
        }while(result&&errno==EINTR);

        if(result)
            return(false);

        Consumed();
        return(true);
    }
}//namespace hgl
