#include<hgl/type/ActiveIDManager.h>

namespace hgl
{
    bool ActiveIDManager::Create(int *id_list,const int count)
    {
        if(!id_list||count<=0)return(false);

        // 检查是否会导致ID溢出
        if(id_count > INT_MAX - count)
        {
            // ID容量已满，无法继续分配
            return false;
        }

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
        // btree_set 无需手动预留容量
        // deque 自动扩展，无需预留
    }

    int ActiveIDManager::CreateActive(int *id,const int count)
    {
        if(!id||count<=0)return(0);

        if(!Create(id,count))return(0);

        // 批量插入到 btree_set
        int added = 0;
        for(int i = 0; i < count; i++)
        {
            if(active_list.insert(id[i]).second)
                ++added;
        }
        return added;
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

            // 将创建的ID推入 deque（直接插入到尾部）
            for(int i = 0; i < batch_size; i++)
            {
                idle_list.push_back(created_ids[i]);
            }

            // 如果需要输出，拷贝到idp
            if(idp)
            {
                for(int i = 0; i < batch_size; i++)
                {
                    idp[total_created + i] = created_ids[i];
                }
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

        // 检查是否有足够的闲置ID
        if(static_cast<int>(idle_list.size()) < count)
            return false;

        // 从 deque 头部取出 count 个ID（FIFO）
        for(int i = 0; i < count; i++)
        {
            id[i] = idle_list.front();
            idle_list.pop_front();
        }

        // 批量加入到 active_list
        for(int i = 0; i < count; i++)
        {
            active_list.insert(id[i]);
        }

        return true;
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

        if(static_cast<int>(idle_list.size()) < count)
        {
            [[maybe_unused]] int created = CreateIdle(count - static_cast<int>(idle_list.size()));
        }

        return Get(id,count);
    }

    /**
    * 释放指定量的ID数据(会从Active列表中取出，放入Idle列表中)
    *
    * 优化：使用批缓冲减少 deque::push_back 调用次数
    */
    int ActiveIDManager::Release(const int *id,int count)
    {
        if(!id||count<=0)return(0);

        int result = 0;

        for(int i = 0; i < count; i++)
        {
            // 从 active_list 删除
            if(active_list.erase(id[i]) > 0)
            {
                // 添加到 idle_list 尾部（FIFO）
                idle_list.push_back(id[i]);
                ++result;
                ++released_count;  // 统计已释放ID
            }
        }

        return result;
    }

    /**
    * 释放所有活跃ID
    */
    int ActiveIDManager::ReleaseAllActive()
    {
        const int count = static_cast<int>(active_list.size());

        if(count > 0)
        {
            // 将所有活跃ID移到闲置队列尾部
            for(int id : active_list)
            {
                idle_list.push_back(id);
            }

            released_count += count;  // 统计已释放ID

            active_list.clear();
        }

        return count;
    }
}//namespace hgl
