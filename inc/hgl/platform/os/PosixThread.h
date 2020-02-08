#ifndef HGL_POSIX_THREAD_INCLUDE
#define HGL_POSIX_THREAD_INCLUDE

#include<pthread.h>
#include<semaphore.h>

#ifdef __APPLE__
#include<dispatch/dispatch.h>
#endif//__APPLE__

namespace hgl
{
    using thread_mutex_ptr  =pthread_mutex_t;

    using thread_ptr        =pthread_t;
    using rwlock_ptr        =pthread_rwlock_t;

#ifdef __APPLE__
    using semaphore_ptr     =dispatch_semaphore_t *;
#else
    using semaphore_ptr     =sem_t *;
#endif//

    using cond_var_ptr      =pthread_cond_t;

    #define THREAD_FUNC     void *
    #define HGL_THREAD_DETACH_SELF  pthread_detach(pthread_self());

    using proc_mutex_ptr    =sem_t *;
}//namespace hgl
#endif//HGL_POSIX_THREAD_INCLUDE
