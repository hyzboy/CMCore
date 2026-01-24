#include<hgl/type/BlockAllocator.h>
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
    BlockAllocator dc;
    
    dc.Init(100);         ///数据链管理器(预定100个块)

    cout<<"BlockAllocator Test"<<endl;

    BlockAllocator::UserNode *un[10];

    for(int i=0;i<10;i++)
    {
        un[i]=dc.Acquire(10);

        if(!un[i])
        {
            cout<<"Acquire UserNode Error!"<<endl;
            break;
        }
        else
        {
            cout<<"Acquire UserNode: ["<<setw(3)<<un[i]->GetStart()<<","<<setw(3)<<un[i]->GetCount()<<"] ";
        }

        out_data_chain(&dc);
    }

    cout<<"---------------------------------------------------------------------------------"<<endl;
    cout<<"Release  0 ";
    dc.Release(un[0]);
    out_data_chain(&dc);

    cout<<"Release  7 ";
    dc.Release(un[7]);
    out_data_chain(&dc);

    cout<<"Release  9 ";
    dc.Release(un[9]);
    out_data_chain(&dc);

    cout<<"Release  4 ";
    dc.Release(un[4]);
    out_data_chain(&dc);

    cout<<"---------------------------------------------------------------------------------"<<endl;
    cout<<"Acquire 20 ";
    BlockAllocator::UserNode *ud20=dc.Acquire(20);
    out_data_chain(&dc);

    cout<<"Release  3 ";
    dc.Release(un[3]);
    out_data_chain(&dc);

    cout<<"Release  2 ";
    dc.Release(un[2]);
    out_data_chain(&dc);

    cout<<"Acquire 15 ";
    BlockAllocator::UserNode *ud15=dc.Acquire(15);
    out_data_chain(&dc);
}