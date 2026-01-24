#include<hgl/type/Queue.h>
#include<iostream>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void TestQueueOrdered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueQueue Ordered Test"<<endl<<endl;

    ValueQueue<int> tab;
    int i;

    for(i=0;i<10;i++)
    {
        cout<<"push "<<i<<endl;

        tab.Push(i);
    }

    cout<<"ValueQueue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<10;i++)
    {
        int val;

        tab.Pop(val);

        cout<<"pop "<<val<<endl;
    }

    cout<<"ValueQueue Count: "<<tab.GetCount()<<endl;
}

void TestQueueUnordered()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueQueue Unordered Test"<<endl<<endl;

    ValueQueue<int> tab;
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
    cout<<"ValueQueue Struct Test"<<endl<<endl;

    ValueQueue<UserInfo> ui_queue;

    for(uint i=0;i<sizeof(user_info_array)/sizeof(UserInfo);i++)
        ui_queue.Push(user_info_array[i]);

    cout<<"ValueQueue Count: "<<ui_queue.GetCount()<<endl;

    for(uint i=0;i<sizeof(user_info_array)/sizeof(UserInfo);i++)
    {
        UserInfo ui;

        ui_queue.Pop(ui);

        cout<<i<<": "<<ui.name<<(ui.sex?" male ":" female ")<<ui.age<<" age."<<endl;
    }
}

// ==================== 新增测试 ====================

/**
 * 测试 ValueQueue 的 Peek 操作
 */
void TestQueuePeek()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueQueue Peek Test"<<endl<<endl;

    ValueQueue<int> q;
    
    // 空队列 Peek
    int val = -1;
    if(!q.Peek(val))
        cout<<"Empty queue peek failed (expected)"<<endl;

    // 添加元素
    for(int i = 0; i < 5; ++i)
        q.Push(i);

    cout<<"ValueQueue Count: "<<q.GetCount()<<endl;

    // Peek 不删除元素
    q.Peek(val);
    cout<<"Peeked value: "<<val<<", ValueQueue Count still: "<<q.GetCount()<<endl;

    // Pop 删除元素
    q.Pop(val);
    cout<<"Popped value: "<<val<<", ValueQueue Count now: "<<q.GetCount()<<endl;
}

/**
 * 测试 ValueQueue 的循环缓冲区行为
 */
void TestQueueCircularBuffer()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueQueue Circular Buffer Test"<<endl<<endl;

    ValueQueue<int> q;

    cout<<"[1] 填充队列..."<<endl;
    for(int i = 0; i < 5; ++i)
        q.Push(i * 10);

    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[2] 弹出 3 个元素..."<<endl;
    int val;
    for(int i = 0; i < 3; ++i)
    {
        q.Pop(val);
        cout<<"Popped: "<<val<<endl;
    }
    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[3] 再添加 3 个元素（测试循环缓冲区）..."<<endl;
    for(int i = 0; i < 3; ++i)
        q.Push((i + 100) * 10);

    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[4] 全部弹出（验证顺序）..."<<endl;
    while(q.Pop(val))
        cout<<"Popped: "<<val<<endl;
}

/**
 * 测试 ValueQueue 的容量管理
 */
void TestQueueCapacity()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueQueue Capacity Test"<<endl<<endl;

    ValueQueue<int> q;

    cout<<"[1] 初始状态..."<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[2] Reserve 预分配..."<<endl;
    q.Reserve(100);
    cout<<"After Reserve(100):"<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[3] 添加 50 个元素..."<<endl;
    for(int i = 0; i < 50; ++i)
        q.Push(i);
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[4] 清空..."<<endl;
    q.Clear();
    cout<<"After Clear():"<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;

    cout<<"\n[5] 释放内存..."<<endl;
    q.Free();
    cout<<"After Free():"<<endl;
    cout<<"AllocCount: "<<q.GetAllocCount()<<", Count: "<<q.GetCount()<<endl;
}

/**
 * 测试 ValueQueue 的 Contains
 */
void TestQueueContains()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValueQueue Contains Test"<<endl<<endl;

    ValueQueue<int> q;

    for(int i = 0; i < 5; ++i)
        q.Push(i * 10);

    cout<<"ValueQueue: ";
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

// ==================== ManagedQueue 测试 ====================

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
    cout<<"ValueQueue Object Test"<<endl<<endl;

    ManagedQueue<QueueTestObject> tab;

    int i;

    for(i=0;i<10;i++)
    {
        QueueTestObject *obj=new QueueTestObject;

        obj->Set(i);

        tab.Push(obj);
    }

    cout<<"ValueQueue Count: "<<tab.GetCount()<<endl;

    for(i=0;i<5;i++)        //只取出5个,剩几个给自动清理处理
    {
        QueueTestObject *obj;

        if(tab.Pop(obj))
            delete obj;
    }

    cout<<"ValueQueue Count: "<<tab.GetCount()<<endl;
}

