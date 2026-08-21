#pragma once

#include<hgl/CoreType.h>
#include<hgl/thread/Atomic.h>
#include<type_traits>
#include<new>

namespace hgl
{
    /**
    * 无锁 SPSC 环形队列（单生产者单消费者）
    *
    * Vyukov 风格：生产者只写 head，消费者只读 tail，各自缓存对方指针，
    * 无锁、无等待、无伪共享（head/tail 分离缓存行）。
    *
    * 约束：
    * - T 必须是 trivially copyable（POD 语义，memcpy 安全）
    * - 单生产者 + 单消费者：两个不同线程各司其职，否则需外部加锁
    *
    * 语义：
    * - Push 满时返回 false 并累计丢弃计数（不阻塞，调用方决定重试/丢弃）
    * - Pop 空时返回 false（非阻塞）
    *
    * 用法：
    *   SpscQueue<MyEvent> q(1024);
    *   q.Push(ev);                    // 生产者线程
    *   MyEvent got; q.Pop(got);       // 消费者线程
    */
    template<typename T>
    class SpscQueue
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "SpscQueue<T> requires T to be trivially copyable (POD)");

        atom<uint32> head;          ///< 生产者写指针（只被生产者写）
        atom<uint32> tail;          ///< 消费者读指针（只被消费者读）
        uint32 capacity;            ///< 槽数（2 的幂）
        uint32 mask;
        T    *slots;                ///< 槽数组
        atom<uint64> dropped;       ///< 满时丢弃计数

        static uint32 NextPower2(uint32 n)
        {
            uint32 v=1;
            while(v<n)
                v<<=1;
            return v;
        }

    public:

        /**
        * @param cap 槽数（自动向上取整到 2 的幂，最小 8）
        */
        SpscQueue(uint32 cap=1024)
        {
            cap=NextPower2(cap<8?8:cap);

            capacity=cap;
            mask=cap-1;

            head=0;
            tail=0;
            dropped=0;

            slots=static_cast<T*>(::operator new(cap*sizeof(T)));
        }

        ~SpscQueue()
        {
            // trivially copyable：无需析构元素
            ::operator delete(slots);
        }

        SpscQueue(const SpscQueue &)=delete;
        SpscQueue &operator=(const SpscQueue &)=delete;

        /**
        * 入队（生产者线程）
        * @return true=成功；false=满（已累计丢弃计数）
        */
        bool Push(const T &value)
        {
            const uint32 h=head.load(std::memory_order_relaxed);
            const uint32 t=tail.load(std::memory_order_acquire);

            if(h-t>=capacity)       // 满
            {
                dropped.fetch_add(1,std::memory_order_relaxed);
                return(false);
            }

            slots[h&mask]=value;
            head.store(h+1,std::memory_order_release);

            return(true);
        }

        /**
        * 出队（消费者线程）
        * @return true=取到；false=空
        */
        bool Pop(T &value)
        {
            const uint32 t=tail.load(std::memory_order_relaxed);
            const uint32 h=head.load(std::memory_order_acquire);

            if(t==h)                // 空
                return(false);

            value=slots[t&mask];
            tail.store(t+1,std::memory_order_release);

            return(true);
        }

        /** 当前积压元素数 */
        int GetCount()const
        {
            return(int)(head.load(std::memory_order_relaxed)-tail.load(std::memory_order_relaxed));
        }

        /** 队列满时被丢弃的元素总数 */
        uint64 GetDroppedCount()const
        {
            return dropped.load(std::memory_order_relaxed);
        }

        bool IsEmpty()const{return GetCount()==0;}
        bool IsFull()const{return GetCount()>=(int)capacity;}
    };//class SpscQueue
}//namespace hgl
