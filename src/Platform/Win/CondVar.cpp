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
