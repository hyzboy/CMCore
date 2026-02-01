#include<hgl/platform/Platform.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<random>
#include<set>
#include<string>

using namespace hgl;
using namespace std;

// 完整性检查函数
template<typename K, typename V>
bool VerifyIntegrity(const BidirectionalMap<K, V>& bmap, const string& operation_name, int op_count)
{
    const auto& keys = bmap.GetKeys();
    const auto& values = bmap.GetValues();
    
    // 1. 检查 keys 和 values 大小一致
    if (keys.size() != values.size())
    {
        cout << "❌ [" << op_count << "] After " << operation_name << ": keys.size()=" << keys.size() 
             << " != values.size()=" << values.size() << endl;
        return false;
    }
    
    // 2. 检查 GetCount() 一致
    if (bmap.GetCount() != (int)keys.size())
    {
        cout << "❌ [" << op_count << "] After " << operation_name << ": GetCount()=" << bmap.GetCount() 
             << " != keys.size()=" << keys.size() << endl;
        return false;
    }
    
    // 3. 验证每个索引位置的 key-value 对存在于哈希表中
    for (size_t i = 0; i < keys.size(); i++)
    {
        V val;
        if (!bmap.Get(keys[i], val))
        {
            cout << "❌ [" << op_count << "] After " << operation_name << ": keys[" << i << "]=" << keys[i] 
                 << " not found in forward map" << endl;
            return false;
        }
        
        if (val != values[i])
        {
            cout << "❌ [" << op_count << "] After " << operation_name << ": keys[" << i << "]=" << keys[i] 
                 << " maps to wrong value: got " << val << ", expected " << values[i] << endl;
            return false;
        }
        
        K key;
        if (!bmap.GetByValue(values[i], key))
        {
            cout << "❌ [" << op_count << "] After " << operation_name << ": values[" << i << "]=" << values[i] 
                 << " not found in reverse map" << endl;
            return false;
        }
        
        if (key != keys[i])
        {
            cout << "❌ [" << op_count << "] After " << operation_name << ": values[" << i << "]=" << values[i] 
                 << " maps to wrong key: got " << key << ", expected " << keys[i] << endl;
            return false;
        }
    }
    
    // 4. 检查没有重复的 KEY
    set<K> unique_keys(keys.begin(), keys.end());
    if (unique_keys.size() != keys.size())
    {
        cout << "❌ [" << op_count << "] After " << operation_name << ": Duplicate keys found! "
             << "unique=" << unique_keys.size() << ", total=" << keys.size() << endl;
        return false;
    }
    
    // 5. 检查没有重复的 VALUE
    set<V> unique_values(values.begin(), values.end());
    if (unique_values.size() != values.size())
    {
        cout << "❌ [" << op_count << "] After " << operation_name << ": Duplicate values found! "
             << "unique=" << unique_values.size() << ", total=" << values.size() << endl;
        return false;
    }
    
    return true;
}

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "BidirectionalMap Debug Test (Strict Verification)" << endl;
    cout << "========================================\n" << endl;
    
    BidirectionalMap<int, string> bmap;
    set<int> present_keys;
    
    mt19937 rng(12345);  // 固定种子保证可复现
    uniform_int_distribution<int> op_dist(0, 2);  // 0:Add, 1:Delete, 2:Change
    uniform_int_distribution<int> key_dist(0, 9999);
    
    int op_count = 0;
    int add_count = 0, delete_count = 0, change_count = 0;
    
    cout << "Running operations with full integrity check after each step..." << endl;
    cout << "Seed: 12345, Key range: 0-9999" << endl << endl;
    
    cout << "Starting test..." << flush;
    
    for (int op = 0; op < 50000; op++)
    {
        int op_type = op_dist(rng);
        int key = key_dist(rng);
        
        if (op % 500 == 0)
            cout << "." << flush;
        
        if (op_type == 0)  // Add
        {
            if (present_keys.find(key) == present_keys.end())
            {
                string value = "v" + to_string(key);
                bool result = bmap.Add(key, value);
                
                if (result)
                {
                    present_keys.insert(key);
                    add_count++;
                    op_count++;
                    
                    if (!VerifyIntegrity(bmap, "Add(" + to_string(key) + ", \"" + value + "\")", op_count))
                    {
                        cout << "\n🛑 Integrity check failed after Add operation!" << endl;
                        cout << "Operation #" << op_count << " (total ops: " << op << ")" << endl;
                        cout << "Adds: " << add_count << ", Deletes: " << delete_count << ", Changes: " << change_count << endl;
                        bmap.DebugDump("State at failure");
                        return 1;
                    }
                    
                    if (op_count % 1000 == 0)
                        cout << "✓ [" << op_count << "] ops completed, size=" << bmap.GetCount() << endl;
                }
            }
        }
        else if (op_type == 1)  // Delete
        {
            if (present_keys.find(key) != present_keys.end())
            {
                bool result = bmap.DeleteByKey(key);
                
                if (result)
                {
                    present_keys.erase(key);
                    delete_count++;
                    op_count++;
                    
                    if (!VerifyIntegrity(bmap, "DeleteByKey(" + to_string(key) + ")", op_count))
                    {
                        cout << "\n🛑 Integrity check failed after Delete operation!" << endl;
                        cout << "Operation #" << op_count << " (total ops: " << op << ")" << endl;
                        cout << "Adds: " << add_count << ", Deletes: " << delete_count << ", Changes: " << change_count << endl;
                        bmap.DebugDump("State at failure");
                        return 1;
                    }
                    
                    if (op_count % 1000 == 0)
                        cout << "✓ [" << op_count << "] ops completed, size=" << bmap.GetCount() << endl;
                }
            }
        }
        else  // Change
        {
            if (present_keys.find(key) != present_keys.end())
            {
                string new_value = "modified_" + to_string(key);
                bool result = bmap.Change(key, new_value);
                
                if (result)
                {
                    change_count++;
                    op_count++;
                    
                    if (!VerifyIntegrity(bmap, "Change(" + to_string(key) + ", \"" + new_value + "\")", op_count))
                    {
                        cout << "\n🛑 Integrity check failed after Change operation!" << endl;
                        cout << "Operation #" << op_count << " (total ops: " << op << ")" << endl;
                        cout << "Adds: " << add_count << ", Deletes: " << delete_count << ", Changes: " << change_count << endl;
                        bmap.DebugDump("State at failure");
                        return 1;
                    }
                    
                    if (op_count % 1000 == 0)
                        cout << "✓ [" << op_count << "] ops completed, size=" << bmap.GetCount() << endl;
                }
            }
        }
    }
    
    cout << "\n========================================" << endl;
    cout << "✅ ALL OPERATIONS COMPLETED SUCCESSFULLY!" << endl;
    cout << "========================================" << endl;
    cout << "Total operations: " << op_count << endl;
    cout << "  Adds: " << add_count << endl;
    cout << "  Deletes: " << delete_count << endl;
    cout << "  Changes: " << change_count << endl;
    cout << "Final size: " << bmap.GetCount() << endl;
    cout << "Reference size: " << present_keys.size() << endl;
    
    if (bmap.GetCount() != (int)present_keys.size())
    {
        cout << "\n❌ FINAL SIZE MISMATCH!" << endl;
        return 1;
    }
    
    cout << "\n✅ All integrity checks passed!" << endl;
    
    return 0;
}
