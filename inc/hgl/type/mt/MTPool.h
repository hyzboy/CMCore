#pragma once

#include<hgl/type/ObjectPool.h>
#include<hgl/thread/RWLock.h>
#include<hgl/thread/ThreadMutex.h>

namespace hgl
{
    template<typename T> class MTPool:public Pool<T>                                                ///多线程数据池
    {
        RWLock lock;

    protected:

        virtual T Create()=0;                                                                       ///<创建数据
        virtual void Clear(T)=0;                                                                    ///<清除数据

    public:

        virtual int GetActiveCount()    {OnlyReadLock rl(lock);return Pool<T>::Active.GetCount();   }
        virtual int GetInactiveCount()  {OnlyReadLock rl(lock);return Pool<T>::Inactive.GetCount(); }
        virtual int GetHistoryMaxCount(){OnlyReadLock rl(lock);return Pool<T>::history_max;         }

    public:

        virtual ~MTPool()=default;

        virtual T *ReadLock(int &c)                                                                 ///<读列表锁定(用于访问整个列表)
        {
            lock.ReadLock();

            c=Pool<T>::GetActiveCount();
            return(Pool<T>::Active.GetData());
        }

        virtual T *WriteLock(int &c)                                                                ///<写列表锁定(用于访问整个列表)
        {
            lock.WriteLock();

            c=Pool<T>::GetActiveCount();
            return(Pool<T>::Active.GetData());
        }

        virtual T *TryReadLock(int &c)                                                              ///<尝试读列表锁定(用于访问整个列表)
        {
            if(!lock.TryReadLock())
                return(nullptr);

            c=Pool<T>::GetActiveCount();
            return(Pool<T>::Active.GetData());
        }

        virtual T *TryWriteLock(int &c)                                                             ///<尝试写列表锁定(用于访问整个列表)
        {
            if(!lock.TryWriteLock())
                return(nullptr);

            c=Pool<T>::GetActiveCount();
            return(Pool<T>::Active.GetData());
        }

        virtual void ReadLock(){lock.ReadLock();}
        virtual void WriteLock(){lock.WriteLock();}
        virtual bool TryReadLock(){return lock.TryReadLock();}
        virtual bool TryWriteLock(){return lock.TryWriteLock();}

        virtual void ReadUnlock(){lock.ReadUnlock();}                                               ///<读访问解锁(用于访问整个列表)
        virtual void WriteUnlock(){lock.WriteUnlock();}                                             ///<写访问解锁(用于访问整个列表)

        virtual T SafeAcquire()                                                                     ///<安全申请一个数据
        {
            T tmp;

            lock.WriteLock();
            tmp=Pool<T>::Acquire();
            lock.WriteUnlock();

            return tmp;
        }

        virtual void SafeAppend(T tmp)                                                              ///<安全添加一个数据
        {
            lock.WriteLock();
            Pool<T>::Append(tmp);
            lock.WriteUnlock();
        }

        virtual bool SafeRelease(T tmp)                                                             ///<安全释放一个数据
        {
            bool result;

            lock.WriteLock();
            result=Pool<T>::Release(tmp);
            lock.WriteUnlock();

            return result;
        }

        virtual int SafeRelease(T *tmp,int num)                                                     ///<安全释放一批数据
        {
            int result;

            lock.WriteLock();
            result=Pool<T>::Release(tmp,num);
            lock.WriteUnlock();

            return result;
        }

        virtual int SafeReleaseAll()                                                                ///<安全释放所有数据
        {
            int result;

            lock.WriteLock();
            result=Pool<T>::ReleaseAll();
            lock.WriteUnlock();

            return(result);
        }

        virtual void SafeClearInactive()                                                            ///<安全清除所有空闲数据
        {
            lock.WriteLock();
            Pool<T>::ClearInactive();
            lock.WriteUnlock();
        }

        virtual void SafeClearAll()                                                                 ///<安全清除所有的
        {
            lock.WriteLock();
            Pool<T>::ClearAll();
            lock.WriteUnlock();
        }
    };//template<typename T> class MTPool

    template<typename T> class _MTObjectPool:public MTPool<T *>                                     ///多线程对象池
    {
        virtual T *Create() override=0;

        virtual void Clear(T *obj) override {   if(obj)delete obj;  }

    public:

        using MTPool<T *>::MTPool;
        virtual ~_MTObjectPool(){MTPool<T *>::ClearAll();}
    };//template<typename T> class MTObjectPool

    template<typename T> class MTObjectPool:public _MTObjectPool<T>                                     ///多线程对象池
    {
        virtual T *Create() override {return(new T());}

        virtual void Clear(T *obj) override {   if(obj)delete obj;  }

    public:

        using _MTObjectPool<T>::_MTObjectPool;
        virtual ~MTObjectPool(){_MTObjectPool<T>::ClearAll();}
    };//template<typename T> class MTObjectPool
}//namespace hgl
