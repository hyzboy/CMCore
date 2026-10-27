#pragma once

#include<hgl/CoreType.h>
#include<hgl/sync/Atomic.h>
#include<hgl/type/MemoryBlock.h>

namespace hgl
{
    /**
     * @brief CN: 有界 SPSC（单生产者、单消费者）无锁环形队列。
     *        EN: Bounded lock-free ring queue for one producer and one consumer (SPSC).
     * @details CN: 使用前提：
     *              - 生命周期内只能有一个线程调用 Push，且只能有另一个线程调用 Pop；
     *                多生产者/多消费者场景应改用 TaskQueue 等相应结构。
     *              - T 必须是可平凡复制类型；队列保存值的副本，不负责管理指针指向的对象。
     *              - 队列容量向上取整到不小于请求值的 2 的幂，最小容量为 8；
     *                分配失败或请求容量超出支持范围时 IsValid 返回 false。
     *              Push 在队列无效或已满时返回 false，并累计到 GetDroppedCount。
     *              GetCount/IsEmpty/IsFull 只是并发观察值，不能用于“检查后再操作”的
     *              同步逻辑；容量及槽位由生产者/消费者的发布与获取顺序保证可见性。
     *          EN: Exactly one thread may call Push and one other thread may call Pop
     *              during the queue lifetime; use a different structure such as
     *              TaskQueue for multiple producers or consumers. T must be trivially
     *              copyable; pointer targets are not owned. Capacity is rounded up to
     *              a power of two with a minimum of 8. IsValid is false on allocation
     *              failure or unsupported capacity. Push returns false and increments
     *              GetDroppedCount when invalid or full. GetCount, IsEmpty and IsFull
     *              are observations only, not synchronization for check-then-act logic.
     */
    template<typename T>
    class SpscQueue
    {
        static_assert(__is_trivially_copyable(T),
                      "SpscQueue<T> requires T to be trivially copyable");

        alignas(64) atom<uint32> head;
        alignas(64) atom<uint32> tail;
        alignas(64) atom<uint64> rejected;
        uint32 capacity;
        uint32 mask;
        uint32 producer_tail;
        uint32 consumer_head;
        T *slots;
        MemoryBlock storage;

        static uint32 NextPower2(uint32 n)
        {
            const uint32 max_capacity=uint32(1)<<30;

            if(n<8)
                n=8;
            if(n>max_capacity)
                return 0;

            uint32 value=8;
            while(value<n)
                value<<=1;

            return value;
        }

    public:

        explicit SpscQueue(uint32 cap=1024)
        {
            capacity=NextPower2(cap);
            if(capacity>static_cast<size_t>(-1)/sizeof(T))
                capacity=0;
            mask=capacity?capacity-1:0;
            producer_tail=0;
            consumer_head=0;
            slots=nullptr;

            head.store(0);
            tail.store(0);
            rejected.store(0);

            if(capacity)
            {
                const uint64 storage_size=uint64(capacity)*sizeof(T);

                if(storage.Reserve(storage_size,alignof(T)))
                    slots=static_cast<T*>(storage.Get());
                else
                {
                    capacity=0;
                    mask=0;
                }
            }
        }

        ~SpscQueue()=default;

        SpscQueue(const SpscQueue &)=delete;
        SpscQueue &operator=(const SpscQueue &)=delete;

        bool Push(const T &value)
        {
            if(!slots)
            {
                rejected.fetch_add(1);
                return(false);
            }

            const uint32 h=head.load(std::memory_order_relaxed);
            uint32 t=producer_tail;

            if(h-t>=capacity)
            {
                t=tail.load(std::memory_order_acquire);
                producer_tail=t;

                if(h-t>=capacity)
                {
                    rejected.fetch_add(1,std::memory_order_relaxed);
                    return(false);
                }
            }

            slots[h&mask]=value;
            head.store(h+1,std::memory_order_release);
            return(true);
        }

        bool Pop(T &value)
        {
            if(!slots)
                return(false);

            const uint32 t=tail.load(std::memory_order_relaxed);
            uint32 h=consumer_head;

            if(t==h)
            {
                h=head.load(std::memory_order_acquire);
                consumer_head=h;

                if(t==h)
                    return(false);
            }

            value=slots[t&mask];
            tail.store(t+1,std::memory_order_release);
            return(true);
        }

        /** @brief CN: 并发状态下的近似积压数量；仅用于观测，不可作为同步依据。 EN: Approximate concurrent backlog; for observation only, not synchronization. */
        int GetCount()const
        {
            if(!capacity)
                return(0);

            const uint32 h=head.load(std::memory_order_acquire);
            const uint32 t=tail.load(std::memory_order_acquire);
            const uint32 count=h-t;

            return count<=capacity?int(count):0;
        }

        /** @brief CN: 因队列无效或已满而被拒绝的 Push 次数；无效队列操作也会计数。 EN: Push calls rejected because the queue is invalid or full; invalid-queue calls are counted too. */
        uint64 GetDroppedCount()const
        {
            return rejected.load(std::memory_order_relaxed);
        }

        uint32 GetCapacity()const{return capacity;}
        bool IsValid()const{return slots!=nullptr;}
        bool IsEmpty()const{return GetCount()==0;}
        bool IsFull()const{return GetCount()>=(int)capacity;}
    };//class SpscQueue
}//namespace hgl
