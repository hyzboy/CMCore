#pragma once

#include<hgl/sync/CondVar.h>
#include<hgl/sync/Mutex.h>
#include<hgl/time/Time.h>
#include<hgl/type/ValueArray.h>

namespace hgl
{
    /**
     * @brief CN: 多生产者、单消费者的双缓冲批处理邮箱。
     *        EN: Double-buffered batch mailbox for multiple producers and one consumer.
     * @details CN: Post 将值复制到当前写批次并唤醒消费者；消费者通过 TrySwap 或
     *              WaitAndSwap 一次取得一整个批次，交换过程持锁，随后可在无锁状态下
     *              读取 GetReceive 返回的数组。处理完成后必须调用 ReleaseReceive，
     *              使该缓冲区可重新成为写入缓冲；同一时刻只允许一个消费者持有接收批次。
     *              邮箱没有固定容量上限，待处理数组可能动态扩展。Close 会拒绝后续
     *              Post 并唤醒等待者，但保留已接受数据供消费者排空；Close 本身不会
     *              丢弃数据。对象及接收批次的值类型需满足 ValueArray 的存储要求。
     *          EN: Post copies values into the current write batch and wakes the
     *              consumer. TrySwap or WaitAndSwap obtains a whole batch under the
     *              lock; the consumer may then read GetReceive without holding it.
     *              ReleaseReceive is required after processing so the buffer can be
     *              reused, and only one consumer may hold a receive batch at a time.
     *              The mailbox has no fixed capacity limit and batches may grow
     *              dynamically. Close rejects future posts and wakes waiters, but
     *              preserves accepted data for draining. T must meet ValueArray's
     *              storage requirements.
     */
    template<typename T>
    class SwapMailbox
    {
        ValueArray<T> buffers[2];
        int write_index=0;
        int receive_index=1;
        bool receive_active=false;
        bool closed=false;
        Mutex lock;
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

        /** @brief CN: 复制投递一个值；邮箱已关闭时返回 false。 EN: Post a copy of one value; return false if the mailbox is closed. */
        bool Post(const T &value)
        {
            MutexLock guard(&lock);

            if(closed)
                return(false);

            buffers[write_index].Add(value);
            changed.Signal();
            return(true);
        }

        /** @brief CN: 批量复制投递；返回实际接受的数量，参数无效或邮箱关闭时返回 0。 EN: Post a batch of values and return the accepted count; return 0 for invalid input or a closed mailbox. */
        int Post(const T *values,int count)
        {
            if(!values||count<=0)
                return(0);

            MutexLock guard(&lock);

            if(closed)
                return(0);

            const int first=buffers[write_index].Add(values,count);
            if(first<0)
                return(0);

            changed.Signal();
            return(count);
        }

        /** @brief CN: 有待处理数据且没有未释放接收批次时，立即交换缓冲区。 EN: Swap immediately when data is pending and no receive batch is active. */
        bool TrySwap()
        {
            MutexLock guard(&lock);
            return SwapLocked();
        }

        /**
         * @brief CN: 等待数据到达或邮箱关闭后尝试交换批次。
         *        EN: Wait for data or mailbox closure, then try to swap a batch.
         * @param timeout CN: 超时秒数；非正数表示无限等待。
         *                EN: Timeout in seconds; non-positive values wait indefinitely.
         * @return CN: 已关闭但仍有数据时仍成功交换以允许排空；false 表示超时或无可交换批次。
         *         EN: A closed mailbox with pending data still swaps successfully for draining;
         *             false indicates timeout or that no batch can be swapped.
         */
        bool WaitAndSwap(double timeout=0)
        {
            MutexLock guard(&lock);
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

        /**
         * @brief CN: 返回当前接收批次的可变数组引用。
         *        EN: Return a mutable reference to the current receive batch.
         * @details CN: 只有成功 Swap 后到 ReleaseReceive 前可访问；不得跨越
         *              ReleaseReceive 保存该引用，也不得让第二个消费者同时处理该数组。
         *          EN: Access is valid only after a successful swap and before
         *              ReleaseReceive. Do not retain the reference past release or
         *              let another consumer process the array concurrently.
         */
        ValueArray<T> &GetReceive(){return buffers[receive_index];}

        /** @brief CN: 清空当前接收批次并允许下一批次交换；必须与成功 Swap 配对调用。 EN: Clear the receive batch and allow another swap; pair this with a successful swap. */
        void ReleaseReceive()
        {
            MutexLock guard(&lock);

            if(receive_active)
            {
                buffers[receive_index].Clear();
                receive_active=false;
            }
        }

        /** @brief CN: 关闭投递端并唤醒全部等待者；已接受的批次仍可被消费。 EN: Close posting and wake all waiters; accepted batches remain available for consumption. */
        void Close()
        {
            MutexLock guard(&lock);
            closed=true;
            changed.Broadcast();
        }

        bool IsClosed()
        {
            MutexLock guard(&lock);
            return closed;
        }

        int GetPendingCount()
        {
            MutexLock guard(&lock);
            return buffers[write_index].GetCount();
        }
    };//class SwapMailbox
}//namespace hgl
