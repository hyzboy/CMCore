#ifndef HGL_PROCESS_MUTEX_INCLUDE
#define HGL_PROCESS_MUTEX_INCLUDE

#include<hgl/platform/Platform.h>

namespace hgl
{
    /**
    * 进程排斥
    */
    class ProcMutex                                                                                 ///进程排斥
    {
        proc_mutex_ptr lock;

    public:

        ProcMutex();
        ~ProcMutex(){Clear();}

        bool Create(const os_char *);                                                               ///<创建进程排斥
        void Clear();                                                                               ///<清除进程排斥

        bool Lock();                                                                                ///<锁定
        bool TryLock();                                                                             ///<尝试锁定

        bool Unlock();                                                                              ///<解锁
    };//class ProcMutex
}//namespace hgl
#endif//HGL_PROCESS_MUTEX_INCLUDE
