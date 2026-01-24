#include<hgl/type/ActiveDataManager.h>
#include<iostream>
#include<vector>
#include<unordered_set>
#include<cassert>
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
    cout<<"("<<hint<<":"<<count<<")";
    if(count<=0)return;
    UserInfo **p=ui;
    cout<<"[";
    bool first=true;
    for(int i=0;i<count;i++)
    {
        if(!first) cout<<", ";
        first=false;
        UserInfo *cur=*p;
        if(cur)
            cout<<cur->name;
        else
            cout<<"null";
        ++p;
    }
    cout<<"]";
}

void DebugOutputArray(const char *hint,ActiveDataManager<UserInfo> &adm,const int *idp,const int count)
{
    cout<<"("<<hint<<" ids:"<<count<<")";
    if(!idp||count<=0){ cout<<"[]"; return; }
    cout<<"[";
    bool first=true;
    for(int i=0;i<count;i++)
    {
        if(!first) cout<<", ";
        first=false;
        int id=idp[i];
        UserInfo *ui=adm.At(id);
        cout<<"{id="<<id<<",ptr="<<ui<<",name="<<(ui?ui->name:"null")<<"}";
    }
    cout<<"]";
}

void DebugOutputArray(const char *hint,ActiveDataManager<UserInfo> &adm,const ValueBuffer<int> &da)
{
    DebugOutputArray(hint,adm,da.GetData(),da.GetCount());
}

void DebugOutputArray(const char *hint,ActiveDataManager<UserInfo> &adm,const ValueQueue<int> &queue)
{
    cout<<"("<<hint<<':'<<queue.GetCount()<<")";
    if(queue.GetCount()<=0)return;
    
    cout<<'[';
    bool first = true;
    for(int id : queue)
    {
        if(!first)cout<<',';
        first = false;
        
        UserInfo *ui = adm.At(id);
        if(ui)
            cout<<ui->name;
        else
            cout<<"null";
    }
    cout<<']';
}

