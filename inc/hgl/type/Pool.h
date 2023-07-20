﻿#ifndef HGL_POOL_INCLUDE
#define HGL_POOL_INCLUDE

#include<hgl/type/List.h>
#include<hgl/type/Queue.h>
#include<hgl/thread/RWLock.h>
#include<hgl/thread/ThreadMutex.h>
#include<hgl/type/SortedSets.h>
namespace hgl
{
    /**
    * 数据池模板用于管于两个队列，一个在用的，一个空闲的
    */
    template<typename T> class Pool                                                                 ///数据池
    {
    protected:

        List<T> Active;
        Queue<T> Inactive;

        int alloc_count;                                                                            ///<已分配的数据数量
        int max_count;                                                                              ///<最大可分配数量
        int history_max;                                                                            ///<历史最大数量

    protected:

        virtual T Create()=0;                                                                       ///<创建数据
        virtual void Clear(T)=0;                                                                    ///<清除数据

        void Clear(T *,int);

    public: //属性

        int GetMaxCount()       const{return max_count;}                                            ///<最大最大限制
        int GetActiveCount()    const{return Active.GetCount();}                                    ///<取得活动数据数量
        int GetInactiveCount()  const{return Inactive.GetCount();}                                  ///<取得非活动数据数量
        int GetHistoryMaxCount()const{return history_max;}                                          ///<取得历史性最大数据数量

        T * GetActiveData()     const{return Active.GetData();}                                     ///<取得所有活跃数据
        T * GetInactiveData()   const{return Inactive.GetData();}                                   ///<取得所有非活跃数据

    public:

        Pool(){alloc_count=0;history_max=0;max_count=0;}
        virtual ~Pool()=default;

        virtual void    PreAlloc(int,bool set_to_max=false);                                       ///<预分配空间

        virtual int     SetMaxCount(int);                                                           ///<设定最大数量限制

        virtual bool    Acquire(T &);                                                               ///<申请一个数据(如果没有空余，创建新的)
        virtual bool    Get(T &);                                                                   ///<获取一个数据(如果没有空余，返回失败)
        virtual void    Append(T);                                                                  ///<添加一个数据
        virtual bool    Release(T);                                                                 ///<释放一个数据
        virtual int     Release(T *,int);                                                           ///<释放一批数据
        virtual int     ReleaseAll();                                                               ///<释放所有数据

                bool    IsActive(const T &data)const{return Active.IsExist(data);}
                bool    IsInactive(const T &data)const{return Inactive.IsExist(data);}

        virtual void    ClearActive();                                                              ///<清除所有活跃的
        virtual void    ClearInactive();                                                            ///<清除所有空闲的
        virtual void    ClearAll();                                                                 ///<清除所有的
    };//template<typename T> class Pool

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

    template<typename T> class _ObjectPool:public Pool<T *>                                         ///对象池
    {
    protected:

        virtual T *Create()=0;

        virtual void Clear(T *obj)  {   if(obj)delete obj;  }

    public:

        using Pool<T *>::Pool;
        virtual ~_ObjectPool(){Pool<T *>::ClearAll();}

        virtual bool Release(T *obj) override                                                       ///<释放一个数据
        {
            if(!obj)return(true);

            return Pool<T *>::Release(obj);
        }
    };//template<typename T> class _ObjectPool

    template<typename T> class ObjectPool:public _ObjectPool<T>                                     ///对象池
    {
        virtual T *Create()override{return(new T());}

    public:

        using _ObjectPool<T>::_ObjectPool;
        virtual ~ObjectPool(){_ObjectPool<T>::ClearAll();}
    };//template<typename T> class ObjectPool

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

    template<typename T> class MemBlockPool:public Pool<T *>                                        ///内存块池
    {
        int memblock_size;

        T *Create()         {   return(new T[memblock_size]);   }

        void Clear(T *obj)  {   if(obj)delete[] obj;    }

    public:

        MemBlockPool(int size){SetSize(size);}
        virtual ~MemBlockPool(){Pool<T *>::ClearAll();}

        void SetSize(int size){memblock_size=size;}
        int GetSize(){return memblock_size;}
    };//template<typename T> class MemBlockPool

    template<typename T> class MTMemBlockPool:public MTPool<T *>                                    ///多线程内存块池
    {
        int memblock_size;

        T *Create()         {   return(new T[memblock_size]);   }

        void Clear(T *obj)  {   if(obj)delete[] obj;    }

    public:

        MTMemBlockPool(int size){SetSize(size);}
        virtual ~MTMemBlockPool(){MTPool<T *>::ClearAll();}

        void SetSize(int size){memblock_size=size;}
        int GetSize(){return memblock_size;}
    };//template<typename T> class MTMemBlockPool
}//namespace hgl
#include<hgl/type/Pool.cpp>
#endif//HGL_POOL_INCLUDE
