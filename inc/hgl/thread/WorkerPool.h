#pragma once

#include<hgl/concurrency/TaskQueue.h>
#include<hgl/thread/Thread.h>
#include<hgl/thread/WorkHandler.h>
#include<hgl/sync/Atomic.h>
#include<hgl/type/ManagedArray.h>

namespace hgl::thread
{
    /** @brief CN: 工作池关闭策略：Drain 执行已接受任务；CancelPending 取消未开始任务。 EN: Pool shutdown mode: Drain runs accepted tasks; CancelPending cancels tasks not yet started. */
    enum class ShutdownMode
    {
        Drain,
        CancelPending
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
     * @brief CN: 由固定数量工作线程组成的任务池。
     *        EN: Task pool backed by a fixed number of worker threads.
     * @details CN: Start 仅可成功调用一次，创建固定数量的 worker；Submit 是非阻塞提交，
     *              返回 false 表示任务未被接受。任务指针为借用关系，指向的对象必须持续
     *              有效，直到对应的 OnWork 或 OnCancel 回调返回。不同 worker 的 OnWork
     *              可并发执行，因此共享状态由 WorkHandler 自行同步。
     *              Stop(Drain) 关闭队列并等待已接受任务全部处理；Stop(CancelPending)
     *              取消队列中尚未取出的任务并调用 OnCancel，仍等待正在执行的 OnWork
     *              返回。Stop 可能阻塞，必须从 worker 回调之外调用；处理回调不可重入
     *              Start/Stop。成功 Stop 后工作池不可重启，析构时会执行 Drain。
     *          EN: Start can succeed only once and creates the fixed worker set. Submit
     *              is non-blocking and false means the task was not accepted. Task
     *              pointers are borrowed; the pointed-to objects must remain alive
     *              until OnWork or OnCancel returns. OnWork may run concurrently across
     *              workers, so WorkHandler must synchronize shared state.
     *              Stop(Drain) closes the queue and waits for accepted tasks to finish.
     *              Stop(CancelPending) cancels queued tasks through OnCancel but still
     *              waits for running OnWork callbacks. Stop may block and must be called
     *              outside worker callbacks; callbacks must not re-enter Start/Stop.
     *              A successfully stopped pool cannot restart; destruction drains it.
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

        /** @brief CN: 提交非空任务指针；池未启动、已停止或队列关闭时返回 false。 EN: Submit a non-null task pointer; false if the pool is not started, is stopped, or its queue is closed. */
        bool Submit(W *work)
        {
            if(!started.load()||stopped.load()||!work)
                return(false);

            return queue.Submit(work);
        }

        /**
         * @brief CN: 顺序提交一组任务指针。
         *        EN: Submit a sequence of task pointers in order.
         * @return CN: 已接受的前缀长度；遇到失败立即停止，不跳过任务继续提交。
         *         EN: Number of tasks accepted from the beginning of the sequence; stops at the first rejection.
         */
        int Submit(W **work,int count)
        {
            if(!work||count<=0||!started.load()||stopped.load())
                return(0);

            int submitted=0;
            while(submitted<count&&Submit(work[submitted]))
                ++submitted;

            return submitted;
        }

        /**
         * @brief CN: 关闭任务入口并等待 worker 结束。
         *        EN: Close task submission and wait for workers to exit.
         * @param mode CN: Drain 排空全部已入队任务；CancelPending 取消仍排队任务，不中断执行中任务。
         *             EN: Drain all queued tasks, or cancel queued tasks without interrupting running work.
         * @return CN: 至少一个 worker 未能 Join 时返回 false。
         *         EN: False if at least one worker could not be joined.
         */
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
