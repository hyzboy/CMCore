#pragma once

#include<hgl/io/InputStream.h>
#include<hgl/io/OutputStream.h>
#include<hgl/proc/Fifo.h>
#include<hgl/thread/ThreadMutex.h>
#include<hgl/type/Smart.h>

namespace hgl::io
{
    class FifoStream;

    // Public API: create wrapper for named fifo and access streams via WeakPtr
    class FifoStream
    {
    public:
        FifoStream();
        ~FifoStream();

        // Create named fifo with given name
        bool Create(const char *name);

        void CloseRead();
        void CloseWrite();

        WeakPtr<InputStream>  GetInputStream();
        WeakPtr<OutputStream> GetOutputStream();

    private:
        // hidden implementation details in cpp
        struct Impl;
        Impl *impl;
    };
}//namespace hgl::io
