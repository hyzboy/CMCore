#include<hgl/type/ValuePool.h>
#include<iostream>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void ShowUserInfoArray(const ValueBuffer<UserInfo> &ua)
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
    ValuePool<UserInfo> pool;

    // Add all user data to the pool
    {
        for(auto ui:user_info_array)
            pool.AppendToActive(ui);
    }

    ShowUserInfoArray(pool.GetActiveView());

    // Release some randomly (idle only, no memory released)
    {
        int count=10+rand()%10;

        for(int i=0;i<count;i++)
            pool.Release(user_info_array[rand()%26]);
    }

    ShowUserInfoArray(pool.GetActiveView());

    cout<<"idle count: "<<pool.GetIdleCount()<<endl;

    // Get all idle data
    {
        int i=0;
        UserInfo ui;

        while(pool.Get(ui))
            cout<<i++<<": get "<<ui.name<<" from idle."<<endl;
    }
}

/**
 * Test ValuePool<T> Create method
 */
void TestPoolCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValuePool Create Test"<<endl<<endl;

    ValuePool<int> pool;

    cout<<"[1] Create 10 new objects..."<<endl;
    for(int i = 0; i < 10; ++i)
    {
        int val = 0;
        if(pool.Create(val))
            cout<<"Created: "<<val<<" (default)"<<endl;
    }

    cout<<"\n[2] Active object count: "<<pool.GetActiveCount()<<endl;
    cout<<"[3] Idle object count: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] Release 5 to idle pool..."<<endl;
    const ValueBuffer<int> &active = pool.GetActiveView();
    for(int i = 0; i < 5 && i < active.GetCount(); ++i)
    {
        pool.Release(active[i]);
        cout<<"Released: "<<active[i]<<endl;
    }

    cout<<"\n[5] Status after release..."<<endl;
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;
}

/**
 * Test ValuePool<T> GetOrCreate method
 */
void TestPoolGetOrCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValuePool GetOrCreate Test"<<endl<<endl;

    ValuePool<int> pool;

    cout<<"[1] First GetOrCreate (should create new object)..."<<endl;
    int val1 = -1;
    if(pool.GetOrCreate(val1))
        cout<<"Got or created: "<<val1<<" (from idle or newly created)"<<endl;

    cout<<"\n[2] Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] Release to idle pool..."<<endl;
    pool.Release(val1);
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] Second GetOrCreate (should get from idle pool)..."<<endl;
    int val2 = -1;
    if(pool.GetOrCreate(val2))
        cout<<"Got or created: "<<val2<<" (from idle pool)"<<endl;

    cout<<"\n[5] Final status..."<<endl;
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;
    cout<<"History max: "<<pool.GetHistoryMaxCount()<<endl;
}

/**
 * Test ValuePool<T> capacity management
 */
void TestPoolCapacity()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValuePool Capacity Test"<<endl<<endl;

    ValuePool<int> pool;

    cout<<"[1] Reserve pre-allocate 100..."<<endl;
    pool.Reserve(100, true);  // true means set as max active count
    cout<<"Pre-allocated, can create up to: "<<pool.GetHistoryMaxCount()<<endl;

    cout<<"\n[2] Create 5 objects..."<<endl;
    for(int i = 0; i < 5; ++i)
    {
        int val = 0;
        pool.Create(val);
    }
    cout<<"Active: "<<pool.GetActiveCount()<<endl;

    cout<<"\n[3] Check if full..."<<endl;
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;

    cout<<"\n[4] Try to create until max limit..."<<endl;
    int created = 0;
    for(int i = 0; i < 100; ++i)
    {
        int val = 0;
        if(pool.Create(val))
            ++created;
        else
            break;
    }

    cout<<"Successfully created: "<<created<<" objects"<<endl;
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;

    cout<<"\n[5] Release all to idle..."<<endl;
    pool.ReleaseActive();
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[6] Clear pool..."<<endl;
    pool.Clear();
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;
}

/**
 * Test ValuePool<T> object query
 */
