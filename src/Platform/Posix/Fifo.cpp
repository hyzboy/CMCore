#include<hgl/proc/Fifo.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>

namespace hgl
{
    bool CreateFifo(fifo_pair &pp,const char *name)
    {
        if(!name) return false;

        // Use /tmp directory if name not an absolute path
        char path[HGL_MAX_PATH];
        if(name[0]=='/')
            strncpy(path,name,HGL_MAX_PATH);
        else
            snprintf(path,HGL_MAX_PATH,"/tmp/%s",name);

        // create FIFO file if not exists
        if(mkfifo(path, 0600) < 0)
        {
            // if already exists, continue
        }

        // open read and write ends
        int r = open(path, O_RDONLY | O_NONBLOCK);
        int w = open(path, O_WRONLY);

        if(r<0 || w<0)
        {
            if(r>=0) close(r);
            if(w>=0) close(w);
            pp[0]=FIFO_NULL;pp[1]=FIFO_NULL;
            return false;
        }

        pp[0]=r; pp[1]=w;
        return true;
    }

    void CloseFifo(fifo_pair &pp)
    {
        if(pp[0]!=FIFO_NULL)
        {
            close(pp[0]); pp[0]=FIFO_NULL;
        }
        if(pp[1]!=FIFO_NULL)
        {
            close(pp[1]); pp[1]=FIFO_NULL;
        }
    }

    void CloseFifo(fifo_ptr h)
    {
        if(h!=FIFO_NULL) close(h);
    }

    int64 ReadFifo(fifo_ptr h, void *buf, int64 size)
    {
        if(h==FIFO_NULL || !buf || size<=0) return -1;
        ssize_t r = read(h, buf, (size_t)size);
        if(r<0) return -1;
        return (int64)r;
    }

    int64 WriteFifo(fifo_ptr h, const void *buf, int64 size)
    {
        if(h==FIFO_NULL || !buf || size<=0) return -1;
        ssize_t w = write(h, buf, (size_t)size);
        if(w<0) return -1;
        return (int64)w;
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
            ssize_t r = read(h, tmp, (size_t)toread);
            if(r<=0) break;
            remain -= r;
        }
        return size - remain;
    }

    void RemoveFifo(const char *name)
    {
        if(!name) return;
        char path[HGL_MAX_PATH];
        if(name[0]=='/')
            strncpy(path,name,HGL_MAX_PATH);
        else
            snprintf(path,HGL_MAX_PATH,"/tmp/%s",name);

        unlink(path);
    }
}
