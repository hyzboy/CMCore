#include<hgl/type/DataType.h>
#include<hgl/type/Queue.h>
#include<iostream>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void TestQueueOrdered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Ordered Test"<<endl<<endl;

    Queue<int> tab;
    int i;

    for(i=0;i<10;i++)
    {
        cout<<"push "<<i<<endl;

        tab.Push(i);
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        int val;

        tab.Pop(val);

        cout<<"pop "<<val<<endl;
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;
}

void TestQueueUnordered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Unordered Test"<<endl<<endl;

    Queue<int> tab;
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

void TestQueueStruct()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Struct Test"<<endl<<endl;

    Queue<UserInfo> ui_queue;

    for(uint i=0;i<sizeof(user_info_array)/sizeof(UserInfo);i++)
        ui_queue.Push(user_info_array[i]);

    cout<<"Queue Count: "<<ui_queue.GetCount()<<endl;

    for(uint i=0;i<sizeof(user_info_array)/sizeof(UserInfo);i++)
    {
        UserInfo ui;

        ui_queue.Pop(ui);

        cout<<i<<": "<<ui.name<<(ui.sex?" male ":" female ")<<ui.age<<" age."<<endl;
    }
}

// ==================== 新增测试 ====================

/**
 * 测试 Queue 的 Peek 操作
 */
void TestQueuePeek()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Peek Test"<<endl<<endl;

    Queue<int> q;

    // 空队列 Peek
    int val = -1;
    if(!q.Peek(val))
        cout<<"Empty queue peek failed (expected)"<<endl;

    // 添加元素
    for(int i = 0; i < 5; ++i)
        q.Push(i);

    cout<<"Queue Count: "<<q.GetCount()<<endl;

    // Peek 不删除元素
    q.Peek(val);
    cout<<"Peeked value: "<<val<<", Queue Count still: "<<q.GetCount()<<endl;

    // Pop 删除元素
    q.Pop(val);
    cout<<"Popped value: "<<val<<", Queue Count now: "<<q.GetCount()<<endl;
}

/**
 * 测试 Queue 的循环缓冲区行为
 */
void TestQueueCircularBuffer()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Circular Buffer Test"<<endl<<endl;

    Queue<int> q;

    cout<<"[1] Filling queue..."<<endl;
    for(int i = 0; i < 5; ++i)
        q.Push(i * 10);

    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[2] Popping 3 elements..."<<endl;
    int val;
    for(int i = 0; i < 3; ++i)
    {
        q.Pop(val);
        cout<<"Popped: "<<val<<endl;
    }
    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[3] Adding 3 more elements (circular buffer test)..."<<endl;
    for(int i = 0; i < 3; ++i)
        q.Push((i + 100) * 10);

    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[4] Popping all (verify order)..."<<endl;
    while(q.Pop(val))
        cout<<"Popped: "<<val<<endl;
}

/**
 * 测试 Queue 的容量管理
 */
void TestQueueCapacity()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Capacity Test"<<endl<<endl;

    Queue<int> q;

    cout<<"[1] Initial state..."<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[2] Reserve pre-allocation..."<<endl;
    q.Reserve(100);
    cout<<"After Reserve(100):"<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[3] Adding 50 elements..."<<endl;
    for(int i = 0; i < 50; ++i)
        q.Push(i);
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[4] Clear..."<<endl;
    q.Clear();
    cout<<"After Clear():"<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[5] Free memory..."<<endl;
    q.Free();
    cout<<"After Free():"<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;
}

/**
 * 测试 Queue 的 Contains
 */
void TestQueueContains()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Contains Test"<<endl<<endl;

    Queue<int> q;

    for(int i = 0; i < 5; ++i)
        q.Push(i * 10);

    cout<<"Queue: ";
    for(int i = 0; i < 5; ++i)
    {
        int val;
        // 需要 Peek
        cout<<i*10<<" ";
    }
    cout<<endl;

    cout<<"Contains(20): "<<(q.Contains(20) ? "yes" : "no")<<endl;
    cout<<"Contains(100): "<<(q.Contains(100) ? "yes" : "no")<<endl;
}

// ==================== 对象指针Queue测试 ====================

class QueueTestObject
{
    int val;

public:

    QueueTestObject(){val=-1;}

    void Set(int v)
    {
        val=v;

        cout<<"QueueTestObject::Set "<<val<<endl;
    }

    ~QueueTestObject()
    {
        cout<<"~QueueTestObject "<<val<<endl;
    }
};

void TestObjectQueue()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Queue Object Test"<<endl<<endl;

    Queue<QueueTestObject*> tab;

    int i;

    for(i=0;i<10;i++)
    {
        QueueTestObject *obj=new QueueTestObject;

        obj->Set(i);

        tab.Push(obj);
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<5;i++)        //取出5个
    {
        QueueTestObject *obj;

        if(tab.Pop(obj))
            delete obj;
    }

    cout<<"Queue Count: "<<tab.GetCount()<<endl;

    // 手动清理剩余对象
    while(tab.GetCount() > 0)
    {
        QueueTestObject *obj;
        tab.Pop(obj);
        delete obj;
    }
}
/**
 * 测试 Queue 类型约束
 */
void TestTypeConstraints()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Type Constraints Test"<<endl<<endl;

    cout<<"[1] Queue<int> - trivially copyable [OK]"<<endl;
    Queue<int> q1;
    q1.Push(42);

    cout<<"[2] Queue<UserInfo> - trivial POD [OK]"<<endl;
    Queue<UserInfo> q2;

    cout<<"[3] Queue<QueueTestObject*> - pointer type (manual management) [OK]"<<endl;
    Queue<QueueTestObject*> oq1;
    oq1.Push(new QueueTestObject());
    QueueTestObject* tmp;
    if(oq1.Pop(tmp)) delete tmp;

    cout<<"\nNote: the following type combinations fail to compile:"<<endl;
    cout<<"- Queue<std::string> - [X] (non-trivial type)"<<endl;
    cout<<"- Queue<QueueTestObject> - [X] (has destructor)"<<endl;
}

int os_main(int,os_char **)
{
    srand(time(nullptr));

    //原生单个数据测试
    TestQueueOrdered();
    TestQueueUnordered();

    //原生结构体测试
    TestQueueStruct();

    // ========== 扩展功能测试 ==========

    TestQueuePeek();
    TestQueueCircularBuffer();
    TestQueueCapacity();
    TestQueueContains();

    //对象指针测试（手动管理）
    TestObjectQueue();

    // 类型约束测试
    TestTypeConstraints();

    cout<<endl;
    cout<<"========================================="<<endl;
    cout<<"All tests completed!"<<endl;
    cout<<"========================================="<<endl;

    return(0);
}
