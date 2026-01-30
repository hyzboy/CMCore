#include<hgl/type/ManagedPool.h>
#include<iostream>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

void ShowUserInfoArray(const std::vector<UserInfoClass *> &ua)
{
    cout<<"user info array "<<ua.size()<<" [";

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
    ManagedPool<UserInfoClass> pool;

    ValueArray<UserInfoClass *> release_list;

    // Add all user data to the pool
    {
        for(auto ui:user_info_array)
        {
            UserInfoClass *uic=new UserInfoClass;

            uic->Set(ui);

            pool.AppendToActive(uic);

            if(rand()%3==1)                 // 1/3 probability of adding to release list
                release_list.Add(uic);
        }
    }

    ShowUserInfoArray(pool.GetActiveView());

    // Release the recorded ones (idle only, not freeing memory)
    pool.Release(release_list.data(),release_list.size());

    ShowUserInfoArray(pool.GetActiveView());

    cout<<"idle count: "<<pool.GetIdleCount()<<endl;

    // Get half of the idle list data
    {
        int i=0;
        UserInfoClass *uic;

        for(int i=0;i<release_list.size()/2;i++)        // Get half of idle list
            if(pool.Get(uic))
                cout<<i<<": get "<<uic->GetName()<<" from idle."<<endl;
    }

    // Leave the other half for auto-release
    cout<<"finished current function, and then auto release."<<endl;

    // Originally automatic, we call it manually here for debug output.
    // The following code is equivalent to ManagedPool::~ManagedPool()
    {
        cout<<"clear active "<<pool.GetActiveCount()<<endl;
        pool.ClearActive();

        cout<<"clear idle "<<pool.GetIdleCount()<<endl;
        pool.ClearIdle();
    }
}

/**
 * Test ManagedPool<T> Create method
 */
void TestObjectPoolCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedPool Create Test"<<endl<<endl;

    ManagedPool<UserInfoClass> pool;

    cout<<"[1] Create 5 new objects..."<<endl;
    for(int i = 0; i < 5; ++i)
    {
        UserInfoClass *uic = nullptr;
        if(pool.Create(uic))
        {
            uic->Set(user_info_array[i]);
            cout<<"Created: "<<uic->GetName()<<endl;
        }
    }

    cout<<"\n[2] Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] Release 2 to idle..."<<endl;
    const std::vector<UserInfoClass *> &active = pool.GetActiveView();
    for(int i = 0; i < 2 && i < active.size(); ++i)
    {
        pool.Release(active[i]);
        cout<<"Released: "<<active[i]->GetName()<<endl;
    }

    cout<<"\n[4] Final status..."<<endl;
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[5] Auto cleanup..."<<endl;
    pool.Clear();
    cout<<"Cleanup completed"<<endl;
}

/**
 * Test ManagedPool<T> GetOrCreate method
 */
void TestObjectPoolGetOrCreate()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedPool GetOrCreate Test"<<endl<<endl;

    ManagedPool<UserInfoClass> pool;

    cout<<"[1] First GetOrCreate..."<<endl;
    UserInfoClass *uic1 = nullptr;
    if(pool.GetOrCreate(uic1))
    {
        uic1->Set(user_info_array[0]);
        cout<<"Got or created: "<<uic1->GetName()<<endl;
    }

    cout<<"\n[2] Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] Release to idle..."<<endl;
    pool.Release(uic1);
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] Second GetOrCreate (get from idle)..."<<endl;
    UserInfoClass *uic2 = nullptr;
    if(pool.GetOrCreate(uic2))
    {
        cout<<"Got or created: "<<uic2->GetName()<<endl;
        cout<<"Is same object: "<<(uic1 == uic2 ? "yes" : "no")<<endl;
    }

    cout<<"\n[5] Cleanup..."<<endl;
    pool.Clear();
}

/**
 * Test ManagedPool<T> capacity management and memory safety
 */
void TestObjectPoolCapacity()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedPool Capacity Test"<<endl<<endl;

    {
        ManagedPool<UserInfoClass> pool;

        cout<<"[1] Reserve pre-allocate..."<<endl;
        pool.reserve(10, true);

        cout<<"\n[2] Create 5 objects..."<<endl;
        for(int i = 0; i < 5; ++i)
        {
            UserInfoClass *uic = nullptr;
            if(pool.Create(uic))
            {
                uic->Set(user_info_array[i]);
                cout<<"Created: "<<uic->GetName()<<endl;
            }
        }

        cout<<"\n[3] Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;
        cout<<"History max: "<<pool.GetHistoryMaxCount()<<endl;

        cout<<"\n[4] Release all to idle..."<<endl;
        pool.ReleaseActive();
        cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

        cout<<"\n[5] Scope ends, auto cleanup..."<<endl;
    }

    cout<<"Cleanup completed"<<endl;
}

