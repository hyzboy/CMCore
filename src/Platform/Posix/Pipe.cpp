#include<hgl/proc/Pipe.h>
#include<unistd.h>
#include<errno.h>

namespace hgl
{
    bool CreatePipe(pipe_pair &pp)
    {
        int fds[2];
        if(pipe(fds)==0)
        {
            pp[0]=fds[0];
            pp[1]=fds[1];
            return true;
        }
        pp[0]=PIPE_NULL;pp[1]=PIPE_NULL;
        return false;
    }

    void ClosePipe(pipe_pair &pp)
    {
        if(pp[0]!=PIPE_NULL)
        {
            close(pp[0]);
            pp[0]=PIPE_NULL;
        }
        if(pp[1]!=PIPE_NULL)
        {
            close(pp[1]);
            pp[1]=PIPE_NULL;
        }
    }

    void ClosePipe(pipe_ptr h)
    {
        if(h!=PIPE_NULL)
            close(h);
    }

    int64 ReadPipe(pipe_ptr h, void *buf, int64 size)
    {
        if(h==PIPE_NULL || !buf || size<=0) return -1;
        ssize_t r = read(h, buf, (size_t)size);
        if(r<0) return -1;
        return (int64)r;
    }

    int64 WritePipe(pipe_ptr h, const void *buf, int64 size)
    {
        if(h==PIPE_NULL || !buf || size<=0) return -1;
        ssize_t w = write(h, buf, (size_t)size);
        if(w<0) return -1;
        return (int64)w;
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
            ssize_t r = read(h, tmp, (size_t)toread);
            if(r<=0) break;
            remain -= r;
        }
        return size - remain;
    }
}
