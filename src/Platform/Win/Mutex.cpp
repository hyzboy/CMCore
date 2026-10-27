#include<hgl/sync/Mutex.h>

#include<wchar.h>

#pragma warning(disable:4800)            // BOOL -> bool 性能损失警告
namespace hgl
{
    Mutex::Mutex()
    {
        // CN: 临界区是进程内线程同步原语；跨进程协调请使用 ProcessMutex。
        // EN: A critical section is an in-process synchronization primitive; use ProcessMutex across processes.
        InitializeCriticalSection(&ptr);
    }

    Mutex::~Mutex()
    {
        DeleteCriticalSection(&ptr);
    }

    /**
    * 取得控制权
    * 如果对象处于排斥状态，则等待
    */
    void Mutex::Lock()
    {
        EnterCriticalSection(&ptr);
    }

    /**
    * 尝试取得控制权
    * @return 是否成功取得控制权
    */
    bool Mutex::TryLock()
    {
        return(TryEnterCriticalSection(&ptr));
    }

    /**
    * 等待并取得控制权
    * @param time 等待的最大时间,时间为0表示尝试
    * @return 是否取得控制权
    */
    bool Mutex::WaitLock(double time_out)
    {
        if(time_out<=0)
            return TryLock();

        const double timeout_ms=time_out*1000.0;
        const DWORD wait_ms=timeout_ms>=double(MAXDWORD)?MAXDWORD:(timeout_ms<1.0?1:DWORD(timeout_ms));
        const DWORD start=GetTickCount();

        for(;;)
        {
            if(TryLock())
                return true;

            if(DWORD(GetTickCount()-start)>=wait_ms)
                return false;

            Sleep(1);
        }
    }

    /**
    * 放弃控制权
    */
    void Mutex::Unlock()
    {
        LeaveCriticalSection(&ptr);
    }
}//namespace hgl
