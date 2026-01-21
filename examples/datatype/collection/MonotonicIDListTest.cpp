#include<hgl/type/MonotonicIDList.h>

#include<iostream>

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

    // 批量添加10个数据，ID0..9
    for(int i=0; i<10; ++i)
        list.Add(i*10);

    cout<<"=== After initial insert (ID0..9) ===\n";
    PrintRange(list,0,9);

    // 删除多个ID
    const int to_remove[]={ 1,3,5,8 };
    for(int id:to_remove)
        list.Remove(id);

    cout<<"\n=== After removing {1,3,5,8} ===\n";
    PrintRange(list,0,9);

    cout<<"\n=== After Shrinking the list ===\n";
    list.Shrink();
    PrintRange(list,0,9);

    // 再添加4个数据，ID10..13
    for(int i=0; i<4; ++i)
        list.Add(100+i*10);

    cout<<"\n=== After inserting 4 more (ID10..13) ===\n";
    PrintRange(list,0,13);

    // 测试删除一个不存在的ID
    bool removed=list.Remove(42);
    cout<<"\nRemove ID42 result: "<<(removed?"true":"false")<<'\n';

    cout<<"\n========== Testing Reindex ==========\n";

    // 使用 DataArray<IDRemap> 存储旧ID到新ID的映射
    DataArray<IDRemap> remap_list;
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

    cout<<"\n=== After Reindex: Checking new IDs (0.."<<(reindexed_count-1)<<") ===\n";
    for(int new_id=0; new_id<reindexed_count; ++new_id)
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
    for(int i=0; i<reindexed_count; ++i)
    {
        int location=list.GetLocation(i);
        if(location!=i)
        {
            cout<<"ERROR: ID "<<i<<" is at location "<<location<<" (should be "<<i<<")\n";
            all_match=false;
        }
    }
    if(all_match)
        cout<<"SUCCESS: All IDs match their locations!\n";

    cout<<"\n=== Verification: No gaps in ID sequence ===\n";
    bool no_gaps=true;
    for(int i=0; i<reindexed_count; ++i)
    {
        if(!list.Contains(i))
        {
            cout<<"ERROR: ID "<<i<<" is missing!\n";
            no_gaps=false;
        }
    }
    if(no_gaps)
        cout<<"SUCCESS: ID sequence is continuous from 0 to "<<(reindexed_count-1)<<"!\n";

    cout<<"\n=== Summary ===\n";
    cout<<"Original items: 10 (IDs 0-9)\n";
    cout<<"Removed: 4 items (IDs 1,3,5,8)\n";
    cout<<"Added back: 4 items (IDs 10-13)\n";
    cout<<"Expected remaining: 10 items\n";
    cout<<"Actual after reindex: "<<reindexed_count<<" items\n";
    cout<<"Reindex "<<(all_match && no_gaps ? "PASSED" : "FAILED")<<"!\n";

    return 0;
}
