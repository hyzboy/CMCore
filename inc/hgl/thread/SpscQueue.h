#pragma once

#include<hgl/CoreType.h>
#include<hgl/thread/Atomic.h>
#include<hgl/type/MemoryBlock.h>
#include<type_traits>

namespace hgl
{
    /**
    * Bounded single-producer/single-consumer ring queue.
    *
    * T must be trivially copyable. Only one thread may call Push and only one
    * different thread may call Pop; query methods are approximate observations,
    * not synchronization primitives.
    */
    template<typename T>
    class SpscQueue
    {
        static_assert(std::is_trivially_copyable_v<T>,
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

        /** Approximate concurrent count; do not use it to synchronize producers or consumers. */
        int GetCount()const
        {
            if(!capacity)
                return(0);

            const uint32 h=head.load(std::memory_order_acquire);
            const uint32 t=tail.load(std::memory_order_acquire);
            const uint32 count=h-t;

            return count<=capacity?int(count):0;
        }

        /** Number of Push attempts rejected because the queue was invalid or full. */
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
