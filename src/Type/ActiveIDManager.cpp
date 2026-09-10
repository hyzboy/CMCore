#include<hgl/type/ActiveIDManager.h>

namespace hgl
{
    void ActiveIDManager::PushIdle(const int id)
    {
        idle_list.push_back(id);
        idle_lookup.insert(id);
    }

    void ActiveIDManager::PopIdle()
    {
        idle_lookup.erase(idle_list.front());
        idle_list.pop_front();
    }

    void ActiveIDManager::ClearIdle()
    {
        idle_list.clear();
        idle_lookup.clear();
    }

    bool ActiveIDManager::Create(int *id_list,const int count)
    {
        if(!id_list||count<=0)return(false);

        // 检查是否会导致ID溢出（与 GetRemainingCapacity() 同一条件）
        if(count > GetRemainingCapacity())
            return(false);

        for(int i=id_count;i<id_count+count;i++)
        {
            *id_list=i;
            ++id_list;
        }

        id_count+=count;

        return(true);
    }

    int ActiveIDManager::CreateActive(int *id,const int count)
    {
        if(!id||count<=0)return(0);

        if(!Create(id,count))return(0);

        // 新编号从 id_count 之后连续分配，绝不可能已在 active_list 中（不变式），
        // 因此 insert 必然成功；返回值恒为 count（全有或全无）。
        for(int i = 0; i < count; i++)
            active_list.insert(id[i]);

        return count;
    }

    /**
        * 创建若干ID，并置于闲置ID列表中。
        * 分配的是全新编号（ID水位之后连续分配），不会与已有编号重复。
        * 语义为“全有或全无”：容量不足时返回0，且不改动任何状态。
        * @return 成功添加的个数（count 或 0）
        */
    int ActiveIDManager::CreateIdle(int *idp,const int count)
    {
        if(count<=0)return(0);

        // 容量预检：保证下列分批 Create 不会中途失败，不会留下“部分成功”的半成品状态
        if(!CanCreate(count))
            return(0);

        constexpr int kBatchSize=512;   // 临时缓冲上限，避免超大栈占用
        int created_ids[kBatchSize];
        int total_created = 0;

        while(total_created < count)
        {
            const int batch_size = (count - total_created > kBatchSize)
                                 ? kBatchSize
                                 : (count - total_created);

            if(!Create(created_ids, batch_size))    // 不可达：CanCreate 已预检容量
                break;

            // 将创建的ID推入闲置集合（顺序 + 成员索引同步维护）
            for(int i = 0; i < batch_size; i++)
            {
                PushIdle(created_ids[i]);

                // 如果需要输出，拷贝到idp
                if(idp)
                    idp[total_created + i] = created_ids[i];
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

        // 全有或全无：闲置不足时直接失败，输出数组与内部状态均不做任何改动
        if(static_cast<int>(idle_list.size()) < count)
            return(false);

        // 从 deque 头部取出 count 个ID（FIFO）并激活
        for(int i = 0; i < count; i++)
        {
            id[i] = idle_list.front();

            PopIdle();
            active_list.insert(id[i]);
        }

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

        const int idle_count = static_cast<int>(idle_list.size());

        if(idle_count < count)
        {
            const int shortage = count - idle_count;

            // CreateIdle 为“全有或全无”：失败时没有任何ID被创建，此处直接失败即可，
            // 不会出现“返回false但状态已改变”的账实不符。
            if(CreateIdle(shortage) != shortage)
                return(false);
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
                PushIdle(id[i]);
                ++result;
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
            // 将所有活跃ID移到闲置队列尾部；active_list 有序，故释放次序为ID升序
            for(int id : active_list)
            {
                PushIdle(id);
            }

            active_list.clear();
        }

        return count;
    }
}//namespace hgl
