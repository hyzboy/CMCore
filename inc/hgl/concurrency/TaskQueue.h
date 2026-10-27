#pragma once

#include<hgl/sync/CondVar.h>
#include<hgl/sync/Mutex.h>
#include<hgl/time/Time.h>
#include<hgl/type/Queue.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    /**
     * @brief CN: 带阻塞等待和关闭语义的多生产者、多消费者 FIFO 任务队列。
     *        EN: FIFO task queue for multiple producers and consumers with blocking waits and closure.
     * @details CN: 多线程可并发 Submit/Pop；队列内部负责同步。WaitPop 可限时等待任务，
     *              非正超时表示无限等待。Close 原子地拒绝新任务并唤醒所有等待消费者，
     *              但让队列中已接受的任务继续排空；空队列关闭后 WaitPop 返回 false。
     *              CloseAndCancel 则在锁内关闭队列并取出所有尚未开始处理的任务，已被
     *              消费者取走的任务不受影响。队列只复制 T 的值。若 T 是指针，指针所指
     *              对象的所有权和生命周期由调用方与任务处理器约定，队列不会自动释放对象。
     *          EN: Submit and Pop may be called concurrently; synchronization is internal.
     *              WaitPop can wait with a timeout in seconds; non-positive values wait
     *              indefinitely. Close rejects new tasks and wakes all consumers while
     *              allowing accepted tasks to drain; WaitPop returns false when a closed
     *              queue is empty. CloseAndCancel closes the queue and extracts tasks not
     *              yet taken by consumers. T is copied by value; pointed-to objects are
     *              not owned or released by the queue.
     */
    template<typename T>
    class TaskQueue
    {
        Queue<T> queue;
        Mutex lock;
        CondVar changed;
        bool closed=false;

    public:

        /** @brief CN: 提交任务副本；队列已关闭或底层容器无法接收时返回 false。 EN: Submit a task copy; return false if closed or the underlying container rejects it. */
        bool Submit(const T &task)
        {
            MutexLock guard(&lock);

            if(closed)
                return(false);

            if(!queue.Push(task))
                return(false);

            changed.Signal();
            return(true);
        }

        /** @brief CN: 立即取出一个任务；队列为空时返回 false。 EN: Try to remove one task immediately; return false when empty. */
        bool TryPop(T &task)
        {
            MutexLock guard(&lock);
            return queue.Pop(task);
        }

        /** @brief CN: 等待任务到达；timeout 单位秒，非正数表示无限等待。 EN: Wait for a task; timeout is in seconds and non-positive values mean no timeout. */
        bool WaitPop(T &task,double timeout=0)
        {
            MutexLock guard(&lock);
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

        /** @brief CN: 停止接收新任务，唤醒等待者，并允许消费者排空已有任务。 EN: Stop accepting tasks, wake waiters, and allow consumers to drain accepted work. */
        void Close()
        {
            MutexLock guard(&lock);
            closed=true;
            changed.Broadcast();
        }

        /** @brief CN: 关闭并移出所有尚未消费的任务；移出的任务由调用方负责处置。 EN: Close and extract all unconsumed tasks; the caller is responsible for handling them. */
        int CloseAndCancel(ValueArray<T> &cancelled)
        {
            MutexLock guard(&lock);
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
            MutexLock guard(&lock);
            return closed;
        }

        int GetCount()
        {
            MutexLock guard(&lock);
            return queue.GetCount();
        }
    };//class TaskQueue
}//namespace hgl
