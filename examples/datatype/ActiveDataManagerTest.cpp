#include<hgl/type/ActiveDataManager.h>
#include<iostream>
#include"UserInfo.h"

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

void DebugOutputArray(const char *hint,UserInfo **ui,const int count)
{
    cout<<"("<<hint<<':'<<count<<")";

    if(count<=0)return;

    UserInfo **p=ui;

    for(int i=0;i<count;i++)
    {
        cout<<(!i?'[':',')<<(*p)->name;
        ++p;
    }

    cout<<']';
}

void DebugOutputArray(const char *hint,ActiveDataManager<UserInfo> &adm,const int *idp,const int count)
{
    if(!idp||count<=0)return;

    UserInfo **ui=new UserInfo *[count];

    adm.GetData(ui,idp,count);

    DebugOutputArray(hint,ui,count);

    delete[] ui;        
}

void DebugOutputArray(const char *hint,ActiveDataManager<UserInfo> &adm,const DataArray<int> &da)
{
    DebugOutputArray(hint,adm,da.GetData(),da.GetCount());
}

void DebugADMOutput(const char *hint,ActiveDataManager<UserInfo> &adm)
{
    cout<<hint<<' ';

    DebugOutputArray("Active",adm,adm.GetActiveArray());
    cout<<' ';
    DebugOutputArray("Idle",adm,adm.GetIdleArray());

    cout<<endl;
}

void WriteUserinfoToADM(ActiveDataManager<UserInfo> &adm,const int *idp,const int count,const int start)
{
    for(int i=0;i<count;i++)
    {
        adm.WriteData(user_info_array[start+i],*idp);

        ++idp;
    }
}

int os_main(int,os_char **)
{
    using namespace hgl;

    ActiveDataManager<UserInfo> adm;

    adm.Reserve(10);

    int start=0;

    int id0[5];
    adm.CreateIdle(id0,5);
    WriteUserinfoToADM(adm,id0,5,start);start+=5;
    DebugADMOutput("CreateIdle(5)",adm);
    std::cout<<std::endl;

    int id1[5];
    adm.CreateActive(id1,5);
    WriteUserinfoToADM(adm,id1,5,start);start+=5;
    DebugADMOutput("CreateActive(5)",adm);
    std::cout<<std::endl;

    int id2[3];
    adm.Get(id2,3);
    DebugADMOutput("Get(user2,3)",adm);
    DebugOutputArray("user2",adm,id2,3);
    std::cout<<std::endl;

    adm.Release(id1,3);
    DebugADMOutput("Release(user1,3)",adm);

    adm.Release(id2,3);
    DebugADMOutput("Release(user2,3)",adm);

    adm.ReleaseAllActive();
    DebugADMOutput("ReleaseAllActive()",adm);

    return(0);
}
