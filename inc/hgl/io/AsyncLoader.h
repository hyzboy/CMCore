#pragma once


#include<hgl/type/String.h>
#include<hgl/object/EnumObject.h>
#include<hgl/thread/Thread.h>
namespace hgl
{
    class HAC;

    /**
     * @brief CN: 基于 Thread 的异步文件加载流程基类。
     *        EN: Base class for asynchronous file-loading workflows built on Thread.
     * @details CN: Load 提供文件与目标 HAC 对象的加载入口；Execute 是线程工作循环，
     *              Update 由调用侧推进需要外部环境参与的状态转换。ProcFinish、ProcEnd、
     *              ProcClear 是供派生类覆写的流程钩子，OnError/OnEnd 是可订阅事件。
     *              状态枚举反映加载流程进度，不等同于 Thread 运行状态。异步执行期间
     *              本对象及其依赖项必须持续有效；回调的具体执行线程需以派生实现为准，
     *              不应仅凭状态名推断。
     *          EN: Load starts processing a file into a target HAC object; Execute is
     *              the worker loop, while callers use Update to advance transitions
     *              requiring the external environment. ProcFinish, ProcEnd and
     *              ProcClear are overridable workflow hooks; OnError and OnEnd are
     *              subscribable events. The state enum describes loading progress, not
     *              Thread activity. Keep this object and its dependencies alive during
     *              asynchronous work. Callback thread affinity depends on derived
     *              implementations and must not be inferred from state names.
     */
    class AsyncLoader:public EnumObject,public Thread
    {
    public:

        /** @brief CN: 加载状态机阶段；lsEnd 表示加载/清理流程已完成。 EN: Loading state; lsEnd means loading and cleanup have completed. */
        enum AsyncLoaderStatus  // CN: 加载器状态 EN: Loader state
        {
            lsNone=0,           ///< @brief CN: 尚未使用。 EN: Not used yet.

            lsLoad,             ///< @brief CN: 正在加载。 EN: Loading.
            lsProc,             ///< @brief CN: 正在处理。 EN: Processing.
            lsProcFinish,       ///< @brief CN: 处理完成。 EN: Processing completed.
            lsClear,            ///< @brief CN: 正在清理。 EN: Cleaning up.
            lsClearFinish,      ///< @brief CN: 清理完成。 EN: Cleanup completed.

            lsEnd,              ///< @brief CN: 流程完成。 EN: Workflow completed.
        };

    protected:

        AsyncLoaderStatus status;

        void SetStatus(AsyncLoaderStatus);

        HAC *loader_hac;
        U16String loader_filename;
        void *loader_pointer;
        void *loader_data;
        int loader_size;

    protected:  // CN: 加载流程阶段回调 EN: Loading workflow stage callbacks

        virtual void ProcFinish()=default;                                                          ///< @brief CN: 后台读取/处理完成后调用的完成阶段钩子。 EN: Completion hook called after background reading or processing.
        virtual void ProcEnd()=default;                                                             ///< @brief CN: 按流程在完成回调之后执行的结束阶段钩子。 EN: End-stage hook called after the completion callback.
        virtual void ProcClear()=default;                                                           ///< @brief CN: 释放或清理加载资源的阶段钩子。 EN: Hook for releasing or cleaning up loading resources.

    public: // CN: 事件 EN: Events

        DefEvent(void,OnError,(AsyncLoader *));                                                     ///< @brief CN: 加载失败时触发的事件。 EN: Event raised when loading fails.
        DefEvent(void,OnEnd,(AsyncLoader *));                                                       ///< @brief CN: 完成事件，在 ProcEnd 后、ProcClear 前触发。 EN: Completion event raised after ProcEnd and before ProcClear.

    public:

        AsyncLoader();
        virtual ~AsyncLoader()=default;

        /** @brief CN: Thread 工作循环入口；由线程系统调用，不应由业务代码手工驱动。 EN: Thread worker-loop entry, invoked by the thread system rather than business code. */
        bool Execute();

        /** @brief CN: 在调用侧推进需要主线程/外部环境参与的状态转换及完成事件。 EN: Advance transitions and completion events that require the caller or external environment. */
        void Update();

    public: // CN: 方法 EN: Methods

        bool Load(HAC *,const u16char *);                                                           ///< @brief CN: 提交文件加载请求；是否接受取决于当前状态。 EN: Submit a file-load request; acceptance depends on the current state.

        AsyncLoaderStatus GetStatus();                                                              ///< @brief CN: 取得当前加载状态。 EN: Return the current loading state.
    };//class AsyncLoader
}//namespace hgl
