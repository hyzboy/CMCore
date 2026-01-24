#include<hgl/type/ActiveIDManager.h>

namespace hgl
{
    bool ActiveIDManager::Create(int *id_list,const int count)
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

    void ActiveIDManager::Reserve(int c)
    {
        active_list.Reserve(c);
        idle_list.Reserve(c);
    }

    int ActiveIDManager::CreateActive(int *id,const int count)
    {
        if(!id||count<=0)return(0);

        if(!Create(id,count))return(0);

        return active_list.Add(id,count);
    }

    /**
        * 创建若干ID，并置于闲置ID列表中。(注：无视闲置列表中已存在的ID)
        * @return 成功添加的个数
        */
    int ActiveIDManager::CreateIdle(int *idp,const int count)
    {
        if(count<=0)return(0);

        const int old_count=idle_list.GetCount();
        const int new_count=old_count+count;

        idle_list.Reserve(new_count);
        idle_list.Resize(new_count);

        int *start=idle_list.begin()+old_count;

        if(!Create(start,count))
        {
            idle_list.Resize(old_count);
            return(0);
        }

        if(idp)
            mem_copy(idp,start,count);

        return(count);
    }


    /**
        * 创建若干ID，并置于闲置ID列表中。(注：无视闲置列表中已存在的ID)
        * @return 成功添加的个数
        */
    int ActiveIDManager::CreateIdle(const int count)
    {
        return CreateIdle(nullptr,count);
    }

    /**
        * 激活指定量的ID数据(优先从Idle中取,没有不会创建新的。激活后会被放入Active列表)
        */
    bool ActiveIDManager::Get(int *id,const int count)
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
    bool ActiveIDManager::GetOrCreate(int *id,const int count)
    {
        if(!id||count<=0)return(false);

        if(idle_list.GetCount()<count)
            CreateIdle(count-idle_list.GetCount());

        return Get(id,count);
    }

    /**
    * 释放指定量的ID数据(会从Active列表中取出，放入Idle列表中)
    */
    int ActiveIDManager::Release(const int *id,int count)
    {
        if(!id||count<=0)return(0);

        int result=0;

        while(count--)
        {
            if(active_list.Delete(*id))
            {
                idle_list.Push(*id);
                ++result;
                ++released_count;  // 统计已释放ID
            }

            ++id;
        }

        return result;
    }

    /**
    * 释放所有活跃ID
    */
    int ActiveIDManager::ReleaseAllActive()
    {
        const int count=active_list.GetCount();

        if(count>0)
        {
            idle_list.Push(active_list.GetData(),count);
            released_count += count;  // 统计已释放ID

            active_list.Clear();
        }

        return count;
    }
}//namespace hgl
