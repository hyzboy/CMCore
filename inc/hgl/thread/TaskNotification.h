#pragma once

#include<hgl/concurrency/SwapMailbox.h>
#include<hgl/type/ValueArray.h>

namespace hgl::thread
{
    /**
     * @brief CN: 统一任务生命周期状态。
     *        EN: Unified task lifecycle states.
     * @details CN: Progress 为非终态的阶段性更新（所有权仍在执行方）；
     *              Done/Failed/Cancelled 为终态，每个任务至多触发一次。
     *          EN: Progress is a non-terminal incremental update (ownership stays
     *              with the executor); Done/Failed/Cancelled are terminal and
     *              fire at most once per task.
     */
    enum class TaskState:uint8
    {
        Pending = 0,    ///< @brief CN: 已接受未开跑。 EN: Accepted but not started.
        Running,        ///< @brief CN: 正在执行。 EN: Executing.
        Progress,       ///< @brief CN: 阶段性进度（非终态）。 EN: Incremental progress (non-terminal).
        Done,           ///< @brief CN: 成功完成（终态）。 EN: Completed successfully (terminal).
        Failed,         ///< @brief CN: 执行失败（终态）。 EN: Failed (terminal).
        Cancelled       ///< @brief CN: 已取消（终态）。 EN: Cancelled (terminal).
    };

    /**
     * @brief CN: 中立的任务事件载荷（POD，值语义，可直接进 ValueArray/SwapMailbox）。
     *        EN: Neutral task event payload (POD, value semantics, storable in ValueArray/SwapMailbox).
     * @details CN: 事件不拥有任何资源；task/user_data 为发起方与执行方约定的借用指针。
     *              completed/total 既可表示字节数也可表示子步数，由 task_kind 语义决定。
     *          EN: The event owns nothing; task/user_data are borrowed pointers
     *              agreed between submitter and executor. completed/total may be
     *              bytes or sub-steps depending on task_kind semantics.
     */
    struct TaskEvent
    {
        void    *task       =nullptr;   ///< @brief CN: 任务/请求对象指针（不拥有）。 EN: Task/request pointer (non-owning).
        void    *user_data  =nullptr;   ///< @brief CN: 发起方绑定的上下文。 EN: Submitter-bound context.
        uint64   task_id    =0;         ///< @brief CN: 任务唯一编号。 EN: Unique task id.
        uint32   task_kind  =0;         ///< @brief CN: 业务自定义类别/通道号。 EN: Application-defined category/channel.
        TaskState state     =TaskState::Pending; ///< @brief CN: 当前状态。 EN: Current state.
        int32    error_code =0;         ///< @brief CN: 错误码，0 表示无错误。 EN: Error code; 0 means none.
        uint64   completed  =0;         ///< @brief CN: 已完成字节/子步数。 EN: Completed bytes or sub-steps.
        uint64   total      =0;         ///< @brief CN: 总字节/总子步数。 EN: Total bytes or sub-steps.
    };

    /**
     * @brief CN: 任务事件接收器接口。
     *        EN: Task event receiver interface.
     * @details CN: 由执行线程在任务状态变化时调用。实现必须线程安全且极轻量：
     *              只做「压入本线程信息池」或「转投下一分发队列」这类动作，
     *              严禁在回调内做阻塞 IO 或长时间计算。
     *              接收器的生命周期由构造方保证覆盖所有可能投递它的执行线程。
     *          EN: Called on the executing thread when task state changes.
     *              Implementations must be thread-safe and extremely lightweight:
     *              only enqueue into a per-thread message pool or forward to the
     *              next dispatch queue; never block or do heavy work inline.
     *              The submitter must keep the receiver alive for the whole time
     *              any executor may still fire events at it.
     */
    class TaskReceiver
    {
    public:
        virtual ~TaskReceiver() = default;

        /** @brief CN: 收到一个任务事件；在执行线程上下文调用。 EN: Receive one task event; runs on the executing thread. */
        virtual void OnTaskEvent(const TaskEvent &event) = 0;
    };

