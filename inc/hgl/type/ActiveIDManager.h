#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/SortedSet.h>

namespace hgl
{
    /**
    * 活跃ID管理器
    */
    class ActiveIDManager
    {
        SortedSet<int> active_list;        ///<活跃ID列表
        Stack<int> idle_list;               ///<闲置ID列表

        int id_count;

    private:

        bool Create(int *id_list,const int count);

    public:

        ActiveIDManager(){id_count=0;}
        ~ActiveIDManager()=default;

        void Reserve(int c);

        int GetActiveCount  ()const{return active_list.GetCount();}
        int GetIdleCount    ()const{return idle_list.GetCount();}
        int GetTotalCount   ()const{return active_list.GetCount()+idle_list.GetCount();}
        int GetHistoryMaxId ()const{return id_count;}

        const DataArray<int> &GetActiveArray()const{return active_list;}
        const DataArray<int> &GetIdleArray()const{return idle_list;}

    public:

        int CreateActive(int *id,const int count=1);                            ///<创建若干ID，并置于活跌ID列表中。(注：不从闲置列表中获取)
        int CreateIdle  (int *idp=nullptr,const int count=1);                   ///<创建若干ID，并置于闲置ID列表中。(注：无视闲置列表中已存在的ID)
        int CreateIdle  (const int count=1);                                    ///<创建若干ID，并置于闲置ID列表中。(注：无视闲置列表中已存在的ID)

        bool Get        (int *id,const int count=1);                            ///<激活指定量的ID数据(优先从Idle中取,没有不会创建新的。激活后会被放入Active列表)
        bool GetOrCreate(int *id,const int count=1);                            ///<激活指定量的ID数据(优从从Idle中取,如果不够则创建新的。激活后会被放入Active列表)

        int GetIdle()                                                           ///<从闲置ID列表中获取一个ID，并将其从闲置列表中删除(注：不会创建新的ID)
        {
            int id;

            if(!Get(&id))
                return(-1);

            return id;
        }

        int Release     (const int *id,int count=1);                            ///<释放指定量的ID数据(会从Active列表中取出，放入Idle列表中)
        int ReleaseAllActive();                                                 ///<释放所有活跃ID(会从Active列表中取出，放入Idle列表中)

        bool IsActive(const int id)const{return active_list.Contains(id);}  ///<确认指定ID是否处于活跃状态
    };//class ActiveIDManager
}//namespace hgl
