#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/thread/Atomic.h>
#include<hgl/log/Log.h>

namespace hgl
{
    /**
    * 信号是用于多线程同步中常用的一种技术<br>
    * Post 的计数受 max_count 限制；Acquire 的 timeout 单位为秒，0 或负数表示无限等待。
    */
    class Semaphore                                                                                 ///信号
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

        virtual bool Post(int n=1);                                                                 ///<发送信号

        virtual bool TryAcquire();                                                                  ///<尝试取得一个信号
        virtual bool Acquire();                                                                     ///<等待并获取一个信号
        virtual bool Acquire(double time);                                                          ///<等待并获取一个信号
    };//class Semaphore
}//namespace hgl
