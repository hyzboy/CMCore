#include<iostream>
#include<hgl/type/ActiveIDManager.h>

using namespace hgl;
using namespace std;

void DebugOutputArray(const char *hint,const int *id,const int count)
{
    cout<<"("<<hint<<':'<<count<<")";

    if(count<=0)return;

    const int *p=id;

    for(int i=0;i<count;i++)
    {
        cout<<(!i?'[':',')<<*p;
        ++p;
    }

    cout<<']';
}

void DebugOutputArray(const char *hint,const DataArray<int> &da)
{
    DebugOutputArray(hint,da.GetData(),da.GetCount());
}

void DebugAIMOutput(const char *hint,const ActiveIDManager &aim)
{
    cout<<hint<<' ';

    DebugOutputArray("Active",aim.GetActiveArray());
    cout<<' ';
    DebugOutputArray("Idle",aim.GetIdleArray());

    cout<<endl;
}

int main(int,char **)
{
    ActiveIDManager aim;

    aim.Reserve(1024);

    DebugAIMOutput("null",aim);

    aim.CreateIdle(5);
    DebugAIMOutput("CreateIdle(5)",aim);

    int id1[5];
    aim.CreateActive(id1,5);
    DebugAIMOutput("CreateActive(user1,5)",aim);
    DebugOutputArray("user1",id1,5);
    std::cout<<std::endl;

    int id2[3];
    aim.Get(id2,3);    
    DebugAIMOutput("Get(user2,3)",aim);
    DebugOutputArray("user2",id2,3);
    std::cout<<std::endl;

    int id3[5];
    aim.GetOrCreate(id3,5);
    DebugAIMOutput("GetOrCreate(user3,5)",aim);
    DebugOutputArray("user3",id3,5);
    std::cout<<std::endl;

    aim.Release(id1,5);
    DebugAIMOutput("Release(user1,5)",aim);

    aim.Release(id2,3);
    DebugAIMOutput("Release(user2,3)",aim);

    aim.ReleaseAllActive();
    DebugAIMOutput("RelaseAllActive()",aim);

    return 0;
}
