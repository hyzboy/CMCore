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
static const uint8 TestBytes[4]={0x41,0x55,0x4c,0x52};

class RoutingHandler:public AsyncFileLoadHandler
{
public:

    SwapMailbox<AsyncFileLoadRequest *> render_queue;
    TaskQueue<AsyncFileLoadRequest *> audio_queue;
    atom_uint errors{0};
    atom_uint cancellations{0};
    atom_uint invalid_routes{0};
    AsyncFileLoadError last_error=AsyncFileLoadError::None;

    bool OnFileLoaded(uint,AsyncFileLoadRequest *request)override
    {
        switch(request->target_id)
        {
            case TextureTarget:
                return render_queue.Post(request);
            case AudioTarget:
                return audio_queue.Submit(request);
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

    if(!audio_worker.Stop(5.0)&&result==0)
        result=9;

    if(result==0
       &&(audio_handler.processed.load()!=1
          ||audio_handler.errors.load()!=0
          ||routing_handler.errors.load()!=0
          ||routing_handler.cancellations.load()!=1
          ||routing_handler.invalid_routes.load()!=0))
        result=10;

    const AsyncFileLoaderStatistics stats=loader.GetStatistics();
    if(result==0
       &&(stats.submitted!=3||stats.popped!=3||stats.loaded!=2
          ||stats.failed!=0||stats.cancelled!=1||stats.pending!=0
          ||stats.loaded_bytes!=sizeof(TestBytes)*2))
        result=11;

    texture_request.ReleaseData();
    audio_request.ReleaseData();
    routing_handler.render_queue.Close();
    filesystem::FileDelete(filename);
    return result;
}
