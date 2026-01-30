#include<hgl/CoreType.h>
#include<hgl/type/ActiveIDManager.h>
#include<vector>
#include<iostream>
#include<cassert>
#include"UserInfo.h"

using namespace hgl;
using namespace std;

// ==================== 展开的 ActiveDataManager 实现 ====================

/**
 * 展开版本的 ActiveDataManager
 * 用于调试 - 在关键位置添加了详细的日志输出
 */
template<typename T>
class ActiveDataManagerDebug
{
protected:
    ActiveIDManager aim;
    std::vector<T> data_array;

public:
    ActiveDataManagerDebug()
    {
        cout << "[ADM] Constructor called" << endl;
    }

    virtual ~ActiveDataManagerDebug()
    {
        cout << "[ADM] Destructor called - calling Free()" << endl;
        Free();
        cout << "[ADM] Destructor completed" << endl;
    }

    // Reserve - 预分配容量
    void Reserve(int c)
    {
        cout << "[ADM::Reserve] Start - reserving " << c << " items" << endl;
        cout << "  Before: aim.GetTotalCount()=" << aim.GetTotalCount()
             << ", data_array.capacity()=" << data_array.capacity() << endl;

        aim.Reserve(c);
        data_array.reserve(c);

        cout << "  After: aim.GetTotalCount()=" << aim.GetTotalCount()
             << ", data_array.capacity()=" << data_array.capacity() << endl;
        cout << "[ADM::Reserve] End" << endl;
    }

    // 查询接口
    int GetActiveCount() const
    {
        return aim.GetActiveCount();
    }

    int GetIdleCount() const
    {
        return aim.GetIdleCount();
    }

    int GetTotalCount() const
    {
        return aim.GetTotalCount();
    }

    int GetHistoryMaxId() const
    {
        return aim.GetHistoryMaxId();
    }

    // 修复：返回 const 引用而不是值，避免浅拷贝导致的双重释放
    const auto& GetActiveSet() const
    {
        return aim.GetActiveSet();
    }

    const auto& GetIdleSet() const
    {
        return aim.GetIdleSet();
    }

    // WriteData - 写入指定ID的数据
    bool WriteData(const T &d, const int id)
    {
        cout << "[ADM::WriteData] Writing to ID " << id << endl;
        cout << "  data_array state: count=" << data_array.size()
             << ", capacity=" << data_array.capacity() << endl;

        if (id < 0 || id >= (int)data_array.size())
        {
            cout << "  Result: FAILED (out of range)" << endl;
            return false;
        }

        data_array[id] = d;

        cout << "  Result: SUCCESS" << endl;
        return true;
    }

    // CreateActive - 创建活跃ID
    void CreateActive(int *id, const int count = 1)
    {
        cout << "[ADM::CreateActive] Start - creating " << count << " active IDs" << endl;
        cout << "  Before: aim.GetTotalCount()=" << aim.GetTotalCount()
             << ", data_array.size()=" << data_array.size() << endl;

        aim.CreateActive(id, count);

        cout << "  After CreateActive: aim.GetTotalCount()=" << aim.GetTotalCount() << endl;
        cout << "  Expanding data_array by " << count << " items..." << endl;
        data_array.resize(data_array.size() + count);

        cout << "  After Expand: data_array.size()=" << data_array.size() << endl;
        cout << "[ADM::CreateActive] End" << endl;
    }

    // CreateIdle - 创建闲置ID
    void CreateIdle(int *idp = nullptr, const int count = 1)
    {
        cout << "[ADM::CreateIdle] Start - creating " << count << " idle IDs" << endl;
        cout << "  Before: aim.GetTotalCount()=" << aim.GetTotalCount()
             << ", data_array.size()=" << data_array.size() << endl;

        aim.CreateIdle(idp, count);

        cout << "  After CreateIdle: aim.GetTotalCount()=" << aim.GetTotalCount() << endl;
        cout << "  Expanding data_array by " << count << " items..." << endl;
        data_array.resize(data_array.size() + count);

        cout << "  After Expand: data_array.size()=" << data_array.size() << endl;
        if(idp)
        {
            cout << "  Created idle IDs: ";
            for(int i = 0; i < count; ++i)
                cout << idp[i] << " ";
            cout << endl;
        }
        cout << "[ADM::CreateIdle] End" << endl;
    }

    // Get - 激活ID
    bool Get(int *id, const int count = 1)
    {
        cout << "[ADM::Get] Getting " << count << " IDs from idle pool" << endl;
        return aim.Get(id, count);
    }

    // GetOrCreate - 获取或创建活跃ID
    bool GetOrCreate(int *id, const int count = 1)
    {
        cout << "[ADM::GetOrCreate] Getting or creating " << count << " IDs" << endl;

        if (!id || count <= 0)
        {
            cout << "  Invalid parameters, returning false" << endl;
            return false;
        }

        int idle_count = GetIdleCount();
        int get_count = (idle_count < count) ? idle_count : count;

        // Try to get IDs from idle pool first
        if (get_count > 0)
        {
            cout << "  Getting " << get_count << " IDs from idle pool" << endl;
            if (!aim.Get(id, get_count))
            {
                cout << "  Failed to get IDs" << endl;
                return false;
            }
            id += get_count;
        }

        // Create new active IDs if needed
        int create_count = count - get_count;
        if (create_count > 0)
        {
            cout << "  Creating " << create_count << " new active IDs" << endl;
            aim.CreateActive(id, create_count);
            data_array.resize(data_array.size() + create_count);
        }

        cout << "[ADM::GetOrCreate] Successfully completed" << endl;
        return true;
    }