void TestPoolQuery()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValuePool Query Test"<<endl<<endl;

    ValuePool<UserInfo> pool;

    cout<<"[1] Add user info to active pool..."<<endl;
    UserInfo ui1 = user_info_array[0];
    UserInfo ui2 = user_info_array[1];
    pool.AppendToActive(ui1);
    pool.AppendToActive(ui2);

    cout<<"\n[2] Check if objects are active..."<<endl;
    cout<<"IsActive(ui1): "<<(pool.IsActive(ui1) ? "yes" : "no")<<endl;
    cout<<"IsActive(ui2): "<<(pool.IsActive(ui2) ? "yes" : "no")<<endl;

    cout<<"\n[3] Check if objects are idle..."<<endl;
    cout<<"IsIdle(ui1): "<<(pool.IsIdle(ui1) ? "yes" : "no")<<endl;

    cout<<"\n[4] Release first object to idle..."<<endl;
    pool.Release(ui1);
    cout<<"IsActive(ui1): "<<(pool.IsActive(ui1) ? "yes" : "no")<<endl;
    cout<<"IsIdle(ui1): "<<(pool.IsIdle(ui1) ? "yes" : "no")<<endl;

    cout<<"\n[5] Get and return to active..."<<endl;
    UserInfo ui_got;
    if(pool.Get(ui_got))
    {
        cout<<"Got from idle: "<<ui_got.name<<endl;
        cout<<"IsActive(ui_got): "<<(pool.IsActive(ui_got) ? "yes" : "no")<<endl;
    }
}

/**
 * Test ValuePool<T> batch operations
 */
void TestPoolBatchOperations()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValuePool Batch Operations Test"<<endl<<endl;

    ValuePool<UserInfo> pool;

    cout<<"[1] Add 5 user info..."<<endl;
    for(int i = 0; i < 5; ++i)
        pool.AppendToActive(user_info_array[i]);

    ShowUserInfoArray(pool.GetActiveView());

    cout<<"\n[2] Prepare for batch release..."<<endl;
    ValueArray<UserInfo> release_arr;
    for(int i = 0; i < 3; ++i)
        release_arr.Add(user_info_array[i]);

    cout<<"\n[3] Release 3 in batch..."<<endl;
    int released = pool.Release(release_arr.GetData(), release_arr.GetCount());
    cout<<"Successfully released: "<<released<<" objects"<<endl;

    cout<<"\n[4] Status after release..."<<endl;
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;
    ShowUserInfoArray(pool.GetActiveView());

    cout<<"\n[5] Get all idle data..."<<endl;
    int count = 0;
    UserInfo ui;
    while(pool.Get(ui))
        cout<<++count<<": "<<ui.name<<endl;
}

/**
 * Test ValuePool<T> boundary conditions
 */
void TestPoolBoundaryConditions()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ValuePool Boundary Conditions Test"<<endl<<endl;

    ValuePool<int> pool;

    cout<<"[1] Empty pool status..."<<endl;
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[2] Get from empty pool..."<<endl;
    int val = -1;
    if(!pool.Get(val))
        cout<<"Get from empty pool failed (expected)"<<endl;

    cout<<"\n[3] Set max count to 1..."<<endl;
    pool.SetMaxActiveCount(1);

    cout<<"\n[4] Create 1..."<<endl;
    pool.Create(val);
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;

    cout<<"\n[5] Try to create second one (should fail)..."<<endl;
    int val2 = -1;
    if(!pool.Create(val2))
        cout<<"Create failed (expected, pool is full)"<<endl;

    cout<<"\n[6] Release and create again..."<<endl;
    pool.Release(val);
    cout<<"IsFull: "<<(pool.IsFull() ? "yes" : "no")<<endl;
    if(pool.Create(val2))
        cout<<"Create successful"<<endl;
}

int main(int,char **)
{
    srand(time(nullptr));

    cout<<"========================================="<<endl;
    cout<<"ValuePool<T> Tests"<<endl;
    cout<<"========================================="<<endl;

    // Original test
    StructPoolTest();

    // ========== New ValuePool<T> tests ==========
    TestPoolCreate();
    TestPoolGetOrCreate();
    TestPoolCapacity();
    TestPoolQuery();
    TestPoolBatchOperations();
    TestPoolBoundaryConditions();

    cout<<endl;
    cout<<"========================================="<<endl;
    cout<<"All ValuePool tests completed!"<<endl;
    cout<<"========================================="<<endl;

    return 0;
}
