#include<hgl/concurrency/SwapMailbox.h>
#include<hgl/concurrency/TaskQueue.h>
#include<hgl/filesystem/FileSystem.h>
#include<hgl/io/AsyncFileLoader.h>
#include<hgl/io/FileOutputStream.h>
#include<hgl/thread/SingleWorker.h>
#include<hgl/thread/WorkHandler.h>
#include<hgl/sync/Atomic.h>

using namespace hgl;
using namespace hgl::io;
using namespace hgl::thread;

static const uint TextureTarget=1;
static const uint AudioTarget=2;
static const uint DestTarget=3;
static const uint8 TestBytes[4]={0x41,0x55,0x4c,0x52};

// 自定义字节源：字节只存在于内存，filename 指向一个必然不存在的文件，
// 用于证明请求确实走了 byte_source 分支而不是文件系统。
class MemoryByteSource:public AsyncFileByteSource
{
    int64 offset=0;

public:

    const OSString &GetRoutePath()const override
    {
        static const OSString route=OS_TEXT("MemoryByteSource");
        return route;
    }

    int64 GetSize()override{return (int64)sizeof(TestBytes);}

    int64 Read(uint8 *buffer,int64 size)override
    {
        if(size<=0)
            return(0);

        int64 n=sizeof(TestBytes)-offset;

        if(n>size)
            n=size;

        for(int64 i=0;i<n;i++)
            buffer[i]=TestBytes[offset+i];

        offset+=n;
        return n;
    }
};

class RoutingHandler:public AsyncFileLoadHandler
{
public:

    SwapMailbox<AsyncFileLoadRequest *> render_queue;
    TaskQueue<AsyncFileLoadRequest *> audio_queue;
    atom_uint errors{0};
    atom_uint cancellations{0};
    atom_uint invalid_routes{0};
    atom_uint dest_delivered{0};
    AsyncFileLoadError last_error=AsyncFileLoadError::None;

    bool OnFileLoaded(uint,AsyncFileLoadRequest *request)override
    {
        switch(request->target_id)
        {
            case TextureTarget:
                return render_queue.Post(request);
            case AudioTarget:
                return audio_queue.Submit(request);
            case DestTarget:
                dest_delivered.fetch_add(1);
                return true;
            default:
                ++invalid_routes;
                return(false);
        }
    }

    void OnFileError(uint,AsyncFileLoadRequest *,AsyncFileLoadError error)override
    {
        last_error=error;
        errors.fetch_add(1);
    }

    void OnFileCancelled(uint,AsyncFileLoadRequest *)override
    {
        cancellations.fetch_add(1);
    }
};

class AudioDecodeHandler:public WorkHandler<AsyncFileLoadRequest>
{
public:

    atom_uint processed{0};
    atom_uint errors{0};

    void OnWork(uint,AsyncFileLoadRequest *request)override
    {
        if(request->GetDataSize()!=sizeof(TestBytes)||!request->GetData())
        {
            errors.fetch_add(1);
            return;
        }

        for(uint i=0;i<sizeof(TestBytes);i++)
            if(request->GetData()[i]!=TestBytes[i])
            {
                errors.fetch_add(1);
                return;
            }

        processed.fetch_add(1);
    }
};

