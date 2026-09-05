#include<hgl/type/BlockAllocator.h>

namespace hgl
{
    BlockAllocator::BlockAllocator()
    {
        max_count=0;
        free_count=0;
    }

    bool BlockAllocator::Init(const int mc)
    {
        max_count=mc;
        free_count=mc;

        series.Init(mc);
        node_pool.Init(mc);
        ud_pool.Init(mc);

        ud_set.Reserve(mc);

        start=node_pool.Acquire();
        end=start;

        start->prev=nullptr;
        start->next=nullptr;
        start->start=0;
        start->count=max_count;

        return(true);
    }

    BlockAllocator::UserNode *BlockAllocator::Acquire(const int acquire_count)
    {
        if(acquire_count<=0)
            return(nullptr);

        if(acquire_count>free_count)
            return(nullptr);

        if(acquire_count>max_count)
            return(nullptr);

        if(start==end)
        {
            free_count-=acquire_count;

            UserNode *ud=ud_pool.Acquire();

            if(!ud){free_count+=acquire_count;return(nullptr);}

            ud->start=start->start;
            ud->count=acquire_count;

            ud_set.Add(ud);

            start->start+=acquire_count;
            start->count=free_count;

            return(ud);
        }

        ChainNode *cn=start;
        ChainNode *fit=nullptr;

        do
        {
            if(cn->count==acquire_count)    //长度正合适，那就用这一段
            {
                fit=cn;
                break;
            }

            if(cn->count>acquire_count)     //长度够了
            {
                if(!fit                     //没有合适的，先记下这个
                   ||fit->count>cn->count)   //这个更小，更合适
                    fit=cn;
            }

            cn=cn->next;
            }while(cn!=nullptr);

            if(!fit)    //没有合适的
                return(nullptr);

            free_count-=acquire_count;

            UserNode *ud=ud_pool.Acquire();

            if(!ud){free_count+=acquire_count;return(nullptr);}

            ud->start=fit->start;
        ud->count=acquire_count;

        ud_set.Add(ud);

        if(fit->count==acquire_count)       //正好合适
        {
            if(fit->prev)
                fit->prev->next=fit->next;
            else
                start=fit->next;

            if(fit->next)
                fit->next->prev=fit->prev;
            else
                end=fit->prev;

            node_pool.Release(fit);
        }
        else
        {
            fit->start+=acquire_count;
            fit->count-=acquire_count;
        }

        return(ud);
    }

    bool BlockAllocator::Release(BlockAllocator::UserNode *ud)
    {
        if(!ud)
            return(false);

        if(!ud_set.Contains(ud))
            return(false);

        // 有序定位：找到第一个 start 大于 ud->start 的节点，插入点即其之前。
        // （旧实现逐分支单侧合并，且尾插分支会打乱链序，相邻空闲段可能永久分裂）
        ChainNode *cur=start;
        ChainNode *prev=nullptr;

        while(cur&&cur->start<ud->start)
        {
            prev=cur;
            cur=cur->next;
        }

        ChainNode *node=nullptr;

        if(prev&&prev->GetEnd()==ud->start)             //与前段相接：并入前段
        {
            node=prev;
            node->count+=ud->count;
        }
        else if(cur&&cur->start==ud->GetEnd())          //与后段相接：并入后段
        {
            node=cur;
            node->count+=ud->count;
            node->start=ud->start;
        }
        else                                            //独立新段：按序插入
        {
            node=node_pool.Acquire();

            node->prev=prev;
            node->next=cur;
            node->start=ud->start;
            node->count=ud->count;

            if(prev)
                prev->next=node;
            else
                start=node;

            if(cur)
                cur->prev=node;
            else
                end=node;
        }

        //级联合并：node 与后段、前段相邻则依次吸并（顺序保证 start 有序）
        if(node->next&&node->GetEnd()==node->next->start)
        {
            ChainNode *nx=node->next;

            node->count+=nx->count;
            node->next=nx->next;

            if(nx->next)
                nx->next->prev=node;
            else
                end=node;

            node_pool.Release(nx);
        }

        if(node->prev&&node->prev->GetEnd()==node->start)
        {
            ChainNode *pv=node->prev;

            pv->count+=node->count;
            pv->next=node->next;

            if(node->next)
                node->next->prev=pv;
            else
                end=pv;

            node_pool.Release(node);
        }

        free_count+=ud->count;                          //空闲数量增加
        ud_set.Delete(ud);
        ud_pool.Release(ud);
        return(true);
    }
}//namespace hgl
