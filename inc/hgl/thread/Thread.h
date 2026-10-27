#pragma once

#include<hgl/type/DataType.h>
#include<hgl/type/FlatOrderedSet.h>
#include<hgl/type/String.h>
#include<hgl/sync/Atomic.h>
#include<hgl/sync/Mutex.h>
#include<hgl/log/Log.h>

namespace hgl
{
    /** @brief CN: 等待平台原生线程句柄结束；不请求停止，也不接管 Thread 对象生命周期。 EN: Wait for a native thread handle to exit; this neither requests stop nor owns the Thread object. */
    bool WaitThreadExit(thread_ptr tp,const double &time_out);

    /**
     * @brief CN: 可继承的线程生命周期封装。
     *        EN: Inheritable wrapper for thread lifecycle management.
     * @details CN: 子类实现 Execute 作为工作循环：返回 true 表示允许下一轮，返回 false
     *              表示自然结束。Start 创建线程；RequestStop 仅设置协作式停止标记并调用
     *              OnStopRequested，不会强行中断阻塞中的系统调用或用户代码；Join 只等待
     *              结束；StopAndJoin 先请求停止再等待。线程对象由创建方持有，必须保证
     *              对象及其依赖项在线程结束前持续有效。只有 Join 成功后才能安全销毁或
     *              重新 Start；带超时等待失败时线程仍可能运行。
     *              生命周期回调顺序为 ProcStartThread → Execute 循环 → ProcEndThread；
     *              ProcStartThread 返回 false 时不会进入工作循环及结束回调。
     *          EN: Implement Execute as the work loop: true permits another iteration,
     *              while false ends it naturally. Start creates the thread. RequestStop
     *              sets a cooperative stop flag and calls OnStopRequested; it does not
     *              interrupt a blocked system call or user code. Join only waits;
     *              StopAndJoin requests stop first. The creator owns the Thread object
     *              and must keep it and its dependencies alive until exit. Destroy or
     *              restart it only after a successful Join; a timed-out wait may leave
     *              the thread running. Lifecycle order is ProcStartThread, the Execute
     *              loop, then ProcEndThread. If ProcStartThread returns false, neither
     *              the loop nor the end callback runs.
     */
    class Thread
    {
        OBJECT_LOGGER

        friend void WaitThread(Thread **,int,double);
        friend THREAD_FUNC ThreadFunc(Thread *tc);

    private:

        thread_ptr tp=0;

        Mutex thread_lock;
        Mutex join_lock;
        atom_bool stop_requested{false};
        atom_bool thread_running{false};

        void SetThreadRunning(bool value){thread_running.store(value);}

#ifdef _DEBUG
        U8String thread_addr_string;                                                              ///< @brief CN: 调试用线程地址字符串。 EN: Thread address string used for debugging.
#endif//_DEBUG

    public:

        virtual ~Thread()=default;

#ifdef _DEBUG
        const U8String &GetThreadAddressString()const{return thread_addr_string;}               ///< @brief CN: 取得调试用线程地址字符串。 EN: Return the thread address string used for debugging.
#endif//_DEBUG

        /**
         * @brief CN: 工作线程循环调用的执行函数。
         *        EN: Work function repeatedly called by the worker thread.
         * @return CN: true 表示继续下一轮；false 表示退出。
         *         EN: true to continue with another iteration; false to exit.
         */
        virtual bool Execute()=0;                                                                   ///< @brief CN: 线程工作函数。 EN: Thread work function.

        virtual bool ProcStartThread()                                                                ///< @brief CN: 在线程 Execute 前调用；false 拒绝启动工作循环。 EN: Called before Execute; false prevents the work loop from starting.
                {
#ifdef _DEBUG
                    u8char thread_addr[(sizeof(thread_ptr)<<1)+1];

                    DataToUpperHexStr(thread_addr,(uint8 *)&tp,sizeof(thread_ptr));

                    thread_addr_string=thread_addr;
#endif//_DEBUG

                    return(true);
                }

        virtual void ProcEndThread(){}                                                              ///< @brief CN: 在线程工作循环结束后调用。 EN: Called after the worker loop exits.

        bool IsRunning()const{return thread_running.load();}       ///< @brief CN: 线程工作体当前是否尚未结束；不表示停止请求状态。 EN: Whether the thread body is still running; this is not the stop-request state.
        bool IsStopRequested()const{return stop_requested.load();} ///< @brief CN: 是否已有线程外部请求协作式停止。 EN: Whether cooperative stop has been requested externally.

    protected:

        /** @brief CN: 子类可在此唤醒自身阻塞等待；不要在此等待线程 Join。 EN: Override to wake blocked work, such as by closing a queue; do not Join here. */
        virtual void OnStopRequested(){}

