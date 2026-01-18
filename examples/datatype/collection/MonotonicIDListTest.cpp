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

    cout<<"After initial insert (ID0..9):\n";
    PrintRange(list,0,9);

    // 删除多个ID
    const int to_remove[]={ 1,3,5,8 };
    for(int id:to_remove)
        list.Remove(id);

    cout<<"After removing {1,3,5,8}:\n";
    PrintRange(list,0,9);

    cout<<"Shrinking the list...\n";
    list.Shrink();
    PrintRange(list,0,9);

    // 再添加4个数据，ID10..13
    for(int i=0; i<4; ++i)
        list.Add(100+i*10);

    cout<<"After inserting 4 more (ID10..13):\n";
    PrintRange(list,0,13);

    // 测试删除一个不存在的ID
    bool removed=list.Remove(42);
    cout<<"Remove ID42 result: "<<(removed?"true":"false")<<'\n';

    cout<<"\n=== Testing Reindex ===\n";

    // 使用 DataArray<IDRemap<int>> 存储旧ID到新ID的映射
    DataArray<IDRemap> remap_list;
    int reindexed_count=list.Reindex(remap_list);

    cout<<"Reindexed "<<reindexed_count<<" items\n";
    cout<<"Old ID -> New ID mapping:\n";
    for(int i=0; i<remap_list.GetCount(); ++i)
    {
        const IDRemap &remap=remap_list[i];
        cout<<"  Old ID "<<remap.old_id<<" -> New ID "<<remap.new_id<<"\n";
    }

    cout<<"\nAfter reindex, checking new IDs (0.."<<(reindexed_count-1)<<"):\n";
    for(int new_id=0; new_id<reindexed_count; ++new_id)
    {
        int *v=list.Get(new_id);
        int location=list.GetLocation(new_id);
        cout<<"New ID "<<new_id<<": ";
        if(v)
            cout<<*v<<" (location: "<<location<<")\n";
        else
            cout<<"not found\n";
    }

    cout<<"\nOld ID -> New ID remapping details:\n";
    for(int i=0; i<remap_list.GetCount(); ++i)
    {
        const IDRemap &remap=remap_list[i];
        if(remap.old_id!=remap.new_id) // 只检查ID变化了的
        {
            // 检查旧ID现在对应的数据是否是新ID的数据
            int *old_val=list.Get(remap.old_id);
            int *new_val=list.Get(remap.new_id);

            cout<<"Old ID "<<remap.old_id<<" -> New ID "<<remap.new_id<<": ";
            if(old_val&&new_val)
            {
                // 旧ID位置的值应该是新ID位置的值（因为重新编号后，旧ID如果碰巧存在，也是因为新ID占用了那个数字）
                cout<<"Old ID location now has value "<<*old_val<<", New ID has value "<<*new_val;
                if(remap.old_id<reindexed_count) // 如果旧ID还在有效范围内，说明被新数据占用
                    cout<<" (Old ID reused by new data)";
                cout<<"\n";
            }
            else
            {
                cout<<"Correctly remapped\n";
            }
        }
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

    return 0;
}
