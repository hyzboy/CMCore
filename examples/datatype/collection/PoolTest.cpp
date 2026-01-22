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

// ==================== 新增测试函数 ====================

/**
 * 测试 Pool<T> 的 Create 方法
 */
void TestPoolCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Pool Create Test"<<endl<<endl;

    Pool<int> pool;

    cout<<"[1] 创建 10 个新对象..."<<endl;
    for(int i = 0; i < 10; ++i)
    {
        int val = 0;
        if(pool.Create(val))
            cout<<"Created: "<<val<<" (default)"<<endl;
    }

    cout<<"\n[2] 活跃对象数量: "<<pool.GetActiveCount()<<endl;
    cout<<"[3] 闲置对象数量: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] 释放 5 个到闲置池..."<<endl;
    const DataArray<int> &active = pool.GetActiveArray();
    for(int i = 0; i < 5 && i < active.GetCount(); ++i)
    {
        pool.Release(active[i]);
        cout<<"Released: "<<active[i]<<endl;
    }

    cout<<"\n[5] 释放后的状态..."<<endl;
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;
}

/**
 * 测试 Pool<T> 的 GetOrCreate 方法
 */
void TestPoolGetOrCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Pool GetOrCreate Test"<<endl<<endl;

    Pool<int> pool;

    cout<<"[1] 第一次 GetOrCreate（应创建新对象）..."<<endl;
    int val1 = -1;
    if(pool.GetOrCreate(val1))
        cout<<"Got or created: "<<val1<<" (from idle or newly created)"<<endl;

    cout<<"\n[2] 活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] 释放到闲置池..."<<endl;
    pool.Release(val1);
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] 第二次 GetOrCreate（应从闲置池获取）..."<<endl;
    int val2 = -1;
    if(pool.GetOrCreate(val2))
        cout<<"Got or created: "<<val2<<" (from idle pool)"<<endl;

    cout<<"\n[5] 最终状态..."<<endl;
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;
    cout<<"历史最大: "<<pool.GetHistoryMaxCount()<<endl;
}

/**
 * 测试 Pool<T> 的容量管理
 */
void TestPoolCapacity()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Pool Capacity Test"<<endl<<endl;

    Pool<int> pool;

    cout<<"[1] Reserve 预分配 100 个..."<<endl;
    pool.Reserve(100, true);  // true 表示设为最大活跃数
    cout<<"已预分配，可创建最多: "<<pool.GetHistoryMaxCount()<<endl;

    cout<<"\n[2] 创建 5 个对象..."<<endl;
    for(int i = 0; i < 5; ++i)
    {
        int val = 0;
        pool.Create(val);
    }
    cout<<"活跃: "<<pool.GetActiveCount()<<endl;

    cout<<"\n[3] 检查是否已满..."<<endl;
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;

    cout<<"\n[4] 尝试创建到达最大限制..."<<endl;
    int created = 0;
    for(int i = 0; i < 100; ++i)
    {
        int val = 0;
        if(pool.Create(val))
            ++created;
        else
            break;
    }
    cout<<"成功创建: "<<created<<" 个"<<endl;
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;

    cout<<"\n[5] 释放所有到闲置..."<<endl;
    pool.ReleaseActive();
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[6] 清空池..."<<endl;
    pool.Clear();
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;
}

/**
 * 测试 Pool<T> 的对象查询
 */
void TestPoolQuery()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Pool Query Test"<<endl<<endl;

    Pool<UserInfo> pool;

    cout<<"[1] 添加用户信息到活跃池..."<<endl;
    UserInfo ui1 = user_info_array[0];
    UserInfo ui2 = user_info_array[1];
    pool.AppendToActive(ui1);
    pool.AppendToActive(ui2);

    cout<<"\n[2] 检查对象是否活跃..."<<endl;
    cout<<"IsActive(ui1): "<<(pool.IsActive(ui1) ? "yes" : "no")<<endl;
    cout<<"IsActive(ui2): "<<(pool.IsActive(ui2) ? "yes" : "no")<<endl;

    cout<<"\n[3] 检查对象是否闲置..."<<endl;
    cout<<"IsIdle(ui1): "<<(pool.IsIdle(ui1) ? "yes" : "no")<<endl;

    cout<<"\n[4] 释放第一个对象到闲置..."<<endl;
    pool.Release(ui1);
    cout<<"IsActive(ui1): "<<(pool.IsActive(ui1) ? "yes" : "no")<<endl;
    cout<<"IsIdle(ui1): "<<(pool.IsIdle(ui1) ? "yes" : "no")<<endl;

    cout<<"\n[5] 获取并返回到活跃..."<<endl;
    UserInfo ui_got;
    if(pool.Get(ui_got))
    {
        cout<<"Got from idle: "<<ui_got.name<<endl;
        cout<<"IsActive(ui_got): "<<(pool.IsActive(ui_got) ? "yes" : "no")<<endl;
    }
}

/**
 * 测试 Pool<T> 的批量操作
 */
