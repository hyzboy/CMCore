#pragma once

#include<hgl/concurrency/PriorityTaskQueue.h>
#include<hgl/sync/Atomic.h>
#include<hgl/sync/Mutex.h>
#include<hgl/thread/ShutdownMode.h>
#include<hgl/thread/Thread.h>
#include<hgl/time/Time.h>
#include<hgl/type/ManagedArray.h>
#include<hgl/type/MemoryBlock.h>
#include<hgl/type/String.h>
#include<hgl/type/ValueArray.h>
#include<hgl/io/FileInputStream.h>

namespace hgl::io
{
    /** @brief CN: 文件请求的默认四级优先级；数值越小越优先。 EN: Default four-level file priority; lower values are higher priority. */
    enum class AsyncFilePriority:uint
    {
        Critical=0,    ///< @brief CN: 关键资源。 EN: Critical resources.
        High=1,        ///< @brief CN: 高优先级资源。 EN: High-priority resources.
        Normal=2,      ///< @brief CN: 普通资源。 EN: Normal-priority resources.
        Low=3          ///< @brief CN: 低优先级预加载资源。 EN: Low-priority prefetch resources.
    };

    static constexpr uint AsyncFileNoWorkerIndex=~uint(0);

    enum class AsyncFileLoadError
    {
        None,               ///< @brief CN: 无错误。 EN: No error.
        OpenFailed,         ///< @brief CN: 文件无法打开。 EN: The file could not be opened.
        SizeUnavailable,    ///< @brief CN: 无法取得文件长度。 EN: The file size could not be queried.
        AllocationFailed,   ///< @brief CN: 无法分配文件缓冲区。 EN: The file buffer could not be allocated.
        ReadFailed,         ///< @brief CN: 文件读取失败或提前结束。 EN: The file read failed or ended prematurely.
        DeliveryRejected    ///< @brief CN: 目标处理器拒绝接收结果。 EN: The destination handler rejected the result.
    };

    /**
     * @brief CN: 调用方持有的异步文件读取请求及其读取结果。
     *        EN: Caller-owned asynchronous file request and its loaded data.
     * @details CN: 请求指针仅借给 AsyncFileLoader；调用方必须保持请求对象有效，
     *              直到 OnFileLoaded、OnFileError 或 OnFileCancelled 返回。成功交付后，
     *              目标处理器继续借用该请求及其 MemoryBlock，直到目标处理完成。
     *              Cancel 是协作式的：不会中断阻塞中的系统读取。单个请求不能重复
     *              并发提交。target_id 由业务定义，用于完成回调分流。
     *          EN: The request pointer is borrowed by AsyncFileLoader. Keep the request
     *              alive until OnFileLoaded, OnFileError or OnFileCancelled returns.
     *              After successful delivery, the destination handler may continue
     *              borrowing the request and its MemoryBlock until processing ends.
     *              Cancel is cooperative and cannot interrupt a blocking OS read. Do
     *              not submit one request more than once concurrently. target_id is
     *              application-defined and is used by completion handlers for routing.
     */
    class AsyncFileLoadRequest
    {
        friend class AsyncFileLoader;

        MemoryBlock data;
        int64 data_size=0;
        double submitted_at=0;
        atom_bool cancelled{false};

    public:

        OSString filename;                          ///< @brief CN: 待读取的文件路径。 EN: Path of the file to read.
        uint target_id=0;                           ///< @brief CN: 业务定义的目标通道编号。 EN: Application-defined destination-channel identifier.
        uint priority=0;                            ///< @brief CN: 实际排队优先级，数值越小优先级越高。 EN: Effective queue priority; lower values are higher.
        void *user_data=nullptr;                    ///< @brief CN: 不拥有的业务上下文指针。 EN: Non-owning application context pointer.
        AsyncFileLoadError error=AsyncFileLoadError::None; ///< @brief CN: 最近一次加载结果的错误码。 EN: Error code for the most recent load result.

