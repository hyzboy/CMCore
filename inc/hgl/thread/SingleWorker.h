#pragma once

#include<hgl/concurrency/TaskQueue.h>
#include<hgl/thread/Thread.h>
#include<hgl/thread/WorkHandler.h>

namespace hgl::thread
{
    /**
     * @brief CN: 使用独立 FIFO 队列驱动一个工作处理器的后台线程。
     *        EN: Background thread that drives one work handler from an independent FIFO queue.
     * @details CN: Submit 可由多个线程并发调用，任务按队列顺序被唯一 worker 处理。
     *              队列和处理器均为借用关系，必须比本对象及其工作线程存活更久。
     *              Stop 会关闭传入队列并排空已接受任务；队列所有者不得在此后复用该队列。
     *              对象不可重启。
     *          EN: Submit may be called concurrently by multiple threads; one worker
     *              processes tasks in queue order. The queue and handler are borrowed
     *              and must outlive this object and its thread. Stop closes the queue
     *              and drains accepted tasks; the queue owner must not reuse it afterward.
     *              This worker cannot be restarted.
     */
    template<typename W>
    class SingleWorker
    {
        class WorkerThread:public Thread
        {
            TaskQueue<W *> *queue;
            WorkHandler<W> *work_handler;

        public:

            WorkerThread(TaskQueue<W *> *task_queue,WorkHandler<W> *handler)
                :queue(task_queue),work_handler(handler)
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
                    work_handler->OnWork(0,work);

                return(true);
            }
        };

        TaskQueue<W *> *queue;
        WorkHandler<W> *work_handler;
        WorkerThread worker;
        bool started=false;
        bool stopped=false;

    public:

        SingleWorker(TaskQueue<W *> *task_queue,WorkHandler<W> *handler)
            :queue(task_queue),work_handler(handler),worker(task_queue,handler)
        {
        }

        ~SingleWorker()
        {
            Stop();
        }

        /** @brief CN: 提交借用任务指针；队列为空指针、任务为空或队列已关闭时返回 false。 EN: Submit a borrowed task pointer; false for a null queue, null task or closed queue. */
        bool Submit(W *work)
        {
            return queue&&work&&queue->Submit(work);
        }

        /** @brief CN: 启动唯一后台 worker；队列和处理器无效或执行器已启动/停止时返回 false。 EN: Start the single background worker; false if dependencies are invalid or the executor was started/stopped. */
        bool Start()
        {
            if(!queue||!work_handler||started||stopped)
                return(false);

            started=worker.Start();
            return started;
        }

        /** @brief CN: 关闭队列并等待已接受任务排空；执行器不可重启。 EN: Close the queue and wait for accepted work to drain; this executor cannot restart. */
        bool Stop(double timeout=0)
        {
            if(stopped)
                return(true);

            if(queue)
                queue->Close();

            if(!worker.Join(timeout))
                return(false);

            stopped=true;
            return(true);
        }

        bool IsRunning()const{return worker.IsRunning();} ///< @brief CN: 查询后台 worker 是否仍在运行。 EN: Return whether the background worker is running.
    };
}//namespace hgl::thread
