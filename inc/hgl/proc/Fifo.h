#ifndef HGL_MULTI_PROC_FIFO_INCLUDE
#define HGL_MULTI_PROC_FIFO_INCLUDE

#include<hgl/platform/Platform.h>

#if HGL_OS==HGL_OS_Windows
#include<windows.h>
#endif//

namespace hgl
{
    #if HGL_OS==HGL_OS_Windows
        using fifo_ptr=HANDLE;
        constexpr fifo_ptr FIFO_NULL=nullptr;
    #else
        using fifo_ptr=int;
        constexpr fifo_ptr FIFO_NULL=-1;
    #endif//

    using fifo_pair=fifo_ptr[2];

    // 在指定名字下创建一对 FIFO 句柄（可在同一进程内使用以便测试）；
    // name 为管道名字（POSIX 为路径，Windows 为管道名, 不带前缀）
    bool CreateFifo(fifo_pair &pp,const char *name);

    void CloseFifo(fifo_pair &pp);
    void CloseFifo(fifo_ptr h);

    int64 ReadFifo(fifo_ptr h, void *buf, int64 size);
    int64 WriteFifo(fifo_ptr h, const void *buf, int64 size);
    int64 SkipFifo(fifo_ptr h, int64 size);

    // 移除命名 FIFO（POSIX 调用 unlink，Windows 可为无操作或关闭服务器）
    void RemoveFifo(const char *name);
}

#endif//HGL_MULTI_PROC_FIFO_INCLUDE
