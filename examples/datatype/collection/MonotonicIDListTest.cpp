#include<hgl/type/MonotonicIDList.h>

#include<iostream>
#include<random>
#include<chrono>
#include<vector>

using namespace hgl;
using namespace std;

static void PrintRange(MonotonicIDList<int> &list,int begin_id,int end_id)
{
    for(int id=begin_id; id<=end_id; ++id)
    {
        cout<<"ID "<<id<<": ";
        if(list.Contains(id))
        {
            int *v=list.Get(id);
            int location=list.GetLocation(id);
            if(v) cout<<*v<<" (location: "<<location<<")";
            cout<<'\n';
        }
        else
        {
            cout<<"not found\n";
        }
    }
}

int main(int,char **)
{
    MonotonicIDList<int> list;
    using IDRemap=MonotonicIDList<int>::IDRemap;

    // 批量添加10个数据，ID1..10
    for(int i=0; i<10; ++i)
        list.Add(i*10);

    cout<<"=== After initial insert (ID1..10) ===\n";
    PrintRange(list,1,10);

    // 删除多个ID
    const int to_remove[]={ 2,4,6,9 };
    for(int id:to_remove)
        list.Remove(id);

    cout<<"\n=== After removing {2,4,6,9} ===\n";
    PrintRange(list,1,10);

    cout<<"\n=== After Shrinking the list ===\n";
    list.Shrink();
    PrintRange(list,1,10);

    // 再添加4个数据，ID11..14（默认1基ID）
    for(int i=0; i<4; ++i)
        list.Add(100+i*10);

    cout<<"\n=== After inserting 4 more (ID11..14) ===\n";
    PrintRange(list,1,14);

    // 测试删除一个不存在的ID
    bool removed=list.Remove(42);
    cout<<"\nRemove ID42 result: "<<(removed?"true":"false")<<'\n';

    cout<<"\n========== Testing Reindex ==========\n";

    // 使用 ValueBuffer<IDRemap> 存储旧ID到新ID的映射
    ValueBuffer<IDRemap> remap_list;
    int reindexed_count=list.Reindex(remap_list);

    cout<<"Reindexed "<<reindexed_count<<" items\n\n";

    // 显示所有的ID映射
    cout<<"=== ID Remapping (old_id -> new_id) ===\n";
    for(int i=0; i<remap_list.GetCount(); ++i)
    {
        const IDRemap &remap=remap_list[i];
        int *value=list.Get(remap.new_id);  // 通过新ID获取数据
        cout<<"Old ID "<<remap.old_id<<" -> New ID "<<remap.new_id;
        if(value)
            cout<<" (value: "<<*value<<")";
        cout<<"\n";
    }

    cout<<"\n=== After Reindex: Checking new IDs (1.."<<(reindexed_count)<<") ===\n";
    for(int new_id=1; new_id<=reindexed_count; ++new_id)
    {
        int *v=list.Get(new_id);
        int location=list.GetLocation(new_id);
        cout<<"New ID "<<new_id<<": ";
        if(v)
            cout<<"value="<<*v<<", location="<<location<<"\n";
        else
            cout<<"ERROR: not found!\n";
    }

    cout<<"\n=== Verification: All IDs now match their locations ===\n";
    bool all_match=true;
    for(int i=1; i<=reindexed_count; ++i)
    {
        int location=list.GetLocation(i);
        if(location!=i-1)
        {
            cout<<"ERROR: ID "<<i<<" is at location "<<location<<" (should be "<<(i-1)<<")\n";
            all_match=false;
        }
    }
    if(all_match)
        cout<<"SUCCESS: All IDs match their locations!\n";

    cout<<"\n=== Verification: No gaps in ID sequence ===\n";
    bool no_gaps=true;
    for(int i=1; i<=reindexed_count; ++i)
    {
        if(!list.Contains(i))
        {
            cout<<"ERROR: ID "<<i<<" is missing!\n";
            no_gaps=false;
        }
    }
    if(no_gaps)
        cout<<"SUCCESS: ID sequence is continuous from 1 to "<<(reindexed_count)<<"!\n";

    cout<<"\n=== Summary ===\n";
    cout<<"Original items: 10 (IDs 1-10)\n";
    cout<<"Removed: 4 items (IDs 2,4,6,9)\n";
    cout<<"Added back: 4 items (IDs 11-14)\n";
    cout<<"Expected remaining: 10 items\n";
    cout<<"Actual after reindex: "<<reindexed_count<<" items\n";
    cout<<"Reindex "<<(all_match && no_gaps ? "PASSED" : "FAILED")<<"!\n";

    // ==================== 大规模随机测试 ====================
    cout<<"\n\n========================================\n";
    cout<<"   Large Scale Random Add/Remove Test\n";
    cout<<"========================================\n";

    auto RunStressTest = [](int total_ops, double remove_ratio) {
        MonotonicIDList<int> stress_list;
        vector<int> active_ids;
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> dis(0.0, 1.0);

        cout<<"\n--- Testing with "<<total_ops<<" operations ("<<(remove_ratio*100)<<"% removal rate) ---\n";

        auto start = chrono::high_resolution_clock::now();

        // 随机添加和删除
        int add_count = 0;
        int remove_count = 0;

        for(int op=0; op<total_ops; ++op)
        {
            if(active_ids.empty() || dis(gen) > remove_ratio)
            {
                // 添加操作
                int id = stress_list.AddGetID(add_count * 1000);
                active_ids.push_back(id);
                ++add_count;
            }
            else
            {
                // 删除操作：随机选择一个活跃ID
                uniform_int_distribution<> idx_dis(0, active_ids.size()-1);
                int idx = idx_dis(gen);
                int id_to_remove = active_ids[idx];

                if(stress_list.Remove(id_to_remove))
                {
                    active_ids.erase(active_ids.begin() + idx);
                    ++remove_count;
                }
            }
        }

        auto after_ops = chrono::high_resolution_clock::now();

        cout<<"  Added: "<<add_count<<" items\n";
        cout<<"  Removed: "<<remove_count<<" items\n";
        cout<<"  Remaining: "<<stress_list.Count()<<" items\n";
        cout<<"  Storage size: "<<stress_list.StorageSize()<<" (fragmentation: "
            <<(stress_list.StorageSize() - stress_list.Count())<<")\n";

        // 执行Shrink
        int shrink_result = stress_list.Shrink();
        auto after_shrink = chrono::high_resolution_clock::now();

        cout<<"  Shrink reclaimed: "<<shrink_result<<" slots\n";
        cout<<"  After shrink storage: "<<stress_list.StorageSize()<<"\n";

        // 执行Reindex
        ValueBuffer<IDRemap> remap;
        int reindex_result = stress_list.Reindex(remap);
        auto after_reindex = chrono::high_resolution_clock::now();

        cout<<"  Reindexed: "<<reindex_result<<" items\n";

        // 验证所有ID连续且与位置匹配
        bool verify_ok = true;
        for(int i=1; i<=reindex_result; ++i)
        {
            if(!stress_list.Contains(i))
            {
                cout<<"  ERROR: Missing ID "<<i<<"\n";
                verify_ok = false;
                break;
            }
            int loc = stress_list.GetLocation(i);
            if(loc != i-1)
            {
                cout<<"  ERROR: ID "<<i<<" at wrong location "<<loc<<" (expected "<<(i-1)<<")\n";
                verify_ok = false;
                break;
            }
        }

        auto ops_time = chrono::duration<double, milli>(after_ops - start).count();
        auto shrink_time = chrono::duration<double, milli>(after_shrink - after_ops).count();
        auto reindex_time = chrono::duration<double, milli>(after_reindex - after_shrink).count();
        auto total_time = chrono::duration<double, milli>(after_reindex - start).count();

        cout<<"  Timing:\n";
        cout<<"    Operations: "<<ops_time<<" ms\n";
        cout<<"    Shrink: "<<shrink_time<<" ms\n";
        cout<<"    Reindex: "<<reindex_time<<" ms\n";
        cout<<"    Total: "<<total_time<<" ms\n";
        cout<<"  Verification: "<<(verify_ok ? "PASSED" : "FAILED")<<"\n";

        return verify_ok;
    };

    // 100级别测试
    bool test100 = RunStressTest(100, 0.3);

    // 1000级别测试
    bool test1000 = RunStressTest(1000, 0.35);

    // 10000级别测试
    bool test10000 = RunStressTest(10000, 0.4);

    cout<<"\n========================================\n";
    cout<<"  Stress Test Summary\n";
    cout<<"========================================\n";
    cout<<"  100-scale test: "<<(test100 ? "PASSED" : "FAILED")<<"\n";
    cout<<"  1000-scale test: "<<(test1000 ? "PASSED" : "FAILED")<<"\n";
    cout<<"  10000-scale test: "<<(test10000 ? "PASSED" : "FAILED")<<"\n";
    cout<<"  Overall: "<<(test100 && test1000 && test10000 ? "ALL PASSED" : "SOME FAILED")<<"\n";

    return 0;
}