        /** @brief CN: 创建一个文件请求；请求为一次性对象，不能并发重复提交。 EN: Create a one-shot file request; do not submit it concurrently more than once. */
        explicit AsyncFileLoadRequest(const OSString &path,uint destination=0,void *context=nullptr)
            :filename(path),target_id(destination),user_data(context)
        {
        }

        void Cancel(){cancelled.store(true);} ///< @brief CN: 请求协作式取消；不会中断阻塞中的系统读取。 EN: Request cooperative cancellation; an active blocking read is not interrupted.
        bool IsCancelled()const{return cancelled.load();} ///< @brief CN: 查询是否已请求取消。 EN: Return whether cancellation has been requested.
        uint8 *GetData(){return static_cast<uint8 *>(data.Get());} ///< @brief CN: 获取读取缓冲区；仅在成功交付后有效。 EN: Return the loaded buffer; valid after successful delivery.
        const uint8 *GetData()const{return static_cast<const uint8 *>(const_cast<MemoryBlock &>(data).Get());} ///< @brief CN: 获取只读读取缓冲区。 EN: Return the loaded buffer as read-only data.
        int64 GetDataSize()const{return data_size;} ///< @brief CN: 获取已读取字节数。 EN: Return the number of loaded bytes.

        /** @brief CN: 释放已读取的文件缓冲区；仅在目标处理结束后调用。 EN: Release loaded bytes; call only after destination processing is complete. */
        void ReleaseData()
        {
            data.Free();
            data_size=0;
        }
    };

    /**
     * @brief CN: 文件读取完成后的业务回调；可据 target_id 将结果投递到目标线程队列。
     *        EN: Business callbacks for loaded files; route results by target_id to destination-thread queues.
     * @details CN: 回调在线程池 worker 上运行；CancelPending 对尚未出队任务的回调则在
     *              调用 Stop 的线程运行。OnFileLoaded 返回 true 表示目标队列已接受请求，
     *              请求及数据缓冲区的生命周期继续由调用方/目标处理器维护；返回 false
     *              表示拒绝交付，加载器释放缓冲区并调用 OnFileError。回调不得抛出异常，
     *              多个 worker 可并发调用回调，处理器必须同步共享状态；也不得从回调内
     *              调用本加载器的 Stop。
     *          EN: Callbacks for completed reads run on loader workers. For queued tasks
     *              cancelled by Stop(CancelPending), OnFileCancelled runs on the Stop
     *              caller's thread. Returning true from OnFileLoaded means the destination
     *              accepted the request; the caller/destination handler then maintains the
     *              request and data lifetime. Returning false rejects delivery, releases
     *              the data and triggers OnFileError. Callbacks must not throw or call
     *              this loader's Stop from inside the callback. Multiple workers may
     *              invoke callbacks concurrently, so shared handler state requires synchronization.
     */
    class AsyncFileLoadHandler
    {
    public:

        virtual ~AsyncFileLoadHandler()=default;
        /** @brief CN: 接收成功读取的请求；返回 true 表示目标通道已接管后续处理。 EN: Receive a successfully read request; true means the destination accepted it. */
        virtual bool OnFileLoaded(uint worker_index,AsyncFileLoadRequest *request)=0;
        /** @brief CN: 接收文件读取或结果交付错误。 EN: Receive a file-read or result-delivery error. */
        virtual void OnFileError(uint worker_index,AsyncFileLoadRequest *request,AsyncFileLoadError error)=0;
        /** @brief CN: 接收任务取消通知；Stop 取消的任务使用无 worker 编号哨兵值。 EN: Receive cancellation; tasks cancelled by Stop use the no-worker sentinel. */
        virtual void OnFileCancelled(uint worker_index,AsyncFileLoadRequest *request)=0;
    };

