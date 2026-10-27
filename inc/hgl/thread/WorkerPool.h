#pragma once

#include<hgl/thread/TaskQueue.h>
#include<hgl/thread/Thread.h>
#include<hgl/thread/Atomic.h>
#include<hgl/type/ManagedArray.h>

namespace hgl::workflow
{
    enum class ShutdownMode
    {
        Drain,
        CancelPending
    };

    template<typename W>
    class WorkProc
    {
    public:

        virtual ~WorkProc()=default;
        virtual void OnWork(uint worker_index,W *work)=0;
        virtual void OnCancel(W *){}
    };

    template<typename W>
    class WorkerThread:public Thread
    {
        TaskQueue<W *> *queue;
        WorkProc<W> *work_proc;
        uint worker_index;

    public:

        WorkerThread(TaskQueue<W *> *q,WorkProc<W> *proc,uint index)
            :queue(q),work_proc(proc),worker_index(index)
        {
        }

        bool Execute()override
        {
            if(!queue||!work_proc)
                return(false);

            W *work=nullptr;
            if(!queue->WaitPop(work))
                return(false);

            if(work)
                work_proc->OnWork(worker_index,work);

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
        ThreadMutex lifecycle_lock;
        WorkProc<W> *work_proc;
        ManagedArray<WorkerThread<W>> workers;
        atom_bool started{false};
        atom_bool stopped{false};

    public:

        explicit WorkerPool(WorkProc<W> *proc):work_proc(proc){}

        ~WorkerPool()
        {
            Stop(ShutdownMode::Drain);
        }

        bool Start(uint worker_count)
        {
            ThreadMutexLock guard(&lifecycle_lock);

            if(!work_proc||worker_count==0||started.load()||stopped.load())
                return(false);

            for(uint i=0;i<worker_count;i++)
            {
                WorkerThread<W> *worker=workers.Create(&queue,work_proc,i);

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
            ThreadMutexLock guard(&lifecycle_lock);

            if(stopped.load())
                return(true);

            if(mode==ShutdownMode::CancelPending)
            {
                ValueArray<W *> cancelled;
                queue.CloseAndCancel(cancelled);

                for(int i=0;i<cancelled.GetCount();i++)
                    work_proc->OnCancel(cancelled[i]);
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
}//namespace hgl::workflow
