#pragma once

#include<hgl/platform/Platform.h>

#if HGL_OS==HGL_OS_Windows
#include<windows.h>
#endif//

namespace hgl
{
    #if HGL_OS==HGL_OS_Windows
        using pipe_ptr=HANDLE;
        constexpr pipe_ptr PIPE_NULL=nullptr;
    #else
        using pipe_ptr=int;
        constexpr pipe_ptr PIPE_NULL=-1;
    #endif//

    using pipe_pair=pipe_ptr[2];

    bool CreatePipe(pipe_pair &);       ///<创建一对通信管道
    void ClosePipe(pipe_pair &);        ///<关闭一对通信管道
    void ClosePipe(pipe_ptr);          ///<关闭单个管道句柄

    // 平台无关的管道读写接口，具体实现放在平台相关的 Pipe.cpp 文件中
    int64 ReadPipe(pipe_ptr, void *buf, int64 size);
    int64 WritePipe(pipe_ptr, const void *buf, int64 size);
    int64 SkipPipe(pipe_ptr, int64 size);

}//namespace hgl
