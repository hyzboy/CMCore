#include<hgl/proc/Pipe.h>

namespace hgl
{
    bool CreatePipe(pipe_pair &pp)
    {
        SECURITY_ATTRIBUTES sa;

        sa.nLength=sizeof(SECURITY_ATTRIBUTES);
        sa.bInheritHandle=TRUE;
        sa.lpSecurityDescriptor=nullptr;

        return ::CreatePipe(&(pp[0]),&(pp[1]),&sa,0);
    }

    void ClosePipe(pipe_pair &pp)
    {
        if(pp[0]!=PIPE_NULL)
        {
            CloseHandle(pp[0]);
            pp[0]=PIPE_NULL;
        }
        if(pp[1]!=PIPE_NULL)
        {
            CloseHandle(pp[1]);
            pp[1]=PIPE_NULL;
        }
    }

    void ClosePipe(pipe_ptr h)
    {
        if(h!=PIPE_NULL)
            CloseHandle(h);
    }

    int64 ReadPipe(pipe_ptr h, void *buf, int64 size)
    {
        if(h==PIPE_NULL || !buf || size<=0) return -1;
        DWORD read_bytes=0;
        if(!ReadFile(h, buf, (DWORD)size, &read_bytes, nullptr))
            return -1;
        return (int64)read_bytes;
    }

    int64 WritePipe(pipe_ptr h, const void *buf, int64 size)
    {
        if(h==PIPE_NULL || !buf || size<=0) return -1;
        DWORD written=0;
        if(!WriteFile(h, buf, (DWORD)size, &written, nullptr))
            return -1;
        return (int64)written;
    }

    int64 SkipPipe(pipe_ptr h, int64 size)
    {
        if(size<=0) return 0;
        const int buf_sz = 4096;
        char tmp[buf_sz];
        int64 remain = size;
        while(remain>0)
        {
            int64 toread = (remain>buf_sz)?buf_sz:remain;
            int64 r = ReadPipe(h, tmp, toread);
            if(r<=0) break;
            remain -= r;
        }
        return size - remain;
    }
}//namespace hgl

