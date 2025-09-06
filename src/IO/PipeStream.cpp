#include<hgl/io/PipeStream.h>

namespace hgl::io
{
    using namespace hgl;

    // Hidden implementations
    class PipeInputStreamImpl: public InputStream
    {
    protected:
        pipe_ptr handle;
        bool own_handle;

    public:
        explicit PipeInputStreamImpl(pipe_ptr h, bool own=true):handle(h),own_handle(own){}
        virtual ~PipeInputStreamImpl(){ Close(); }

        virtual void Close() override
        {
            if(handle!=PIPE_NULL && own_handle)
            {
                hgl::ClosePipe(handle);
                handle=PIPE_NULL;
            }
        }

        virtual int64 Read(void *buf,int64 size) override
        {
            return ReadPipe(handle, buf, size);
        }

        virtual int64 Peek(void *buf,int64 size) override { return -1; }
        virtual int64 ReadFully(void *buf,int64 buf_size) override
        {
            if(!buf||buf_size<=0)return -1;

            int64 total=0;
            while(total<buf_size)
            {
                int64 r=Read((char*)buf+total,buf_size-total);
                if(r<=0)break;
                total+=r;
            }

            return total;
        }

        virtual bool CanRestart()const override{return false;}
        virtual bool CanSeek()const override{return false;}
        virtual bool CanSize()const override{return false;}
        virtual bool CanPeek()const override{return false;}

        virtual bool Restart()override{return false;}
        virtual int64 Skip(int64 size)override
        {
            return SkipPipe(handle, size);
        }
        virtual int64 Seek(int64,SeekOrigin=SeekOrigin::Begin)override{return -1;}
        virtual int64 Tell()const override{return -1;}
        virtual int64 GetSize()const override{return -1;}
        virtual int64 Available()const override{return 0;}
    };


    class PipeOutputStreamImpl: public OutputStream
    {
    protected:
        pipe_ptr handle;
        bool own_handle;

    public:
        explicit PipeOutputStreamImpl(pipe_ptr h,bool own=true):handle(h),own_handle(own){}
        virtual ~PipeOutputStreamImpl(){ Close(); }

        virtual void Close() override
        {
            if(handle!=PIPE_NULL && own_handle)
            {
                hgl::ClosePipe(handle);
                handle=PIPE_NULL;
            }
        }

        virtual int64 Write(const void *buf,int64 size) override
        {
            return WritePipe(handle, buf, size);
        }

        virtual int64 WriteFully(const void *buf,int64 buf_size) override
        {
            if(!buf||buf_size<=0)return -1;

            int64 total=0;
            while(total<buf_size)
            {
                int64 w=Write((char*)buf+total,buf_size-total);
                if(w<=0)break;
                total+=w;
            }

            return total;
        }

        virtual bool CanRestart()const override{return false;}
        virtual bool CanSeek()const override{return false;}
        virtual bool CanSize()const override{return false;}

        virtual bool Restart()override{return false;}
        virtual int64 Seek(int64,SeekOrigin=SeekOrigin::Begin)override{return -1;}
        virtual int64 Tell()const override{return -1;}
        virtual int64 GetSize()const override{return -1;}
        virtual int64 Available()const override{return 0;}
    };


    Pipe::Pipe()
    {
        pp[0]=PIPE_NULL; pp[1]=PIPE_NULL;
        input_stream=nullptr;
        output_stream=nullptr;
    }

    Pipe::~Pipe()
    {
        CloseRead();
        CloseWrite();
    }

    bool Pipe::Create()
    {
        ThreadMutexLock lock(&mutex);
        return hgl::CreatePipe(pp);
    }

    void Pipe::CloseRead()
    {
        ThreadMutexLock lock(&mutex);

        if(input_stream)
        {
            // Reset shared pointer; external weak pointers will expire
            input_stream = SharedPtr<InputStream>();
        }

        if(pp[0]!=PIPE_NULL)
        {
            hgl::ClosePipe(pp[0]);
            pp[0]=PIPE_NULL;
        }
    }

    void Pipe::CloseWrite()
    {
        ThreadMutexLock lock(&mutex);

        if(output_stream)
        {
            output_stream = SharedPtr<OutputStream>();
        }

        if(pp[1]!=PIPE_NULL)
        {
            hgl::ClosePipe(pp[1]);
            pp[1]=PIPE_NULL;
        }
    }

    WeakPtr<InputStream> Pipe::GetInputStream()
    {
        ThreadMutexLock lock(&mutex);

        if(pp[0]==PIPE_NULL) return WeakPtr<InputStream>();
        if(!input_stream)
        {
            input_stream = SharedPtr<InputStream>(new PipeInputStreamImpl(pp[0], false));
        }
        return WeakPtr<InputStream>(input_stream);
    }

    WeakPtr<OutputStream> Pipe::GetOutputStream()
    {
        ThreadMutexLock lock(&mutex);

        if(pp[1]==PIPE_NULL) return WeakPtr<OutputStream>();
        if(!output_stream)
        {
            output_stream = SharedPtr<OutputStream>(new PipeOutputStreamImpl(pp[1], false));
        }
        return WeakPtr<OutputStream>(output_stream);
    }
}//namespace hgl::io
