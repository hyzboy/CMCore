#include<hgl/type/Pool.h>
#include<iostream>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void ShowUserInfoArray(const DataArray<UserInfo> &ua)
{
    cout<<"user info array "<<ua.GetCount()<<" ";

    bool first=true;

    for(auto &ui:ua)
    {
        if(first){cout<<'['<<ui.name;first=false;}
             else cout<<','<<ui.name;
    }

    cout<<"]"<<endl;
};

void StructPoolTest()
{
    Pool<UserInfo> pool;

    //添加所有的人物数据到池中
    {
        for(auto ui:user_info_array)
            pool.AppendToActive(ui);
    }

    ShowUserInfoArray(pool.GetActiveArray());

    //随机释放几个(仅闲置，并不释放内存)
    {
        int count=10+rand()%10;

        for(int i=0;i<count;i++)
            pool.Release(user_info_array[rand()%26]);
    }

    ShowUserInfoArray(pool.GetActiveArray());

    cout<<"idle count: "<<pool.GetIdleCount()<<endl;

    //取出所有闲置数据
    {
        int i=0;
        UserInfo ui;

        while(pool.Get(ui))
            cout<<i++<<": get "<<ui.name<<" from idle."<<endl;
    }
}

void ShowUserInfoArray(const DataArray<UserInfoClass *> &ua)
{
    cout<<"user info array "<<ua.GetCount()<<" [";

    bool first=true;

    for(auto *uic:ua)
    {
        if(first){cout<<'['<<uic->GetName();first=false;}
             else cout<<','<<uic->GetName();
    }

    cout<<"]"<<endl;
};

void ObjectPoolTest()
{
    ObjectPool<UserInfoClass> pool;

    ArrayList<UserInfoClass *> release_list;

    //添加所有的人物数据到池中
    {
        for(auto ui:user_info_array)
        {
            UserInfoClass *uic=new UserInfoClass;

            uic->Set(ui);

            pool.AppendToActive(uic);

            if(rand()%3==1)                 //有1/3的概率，将这个数据放入释放列表
                release_list.Add(uic);
        }
    }

    ShowUserInfoArray(pool.GetActiveArray());

    //释放刚刚记录的几个(仅闲置，并不释放内存)
    pool.Release(release_list.GetData(),release_list.GetCount());

    ShowUserInfoArray(pool.GetActiveArray());

    cout<<"idle count: "<<pool.GetIdleCount()<<endl;

    //取出闲置列表中的一半数据
    {
        int i=0;
        UserInfoClass *uic;

        for(int i=0;i<release_list.GetCount()/2;i++)        //取闲置列表中的一半数据
            if(pool.Get(uic))
                cout<<i<<": get "<<uic->GetName()<<" from idle."<<endl;
    }

    //剩下的一半留给自动释放
    cout<<"finished current function, and then auto release."<<endl;

    //原本是自动，我们这里手写调用一下，方便输出调试信息。
    //以下代码等同ObjectPool::~ObjectPool()
    {
        cout<<"clear active "<<pool.GetActiveCount()<<endl;
        pool.ClearActive();

        cout<<"clear idle "<<pool.GetIdleCount()<<endl;
        pool.ClearIdle();
    }
}

void main()
{
    srand(time(nullptr));

    StructPoolTest();

    ObjectPoolTest();
}