    public: // CN: 线程运行控制 EN: Thread lifecycle control

        virtual bool Start();                                                                       ///< @brief CN: 启动线程；句柄未 Join 回收前不能重复启动。 EN: Start the thread; do not restart until its handle has been joined.

        virtual bool IsCurThread();                                                                 ///< @brief CN: 判断调用者是否就是该 Thread 的工作线程。 EN: Check whether the caller is this Thread's worker thread.

        /** @brief CN: 请求协作式停止；工作体须检查标志或由回调解除阻塞。 EN: Request cooperative stop; the worker must check the flag or be unblocked by the callback. */
        virtual void RequestStop()
        {
            if(!stop_requested.exchange(true))
                OnStopRequested();
        }

        /** @brief CN: 先 RequestStop，再等待结束；超时为秒，非正数表示无限等待。 EN: Request stop, then wait for exit; timeout is in seconds and non-positive means infinite.
         */
        virtual bool StopAndJoin(const double &time_out=0);

        /** @brief CN: 只等待自然结束，不发送停止请求；超时为秒，非正数表示无限等待。 EN: Wait for natural exit without requesting stop; timeout is in seconds and non-positive means infinite.
         */
        virtual bool Join(const double &time_out=0);
    };//class Thread

    /** @brief CN: 等待线程集合中的任意一个结束；不请求停止或回收 Thread 对象。 EN: Wait for any thread in the set to exit; does not request stop or own Thread objects. */
    void WaitThread(Thread **,int,double time=0);

    /**
     * @brief CN: 对外部拥有的 Thread 指针集合进行批量启动、停止和状态统计。
     *        EN: Manage starting, stopping and status queries for externally owned Thread pointers.
     * @details CN: 管理器不负责 delete 线程对象；Close 会对每个登记对象执行
     *              StopAndJoin 并清空集合。登记的线程对象必须比管理器及其 Close 操作
     *              存活得更久。该类自身不提供对 Add/Delete 与并发 Start/Close 的同步。
     *          EN: The manager does not delete Thread objects. Close calls StopAndJoin
     *              on each registered object and clears the set. Registered threads
     *              must outlive the manager and its Close operation. Add/Delete are not
     *              synchronized with concurrent Start/Close calls.
     */
    template<typename THREAD> class MultiThreadManage
    {
    protected:

        FlatOrderedSet<THREAD *> thread_set;

    public:

        virtual ~MultiThreadManage()
        {
            Close();
        }

        THREAD *GetThread(int index)
        {
            THREAD *result;

            if(thread_set.Get(index,result))
                return result;

            return nullptr;
        }

        /**
         * @brief CN: 将一个外部拥有的线程指针加入管理集合。
         *        EN: Add a pointer to an externally owned thread to the managed set.
         * @return CN: 是否添加成功。
         *         EN: Whether the thread was added.
         */
        bool Add(THREAD *p)
        {
            if(!p)return(false);

            return(thread_set.Add(p)!=-1);
        }

        /**
         * @brief CN: 从管理集合中删除线程指针；不停止或销毁线程对象。
         *        EN: Remove a thread pointer from the set without stopping or deleting the object.
         */
        void Delete(THREAD *p)
        {
            if(!p)return;

            thread_set.Delete(p);
        }

        /**
         * @brief CN: 请求停止并等待所有登记线程结束，然后清空集合。
         *        EN: Request stop, join all registered threads, then clear the set.
         */
        void Close()
        {
            const int count=thread_set.GetCount();

            THREAD **p=thread_set.GetData();

            for(int i=0;i<count;i++)
            {
                (*p)->StopAndJoin();
                ++p;
            }

            thread_set.Clear();
        }

        /**
         * @brief CN: 尝试启动所有登记线程。
         *        EN: Attempt to start all registered threads.
         * @return CN: 成功启动的线程数量。
         *         EN: Number of threads successfully started.
         */
        int Start()
        {
            const int count=thread_set.GetCount();

            THREAD **p=thread_set.GetData();

            int total=0;

            for(int i=0;i<count;i++)
            {
                if((*p)->Start())
                    ++total;

                ++p;
            }

            return total;
        }

        /**
         * @brief CN: 统计当前仍在运行的登记线程数量。
         *        EN: Count registered threads that are currently running.
         */
        int IsRunning()
        {
            const int count=thread_set.GetCount();

            THREAD **p=thread_set.GetData();

            int total=0;

            for(int i=0;i<count;i++)
            {
                if((*p)->IsRunning())
                    ++total;

                ++p;
            }

            return total;
        }
    };//class MultiThreadManage
}//namespace hgl
