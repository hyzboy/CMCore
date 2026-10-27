#include<hgl/sync/Semaphore.h>
#include<wchar.h>
#pragma warning(disable:4800)            // int -> bool 性能损失警告

namespace hgl
{
    /**
    * @param max_count 最大计数
    */
    Semaphore::Semaphore(int max_count)
    {
        if(max_count<=0)
        {
            LogError(OS_TEXT("CreateSemaphore error,max_count must be positive."));
            return;
        }

        this->max_count=max_count;
        ptr=CreateSemaphoreW(nullptr,0,max_count,nullptr);

        if(!ptr)
            LogError(OS_TEXT("CreateSemaphore error,max_count=")+OSString::numberOf(max_count));
    }

    Semaphore::~Semaphore()
    {
        if(ptr)
            CloseHandle(ptr);
    }

    /**
    * 发送信号
    * @param n 发送的信号数量
    * @return 是否释放成功
    */
    bool Semaphore::Post(int n)
    {
        // CN: 先在 HGL 计数器中预留名额，再通知系统信号量；系统调用失败时回滚预留。
        // EN: Reserve permits in the HGL counter before signaling the OS semaphore; roll back if the system call fails.
        if(!ptr||!ReservePost(n))return(false);

        if(ReleaseSemaphore(ptr,n,nullptr))
            return(true);

        UndoPost(n);
        return(false);
    }

    /**
    * 尝试获取一个信号
    * @return 是否有取得信号
    */
    bool Semaphore::TryAcquire()
    {
        if(!ptr)return false;

        if(WaitForSingleObject(ptr,0)!=WAIT_OBJECT_0)
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
        if(!ptr)return false;

        if(WaitForSingleObject(ptr,INFINITE)!=WAIT_OBJECT_0)
            return(false);

        Consumed();
        return(true);
    }

    /**
    * 等待并获取一个信号
    * @param time 等待的最长时间,使用0表示无限等待.(单位秒)
    * @return 是否等待到了,如果超过最长时间,仍未等到即为超时,返回false
    */
    bool Semaphore::Acquire(double time_out)
    {
        if(!ptr)return false;

        if(time_out<=0)
            return Acquire();

        const double timeout_ms=time_out*1000.0;
        const DWORD wait_time=timeout_ms>=double(INFINITE-1)?INFINITE-1:(timeout_ms<1.0?1:DWORD(timeout_ms));

        if(WaitForSingleObject(ptr,wait_time)!=WAIT_OBJECT_0)
            return(false);

        Consumed();
        return(true);
    }
}//namespace hgl
