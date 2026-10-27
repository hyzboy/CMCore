#include<hgl/sync/CondVar.h>
#include<hgl/sync/Mutex.h>
#include<hgl/sync/RWLock.h>

#pragma warning(disable:4800)            // BOOL -> bool 性能损失警告
namespace hgl
{
    CondVar::CondVar()
    {
        cond_var = new CONDITION_VARIABLE;
        InitializeConditionVariable(cond_var);
    }

    CondVar::~CondVar()
    {
        delete cond_var;
    }

    bool CondVar::Wait(Mutex *tm, double time)
    {
        // CN: Windows API 会在等待期间原子释放临界区，并在返回前重新获取该锁。
        // EN: The Windows API atomically releases the critical section while waiting and reacquires it before returning.
        return SleepConditionVariableCS(cond_var,tm->GetNativeHandle(),(DWORD)(time>0?time*1000:INFINITE));
    }

    bool CondVar::Wait(RWLock *lock, double time, bool read)
    {
        return SleepConditionVariableSRW(cond_var,lock->GetRWLock(),(DWORD)(time>0?time*1000:INFINITE),read?CONDITION_VARIABLE_LOCKMODE_SHARED:0);
    }

    void CondVar::Signal()
    {
        WakeConditionVariable(cond_var);
    }

    void CondVar::Broadcast()
    {
        WakeAllConditionVariable(cond_var);
    }
}//namespace hgl
