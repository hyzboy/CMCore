#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/sync/Atomic.h>
#include<hgl/log/Log.h>

namespace hgl
{
    /**
     * @brief CN: 计数信号量，用于传递可累计的许可或通知数量。
     *        EN: Counting semaphore for passing a countable number of permits or notifications.
     * @details CN: 每次成功 Post(n) 增加 n 个许可，每次成功 Acquire 消耗一个许可；
     *              TryAcquire 不等待。许可数量受构造时指定的最大值限制，超过上限的
     *              Post 会失败。带超时的 Acquire 使用秒为单位，超时为零或负数表示
     *              无限等待。信号量只计数，不保护与许可关联的数据本身。
     *          EN: Each successful Post(n) adds n permits and each successful Acquire
     *              consumes one; TryAcquire does not wait. The constructor sets the
     *              maximum permit count, and Post fails if that limit would be exceeded.
     *              Acquire timeouts are in seconds; non-positive values wait
     *              indefinitely. The semaphore counts permits but does not protect
     *              associated data.
     */
    class Semaphore                                                                                 ///< @brief CN: 计数信号量。 EN: Counting semaphore.
    {
        OBJECT_LOGGER

    private:

        semaphore_ptr ptr=nullptr;
        int max_count=0;
        atom_int count{0};

    protected:

        bool ReservePost(int n)
        {
            if(n<=0||n>max_count)
                return(false);

            int current=count.load();

            while(current<=max_count-n)
                if(count.compare_exchange_weak(current,current+n))
                    return(true);

            return(false);
        }

        void UndoPost(int n){count.fetch_sub(n);}
        void Consumed(){count.fetch_sub(1);}

    public:

        Semaphore(int=1024);
        virtual ~Semaphore();

        virtual bool Post(int n=1);                                                                 ///< @brief CN: 发布 n 个许可。 EN: Post n permits.

        virtual bool TryAcquire();                                                                  ///< @brief CN: 非阻塞尝试获取一个许可。 EN: Try to acquire one permit without blocking.
        virtual bool Acquire();                                                                     ///< @brief CN: 阻塞等待并获取一个许可。 EN: Wait until one permit is acquired.
        virtual bool Acquire(double time);                                                          ///< @brief CN: 限时等待并获取一个许可，单位秒。 EN: Acquire one permit with a timeout in seconds.
    };//class Semaphore
}//namespace hgl
