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

        int created_ids[512];  // 临时缓冲，支持一次性创建最多512个ID
        int batch_size = (count > 512) ? 512 : count;
        int total_created = 0;

        while(total_created < count)
        {
            batch_size = (count - total_created > 512) ? 512 : (count - total_created);

            if(!Create(created_ids, batch_size))
            {
                return total_created;
            }

            // 将创建的ID推入Queue
            if(!idle_list.Push(created_ids, batch_size))
            {
                return total_created;
            }

            // 如果需要输出，拷贝到idp
            if(idp)
            {
                mem_copy(idp + total_created, created_ids, batch_size);
            }

            total_created += batch_size;
        }

        return total_created;
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
        * 
        * FIFO语义：从队列头部一次取出count个ID（先进先出）
        * @return 是否全部成功取出
        */
    bool ActiveIDManager::Get(int *id,const int count)
    {
        if(!id||count<=0)return(false);

        // Queue没有批量Pop，需要循环取出
        for(int i=0;i<count;i++)
        {
            if(!idle_list.Pop(id[i]))
                return(false);
        }

        active_list.Add(id,count);

        return(true);
    }

    /**
    * 激活指定量的ID数据(优从从Idle中取，如果不够则创建新的。激活后会被放入Active列表)
    * 
    * FIFO语义：
    * 1. 先从Queue头部取出全部可用的idle ID（不创建新ID）
    * 2. 如果不足，创建新的idle ID后再一次性提取
    * 这确保ID复用的公平性：最先释放的ID最先被重新使用
    */
    bool ActiveIDManager::GetOrCreate(int *id,const int count)
    {
        if(!id||count<=0)return(false);

        if(idle_list.GetCount()<count)
        {
            [[maybe_unused]] int created = CreateIdle(count-idle_list.GetCount());
        }

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
