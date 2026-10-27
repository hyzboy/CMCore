#pragma once

#include<hgl/concurrency/TaskQueue.h>
#include<hgl/thread/WorkHandler.h>

namespace hgl::thread
{
    /**
     * @brief CN: 由调用线程主动驱动的工作队列，不创建后台线程。
     *        EN: Work queue driven explicitly by the caller without creating a background thread.
     * @details CN: 多个生产者可以并发 Submit；Process 只能由一个消费线程调用，
     *              例如主线程帧循环。任务指针为借用关系，调用方需保证任务在
     *              OnWork 或 OnCancel 返回前有效。
     *          EN: Multiple producers may submit concurrently. Process must be called
     *              by a single consumer, such as a main-thread frame loop. Task pointers
     *              are borrowed and must remain valid until OnWork or OnCancel returns.
     */
    template<typename W>
    class PolledWorker
    {
        TaskQueue<W *> *queue;
        WorkHandler<W> *work_handler;
        uint worker_index;

    public:

        /** @brief CN: 绑定外部队列和处理器；两者均由调用方拥有。 EN: Bind an externally owned queue and handler. */
        PolledWorker(TaskQueue<W *> *task_queue,WorkHandler<W> *handler,uint index=0)
            :queue(task_queue),work_handler(handler),worker_index(index)
        {
        }

        /** @brief CN: 提交借用任务指针。 EN: Submit a borrowed task pointer. */
        bool Submit(W *work)
        {
            return queue&&work_handler&&work&&queue->Submit(work);
        }

        /**
         * @brief CN: 在调用线程处理当前队列中的任务。
         *        EN: Process tasks currently available on the calling thread.
         * @param max_count CN: 最大处理数；非正数表示处理到当前队列为空。
         *                  EN: Maximum tasks to process; non-positive means drain currently queued tasks.
         * @return CN: 实际处理的任务数。
         *         EN: Number of tasks processed.
         */
        int Process(int max_count=0)
        {
            if(!work_handler)
                return(0);

            int processed=0;
            W *work=nullptr;

            while(queue&&(max_count<=0||processed<max_count)&&queue->TryPop(work))
            {
                if(work)
                    work_handler->OnWork(worker_index,work);

                ++processed;
            }

            return processed;
        }

        /** @brief CN: 拒绝后续投递；已接受任务仍可由 Process 排空。 EN: Reject future submissions; accepted tasks remain available to Process. */
        void Close()
        {
            if(queue)
                queue->Close();
        }

        int GetPendingCount(){return queue?queue->GetCount():0;} ///< @brief CN: 返回队列中尚未取出的任务数。 EN: Return the number of tasks not yet removed from the queue.
    };
}//namespace hgl::thread