void TestPoolBatchOperations()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Pool Batch Operations Test"<<endl<<endl;

    Pool<UserInfo> pool;

    cout<<"[1] 添加 5 个用户信息..."<<endl;
    for(int i = 0; i < 5; ++i)
        pool.AppendToActive(user_info_array[i]);

    ShowUserInfoArray(pool.GetActiveArray());

    cout<<"\n[2] 准备批量释放..."<<endl;
    ArrayList<UserInfo> release_arr;
    for(int i = 0; i < 3; ++i)
        release_arr.Add(user_info_array[i]);

    cout<<"\n[3] 批量释放 3 个..."<<endl;
    int released = pool.Release(release_arr.GetData(), release_arr.GetCount());
    cout<<"成功释放: "<<released<<" 个"<<endl;

    cout<<"\n[4] 释放后状态..."<<endl;
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;
    ShowUserInfoArray(pool.GetActiveArray());

    cout<<"\n[5] 取出所有闲置数据..."<<endl;
    int count = 0;
    UserInfo ui;
    while(pool.Get(ui))
        cout<<++count<<": "<<ui.name<<endl;
}

/**
 * 测试 Pool<T> 的边界条件
 */
void TestPoolBoundaryConditions()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"Pool Boundary Conditions Test"<<endl<<endl;

    Pool<int> pool;

    cout<<"[1] 空池状态..."<<endl;
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[2] 从空池获取..."<<endl;
    int val = -1;
    if(!pool.Get(val))
        cout<<"从空池获取失败（预期）"<<endl;

    cout<<"\n[3] 设置最大数为 1..."<<endl;
    pool.SetMaxActiveCount(1);

    cout<<"\n[4] 创建 1 个..."<<endl;
    pool.Create(val);
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;

    cout<<"\n[5] 尝试创建第 2 个（应失败）..."<<endl;
    int val2 = -1;
    if(!pool.Create(val2))
        cout<<"创建失败（预期，池已满）"<<endl;

    cout<<"\n[6] 释放后重新创建..."<<endl;
    pool.Release(val);
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;
    if(pool.Create(val2))
        cout<<"创建成功"<<endl;
}

// ==================== ObjectPool 新增测试 ====================

/**
 * 测试 ObjectPool<T> 的 Create 方法
 */
void TestObjectPoolCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectPool Create Test"<<endl<<endl;

    ObjectPool<UserInfoClass> pool;

    cout<<"[1] 创建 5 个新对象..."<<endl;
    for(int i = 0; i < 5; ++i)
    {
        UserInfoClass *uic = nullptr;
        if(pool.Create(uic))
        {
            uic->Set(user_info_array[i]);
            cout<<"Created: "<<uic->GetName()<<endl;
        }
    }

    cout<<"\n[2] 活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] 释放 2 个到闲置..."<<endl;
    const DataArray<UserInfoClass *> &active = pool.GetActiveArray();
    for(int i = 0; i < 2 && i < active.GetCount(); ++i)
    {
        pool.Release(active[i]);
        cout<<"Released: "<<active[i]->GetName()<<endl;
    }

    cout<<"\n[4] 最终状态..."<<endl;
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[5] 自动清理..."<<endl;
    pool.Clear();
    cout<<"清理完成"<<endl;
}

/**
 * 测试 ObjectPool<T> 的 GetOrCreate 方法
 */
void TestObjectPoolGetOrCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectPool GetOrCreate Test"<<endl<<endl;

    ObjectPool<UserInfoClass> pool;

    cout<<"[1] 第一次 GetOrCreate..."<<endl;
    UserInfoClass *uic1 = nullptr;
    if(pool.GetOrCreate(uic1))
    {
        uic1->Set(user_info_array[0]);
        cout<<"Got or created: "<<uic1->GetName()<<endl;
    }

    cout<<"\n[2] 活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] 释放到闲置..."<<endl;
    pool.Release(uic1);
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] 第二次 GetOrCreate（从闲置获取）..."<<endl;
    UserInfoClass *uic2 = nullptr;
    if(pool.GetOrCreate(uic2))
    {
        cout<<"Got or created: "<<uic2->GetName()<<endl;
        cout<<"是否同一个对象: "<<(uic1 == uic2 ? "yes" : "no")<<endl;
    }

    cout<<"\n[5] 清理..."<<endl;
    pool.Clear();
}

/**
 * 测试 ObjectPool<T> 的容量管理和内存安全
 */
void TestObjectPoolCapacity()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectPool Capacity Test"<<endl<<endl;

    {
        ObjectPool<UserInfoClass> pool;

        cout<<"[1] Reserve 预分配..."<<endl;
        pool.Reserve(10, true);

        cout<<"\n[2] 创建 5 个对象..."<<endl;
        for(int i = 0; i < 5; ++i)
        {
            UserInfoClass *uic = nullptr;
            if(pool.Create(uic))
            {
                uic->Set(user_info_array[i]);
                cout<<"Created: "<<uic->GetName()<<endl;
            }
        }

        cout<<"\n[3] 活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;
        cout<<"历史最大: "<<pool.GetHistoryMaxCount()<<endl;

        cout<<"\n[4] 释放所有到闲置..."<<endl;
        pool.ReleaseActive();
        cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

        cout<<"\n[5] 作用域结束，自动清理..."<<endl;
    }

    cout<<"清理完成"<<endl;
}