int main()
{
    const OSString filename=OS_TEXT("AsyncFileLoaderTest.bin");
    {
        OpenFileOutputStream output(filename,FileOpenMode::CreateTrunc);
        if(!output||output->Write(TestBytes,sizeof(TestBytes))!=sizeof(TestBytes))
        {
            filesystem::FileDelete(filename);
            return(1);
        }
    }

    RoutingHandler routing_handler;
    AudioDecodeHandler audio_handler;
    SingleWorker<AsyncFileLoadRequest> audio_worker(&routing_handler.audio_queue,&audio_handler);
    AsyncFileLoader loader(&routing_handler);
    AsyncFileLoadRequest texture_request(filename,TextureTarget);
    AsyncFileLoadRequest audio_request(filename,AudioTarget);
    AsyncFileLoadRequest cancelled_request(filename,AudioTarget);
    cancelled_request.Cancel();

    MemoryByteSource mem_source;
    AsyncFileLoadRequest source_request(OS_TEXT("MissingByteSourceTarget.bin"),AudioTarget);
    source_request.byte_source=&mem_source;
    source_request.expected_bytes=sizeof(TestBytes);

    // 外部目标缓冲：文件系统路径直写调用方缓冲
    uint8 dest_buf[16]={0};
    AsyncFileLoadRequest dest_request(filename,DestTarget);
    dest_request.SetDestinationBuffer(dest_buf,sizeof(dest_buf));

    // 外部目标缓冲：自定义字节源同样直写调用方缓冲
    uint8 dest_src_buf[16]={0};
    MemoryByteSource dest_mem_source;
    AsyncFileLoadRequest dest_source_request(OS_TEXT("MissingDestByteSource.bin"),DestTarget);
    dest_source_request.byte_source=&dest_mem_source;
    dest_source_request.SetDestinationBuffer(dest_src_buf,sizeof(dest_src_buf));

    // 容量不足的直写请求：应以 BufferTooSmall 失败
    uint8 small_buf[2]={0};
    AsyncFileLoadRequest small_request(filename,DestTarget);
    small_request.SetDestinationBuffer(small_buf,sizeof(small_buf));

    int result=0;
    bool loader_started=false;
    if(!audio_worker.Start())
        result=2;

    if(result==0)
    {
        loader_started=loader.Start(2);
        if(!loader_started)
            result=3;
    }

    if(result==0
       &&(!loader.Submit(&audio_request,AsyncFilePriority::Low)
          ||!loader.Submit(&texture_request,AsyncFilePriority::Critical)
          ||!loader.Submit(&source_request,AsyncFilePriority::Normal)
          ||!loader.Submit(&dest_request,AsyncFilePriority::Normal)
          ||!loader.Submit(&dest_source_request,AsyncFilePriority::Normal)
          ||!loader.Submit(&small_request,AsyncFilePriority::Normal)
          ||!loader.Submit(&cancelled_request,AsyncFilePriority::Normal)))
        result=4;

    if(loader_started&&!loader.Stop(ShutdownMode::Drain)&&result==0)
        result=5;

    if(result==0)
    {
        if(!routing_handler.render_queue.WaitAndSwap(5.0))
            result=6;
        else
        {
            ValueArray<AsyncFileLoadRequest *> &batch=routing_handler.render_queue.GetReceive();
            if(batch.GetCount()!=1||batch[0]!=&texture_request
               ||batch[0]->GetDataSize()!=sizeof(TestBytes))
                result=7;
            else
                for(uint i=0;i<sizeof(TestBytes);i++)
                    if(batch[0]->GetData()[i]!=TestBytes[i])
                        result=8;
            routing_handler.render_queue.ReleaseReceive();
        }
    }

    if(result==0)
    {
        for(uint i=0;i<sizeof(TestBytes);i++)
            if(dest_buf[i]!=TestBytes[i]||dest_src_buf[i]!=TestBytes[i])
                result=12;

        if(dest_request.GetData()!=dest_buf||dest_request.GetDataSize()!=sizeof(TestBytes))
            result=12;
    }

    if(!audio_worker.Stop(5.0)&&result==0)
        result=9;

    if(result==0
       &&(audio_handler.processed.load()!=2
          ||audio_handler.errors.load()!=0
          ||routing_handler.errors.load()!=1
          ||routing_handler.last_error!=AsyncFileLoadError::BufferTooSmall
          ||routing_handler.cancellations.load()!=1
          ||routing_handler.invalid_routes.load()!=0
          ||routing_handler.dest_delivered.load()!=2))
        result=10;

    const AsyncFileLoaderStatistics stats=loader.GetStatistics();
    if(result==0
       &&(stats.submitted!=7||stats.popped!=7||stats.loaded!=5
          ||stats.failed!=1||stats.cancelled!=1||stats.pending!=0
          ||stats.loaded_bytes!=sizeof(TestBytes)*5||stats.read_samples!=6))
        result=11;

    texture_request.ReleaseData();
    audio_request.ReleaseData();
    source_request.ReleaseData();
    routing_handler.render_queue.Close();
    filesystem::FileDelete(filename);
    return result;
}