    struct AsyncFileLoaderStatistics
    {
        uint64 submitted=0;     ///< @brief CN: 已入队请求数。 EN: Number of accepted submissions.
        uint64 popped=0;        ///< @brief CN: 已由 worker 取出的请求数。 EN: Number of requests dequeued by workers.
        uint64 loaded=0;        ///< @brief CN: 成功交付给目标处理器的请求数。 EN: Number of requests delivered successfully.
        uint64 failed=0;        ///< @brief CN: 读取或结果交付失败数。 EN: Number of read or delivery failures.
        uint64 cancelled=0;     ///< @brief CN: 已取消请求数。 EN: Number of cancelled requests.
        uint64 loaded_bytes=0;  ///< @brief CN: 成功交付的总字节数。 EN: Total bytes delivered successfully.
        uint64 pending=0;       ///< @brief CN: 当前仍在队列中的请求数。 EN: Current number of queued requests.
        uint64 peak_pending=0;  ///< @brief CN: 历史待处理请求峰值。 EN: Peak queued-request count.
        uint64 queue_wait_microseconds=0; ///< @brief CN: 已出队请求的累计排队等待时间，单位微秒。 EN: Cumulative queued wait time for dequeued requests, in microseconds.
        uint64 read_microseconds=0;       ///< @brief CN: 文件打开和读取阶段的累计耗时，单位微秒。 EN: Cumulative file-open and read duration, in microseconds.
        uint64 read_samples=0;            ///< @brief CN: 已完成或失败的文件读取尝试数。 EN: Number of completed or failed read attempts.
    };

    /**
     * @brief CN: 共享优先级队列驱动的异步文件读取服务。
     *        EN: Asynchronous file-reading service driven by a shared priority queue.
     * @details CN: 多线程可提交文件请求；worker 从全局队列按优先级读取文件原始字节，
     *              然后通过 AsyncFileLoadHandler 将请求分发到渲染、音频或主线程队列。
     *              优先级数值越小越高；只保证仍在队列中的任务优先级，不抢占已经开始
     *              的读取，持续高优先级任务可能使低优先级任务饥饿。目标处理器应选用
     *              能匹配 worker 写入者数量的通道；多 IO worker 不可直接写入单生产者
     *              SPSC 队列。Start 仅可成功一次；Stop(Drain) 排空请求，Stop(CancelPending)
     *              取消尚未开始的请求，但不会中断正在执行的系统读取。
     *          EN: Multiple threads may submit requests. Workers read raw file bytes from
     *              a shared priority queue and route requests through AsyncFileLoadHandler
     *              to render, audio or main-thread queues. Lower numeric values have
     *              higher priority. Only queued tasks are reordered; reads already started
     *              are not preempted, and sustained high-priority traffic may starve lower
     *              priorities. Destination queues must match the number of IO-worker
     *              producers; multiple IO workers must not write directly to an SPSC
     *              queue. Start succeeds only once. Drain processes accepted requests;
     *              CancelPending cancels queued requests but does not interrupt active OS reads.
     */
    class AsyncFileLoader
    {
        class Worker:public Thread
        {
            AsyncFileLoader *loader;
            uint worker_index;

        public:

            Worker(AsyncFileLoader *owner,uint index)
                :loader(owner),worker_index(index)
            {
            }

            bool Execute()override;
        };

        PriorityTaskQueue<AsyncFileLoadRequest *,4> queue;
        Mutex lifecycle_lock;
        AsyncFileLoadHandler *handler;
        ManagedArray<Worker> workers;
        atom_bool started{false};
        atom_bool stopped{false};
        atom_uint64 loaded_count{0};
        atom_uint64 failed_count{0};
        atom_uint64 cancelled_count{0};
        atom_uint64 loaded_byte_count{0};
        atom_uint64 queue_wait_microseconds{0};
        atom_uint64 read_microseconds{0};
        atom_uint64 read_sample_count{0};

        void RecordReadDuration(double start_time)
        {
            const double elapsed=GetUptimeSec()-start_time;
            if(elapsed>0)
                read_microseconds.fetch_add(static_cast<uint64>(elapsed*1000000.0));

            read_sample_count.fetch_add(1);
        }

