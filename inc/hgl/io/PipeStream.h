#pragma once

#include<hgl/io/InputStream.h>
#include<hgl/io/OutputStream.h>
#include<hgl/proc/Pipe.h>
#include<hgl/thread/ThreadMutex.h>
#include<hgl/type/Smart.h>

namespace hgl::io
{
    /**
        * 管道封装类——管理一对管道句柄
        * 注意：实现细节隐藏到 CPP 中，类线程安全，返回 WeakPtr
        */
    class Pipe
    {
    public:
        pipe_pair pp;

    private:
        // 由 Pipe 管理的输入/输出流智能指针，具体实现隐藏在 CPP 中
        SharedPtr<InputStream>  input_stream;
        SharedPtr<OutputStream> output_stream;

        ThreadMutex mutex; // 保护成员的线程互斥锁

    public:
        Pipe();
        ~Pipe();

        bool Create();

        void CloseRead();
        void CloseWrite();

        // 如果对应端已关闭则返回 空 WeakPtr；否则返回 WeakPtr<InputStream>/WeakPtr<OutputStream>
        WeakPtr<InputStream>  GetInputStream();
        WeakPtr<OutputStream> GetOutputStream();

        pipe_ptr GetRead() const { return pp[0]; }
        pipe_ptr GetWrite() const { return pp[1]; }
    };
}//namespace hgl::io
