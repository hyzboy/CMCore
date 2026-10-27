#pragma once

#include<hgl/thread/CondVar.h>
#include<hgl/thread/ThreadMutex.h>
#include<hgl/time/Time.h>
#include<hgl/type/Queue.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    /**
    * Multi-producer, multi-consumer task queue for trivially copyable values
    * (including pointers). Close rejects new tasks but lets consumers drain
    * accepted tasks. CloseAndCancel atomically removes pending tasks.
    */
    template<typename T>
    class TaskQueue
    {
        Queue<T> queue;
        ThreadMutex lock;
        CondVar changed;
        bool closed=false;

    public:

        bool Submit(const T &task)
        {
            ThreadMutexLock guard(&lock);

            if(closed)
                return(false);

            if(!queue.Push(task))
                return(false);

            changed.Signal();
            return(true);
        }

        bool TryPop(T &task)
        {
            ThreadMutexLock guard(&lock);
            return queue.Pop(task);
        }

        bool WaitPop(T &task,double timeout=0)
        {
            ThreadMutexLock guard(&lock);
            const double start=timeout>0?GetUptimeSec():0;

            while(queue.IsEmpty()&&!closed)
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

                    if(!changed.Wait(&lock,remaining)&&queue.IsEmpty()&&!closed)
                        return(false);
                }
            }

            return queue.Pop(task);
        }

        void Close()
        {
            ThreadMutexLock guard(&lock);
            closed=true;
            changed.Broadcast();
        }

        int CloseAndCancel(ValueArray<T> &cancelled)
        {
            ThreadMutexLock guard(&lock);
            closed=true;

            T task;
            int count=0;
            while(queue.Pop(task))
            {
                cancelled.Add(task);
                ++count;
            }

            changed.Broadcast();
            return count;
        }

        bool IsClosed()
        {
            ThreadMutexLock guard(&lock);
            return closed;
        }

        int GetCount()
        {
            ThreadMutexLock guard(&lock);
            return queue.GetCount();
        }
    };//class TaskQueue
}//namespace hgl