void DebugADMOutput(const char *hint,ActiveDataManager<UserInfo> &adm)
{
    cout<<hint<<' ';
    DebugOutputArray("Active",adm,adm.GetActiveView());
    cout<<' ';
    DebugOutputArray("Idle",adm,adm.GetIdleView());
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
    cout << "========================================" << endl;
    cout << "ActiveDataManager Comprehensive Tests" << endl;
    cout << "========================================" << endl << endl;

    // Test 1: Initial State
    {
        cout << "=== Test 1: Initial State ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(10);
        cout << "  Active: " << adm.GetActiveCount() << ", Idle: " << adm.GetIdleCount() 
             << ", Total: " << adm.GetTotalCount() << endl;
        assert(adm.GetActiveCount() == 0);
        assert(adm.GetIdleCount() == 0);
        assert(adm.GetTotalCount() == 0);
        cout << "Test 1 passed." << endl << endl;
    }

    // Test 2: Create Idle Data
    {
        cout << "=== Test 2: Create Idle Data ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(20);
        int idle_ids[5];
        adm.CreateIdle(idle_ids, 5);
        WriteUserinfoToADM(adm, idle_ids, 5, 0);
        cout << "  Created idle IDs: ";
        for (int i = 0; i < 5; ++i) cout << idle_ids[i] << " ";
        cout << endl;
        cout << "  Active: " << adm.GetActiveCount() << ", Idle: " << adm.GetIdleCount() << endl;
        assert(adm.GetIdleCount() == 5 && adm.GetActiveCount() == 0);
        DebugADMOutput("  State:", adm);
        cout << "Test 2 passed." << endl << endl;
    }

    // Test 3: Create Active Data
    {
        cout << "=== Test 3: Create Active Data ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(20);
        int active_ids[5];
        adm.CreateActive(active_ids, 5);
        WriteUserinfoToADM(adm, active_ids, 5, 5);
        cout << "  Created active IDs: ";
        for (int i = 0; i < 5; ++i) cout << active_ids[i] << " ";
        cout << endl;
        cout << "  Active: " << adm.GetActiveCount() << ", Idle: " << adm.GetIdleCount() << endl;
        assert(adm.GetActiveCount() == 5 && adm.GetIdleCount() == 0);
        DebugADMOutput("  State:", adm);
        cout << "Test 3 passed." << endl << endl;
    }

    // Test 4: Mixed Create and Get
    {
        cout << "=== Test 4: Mixed Create and Get ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(30);
        int idle_ids[5], active_ids[5], user_ids[3];
        adm.CreateIdle(idle_ids, 5);
        WriteUserinfoToADM(adm, idle_ids, 5, 0);
        adm.CreateActive(active_ids, 5);
        WriteUserinfoToADM(adm, active_ids, 5, 5);
        DebugADMOutput("  Before Get:", adm);
        bool got = adm.Get(user_ids, 3);
        cout << "  Get(3) returned: " << got << ", Retrieved IDs: ";
        for (int i = 0; i < 3; ++i) cout << user_ids[i] << " ";
        cout << endl;
        assert(got && adm.GetActiveCount() == 8 && adm.GetIdleCount() == 2);
        DebugADMOutput("  After Get:", adm);
        cout << "Test 4 passed." << endl << endl;
    }

    // Test 5: GetOrCreate
    {
        cout << "=== Test 5: GetOrCreate (Mix Reuse + Create New) ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(50);
        int idle_ids[5], active_ids[3], more_ids[8];
        adm.CreateIdle(idle_ids, 5);
        WriteUserinfoToADM(adm, idle_ids, 5, 0);
        adm.CreateActive(active_ids, 3);
        WriteUserinfoToADM(adm, active_ids, 3, 5);
        DebugADMOutput("  Before GetOrCreate:", adm);
        bool result = adm.GetOrCreate(more_ids, 8);
        cout << "  GetOrCreate(8) returned: " << result << endl;
        cout << "  Retrieved IDs: ";
        for (int i = 0; i < 8; ++i) cout << more_ids[i] << " ";
        cout << endl;
        assert(result && adm.GetIdleCount() == 0 && adm.GetActiveCount() == 11);
        WriteUserinfoToADM(adm, more_ids, 8, 10);
        unordered_set<int> all_ids;
        all_ids.insert(active_ids, active_ids + 3);
        all_ids.insert(more_ids, more_ids + 8);
        cout << "  Unique IDs: " << all_ids.size() << " (expected: 11)" << endl;
        assert((int)all_ids.size() == 11);
        DebugADMOutput("  After GetOrCreate:", adm);
        cout << "Test 5 passed." << endl << endl;
    }

    // Test 6: Data Read/Write Operations
    {
        cout << "=== Test 6: Data Read/Write Operations ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(20);
        int ids[5];
        adm.CreateActive(ids, 5);
        for (int i = 0; i < 5; ++i) adm.WriteData(user_info_array[i], ids[i]);
        cout << "  Written 5 records" << endl;
        
        UserInfo info;
        bool ok = adm.GetData(info, ids[0]);
        cout << "  Read single: " << (ok ? info.name : "FAILED") << endl;
        assert(ok && string(info.name) == string(user_info_array[0].name));
        
        UserInfo *ptr = adm.At(ids[1]);
        cout << "  At pointer: " << (ptr ? ptr->name : "nullptr") << endl;
        assert(ptr != nullptr);
        
        UserInfo infos[5];
        ok = adm.GetData(infos, ids, 5);
        cout << "  Batch read: " << (ok ? "OK" : "FAILED") << endl;
        for (int i = 0; i < 5; ++i) assert(string(infos[i].name) == string(user_info_array[i].name));
        cout << "Test 6 passed." << endl << endl;
    }

    // Test 7: Release and Reuse
    {
        cout << "=== Test 7: Release and Reuse IDs ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(20);
        int start_ids[5], reuse_ids[2];
        adm.CreateActive(start_ids, 5);
        WriteUserinfoToADM(adm, start_ids, 5, 0);
        DebugADMOutput("  Before Release:", adm);
        int released = adm.Release(start_ids, 2);
        cout << "  Released 2 IDs: " << released << endl;
        assert(released == 2 && adm.GetActiveCount() == 3 && adm.GetIdleCount() == 2);
        DebugADMOutput("  After Release:", adm);
        
        bool got = adm.Get(reuse_ids, 2);
        cout << "  Reused: " << (got ? "OK" : "FAILED") << endl;
        assert(got && adm.GetActiveCount() == 5);
        
        UserInfo info;
        adm.GetData(info, reuse_ids[0]);
        cout << "  Data preserved: " << info.name << endl;
        assert(string(info.name) == string(user_info_array[0].name));
        cout << "Test 7 passed." << endl << endl;
    }

    // Test 8: Release All Active
    {
        cout << "=== Test 8: Release All Active ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(30);
        int ids[10];
        adm.CreateActive(ids, 10);
        WriteUserinfoToADM(adm, ids, 10, 0);
        DebugADMOutput("  Before ReleaseAllActive:", adm);
        int released = adm.ReleaseAllActive();
        cout << "  Released all: " << released << endl;
        assert(released == 10 && adm.GetActiveCount() == 0 && adm.GetIdleCount() == 10);
        DebugADMOutput("  After ReleaseAllActive:", adm);
        cout << "Test 8 passed." << endl << endl;
    }

    // Test 9: Data Integrity
    {
        cout << "=== Test 9: Data Integrity After Complex Operations ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(30);
        int idle_ids[3], active_ids[4], user_ids[2], new_ids[5];
        adm.CreateIdle(idle_ids, 3);
        WriteUserinfoToADM(adm, idle_ids, 3, 0);
        adm.CreateActive(active_ids, 4);
        WriteUserinfoToADM(adm, active_ids, 4, 3);
        cout << "  Phase 1: Created 3 idle + 4 active" << endl;
        
        adm.Get(user_ids, 2);
        cout << "  Phase 2: Got 2 from idle" << endl;
        
        adm.GetOrCreate(new_ids, 5);
        WriteUserinfoToADM(adm, new_ids, 5, 7);
        cout << "  Phase 3: GetOrCreate 5" << endl;
        
        int to_release[3] = {user_ids[0], active_ids[0], active_ids[1]};
        adm.Release(to_release, 3);
        cout << "  Phase 4: Released 3 IDs" << endl;
        
        const ValueBuffer<int> &active_array = adm.GetActiveView();
        for (int i = 0; i < active_array.GetCount(); ++i) {
            int id = active_array[i];
            UserInfo *ptr = adm.At(id);
            assert(ptr != nullptr);
        }
        cout << "  Data integrity verified" << endl;
        cout << "Test 9 passed." << endl << endl;
    }

    // Test 10: GetIdle Convenience
    {
        cout << "=== Test 10: GetIdle Convenience Function ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(10);
        int idle_ids[5];
        adm.CreateIdle(idle_ids, 5);
        WriteUserinfoToADM(adm, idle_ids, 5, 0);
        UserInfo *ptr = adm.GetIdle();
        cout << "  GetIdle() returned: " << (ptr ? ptr->name : "nullptr") << endl;
        assert(ptr != nullptr && adm.GetActiveCount() == 1 && adm.GetIdleCount() == 4);
        cout << "Test 10 passed." << endl << endl;
    }

    // Test 11: IsActive Status Check
    {
        cout << "=== Test 11: IsActive Status Check ===" << endl;
        ActiveDataManager<UserInfo> adm;
        adm.Reserve(10);
        int idle_ids[3], active_ids[3];
        adm.CreateIdle(idle_ids, 3);
        adm.CreateActive(active_ids, 3);
        cout << "  Checking idle IDs IsActive state (should be false)..." << endl;
        for (int i = 0; i < 3; ++i) {
            assert(!adm.IsActive(idle_ids[i]));
        }
        cout << "  Checking active IDs IsActive state (should be true)..." << endl;
        for (int i = 0; i < 3; ++i) {
            assert(adm.IsActive(active_ids[i]));
        }
        cout << "Test 11 passed." << endl << endl;
    }

    cout << "========================================" << endl;
    cout << "All ActiveDataManager tests passed!" << endl;
    cout << "========================================" << endl;
    return 0;
}
