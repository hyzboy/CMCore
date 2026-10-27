#pragma once

#include<hgl/concurrency/TaskQueue.h>
#include<hgl/thread/Thread.h>
#include<hgl/sync/Atomic.h>
#include<hgl/type/ManagedArray.h>

namespace hgl::thread
{
    enum class ShutdownMode
    {
        Drain,
        CancelPending
    };

    template<typename W>
    class WorkHandler
    {
    public:

        virtual ~WorkHandler()=default;
        virtual void OnWork(uint worker_index,W *work)=0;
        virtual void OnCancel(W *){}
    };

    template<typename W>
    class WorkerThread:public Thread
    {
        TaskQueue<W *> *queue;
        WorkHandler<W> *work_handler;
        uint worker_index;

    public:

        WorkerThread(TaskQueue<W *> *q,WorkHandler<W> *handler,uint index)
            :queue(q),work_handler(handler),worker_index(index)
        {
        }

        bool Execute()override
        {
            if(!queue||!work_handler)
                return(false);

            W *work=nullptr;
            if(!queue->WaitPop(work))
                return(false);

            if(work)
                work_handler->OnWork(worker_index,work);

            return(true);
        }
    };

    /**
    * Fixed-size worker pool. Submitted pointers are borrowed: their storage
    * must remain valid until OnWork or OnCancel returns. Stop must be called
    * from outside a worker callback; callbacks must not re-enter Start or Stop.
    */
    template<typename W>
    class WorkerPool
    {
        TaskQueue<W *> queue;
        Mutex lifecycle_lock;
        WorkHandler<W> *work_handler;
        ManagedArray<WorkerThread<W>> workers;
        atom_bool started{false};
        atom_bool stopped{false};

    public:

        explicit WorkerPool(WorkHandler<W> *handler):work_handler(handler){}

        ~WorkerPool()
        {
            Stop(ShutdownMode::Drain);
        }

        bool Start(uint worker_count)
        {
            MutexLock guard(&lifecycle_lock);

            if(!work_handler||worker_count==0||started.load()||stopped.load())
                return(false);

            for(uint i=0;i<worker_count;i++)
            {
                WorkerThread<W> *worker=workers.Create(&queue,work_handler,i);

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

        bool Submit(W *work)
        {
            if(!started.load()||stopped.load()||!work)
                return(false);

            return queue.Submit(work);
        }

        int Submit(W **work,int count)
        {
            if(!work||count<=0||!started.load()||stopped.load())
                return(0);

            int submitted=0;
            while(submitted<count&&Submit(work[submitted]))
                ++submitted;

            return submitted;
        }

        bool Stop(ShutdownMode mode=ShutdownMode::Drain)
        {
            MutexLock guard(&lifecycle_lock);

            if(stopped.load())
                return(true);

            if(mode==ShutdownMode::CancelPending)
            {
                ValueArray<W *> cancelled;
                queue.CloseAndCancel(cancelled);

                for(int i=0;i<cancelled.GetCount();i++)
                    work_handler->OnCancel(cancelled[i]);
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

        int GetPendingCount(){return queue.GetCount();}
        int GetWorkerCount()const{return workers.GetCount();}
    };
}//namespace hgl::thread