    /**
     * @brief CN: 每线程批量信息池（多生产者、单消费者）。
     *        EN: Per-thread batched message pool (multi-producer, single-consumer).
     * @details CN: 内部封装 SwapMailbox。任意工作线程可 Post 通知；所属线程在固定
     *              相位用 Drain 一次性取走整批（一次锁交换处理 N 条），或在事件驱动
     *              线程里用 WaitAndDrain 阻塞等待整批。Close 后仍可排空已接受数据。
     *          EN: Wraps a SwapMailbox. Any worker may Post; the owning thread
     *              Drain()s whole batches (one lock swap for N events) at a fixed
     *              phase, or an event-driven thread blocks in WaitAndDrain().
     *              Accepted data can still be drained after Close.
     */
    template<typename EventT = TaskEvent>
    class ThreadMessagePool
    {
        SwapMailbox<EventT> mailbox;

    public:

        /** @brief CN: 投递一条通知；池已关闭时返回 false。 EN: Post one event; false if the pool is closed. */
        bool Post(const EventT &event){return mailbox.Post(event);}

        /** @brief CN: 批量投递；返回实际接受数量。 EN: Post a batch; return the accepted count. */
        int Post(const EventT *events,int count){return mailbox.Post(events,count);}

        /**
         * @brief CN: 非阻塞排空：交换并遍历一批，返回处理条数。
         *        EN: Non-blocking drain: swap and visit one batch; return processed count.
         * @param visitor     形如 void(const EventT&) 的可调用对象。
         * @param max_batches 最多连续交换的批次数（默认 1）。
         */
        template<typename F>
        int Drain(F &&visitor,int max_batches=1)
        {
            int processed=0;

            for(int i=0;i<max_batches;++i)
            {
                if(!mailbox.TrySwap())break;

                ValueArray<EventT> &batch=mailbox.GetReceive();
                const int n=batch.GetCount();

                for(int k=0;k<n;++k)
                {
                    visitor(batch[k]);
                    ++processed;
                }

                mailbox.ReleaseReceive();
            }

            return(processed);
        }

        /**
         * @brief CN: 阻塞排空：等待数据到达或池关闭后处理整批。
         *        EN: Blocking drain: wait for data or closure, then process whole batches.
         * @param timeout 单次等待秒数；非正数表示无限等待。
         * @return CN: 本次调用处理的条数；0 且 timeout>0 表示超时。
         *         EN: Events processed; 0 with timeout>0 means timeout.
         */
        template<typename F>
        int WaitAndDrain(F &&visitor,double timeout=0)
        {
            int processed=0;

            while(mailbox.WaitAndSwap(timeout))
            {
                ValueArray<EventT> &batch=mailbox.GetReceive();
                const int n=batch.GetCount();

                for(int k=0;k<n;++k)
                {
                    visitor(batch[k]);
                    ++processed;
                }

                mailbox.ReleaseReceive();

                if(timeout>0)break;     //超时语义下只消费已到的一批
            }

            return(processed);
        }

        /** @brief CN: 有待排空数据时立即处理一批（TrySwap 成功与否决定是否遍历）。 EN: Process one pending batch if immediately available. */
        template<typename F>
        bool DrainOneBatch(F &&visitor)
        {
            if(!mailbox.TrySwap())return(false);

            ValueArray<EventT> &batch=mailbox.GetReceive();
            const int n=batch.GetCount();

            for(int k=0;k<n;++k)visitor(batch[k]);

            mailbox.ReleaseReceive();
            return(true);
        }

        /** @brief CN: 关闭投递端；已接受数据仍可被排空。 EN: Close posting; accepted data remains drainable. */
        void Close(){mailbox.Close();}

        /** @brief CN: 池是否已关闭。 EN: Whether the pool is closed. */
        bool IsClosed(){return mailbox.IsClosed();}

        /** @brief CN: 当前待交换条数（不含已换出的接收批次）。 EN: Pending count (excluding the swapped-out batch). */
        int GetPendingCount(){return mailbox.GetPendingCount();}
    };//class ThreadMessagePool

    /**
     * @brief CN: 标准入池接收器：把任务事件直接压入绑定的信息池。
     *        EN: Standard inbox receiver: pushes task events into a bound pool.
     */
    class InboxTaskReceiver final : public TaskReceiver
    {
        ThreadMessagePool<TaskEvent> *pool;

    public:
        explicit InboxTaskReceiver(ThreadMessagePool<TaskEvent> *in) :pool(in) {}

        void OnTaskEvent(const TaskEvent &event) override
        {
            if(pool)pool->Post(event);
        }
    };//class InboxTaskReceiver

}//namespace hgl::thread
