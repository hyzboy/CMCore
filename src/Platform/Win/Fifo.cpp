#include<hgl/proc/Fifo.h>
#include<windows.h>
#include<string>

namespace hgl
{
    // Windows named pipe: create a pair by creating client and server handles to the same pipe name.
    bool CreateFifo(fifo_pair &pp,const char *name)
    {
        if(!name) return false;

        std::string pipe_name = "\\\\.\\pipe\\" + std::string(name);

        // Create server end
        HANDLE server = CreateNamedPipeA(
            pipe_name.c_str(),
            PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_WAIT,
            1, // max instances
            4096,
            4096,
            0,
            nullptr);

        if(server==INVALID_HANDLE_VALUE) return false;

        // Connect server (blocking) so client can open
        BOOL connected = ConnectNamedPipe(server, nullptr) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
        if(!connected)
        {
            CloseHandle(server);
            return false;
        }

        // Open client end
        HANDLE client = CreateFileA(
            pipe_name.c_str(),
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr);

        if(client==INVALID_HANDLE_VALUE)
        {
            CloseHandle(server);
            pp[0]=FIFO_NULL;pp[1]=FIFO_NULL;
            return false;
        }

        // For simplicity, treat server as read, client as write
        pp[0]=server;
        pp[1]=client;
        return true;
    }

    void CloseFifo(fifo_pair &pp)
    {
        if(pp[0]!=FIFO_NULL)
        {
            CloseHandle(pp[0]); pp[0]=FIFO_NULL;
        }
        if(pp[1]!=FIFO_NULL)
        {
            CloseHandle(pp[1]); pp[1]=FIFO_NULL;
        }
    }

    void CloseFifo(fifo_ptr h)
    {
        if(h!=FIFO_NULL) CloseHandle(h);
    }

    int64 ReadFifo(fifo_ptr h, void *buf, int64 size)
    {
        if(h==FIFO_NULL || !buf || size<=0) return -1;
        DWORD read_bytes=0;
        if(!ReadFile(h, buf, (DWORD)size, &read_bytes, nullptr))
            return -1;
        return (int64)read_bytes;
    }

    int64 WriteFifo(fifo_ptr h, const void *buf, int64 size)
    {
        if(h==FIFO_NULL || !buf || size<=0) return -1;
        DWORD written=0;
        if(!WriteFile(h, buf, (DWORD)size, &written, nullptr))
            return -1;
        return (int64)written;
    }

    int64 SkipFifo(fifo_ptr h, int64 size)
    {
        if(size<=0) return 0;
        const int buf_sz = 4096;
        char tmp[buf_sz];
        int64 remain = size;
        while(remain>0)
        {
            int64 toread = (remain>buf_sz)?buf_sz:remain;
            int64 r = ReadFifo(h, tmp, toread);
            if(r<=0) break;
            remain -= r;
        }
        return size - remain;
    }

    void RemoveFifo(const char *name)
    {
        // no-op on Windows for this simple implementation
        (void)name;
    }
}
