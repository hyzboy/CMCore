#pragma once

#include<hgl/thread/CondVar.h>
#include<hgl/thread/ThreadMutex.h>
#include<hgl/time/Time.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    /**
    * Multi-producer, single-consumer batch mailbox.
    * Posted values are copied into an unbounded pending batch. The consumer
    * swaps batches under a mutex and processes the receive batch without a lock.
    */
    template<typename T>
    class SwapMailbox
    {
        ValueArray<T> buffers[2];
        int write_index=0;
        int receive_index=1;
        bool receive_active=false;
        bool closed=false;
        ThreadMutex lock;
        CondVar changed;

        bool SwapLocked()
        {
            if(receive_active||buffers[write_index].IsEmpty())
                return(false);

            const int old_receive=receive_index;
            receive_index=write_index;
            write_index=old_receive;
            receive_active=true;
            return(true);
        }

    public:

        bool Post(const T &value)
        {
            ThreadMutexLock guard(&lock);

            if(closed)
                return(false);

            buffers[write_index].Add(value);
            changed.Signal();
            return(true);
        }

        int Post(const T *values,int count)
        {
            if(!values||count<=0)
                return(0);

            ThreadMutexLock guard(&lock);

            if(closed)
                return(0);

            const int first=buffers[write_index].Add(values,count);
            if(first<0)
                return(0);

            changed.Signal();
            return(count);
        }

        bool TrySwap()
        {
            ThreadMutexLock guard(&lock);
            return SwapLocked();
        }

        bool WaitAndSwap(double timeout=0)
        {
            ThreadMutexLock guard(&lock);
            const double start=timeout>0?GetUptimeSec():0;

            while(!receive_active&&buffers[write_index].IsEmpty()&&!closed)
            {
                if(timeout<=0)
                {
                    if(!changed.Wait(&lock))
                        return(false);
                }
                else
                {
                    const double remaining=timeout-(GetUptimeSec()-start);
                    if(remaining<=0)
                        return(false);

                    if(!changed.Wait(&lock,remaining)
                       &&buffers[write_index].IsEmpty()&&!closed)
                        return(false);
                }
            }

            return SwapLocked();
        }

        ValueArray<T> &GetReceive(){return buffers[receive_index];}

        void ReleaseReceive()
        {
            ThreadMutexLock guard(&lock);

            if(receive_active)
            {
                buffers[receive_index].Clear();
                receive_active=false;
            }
        }

        void Close()
        {
            ThreadMutexLock guard(&lock);
            closed=true;
            changed.Broadcast();
        }

        bool IsClosed()
        {
            ThreadMutexLock guard(&lock);
            return closed;
        }

        int GetPendingCount()
        {
            ThreadMutexLock guard(&lock);
            return buffers[write_index].GetCount();
        }
    };//class SwapMailbox
}//namespace hgl