    // Release - 释放指定ID
    int Release(const int *id, const int count = 1)
    {
        cout << "[ADM::Release] Releasing " << count << " IDs" << endl;
        return aim.Release(id, count);
    }

    // ReleaseAllActive - 释放所有活跃ID
    int ReleaseAllActive()
    {
        cout << "[ADM::ReleaseAllActive] Releasing all active IDs" << endl;
        return aim.ReleaseAllActive();
    }

    // Clear - 清空所有活跃数据
    void Clear()
    {
        cout << "[ADM::Clear] Clearing all active data" << endl;
        aim.ReleaseAllActive();
    }

    // Free - 清空所有数据并释放内存
    void Free()
    {
        cout << "[ADM::Free] Start - freeing all data and memory" << endl;
        cout << "  Calling aim.Free()..." << endl;
        aim.Free();
        cout << "  Calling data_array.Free()..." << endl;
        data_array.clear();
        data_array.shrink_to_fit();
        cout << "[ADM::Free] End" << endl;
    }

    // GetData - 获取指定ID的数据指针
    T *At(const int id)
    {
        if (id < 0 || id >= (int)data_array.size())
            return nullptr;
        return &data_array[id];
    }

    // 批量获取数据
    bool GetData(T **da, const int *idp, const int count) const
    {
        int result = 0;

        for (int i = 0; i < count; i++)
        {
            if (*idp >= 0 && *idp < (int)data_array.size())
                *da = (T*)&data_array[*idp];
            else
                *da = nullptr;

            if (*da)
                ++result;

            ++da;
            ++idp;
        }

        return result == count;
    }
};

// ==================== 调试输出函数 ====================

template<typename Container>
void DebugOutputArray(const char *hint, ActiveDataManagerDebug<UserInfo> &adm, const Container &active_ids)
{
    cout<<"("<<hint<<":"<<active_ids.size()<<")";
    if(active_ids.empty())return;

    cout<<"[";
    bool first=true;
    for(int id : active_ids)
    {
        if(!first) cout<<", ";
        first=false;
        UserInfo *cur=adm.At(id);
        if(cur)
            cout<<cur->name;
        else
            cout<<"null";
    }
    cout<<"]";
}

void DebugADMOutput(const char *hint,ActiveDataManagerDebug<UserInfo> &adm)
{
    cout<<hint<<' ';
    DebugOutputArray("Active",adm,adm.GetActiveSet());
    cout<<' ';
    cout<<endl;
}

void DebugADMDetailedState(const char *hint, ActiveDataManagerDebug<UserInfo> &adm)
{
    cout << "========================================" << endl;
    cout << hint << endl;
    cout << "========================================" << endl;

    cout << "  Basic Counts:" << endl;
    cout << "    ActiveCount: " << adm.GetActiveCount() << endl;
    cout << "    IdleCount: " << adm.GetIdleCount() << endl;
    cout << "    TotalCount: " << adm.GetTotalCount() << endl;
    cout << "    HistoryMaxId: " << adm.GetHistoryMaxId() << endl;

    cout << "\n  ActiveSet:" << endl;
    auto &active_view = adm.GetActiveSet();
    int count = active_view.size();
    cout << "    Count: " << count << endl;
    if (count > 0) {
        cout << "    Active IDs: [";
        bool first = true;
        for (int id : active_view) {
            if (!first) cout << ", ";
            first = false;
            cout << id;
        }
        cout << "]" << endl;
    }

    cout << "========================================" << endl << endl;
}

int main(int,char **)
{
    // Phase 4: Reserve + CreateIdle + WriteData (all 5 items)
    cout << "=== Phase 4: Reserve + CreateIdle + WriteData (all 5 items) ===" << endl;
    {
        cout << "  Creating ActiveDataManagerDebug..." << endl;
        ActiveDataManagerDebug<UserInfo> adm;

        cout << "\n[BEFORE Reserve] =====" << endl;

        cout << "  Calling Reserve(20)..." << endl;
        adm.Reserve(20);

        cout << "\n[AFTER Reserve] =====" << endl;
        DebugADMDetailedState("State after Reserve(20)", adm);

        cout << "  Calling CreateIdle(5)..." << endl;
        int idle_ids[5];
        adm.CreateIdle(idle_ids, 5);

        cout << "\n[AFTER CreateIdle] =====" << endl;
        DebugADMDetailedState("State after CreateIdle(5)", adm);

        cout << "  Writing data to all 5 idle items..." << endl;
        for(int i = 0; i < 5; ++i)
        {
            cout << "  Writing user_info_array[" << i << "] to ID " << idle_ids[i] << endl;
            adm.WriteData(user_info_array[i], idle_ids[i]);
        }

        cout << "\n[AFTER WriteData] =====" << endl;
        DebugADMDetailedState("State after WriteData", adm);

        cout << "  Created idle IDs: ";
        for (int i = 0; i < 5; ++i) cout << idle_ids[i] << " ";
        cout << endl;

        DebugADMOutput("  State:", adm);
        cout << "  Active: " << adm.GetActiveCount() << ", Idle: " << adm.GetIdleCount() << endl;

        cout << "\n[BEFORE Destruction] =====" << endl;
        DebugADMDetailedState("Final state before scope ends", adm);

        cout << "  Scope ending - triggering destruction..." << endl;
    }
    cout << "  Phase 4 destruction completed successfully." << endl << endl;

    return 0;
}
