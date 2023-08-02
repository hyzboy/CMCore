#ifndef HGL_POOL_INCLUDE
#define HGL_POOL_INCLUDE

#include<hgl/type/List.h>
#include<hgl/type/Queue.h>
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

        virtual void    PreAlloc(int,bool set_to_max=false);                                        ///<预分配空间

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
}//namespace hgl
#include<hgl/type/Pool.cpp>
#endif//HGL_POOL_INCLUDE