        void Process(uint worker_index,AsyncFileLoadRequest *request)
        {
            if(!request)
                return;

            if(request->IsCancelled())
            {
                request->ReleaseData();
                request->error=AsyncFileLoadError::None;
                handler->OnFileCancelled(worker_index,request);
                cancelled_count.fetch_add(1);
                return;
            }

            const double read_start=GetUptimeSec();
            OpenFileInputStream input(request->filename);
            if(!input)
            {
                RecordReadDuration(read_start);
                Fail(worker_index,request,AsyncFileLoadError::OpenFailed);
                return;
            }

            const int64 size=input->GetSize();
            if(size<0)
            {
                RecordReadDuration(read_start);
                Fail(worker_index,request,AsyncFileLoadError::SizeUnavailable);
                return;
            }

            request->ReleaseData();

            if(size>0&&!request->data.Reserve(size))
            {
                RecordReadDuration(read_start);
                Fail(worker_index,request,AsyncFileLoadError::AllocationFailed);
                return;
            }

            int64 offset=0;
            while(offset<size)
            {
                if(request->IsCancelled())
                {
                    RecordReadDuration(read_start);
                    request->ReleaseData();
                    handler->OnFileCancelled(worker_index,request);
                    cancelled_count.fetch_add(1);
                    return;
                }

                const int64 read_size=input->Read(static_cast<uint8 *>(request->data.Get())+offset,size-offset);
                if(read_size<=0)
                {
                    RecordReadDuration(read_start);
                    Fail(worker_index,request,AsyncFileLoadError::ReadFailed);
                    return;
                }

                offset+=read_size;
            }

            RecordReadDuration(read_start);
            request->data_size=size;
            request->error=AsyncFileLoadError::None;

            if(request->IsCancelled())
            {
                request->ReleaseData();
                handler->OnFileCancelled(worker_index,request);
                cancelled_count.fetch_add(1);
                return;
            }

            if(!handler->OnFileLoaded(worker_index,request))
            {
                Fail(worker_index,request,AsyncFileLoadError::DeliveryRejected);
                return;
            }

            loaded_count.fetch_add(1);
            loaded_byte_count.fetch_add(static_cast<uint64>(size));
        }

        void Fail(uint worker_index,AsyncFileLoadRequest *request,AsyncFileLoadError error)
        {
            request->ReleaseData();
            request->error=error;
            handler->OnFileError(worker_index,request,error);
            failed_count.fetch_add(1);
        }

    public:

        explicit AsyncFileLoader(AsyncFileLoadHandler *load_handler)
            :handler(load_handler)
        {
        }

        ~AsyncFileLoader()
        {
            Stop(thread::ShutdownMode::Drain);
        }

        /** @brief CN: 启动固定数量 IO worker；仅可成功一次。 EN: Start a fixed set of IO workers; this can succeed only once. */
        bool Start(uint worker_count)
        {
            MutexLock guard(&lifecycle_lock);

            if(!handler||worker_count==0||started.load()||stopped.load())
                return(false);

            for(uint i=0;i<worker_count;i++)
            {
                Worker *worker=workers.Create(this,i);
                if(!worker||!worker->Start())
                {
                    queue.Close();
                    for(int j=0;j<workers.GetCount();j++)
                        workers[j]->Join();
                    workers.Clear();
                    stopped.store(true);
                    return(false);
                }
            }

            started.store(true);
            return(true);
        }

        /** @brief CN: 提交一个调用方持有的文件请求及优先级。 EN: Submit a caller-owned file request with a priority level. */
        bool Submit(AsyncFileLoadRequest *request,uint priority)
        {
            if(!request||!started.load()||stopped.load()||priority>=4)
                return(false);

            request->priority=priority;
            request->error=AsyncFileLoadError::None;
            request->submitted_at=GetUptimeSec();
            return queue.Submit(request,priority);
        }

