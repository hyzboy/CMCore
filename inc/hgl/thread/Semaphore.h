﻿#ifndef HGL_SEMAPHORE_INCLUDE
#define HGL_SEMAPHORE_INCLUDE

#include<hgl/platform/Platform.h>
#include<hgl/log/Log.h>

namespace hgl
{
    /**
    * 信号是用于多线程同步中常用的一种技术<br>
    * 注意信号的用法是要有信号被发送出来，才能获取到信号。
    */
    class Semaphore                                                                                 ///信号
    {
        OBJECT_LOGGER

    private:

        semaphore_ptr ptr;

    public:

        Semaphore(int=1024);
        virtual ~Semaphore();

        virtual bool Post(int n=1);                                                                 ///<发送信号

        virtual bool TryAcquire();                                                                  ///<尝试取得一个信号
        virtual bool Acquire();                                                                     ///<等待并获取一个信号
        virtual bool Acquire(double time);                                                          ///<等待并获取一个信号
    };//class Semaphore
}//namespace hgl
#endif//HGL_SEMAPHORE_INCLUDE
