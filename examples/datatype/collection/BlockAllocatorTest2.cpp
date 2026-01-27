#include<hgl/type/BlockAllocator.h>
#include<hgl/type/ValueArray.h>
#include<iostream>
#include<iomanip>
#include<random>

using namespace std;
using namespace hgl;

void out_data_chain(BlockAllocator *dc)
{
    BlockAllocator::ChainNode *start =dc->GetStartNode();
    BlockAllocator::ChainNode *end   =dc->GetEndNode();

    BlockAllocator::ChainNode *node  =start;

    cout<<"Data Chain: ";

    while(node)
    {
        cout<<"["<<setw(3)<<node->start<<","<<setw(3)<<node->count<<"]";

        if(node==end)
            break;

        node=node->next;
    }

    cout<<endl;
}

int os_main(int,os_char **)
{
    constexpr const int BLOCK_SIZE=100;

    BlockAllocator dc;

    dc.Init(BLOCK_SIZE);                  ///数据链管理器

    cout<<"BlockAllocator Test"<<endl;

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(1,BLOCK_SIZE);
    uniform_int_distribution<int> dis_un_count(BLOCK_SIZE/10,BLOCK_SIZE/4);
    uniform_int_distribution<int> dis_block_count(1,BLOCK_SIZE/10);

    ValueArray<BlockAllocator::UserNode *> user_node_list;
    int free_count=dc.GetFreeCount();

    do
    {
        int ubc=hgl_min(dis_block_count(gen),free_count);

        BlockAllocator::UserNode *un=dc.Acquire(ubc);

        if(!un)  //失败了
        {
            cout<<"Acquire UserNode Error! FreeCount "<<free_count<<endl;
            break;
        }
        else
        {
            free_count-=ubc;

            cout<<"Acquire UserNode: ["<<setw(3)<<un->GetStart()<<","<<setw(3)<<un->GetCount()<<"] ";
        }

        out_data_chain(&dc);

        user_node_list.Add(un);
    }while(free_count>0);

    bernoulli_distribution dis_op(0.5);    //使用伯努利分布随机数产生操作类型，50%的概率是Acquire，50%的概率是Release

    const int top_op_count=dis_un_count(gen);   //最上层操作次数
    int op_run_count=0;                         //执行成功的次数

    do
    {
        bool op=dis_op(gen);                    //这次是acquire还是release
        int op_count=dis_un_count(gen);         //操作数量

        if(op)  //acquire
        {
            op_count=hgl_min(op_count,dc.GetFreeCount());

            if(op_count<=0)
                continue;

            BlockAllocator::UserNode *un=dc.Acquire(op_count);

            if(un)
            {
                cout<<"Acquire ["<<setw(3)<<un->GetStart()<<","<<setw(3)<<un->GetCount()<<"] ";

                user_node_list.Add(un);
            }
            else
            {
                cout<<"Acquire "<<op_count<<" blocks failed!"<<endl;
                continue;
            }
        }
        else    //release
        {
            int pos=dis(gen)%user_node_list.GetCount();

            BlockAllocator::UserNode *un;

            user_node_list.Get(pos,un);

            if(dc.Release(un))
            {
                cout<<"Release ["<<setw(3)<<un->GetStart()<<","<<setw(3)<<un->GetCount()<<"] ";

                user_node_list.Delete(pos);
                ++op_run_count;
            }
            else
            {
                cout<<"Release UserNode "<<pos<<" failed!"<<endl;
                continue;
            }
        }

        out_data_chain(&dc);
        ++op_run_count;
    }while(op_run_count<top_op_count);      //执行成功次数满了才能退出，失败的次数不算

    return 0;
}
