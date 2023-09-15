#pragma once

#include<hgl/type/DataArray.h>
#include<hgl/type/Stack.h>
#include<hgl/type/SortedSets.h>

namespace hgl
{
    /**
    * 活跃ID管理器
    */
    class ActiveIDManager
    {
        SortedSets<int> active_list;        ///<活跃ID列表
        Stack<int> idle_list;               ///<闲置ID列表

        int id_count;

    private:

        bool Create(int *id_list,const int count)
        {
            if(!id_list||count<=0)return(false);

            for(int i=id_count;i<id_count+count;i++)
            {
                *id_list=i;
                ++id_list;
            }

            id_count+=count;

            return(true);
        }

    public:

        ActiveIDManager()
        {
            id_count=0;
        }

        ~ActiveIDManager()=default;

        void Alloc(int c)
        {
            active_list.PreAlloc(c);
            idle_list.PreAlloc(c);
        }

        int GetActiveCount  ()const{return active_list.GetCount();}
        int GetIdleCount    ()const{return idle_list.GetCount();}
        int GetTotalCount   ()const{return active_list.GetCount()+idle_list.GetCount();}
        int GetHistoryMaxId ()const{return id_count;}

        const DataArray<int> &GetActiveArray()const{return active_list;}
        const DataArray<int> &GetIdleArray()const{return idle_list;}

    public:

        /**
         * 创建若干ID，并置于活跌ID列表中。（注：不从闲置列表中获取)
         * @return 成功添加的个数
         */
        int CreateActive(int *id,const int count=1)
        {
            if(!id||count<=0)return(0);

            if(!Create(id,count))return(0);

            return active_list.Add(id,count);
        }

        /**
         * 创建若干ID，并置于闲置ID列表中。(注：无视闲置列表中已存在的ID)
         * @return 成功添加的个数
         */
        int CreateIdle(const int count=1)
        {
            if(count<=0)return(0);

            const int new_count=idle_list.GetCount()+count;

            idle_list.PreAlloc(new_count);

            int *end=idle_list.end();

            if(!Create(end,count))return(0);

            idle_list.SetCount(new_count);

            return(count);
        }

        /**
         * 激活指定量的ID数据(优先从Idle中取,没有不会创建新的。激活后会被放入Active列表)
         */
        bool Get(int *id,const int count=1)
        {
            if(!id||count<=0)return(false);

            if(!idle_list.Pop(id,count))
                return(false);

            active_list.Add(id,count);

            return(true);
        }

        /**
        * 激活指定量的ID数据(优从从Idle中取，如果不够则创建新的。激活后会被放入Active列表)
        */
        bool GetOrCreate(int *id,const int count=1)
        {
            if(!id||count<=0)return(false);

            if(idle_list.GetCount()<count)
                CreateIdle(count-idle_list.GetCount());

            return Get(id,count);
        }

        /**
        * 释放指定量的ID数据(会从Active列表中取出，放入Idle列表中)
        */
        int Release(int *id,int count=1)
        {
            if(!id||count<=0)return(false);

            int result=0;

            while(count--)
            {
                if(active_list.Delete(*id))
                {
                    idle_list.Push(*id);
                    ++result;
                }

                ++id;
            }

            return result;
        }

        /**
        * 释放所有活跃ID
        */
        int ReleaseAllActive()
        {
            const int count=active_list.GetCount();

            if(count>0)
            {
                idle_list.Push(active_list.GetData(),count);

                active_list.Clear();
            }

            return count;
        }
    };//class ActiveIDManager
}//namespace hgl
