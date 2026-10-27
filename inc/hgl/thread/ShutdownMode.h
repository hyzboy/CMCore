#pragma once

namespace hgl::thread
{
    /** @brief CN: 任务执行器的关闭策略。 EN: Shutdown policy for task executors. */
    enum class ShutdownMode
    {
        Drain,          ///< @brief CN: 排空所有已接受任务。 EN: Drain all accepted tasks.
        CancelPending   ///< @brief CN: 取消尚未开始执行的任务。 EN: Cancel tasks that have not started.
    };
}//namespace hgl::thread
