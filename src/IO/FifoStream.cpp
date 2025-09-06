#include<hgl/io/FifoStream.h>
#include<hgl/type/String.h>
#include<string>

#if HGL_OS!=HGL_OS_Windows
#include<unistd.h>
#include<errno.h>
#include<string.h>
#endif

namespace hgl
{
    namespace io
    {
        using namespace hgl;

        struct FifoStream::Impl
        {
            fifo_pair pp;
            SharedPtr<InputStream>  in;
            SharedPtr<OutputStream> out;
            ThreadMutex mutex;
            std::string name;

            Impl()
            {
                pp[0]=FIFO_NULL; pp[1]=FIFO_NULL;
            }

            ~Impl()
            {
                if(in) in = SharedPtr<InputStream>();
                if(out) out = SharedPtr<OutputStream>();
                CloseFifo(pp);
                if(!name.empty()) RemoveFifo(name.c_str());
            }
        };

        // Hidden stream impls reuse Pipe-style implementations
        class FifoInputStreamImpl: public InputStream
        {
        protected:
            fifo_ptr handle;
            bool own_handle;

        public:
            explicit FifoInputStreamImpl(fifo_ptr h, bool own=true):handle(h),own_handle(own){}
            virtual ~FifoInputStreamImpl(){ Close(); }

            virtual void Close() override
            {
                if(handle!=FIFO_NULL && own_handle)
                {
                    hgl::CloseFifo(handle);
                    handle=FIFO_NULL;
                }
            }

            virtual int64 Read(void *buf,int64 size) override
            {
                return ReadFifo(handle, buf, size);
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
                return SkipFifo(handle, size);
            }
            virtual int64 Seek(int64,SeekOrigin=SeekOrigin::Begin)override{return -1;}
            virtual int64 Tell()const override{return -1;}
            virtual int64 GetSize()const override{return -1;}
            virtual int64 Available()const override{return 0;}
        };

        class FifoOutputStreamImpl: public OutputStream
        {
        protected:
            fifo_ptr handle;
            bool own_handle;

        public:
            explicit FifoOutputStreamImpl(fifo_ptr h,bool own=true):handle(h),own_handle(own){}
            virtual ~FifoOutputStreamImpl(){ Close(); }

            virtual void Close() override
            {
                if(handle!=FIFO_NULL && own_handle)
                {
                    hgl::CloseFifo(handle);
                    handle=FIFO_NULL;
                }
            }

            virtual int64 Write(const void *buf,int64 size) override
            {
                return WriteFifo(handle, buf, size);
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

        FifoStream::FifoStream()
        {
            impl=new Impl();
        }

        FifoStream::~FifoStream()
        {
            delete impl; impl=nullptr;
        }

        bool FifoStream::Create(const char *name)
        {
            ThreadMutexLock lock(&impl->mutex);

            if(!name) return false;

            impl->name = name;

            if(!CreateFifo(impl->pp, name))
            {
                impl->pp[0]=FIFO_NULL; impl->pp[1]=FIFO_NULL;
                return false;
            }

            // Create stream wrappers (shared)
            if(impl->pp[0]!=FIFO_NULL)
                impl->in = SharedPtr<InputStream>(new FifoInputStreamImpl(impl->pp[0], false));
            if(impl->pp[1]!=FIFO_NULL)
                impl->out = SharedPtr<OutputStream>(new FifoOutputStreamImpl(impl->pp[1], false));

            return true;
        }

        void FifoStream::CloseRead()
        {
            ThreadMutexLock lock(&impl->mutex);
            if(impl->in)
                impl->in = SharedPtr<InputStream>();
            if(impl->pp[0]!=FIFO_NULL)
            {
                CloseFifo(impl->pp[0]); impl->pp[0]=FIFO_NULL;
            }
        }

        void FifoStream::CloseWrite()
        {
            ThreadMutexLock lock(&impl->mutex);
            if(impl->out)
                impl->out = SharedPtr<OutputStream>();
            if(impl->pp[1]!=FIFO_NULL)
            {
                CloseFifo(impl->pp[1]); impl->pp[1]=FIFO_NULL;
            }
        }

        WeakPtr<InputStream> FifoStream::GetInputStream()
        {
            ThreadMutexLock lock(&impl->mutex);
            if(impl->pp[0]==FIFO_NULL) return WeakPtr<InputStream>();
            if(!impl->in)
                impl->in = SharedPtr<InputStream>(new FifoInputStreamImpl(impl->pp[0], false));
            return WeakPtr<InputStream>(impl->in);
        }

        WeakPtr<OutputStream> FifoStream::GetOutputStream()
        {
            ThreadMutexLock lock(&impl->mutex);
            if(impl->pp[1]==FIFO_NULL) return WeakPtr<OutputStream>();
            if(!impl->out)
                impl->out = SharedPtr<OutputStream>(new FifoOutputStreamImpl(impl->pp[1], false));
            return WeakPtr<OutputStream>(impl->out);
        }

    }
}
