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
     *              请求停止会关闭队列并排空已接受任务；对象不可重启。
     *          EN: Submit may be called concurrently by multiple threads; one worker
     *              processes tasks in queue order. The queue and handler are borrowed
     *              and must outlive this object and its thread. Stop closes the queue
     *              and drains accepted tasks; this worker cannot be restarted.
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

    public:

        SingleWorker(TaskQueue<W *> *task_queue,WorkHandler<W> *handler)
            :queue(task_queue),work_handler(handler),worker(task_queue,handler)
        {
        }

        ~SingleWorker()
        {
            Stop();
        }

        /** @brief CN: 提交借用任务指针；线程未运行或队列关闭时返回 false。 EN: Submit a borrowed task pointer; false if the thread is not running or the queue is closed. */
        bool Submit(W *work)
        {
            return queue&&work&&queue->Submit(work);
        }

        bool Start()
        {
            return queue&&work_handler&&worker.Start();
        }

        /** @brief CN: 关闭队列并等待已接受任务排空；执行器不可重启。 EN: Close the queue and wait for accepted work to drain; this executor cannot restart. */
        bool Stop(double timeout=0)
        {
            if(queue)
                queue->Close();

            return worker.Join(timeout);
        }

        bool IsRunning()const{return worker.IsRunning();}
    };
}//namespace hgl::thread