        /** @brief CN: 使用具名默认优先级提交请求。 EN: Submit a request using a named default priority. */
        bool Submit(AsyncFileLoadRequest *request,AsyncFilePriority priority)
        {
            return Submit(request,static_cast<uint>(priority));
        }

        /** @brief CN: 按顺序提交请求数组；返回已接受的前缀数量。 EN: Submit requests in order and return the accepted prefix length. */
        int Submit(AsyncFileLoadRequest **requests,const uint *priorities,int count)
        {
            if(!requests||!priorities||count<=0||!started.load()||stopped.load())
                return(0);

            int submitted=0;
            while(submitted<count&&Submit(requests[submitted],priorities[submitted]))
                ++submitted;

            return submitted;
        }

        /**
         * @brief CN: 关闭队列并等待所有 worker 退出。
         *        EN: Close the queue and wait for all workers to exit.
         * @return CN: 所有 worker 均成功 Join 时返回 true。
         *         EN: True if every worker is joined successfully.
         */
        bool Stop(thread::ShutdownMode mode=thread::ShutdownMode::Drain)
        {
            MutexLock guard(&lifecycle_lock);

            if(stopped.load())
                return(true);

            if(mode==thread::ShutdownMode::CancelPending)
            {
                ValueArray<AsyncFileLoadRequest *> cancelled;
                queue.CloseAndCancel(cancelled);

                for(int i=0;i<cancelled.GetCount();i++)
                {
                    AsyncFileLoadRequest *request=cancelled[i];
                    if(request)
                    {
                        request->ReleaseData();
                        request->error=AsyncFileLoadError::None;
                        handler->OnFileCancelled(AsyncFileNoWorkerIndex,request);
                        cancelled_count.fetch_add(1);
                    }
                }
            }
            else
            {
                queue.Close();
            }

            bool joined=true;
            for(int i=0;i<workers.GetCount();i++)
                if(!workers[i]->Join())
                    joined=false;

            if(joined)
            {
                workers.Clear();
                started.store(false);
                stopped.store(true);
            }

            return joined;
        }

        int GetPendingCount(){return queue.GetPendingCount();}
        int GetPriorityPendingCount(uint priority){return queue.GetPriorityPendingCount(priority);}
        int GetWorkerCount()const{return workers.GetCount();} ///< @brief CN: 返回固定 worker 数。 EN: Return the fixed worker count.

        /** @brief CN: 取得线程安全的累计指标与队列快照。 EN: Return cumulative counters and a queue snapshot. */
        AsyncFileLoaderStatistics GetStatistics()
        {
            const PriorityTaskQueue<AsyncFileLoadRequest *,4>::Statistics queue_stats=queue.GetStatistics();
            AsyncFileLoaderStatistics result;
            result.submitted=queue_stats.submitted;
            result.popped=queue_stats.popped;
            result.loaded=loaded_count.load();
            result.failed=failed_count.load();
            result.cancelled=cancelled_count.load();
            result.loaded_bytes=loaded_byte_count.load();
            result.pending=queue_stats.pending;
            result.peak_pending=queue_stats.peak_pending;
            result.queue_wait_microseconds=queue_wait_microseconds.load();
            result.read_microseconds=read_microseconds.load();
            result.read_samples=read_sample_count.load();
            return result;
        }
    };

    inline bool AsyncFileLoader::Worker::Execute()
    {
        if(!loader)
            return(false);

        AsyncFileLoadRequest *request=nullptr;
        uint priority=0;
        if(!loader->queue.WaitPop(request,priority))
            return(false);

        if(request)
        {
            request->priority=priority;
            const double elapsed=GetUptimeSec()-request->submitted_at;
            if(elapsed>0)
                loader->queue_wait_microseconds.fetch_add(static_cast<uint64>(elapsed*1000000.0));
        }

        loader->Process(worker_index,request);
        return(true);
    }
}//namespace hgl::io
