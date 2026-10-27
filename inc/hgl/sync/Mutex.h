#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/Macro.h>
namespace hgl
{
    /**
    * 排斥(线程)访问类可以判断数据在被一个代码段访问时，不能被另一代码段访问<br>
    * 注：此排斥仅在当前进程内有效
    */
    class Mutex                                                                                     ///排斥访问类(仅当前进程)
    {
        thread_mutex_ptr ptr;

    public:

        Mutex();                                                                                    ///<本类构造函数
        virtual ~Mutex();                                                                           ///<本类析构函数

        thread_mutex_ptr *GetNativeHandle(){return &ptr;}

        virtual void    Lock();                                                                     ///<取得的控制权(如果对象处于排斥状态，则等待)
        virtual bool    TryLock();                                                                  ///<尝试取得控制权
#if !defined(__APPLE__)&&!defined(__ANDROID__)
        virtual bool    WaitLock(const double=0);                                                   ///<等待并取得控制权
#endif//__APPLE__
        virtual void    Unlock();                                                                   ///<放弃控制权
    };//class Mutex

    /**
     * 线程排斥对象
     */
    template<typename T> class MutexObject:public Mutex
    {
        T *data;

    public:

        MutexObject()
        {
            data=new T;
        }

        MutexObject(T *t)
        {
            data=t;
        }

        virtual ~MutexObject()
        {
            SAFE_CLEAR(data);
        }

        void operator = (T *t)
        {
            if(data)
                delete data;

            data=t;
        }

        T *operator ->()
        {
            return data;
        }

        bool valid()const
        {
            return data;
        }

        bool operator !()
        {
            return !data;
        }
    };//class MutexObject

    /**
     * 线程排斥对像阵列
     */
    template<typename T> class MutexObjectArray
    {
    protected:

        MutexObject<T> *items;

    public:

        MutexObjectArray()
        {
            items=nullptr;
        }

        MutexObjectArray(int count)
        {
            if(count<=0)
            {
                items=nullptr;
            }
            else
            {
                items=new MutexObject<T>[count];
            }
        }

        virtual ~MutexObjectArray()
        {
            delete[] items;     //delete nullptr不是个错误
        }

        MutexObject<T> &operator [](int index)
        {
            return items[index];
        }
    };//class MutexObjectArray

    /**
     * 智能自释放线程排斥锁
     */
    class MutexLock
    {
        Mutex *mutex;

    public:

        MutexLock(const MutexLock &)=delete;
        MutexLock &operator=(const MutexLock &)=delete;

        MutexLock(Mutex *mutex_to_lock)
        {
            mutex=mutex_to_lock;

            if(mutex)
                mutex->Lock();
        }

        ~MutexLock()
        {
            if(mutex)
                mutex->Unlock();
        }

        void Lock()
        {
            if(mutex)
                mutex->Lock();
        }

        void Unlock()
        {
            if(mutex)
                mutex->Unlock();
        }

        void Clear()
        {
            if(mutex)
            {
                mutex->Unlock();
                mutex=nullptr;
            }
        }
    };//class MutexLock
}//namespace hgl