/**
 * 测试 ManagedQueue 的值类型接口
 */
void TestObjectQueueValueType()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedQueue Value Type Test"<<endl<<endl;

    ManagedQueue<QueueTestObject> q;

    cout<<"[1] 通过值类型添加..."<<endl;
    {
        QueueTestObject obj;
        obj.Set(100);
        q.Push(obj);
    }

    cout<<"\n[2] 通过值类型弹出..."<<endl;
    QueueTestObject obj;
    if(q.Pop(obj))
    {
        cout<<"Popped successfully"<<endl;
    }
    cout<<"ValueQueue Count: "<<q.GetCount()<<endl;
}

/**
 * 测试 ManagedQueue 的内存管理
 */
void TestObjectQueueMemoryManagement()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedQueue Memory Management Test"<<endl<<endl;

    {
        ManagedQueue<QueueTestObject> q;

        cout<<"[1] 添加 5 个对象..."<<endl;
        for(int i = 0; i < 5; ++i)
        {
            QueueTestObject *obj = new QueueTestObject;
            obj->Set(i * 100);
            q.Push(obj);
        }

        cout<<"\n[2] 队列中有 "<<q.GetCount()<<" 个对象"<<endl;

        cout<<"\n[3] 弹出 2 个对象..."<<endl;
        QueueTestObject *obj = nullptr;
        for(int i = 0; i < 2; ++i)
        {
            if(q.Pop(obj))
                delete obj;
        }

        cout<<"\n[4] 队列中还有 "<<q.GetCount()<<" 个对象"<<endl;

        cout<<"\n[5] 销毁队列，剩余对象自动清理..."<<endl;
    }

    cout<<"\n[6] 队列已销毁"<<endl;
}

/**
 * 测试 ManagedQueue 的大量操作
 */
void TestObjectQueueStress()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedQueue Stress Test"<<endl<<endl;

    ManagedQueue<QueueTestObject> q;

    cout<<"[1] 添加 100 个对象..."<<endl;
    for(int i = 0; i < 100; ++i)
    {
        QueueTestObject *obj = new QueueTestObject;
        obj->Set(i);
        q.Push(obj);
    }

    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[2] 弹出 50 个对象..."<<endl;
    QueueTestObject *obj = nullptr;
    int popped_count = 0;
    for(int i = 0; i < 50; ++i)
    {
        if(q.Pop(obj))
        {
            delete obj;
            ++popped_count;
        }
    }

    cout<<"Successfully popped: "<<popped_count<<" objects"<<endl;
    cout<<"Count: "<<q.GetCount()<<endl;

    cout<<"\n[3] 自动清理剩余 "<<q.GetCount()<<" 个对象..."<<endl;
    q.Clear();
    cout<<"Cleared"<<endl;
}

/**
 * 对比 ValueQueue 和 ManagedQueue 的类型约束
 */
void TestTypeConstraints()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Type Constraints Test"<<endl<<endl;

    cout<<"[1] ValueQueue<int> - 平凡类型 [OK]"<<endl;
    ValueQueue<int> q1;
    q1.Push(42);

    cout<<"[2] ValueQueue<UserInfo> - 平凡 POD [OK]"<<endl;
    ValueQueue<UserInfo> q2;

    cout<<"[3] ManagedQueue<QueueTestObject> - 非平凡类型 [OK]"<<endl;
    ManagedQueue<QueueTestObject> oq1;

    cout<<"\n注意：以下类型组合会编译失败："<<endl;
    cout<<"- ValueQueue<std::string> - [X]"<<endl;
    cout<<"- ValueQueue<ManagedQueue<T>> - [X]"<<endl;
    cout<<"- ManagedQueue<int> - [X]"<<endl;
    cout<<"- ManagedQueue<float> - [X]"<<endl;
}

int os_main(int,os_char **)
{
    srand(time(nullptr));

    //原生单个数据测试
    TestQueueOrdered();
    TestQueueUnordered();

    //原生结构体测试
    TestQueueStruct();

    // ========== 新增测试 ==========
    
    // ValueQueue 的扩展功能测试
    TestQueuePeek();
    TestQueueCircularBuffer();
    TestQueueCapacity();
    TestQueueContains();

    //对象测试
    TestObjectQueue();

    // ManagedQueue 的扩展功能测试
    TestObjectQueueValueType();
    TestObjectQueueMemoryManagement();
    TestObjectQueueStress();

    // 类型约束测试
    TestTypeConstraints();

    cout<<endl;
    cout<<"========================================="<<endl;
    cout<<"All tests completed!"<<endl;
    cout<<"========================================="<<endl;

    return(0);
}