/**
 * Test ManagedPool<T> batch operations
 */
void TestObjectPoolBatchOperations()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedPool Batch Operations Test"<<endl<<endl;

    ManagedPool<UserInfoClass> pool;

    cout<<"[1] Create and add 6 objects..."<<endl;
    ValueArray<UserInfoClass *> all_objs;
    for(int i = 0; i < 6; ++i)
    {
        UserInfoClass *uic = new UserInfoClass;
        uic->Set(user_info_array[i]);
        pool.AppendToActive(uic);
        all_objs.Add(uic);
    }
    cout<<"Added to active pool"<<endl;

    ShowUserInfoArray(pool.GetActiveView());

    cout<<"\n[2] Release first 3 in batch..."<<endl;
    int released = pool.Release(all_objs.data(), 3);
    cout<<"Successfully released: "<<released<<" objects"<<endl;

    cout<<"\n[3] Current status..."<<endl;
    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[4] Get idle objects..."<<endl;
    UserInfoClass *uic = nullptr;
    int got = 0;
    while(pool.Get(uic))
        cout<<++got<<": "<<uic->GetName()<<endl;

    cout<<"\n[5] Cleanup..."<<endl;
    pool.Clear();
}

/**
 * Test ManagedPool<T> stress test
 */
void TestObjectPoolStress()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedPool Stress Test"<<endl<<endl;

    {
        ManagedPool<UserInfoClass> pool;

        cout<<"[1] Create 100 objects..."<<endl;
        for(int i = 0; i < 100; ++i)
        {
            UserInfoClass *uic = nullptr;
            if(pool.GetOrCreate(uic))
            {
                uic->Set(user_info_array[i % 26]);
            }
        }
        cout<<"Creation completed"<<endl;
        cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

        cout<<"\n[2] Release half..."<<endl;
        const std::vector<UserInfoClass *> &active = pool.GetActiveView();
        int to_release = active.size() / 2;
        ValueArray<UserInfoClass *> release_list;
        for(int i = 0; i < to_release; ++i)
            release_list.Add(active[i]);

        int released = pool.Release(release_list.data(), release_list.size());
        cout<<"Released: "<<released<<" objects"<<endl;
        cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

        cout<<"\n[3] Get idle objects and reuse..."<<endl;
        UserInfoClass *uic = nullptr;
        int got = 0;
        while(got < 20 && pool.Get(uic))
        {
            uic->Set(user_info_array[++got % 26]);
            cout<<"Reused: "<<uic->GetName()<<" (No. "<<got<<")"<<endl;
        }

        cout<<"\n[4] Final status..."<<endl;
        cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;
        cout<<"History max: "<<pool.GetHistoryMaxCount()<<endl;

        cout<<"\n[5] Scope ends, auto cleanup all objects..."<<endl;
    }

    cout<<"Stress test completed"<<endl;
}

/**
 * ManagedPool direct Append operation test
 */
void TestObjectPoolDirectAppend()
{
    cout<<endl;
    cout<<"-----------------------------------------"<<endl;
    cout<<"ManagedPool Direct Append Test"<<endl<<endl;

    ManagedPool<UserInfoClass> pool;

    cout<<"[1] Direct add to active..."<<endl;
    for(int i = 0; i < 3; ++i)
    {
        UserInfoClass *uic = new UserInfoClass;
        uic->Set(user_info_array[i]);
        pool.AppendToActive(uic);
    }

    ShowUserInfoArray(pool.GetActiveView());

    cout<<"\n[2] Direct add to idle..."<<endl;
    UserInfoClass *idle_obj = new UserInfoClass;
    idle_obj->Set(user_info_array[10]);
    pool.AppendToIdle(idle_obj);

    cout<<"Active: "<<pool.GetActiveCount()<<", Idle: "<<pool.GetIdleCount()<<endl;

    cout<<"\n[3] Get from idle..."<<endl;
    UserInfoClass *got = nullptr;
    if(pool.Get(got))
        cout<<"Got: "<<got->GetName()<<endl;

    cout<<"\n[4] Cleanup..."<<endl;
    pool.Clear();
}

int main(int,char **)
{
    srand(time(nullptr));

    cout<<"========================================="<<endl;
    cout<<"ManagedPool<T> Tests"<<endl;
    cout<<"========================================="<<endl;

    // Original test
    ObjectPoolTest();

    // ========== New ManagedPool<T> tests ==========
    TestObjectPoolCreate();
    TestObjectPoolGetOrCreate();
    TestObjectPoolCapacity();
    TestObjectPoolBatchOperations();
    TestObjectPoolStress();
    TestObjectPoolDirectAppend();

    cout<<endl;
    cout<<"========================================="<<endl;
    cout<<"All ManagedPool tests completed!"<<endl;
    cout<<"========================================="<<endl;

    return 0;
}
