#pragma once

#include<hgl/type/List.h>
#include<hgl/type/Queue.h>
#include<hgl/type/LifetimeCallback.h>
namespace hgl
{
    /**
    * 数据池模板用于管于两个队列，一个在用的，一个空闲的。默认情部下空闲队列使用Queue模板管理(先入先出，总是使用最早扔进去的数据，可手动换成Stack性能更好)，活动队列使用List模板管理(无序)。
    */
    template<typename T,typename AT,typename IT,typename DEFAULT_DLC> class _Pool                                        ///数据池
    {
    protected:

        DEFAULT_DLC default_dlc;

        AT Active;
        IT Inactive;

        int max_active_count;
        int history_max;                                                                            ///<历史最大数量

        void UpdateHistoryMax()
        {
            if(Active.GetCount()+Inactive.GetCount()>history_max)
                history_max=Active.GetCount()+Inactive.GetCount();
        }

    protected:

        DataLifetimeCallback<T> *dlc;                                                               ///<数据生命周期回调函数

    public: //属性

        int GetActiveCount()    const{return Active.GetCount();}                                    ///<取得活动数据数量
        int GetInactiveCount()  const{return Inactive.GetCount();}                                  ///<取得非活动数据数量
        int GetHistoryMaxCount()const{return history_max;}                                          ///<取得历史性最大数据数量

        DataArray<T> & GetActiveArray(){return Active.GetArray();}                                  ///<取得所有活跃数据

        bool IsActive   (const T &data)const{return Active.IsExist(data);}                          ///<是否为活跃的
        bool IsInactive (const T &data)const{return Inactive.IsExist(data);}                        ///<是否为非活跃的

        bool IsFull()const                                                                          ///<活跃队列是否已满
        {
            if(max_active_count>0&&Active.GetCount()>=max_active_count)
                return(true);

            return(false);
        }

    public:

        _Pool()
        {
            max_active_count=0;
            history_max=0;
            dlc=&default_dlc;
        }
        virtual ~_Pool()=default;

        virtual void    SetDataLifetimeCallback(DataLifetimeCallback<T> *cb)                        ///<设定数据生命周期回调函数
                        {
                            dlc=cb?cb:&default_dlc;
                        }

        virtual void    PreAlloc(int count,bool set_to_max=false)                                   ///<预分配空间
                        {
                            Active.PreAlloc(count);
                            Inactive.PreAlloc(count);

                            if(set_to_max)
                                max_active_count=count;
                        }

        virtual void    SetMaxActiveCount(int mc){max_active_count=mc;}                             ///<设定最大活跃数量限制

        virtual bool    Create(T &value)                                                            ///<创建一个数据,并放置在活跃队列中
                        {
                            if(IsFull())
                                return(false);

                            if(!dlc->Create(&value))
                                return(false);

                            Active.Add(value);
                            UpdateHistoryMax();
                            return(true);
                        }

        virtual bool    GetOrCreate(T &value)                                                       ///<获取一个数据(如果没有空余，创建新的)
                        {
                            if(!Inactive.Pop(value))
                            {
                                if(IsFull())
                                    return(false);

                                if(!dlc->Create(&value))
                                    return(false);
                            }
                            else
                            {
                                dlc->OnActive(&value);
                            }

                            Active.Add(value);
                            UpdateHistoryMax();
                            return(true);
                        }

        virtual bool    Get(T &value)                                                               ///<获取一个数据(如果没有空余，返回失败)
                        {
                            if(!Inactive.Pop(value))
                                return(false);

                            dlc->OnActive(&value);

                            Active.Add(value);
                            return(true);
                        }

        virtual bool    Append(T value)                                                             ///<添加一个外部创建的数据入活跃队列
                        {
                            if(IsFull())
                                return(false);

                            Active.Add(value);

                            UpdateHistoryMax();
                            return(true);
                        }

        virtual bool    Release(T value)                                                            ///<释放一个数据
                        {
                            int index=Active.Find(value);

                            if(index>=0)
                            {
                                Active.Delete(index);

                                if(!Inactive.Push(value))
                                    return(false);

                                dlc->OnIdle(&value);
                                return(true);
                            }

                            return(false);
                        }

        virtual int     Release(T *vl,int count)                                                    ///<释放一批数据
                        {
                            if(!vl||count<=0)
                                return(0);

                            int result=0;

                            for(int i=0;i<count;i++)
                            {
                                if(Release(vl[i]))
                                    ++result;
                            }

                            return(result);
                        }

        virtual void    ReleaseActive()                                                             ///<释放所有活跃数据
                        {
                            dlc->OnIdle(Active.GetData(),Active.GetCount());

                            Inactive.Push(Active.GetData(),Active.GetCount());
                            Active.ClearData();
                        }

        virtual void    ClearActive()
                        {
                            dlc->Clear(Active.GetData(),Active.GetCount());

                            Active.ClearData();
                        }

        virtual void    ClearInactive()                                                             ///<清除所有非活跃数据
                        {
                            Inactive.Clear(dlc);
                        }
    };//template<typename T,typename AT,typename IT> class _Pool

    template<typename T> using Pool         =_Pool<T,   List<T>,    Queue<T>,       DataLifetimeCallback<T>>;           ///<数据池模板

    template<typename T> class ObjectPool:public _Pool<T *, List<T *>,  ObjectQueue<T>, DefaultObjectLifetimeCallback<T>>  ///<对象池
    {
    public:

        using _Pool<T *, List<T *>,  ObjectQueue<T>, DefaultObjectLifetimeCallback<T>>::_Pool;

        virtual ~ObjectPool() override
        {
            ClearActive();
            ClearInactive();
        }
    };
}//namespace hgl
