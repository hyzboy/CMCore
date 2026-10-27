#pragma once

#include<hgl/sync/CondVar.h>
#include<hgl/sync/Mutex.h>
#include<hgl/time/Time.h>
#include<hgl/type/Queue.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    /**
     * @brief CN: 支持固定多级优先级、阻塞等待和关闭语义的多生产者多消费者任务队列。
     *        EN: Multi-producer, multi-consumer task queue with fixed priority levels, blocking waits and closure.
     * @details CN: 优先级数值越小越高；每级内部保持 FIFO。队列每次取出最高优先级
     *              的待处理任务，持续高优先级负载可能使低优先级任务饥饿。优先级仅
     *              影响尚未出队的任务，不能抢占正在执行的任务。T 必须是 Queue<T>
     *              支持的平凡可复制类型；指针所指对象的所有权由调用方管理。
     *              Submit、取出、关闭及统计快照均由同一把锁保护。
     *          EN: Lower numeric values have higher priority; each level is FIFO. The
     *              highest-priority pending task is always selected, so sustained
     *              high-priority traffic may starve lower levels. Priority cannot
     *              preempt work already dequeued. T must be trivially copyable as
     *              required by Queue<T>; pointed-to objects remain caller-owned.
     *              Submission, removal, closure and metrics snapshots use one lock.
     */
    template<typename T,uint LevelCount=4>
    class PriorityTaskQueue
    {
        static_assert(LevelCount>0,"PriorityTaskQueue requires at least one priority level.");

    public:

        struct Statistics
        {
            uint64 submitted=0;
            uint64 popped=0;
            uint64 cancelled=0;
            uint64 peak_pending=0;
            uint64 pending=0;
        };

    private:

        Queue<T> queues[LevelCount];
        uint64 submitted_count=0;
        uint64 popped_count=0;
        uint64 cancelled_count=0;
        uint64 peak_pending=0;
        bool closed=false;
        Mutex lock;
        CondVar changed;

        bool PopLocked(T &task,uint &priority)
        {
            for(uint i=0;i<LevelCount;i++)
            {
                if(queues[i].Pop(task))
                {
                    priority=i;
                    ++popped_count;
                    return(true);
                }
            }

            return(false);
        }

    public:

        /** @brief CN: 按指定优先级提交任务；优先级越小越高，越界或关闭时返回 false。 EN: Submit at a given priority; lower values are higher priority; false for an invalid level or closed queue. */
        bool Submit(const T &task,uint priority)
        {
            if(priority>=LevelCount)
                return(false);

            MutexLock guard(&lock);

            if(closed||!queues[priority].Push(task))
                return(false);

            ++submitted_count;

            uint64 pending=0;
            for(uint i=0;i<LevelCount;i++)
                pending+=queues[i].GetCount();

            if(pending>peak_pending)
                peak_pending=pending;

            changed.Signal();
            return(true);
        }

        /** @brief CN: 非阻塞取出最高优先级任务；队列为空时返回 false。 EN: Non-blocking removal of the highest-priority task; false when empty. */
        bool TryPop(T &task,uint &priority)
        {
            MutexLock guard(&lock);
            return PopLocked(task,priority);
        }

        /** @brief CN: 等待最高优先级任务；timeout 秒数非正时无限等待。 EN: Wait for the highest-priority task; non-positive timeout waits indefinitely. */
        bool WaitPop(T &task,uint &priority,double timeout=0)
        {
            MutexLock guard(&lock);
            const double start=timeout>0?GetUptimeSec():0;

            while(IsEmptyLocked()&&!closed)
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

                    if(!changed.Wait(&lock,remaining)&&IsEmptyLocked()&&!closed)
                        return(false);
                }
            }

            return PopLocked(task,priority);
        }

        /** @brief CN: 关闭投递端、唤醒所有等待者，并允许消费者排空已接受任务。 EN: Close submissions, wake all waiters, and allow consumers to drain accepted tasks. */
        void Close()
        {
            MutexLock guard(&lock);
            closed=true;
            changed.Broadcast();
        }

        /**
         * @brief CN: 关闭并按优先级顺序取出所有尚未消费的任务。
         *        EN: Close the queue and extract all tasks not yet consumed, in priority order.
         * @return CN: 被取消并写入 cancelled 的任务数。
         *         EN: Number of tasks cancelled and appended to cancelled.
         */
        int CloseAndCancel(ValueArray<T> &cancelled)
        {
            MutexLock guard(&lock);
            closed=true;

            int count=0;
            for(uint priority=0;priority<LevelCount;priority++)
            {
                T task;
                while(queues[priority].Pop(task))
                {
                    cancelled.Add(task);
                    ++count;
                }
            }

            cancelled_count+=count;
            changed.Broadcast();
            return count;
        }

        bool IsClosed()
        {
            MutexLock guard(&lock);
            return closed;
        }

        int GetPendingCount()
        {
            MutexLock guard(&lock);
            return GetPendingCountLocked();
        }

        int GetPriorityPendingCount(uint priority)
        {
            if(priority>=LevelCount)
                return(0);

            MutexLock guard(&lock);
            return queues[priority].GetCount();
        }

        Statistics GetStatistics()
        {
            MutexLock guard(&lock);
            Statistics result;
            result.submitted=submitted_count;
            result.popped=popped_count;
            result.cancelled=cancelled_count;
            result.peak_pending=peak_pending;
            result.pending=GetPendingCountLocked();
            return result;
        }

    private:

        bool IsEmptyLocked()const
        {
            for(uint i=0;i<LevelCount;i++)
                if(!queues[i].IsEmpty())
                    return(false);

            return(true);
        }

        int GetPendingCountLocked()const
        {
            int pending=0;
            for(uint i=0;i<LevelCount;i++)
                pending+=queues[i].GetCount();

            return pending;
        }
    };//class PriorityTaskQueue
}//namespace hgl
