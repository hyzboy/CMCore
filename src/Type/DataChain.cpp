#include<hgl/type/DataChain.h>

namespace hgl
{
    DataChain::DataChain(const int mc):series(mc),node_pool(mc),ud_pool(mc)
    {
        max_count=mc;
        free_count=mc;

        ud_set.PreAlloc(mc);

        start=node_pool.Acquire();
        end=start;

        start->prev=nullptr;
        start->next=nullptr;
        start->start=0;
        start->count=max_count;
    }

    DataChain::UserNode *DataChain::Acquire(const int acquire_count)
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
        }while(cn!=end);

        if(!fit)    //没有合适的
            return(nullptr);

        free_count-=acquire_count;

        UserNode *ud=ud_pool.Acquire();

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

    bool DataChain::Release(DataChain::UserNode *ud)
    {
        if(!ud)
            return(false);

        if(!ud_set.IsMember(ud))
            return(false);

        const int ud_end=ud->GetEnd();

        ChainNode *cur=start;
        ChainNode *next;
        int cur_end;

        do
        {
            cur_end=cur->GetEnd();

            if(cur_end==ud->start)          //接在一起了
            {
                cur->count+=ud->count;      //count扩充

                free_count+=ud->count;      //空闲数量增加
                ud_set.Delete(ud);
                return(true);
            }
            else if(cur->start==ud_end)     //接在一起了
            {
                cur->count+=ud->count;
                cur->start-=ud->count;

                free_count+=ud->count;      //空闲数量增加
                ud_set.Delete(ud);
                return(true);
            }
            else if(cur->start>ud_end)      //在前面
            {
                ChainNode *new_cn=node_pool.Acquire();

                new_cn->prev=cur->prev;
                new_cn->next=cur;
                new_cn->start=ud->start;
                new_cn->count=ud->count;

                if(cur->prev)
                    cur->prev->next=new_cn;
                else
                    start=new_cn;

                cur->prev=new_cn;

                free_count+=ud->count;      //空闲数量增加
                ud_set.Delete(ud);
                return(true);
            }

            next=cur->next;

            if(next)
            {
                if(next->start==ud_end)     //下一个块接着，那就合并
                {
                    next->count+=ud->count;    //count扩充
                    next->start-=ud->count;    //start前移,理论上==ud->start，调试时检查一下

                    free_count+=ud->count;      //空闲数量增加
                    ud_set.Delete(ud);
                    return(true);
                }
                else
                    if(next->start>ud_end)      //下一个块不接着，但是在后面，那就新建一个节点
                    {
                        ChainNode *new_cn=node_pool.Acquire();

                        new_cn->prev=cur;
                        new_cn->next=next;
                        new_cn->start=ud->start;
                        new_cn->count=ud->count;

                        cur->next=new_cn;
                        next->prev=new_cn;

                        free_count+=ud->count;      //空闲数量增加
                        ud_set.Delete(ud);
                        return(true);
                    }
                    else //next->start<ud_end
                    {
                        if(next->start>=ud->start)  //这个块在中间，那就是出错了
                        {
                            //                            LOG_ERROR(OS_TEXT("DataChain::Release, block cross."));
                            return(false);
                        }
                    }
            }
            else    //都没下一个了，那这个就当下一个吧
            {
                ChainNode *new_cn=node_pool.Acquire();

                new_cn->start=ud->start;
                new_cn->count=ud->count;

                if(cur==start)      //卧槽这是第一个
                {
                    new_cn->prev=nullptr;
                    new_cn->next=cur;

                    cur->prev=new_cn;

                    start=new_cn;
                }
                else
                {
                    new_cn->prev=cur;
                    new_cn->next=nullptr;

                    cur->next=new_cn;
                    end=new_cn;
                }

                free_count+=ud->count;      //空闲数量增加
                ud_set.Delete(ud);
                return(true);
            }

            cur=next;
        }while(cur<=end);

        //未找到？？
        return(false);
    }
}//namespace hgl
