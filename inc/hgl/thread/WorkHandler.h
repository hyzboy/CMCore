#pragma once

#include<hgl/type/DataType.h>

namespace hgl::thread
{
    /**
     * @brief CN: 与任务投递和调度拓扑无关的工作处理接口。
     *        EN: Work-processing interface independent of task posting and scheduling topology.
     * @details CN: 执行器以借用指针调用此接口，不转移任务对象所有权。多个 worker
     *              可能并发调用同一个处理器；实现类必须自行同步共享状态。OnCancel
     *              表示任务未开始执行且被取消，默认不做处理。
     *          EN: Executors call this interface with borrowed pointers and do not take
     *              ownership of task objects. Multiple workers may call one handler
     *              concurrently, so implementations must synchronize shared state.
     *              OnCancel reports a task that was cancelled before execution.
     */
    template<typename W>
    class WorkHandler
    {
    public:

        virtual ~WorkHandler()=default;

        /** @brief CN: 执行一个任务；同一处理器可能被多个 worker 并发调用。 EN: Execute one task; the same handler may be called concurrently by multiple workers. */
        virtual void OnWork(uint worker_index,W *work)=0;

        /** @brief CN: 通知一个尚未开始执行的任务已取消。 EN: Notify that a task was cancelled before execution began. */
        virtual void OnCancel(W *){}
    };
}//namespace hgl::thread