/**
 * 测试 ObjectPool<T> 的批量操作
 */
void TestObjectPoolBatchOperations()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectPool Batch Operations Test"<<endl<<endl;

    ObjectPool<UserInfoClass> pool;

    cout<<"[1] 创建并添加 6 个对象..."<<endl;
    ArrayList<UserInfoClass *> all_objs;
    for(int i = 0; i < 6; ++i)
    {
        UserInfoClass *uic = new UserInfoClass;
        uic->Set(user_info_array[i]);
        pool.AppendToActive(uic);
        all_objs.Add(uic);
    }
    cout<<"已添加到活跃池"<<endl;

    ShowUserInfoArray(pool.GetActiveArray());

    cout<<"\n[2] 批量释放前 3 个..."<<endl;
    int released = pool.Release(all_objs.GetData(), 3);
    cout<<"成功释放: "<<released<<" 个"<<endl;

    cout<<"\n[3] 当前状态..."<<endl;
    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] 取出闲置对象..."<<endl;
    UserInfoClass *uic = nullptr;
    int got = 0;
    while(pool.Get(uic))
        cout<<++got<<": "<<uic->GetName()<<endl;

    cout<<"\n[5] 清理..."<<endl;
    pool.Clear();
}

/**
 * 测试 ObjectPool<T> 的压力测试
 */
void TestObjectPoolStress()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectPool Stress Test"<<endl<<endl;

    {
        ObjectPool<UserInfoClass> pool;

        cout<<"[1] 创建 100 个对象..."<<endl;
        for(int i = 0; i < 100; ++i)
        {
            UserInfoClass *uic = nullptr;
            if(pool.GetOrCreate(uic))
            {
                uic->Set(user_info_array[i % 26]);
            }
        }
        cout<<"创建完成"<<endl;
        cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

        cout<<"\n[2] 释放一半..."<<endl;
        const DataArray<UserInfoClass *> &active = pool.GetActiveArray();
        int to_release = active.GetCount() / 2;
        ArrayList<UserInfoClass *> release_list;
        for(int i = 0; i < to_release; ++i)
            release_list.Add(active[i]);

        int released = pool.Release(release_list.GetData(), release_list.GetCount());
        cout<<"释放了: "<<released<<" 个"<<endl;
        cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

        cout<<"\n[3] 取出闲置对象并重新添加..."<<endl;
        UserInfoClass *uic = nullptr;
        int got = 0;
        while(got < 20 && pool.Get(uic))
        {
            uic->Set(user_info_array[++got % 26]);
            cout<<"重用: "<<uic->GetName()<<" (第 "<<got<<" 个)"<<endl;
        }

        cout<<"\n[4] 最终状态..."<<endl;
        cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;
        cout<<"历史最大: "<<pool.GetHistoryMaxCount()<<endl;

        cout<<"\n[5] 作用域结束，自动清理所有对象..."<<endl;
    }

    cout<<"压力测试完成"<<endl;
}

/**
 * ObjectPool 的 Append 直接操作测试
 */
void TestObjectPoolDirectAppend()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ObjectPool Direct Append Test"<<endl<<endl;

    ObjectPool<UserInfoClass> pool;

    cout<<"[1] 直接添加到活跃..."<<endl;
    for(int i = 0; i < 3; ++i)
    {
        UserInfoClass *uic = new UserInfoClass;
        uic->Set(user_info_array[i]);
        pool.AppendToActive(uic);
    }

    ShowUserInfoArray(pool.GetActiveArray());

    cout<<"\n[2] 直接添加到闲置..."<<endl;
    UserInfoClass *idle_obj = new UserInfoClass;
    idle_obj->Set(user_info_array[10]);
    pool.AppendToIdle(idle_obj);

    cout<<"活跃: "<<pool.GetActiveCount()<<", 闲置: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] 从闲置获取..."<<endl;
    UserInfoClass *got = nullptr;
    if(pool.Get(got))
        cout<<"Got: "<<got->GetName()<<endl;

    cout<<"\n[4] 清理..."<<endl;
    pool.Clear();
}

int main(int,char **)
{
    srand(time(nullptr));

    //原始测试
    StructPoolTest();
    ObjectPoolTest();

    // ========== 新增 Pool<T> 测试 ==========
    TestPoolCreate();
    TestPoolGetOrCreate();
    TestPoolCapacity();
    TestPoolQuery();
    TestPoolBatchOperations();
    TestPoolBoundaryConditions();

    // ========== 新增 ObjectPool<T> 测试 ==========
    TestObjectPoolCreate();
    TestObjectPoolGetOrCreate();
    TestObjectPoolCapacity();
    TestObjectPoolBatchOperations();
    TestObjectPoolStress();
    TestObjectPoolDirectAppend();

    cout<<endl;
    cout<<"========================================="<<endl;
    cout<<"All Pool tests completed!"<<endl;
    cout<<"========================================="<<endl;

    return 0;
}
