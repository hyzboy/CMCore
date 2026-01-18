#include<iostream>
#include<hgl/type/ActiveMemoryBlockManager.h>
#include<hgl/type/MemoryBlock.h>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void DebugOutputArray(const char *hint,const int **id,const int count)
{
    cout<<"("<<hint<<':'<<count<<")";

    if(count<=0)return;

    const int **p=id;

    for(int i=0;i<count;i++)
    {
        cout<<(!i?'[':',')<<*p;
        ++p;
    }

    cout<<']';
}

void DebugOutputArray(const char *hint,const char **str,const int count)
{
    cout<<"("<<hint<<':'<<count<<")";

    if(count<=0)return;

    const char **p=str;

    for(int i=0;i<count;i++)
    {
        cout<<(!i?'[':',')<<*p;
        ++p;
    }

    cout<<']';
}

void DebugOutputArray(const char *hint,const ActiveMemoryBlockManager &ambm,const int *idp,const int count)
{
    if(!idp||count<=0)return;

    char **data=new char *[count];

    ambm.GetData((void **)data,idp,count);

    DebugOutputArray(hint,(const char **)data,count);

    delete[] data;
}

void DebugOutputArray(const char *hint,const ActiveMemoryBlockManager &ambm,const DataArray<int> &da)
{
    DebugOutputArray(hint,ambm,da.GetData(),da.GetCount());
}

void DebugAIMOutput(const char *hint,const ActiveMemoryBlockManager &ambm)
{
    cout<<hint<<' ';

    DebugOutputArray("Active",ambm,ambm.GetActiveArray());
    cout<<' ';
    DebugOutputArray("Idle",ambm,ambm.GetIdleArray());

    cout<<endl;
}

void WriteUsernameToAIM(ActiveMemoryBlockManager &ambm,const int *id,const int count,const int start)
{
    for(int i=0;i<count;i++)
        ambm.WriteData(user_info_array[start+i].name,id[i]);
}

int main(int,char **)
{
    ActiveMemoryBlockManager ambm(8);

    ambm.Reserve(16);

    int start=0;

    DebugAIMOutput("null",ambm);

    int id0[5];
    ambm.CreateIdle(id0,5);
    WriteUsernameToAIM(ambm,id0,5,start);start+=5;
    DebugAIMOutput("CreateIdle(5)",ambm);

    int id1[5];
    ambm.CreateActive(id1,5);
    WriteUsernameToAIM(ambm,id1,5,start);start+=5;
    DebugAIMOutput("CreateActive(user1,5)",ambm);

    DebugOutputArray("user1",ambm,id1,5);
    std::cout<<std::endl;

    int id2[3];
    ambm.Get(id2,3);
    DebugAIMOutput("Get(user2,3)",ambm);
    DebugOutputArray("user2",ambm,id2,3);
    std::cout<<std::endl;

    //int id3[5];
    //ambm.GetOrCreate(id3,5);
    //DebugAIMOutput("GetOrCreate(user3,5)",ambm);
    //DebugOutputArray("user3",ambm,id3,5);
    //std::cout<<std::endl;

    ambm.Release(id1,5);
    DebugAIMOutput("Release(user1,5)",ambm);

    ambm.Release(id2,3);
    DebugAIMOutput("Release(user2,3)",ambm);

    ambm.ReleaseAllActive();
    DebugAIMOutput("RelaseAllActive()",ambm);

    return 0;
}
