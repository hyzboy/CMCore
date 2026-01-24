#include<hgl/type/Stack.h>
#include<iostream>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void TestStackOrdered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueStack Ordered Test"<<endl<<endl;

    ValueStack<int> tab;
    int i;

    for(i=0;i<10;i++)
    {
        cout<<"push "<<i<<endl;

        tab.Push(i);
    }

    cout<<"ValueStack Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        int val;

        tab.Pop(val);

        cout<<"pop "<<val<<endl;
    }

    cout<<"ValueStack Count: "<<tab.GetCount()<<endl;
}

void TestStackUnordered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueStack Unordered Test"<<endl<<endl;

    ValueStack<int> tab;
    int i;
    int val;

    for(i=0;i<20;i++)
    {
        if(rand()&1)
        {
            if(tab.Pop(val))
                cout<<"pop "<<val<<endl;
            else
                cout<<"pop error"<<endl;
        }
        else
        {
            cout<<"push "<<i<<endl;

            tab.Push(i);
        }
    }
}

void TestStackStruct()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueStack Struct Test"<<endl<<endl;

    ValueStack<UserInfo> ui_queue;

    for(uint i=0;i<sizeof(user_info_array)/sizeof(UserInfo);i++)
        ui_queue.Push(user_info_array[i]);

    cout<<"ValueStack Count: "<<ui_queue.GetCount()<<endl;

    for(uint i=0;i<sizeof(user_info_array)/sizeof(UserInfo);i++)
    {
        UserInfo ui;

        ui_queue.Pop(ui);

        cout<<i<<": "<<ui.name<<(ui.sex?" male ":" female ")<<ui.age<<" age."<<endl;
    }
}

class StackTestObject
{
    int val;

public:

    StackTestObject(){val=-1;}

    void Set(int v)
    {
        val=v;

        cout<<"StackTestObject::Set "<<val<<endl;
    }

    ~StackTestObject()
    {
        cout<<"~StackTestObject "<<val<<endl;
    }
};

void TestObjectStack()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueStack Object Test"<<endl<<endl;

    ManagedStack<StackTestObject> tab;

    int i;

    for(i=0;i<10;i++)
    {
        StackTestObject *obj=new StackTestObject;

        obj->Set(i);

        tab.Push(obj);
    }

    cout<<"ValueStack Count: "<<tab.GetCount()<<endl;

    for(i=0;i<5;i++)        //只取出5个,剩几个给自动清理处理
    {
        StackTestObject *obj=tab.Pop();

        if(obj)
            delete obj;
    }

    cout<<"ValueStack Count: "<<tab.GetCount()<<endl;
}

int os_main(int,os_char **)
{
    srand(time(nullptr));

    //原生单个数据测试
    TestStackOrdered();
    TestStackUnordered();

    //原生结构体测试
    TestStackStruct();

    //对象测试
    TestObjectStack();

    return(0);
}
