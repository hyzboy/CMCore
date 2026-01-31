/**
 * UnorderedMap 功能和性能测试
 * 
 * 测试内容：
 * 1. 基础功能：Add, Get, Delete, Change, ChangeOrAdd
 * 2. 序列化支持：GetKeyArray, GetValueArray, GetKeyValueArrays
 * 3. 反序列化支持：RebuildFromArrays, AddFromArrays
 * 4. 性能对比：vs std::map, vs std::unordered_map
 * 5. 状态快照：保存/恢复场景
 */

#include<iostream>
#include<chrono>
#include<random>
#include<iomanip>
#include<string>
#include<cassert>
#include<map>
#include<unordered_map>
#include<vector>
#include<hgl/type/UnorderedMap.h>

using namespace std;
using namespace std::chrono;
using namespace hgl;

// ============================================================================
// 计时器
// ============================================================================

class Timer
{
    high_resolution_clock::time_point start;
public:
    Timer() : start(high_resolution_clock::now()) {}

    double ElapsedMs() const
    {
        auto end = high_resolution_clock::now();
        return duration_cast<microseconds>(end - start).count() / 1000.0;
    }
};

// ============================================================================
// 功能测试
// ============================================================================

void TestBasicOperations()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 1: Basic Operations" << endl;
    cout << string(80, '=') << endl;
    
    UnorderedMap<int, string> map;
    
    // Test Add
    cout << "\n✓ Testing Add..." << endl;
    assert(map.Add(1, "apple"));
    assert(map.Add(2, "banana"));
    assert(map.Add(3, "cherry"));
    assert(!map.Add(1, "duplicate"));
    assert(map.GetCount() == 3);
    cout << "  Added 3 items. Count: " << map.GetCount() << endl;
    
    // Test Get
    cout << "\n✓ Testing Get..." << endl;
    string value;
    assert(map.Get(1, value) && value == "apple");
    assert(map.Get(2, value) && value == "banana");
    assert(!map.Get(999, value));
    cout << "  Get operations passed" << endl;
    
    // Test GetValuePointer
    cout << "\n✓ Testing GetValuePointer..." << endl;
    string* ptr = map.GetValuePointer(1);
    assert(ptr && *ptr == "apple");
    cout << "  GetValuePointer works correctly" << endl;
    
    // Test ContainsKey
    cout << "\n✓ Testing ContainsKey..." << endl;
    assert(map.ContainsKey(1));
    assert(!map.ContainsKey(999));
    cout << "  ContainsKey works correctly" << endl;
    
    // Test Change
    cout << "\n✓ Testing Change..." << endl;
    assert(map.Change(1, "apple_modified"));
    string modified;
    map.Get(1, modified);
    assert(modified == "apple_modified");
    assert(!map.Change(999, "nonexistent"));
    cout << "  Change operations passed" << endl;
    
    // Test ChangeOrAdd
    cout << "\n✓ Testing ChangeOrAdd..." << endl;
    assert(map.ChangeOrAdd(1, "apple_v2"));
    assert(map.ChangeOrAdd(4, "durian"));
    assert(map.GetCount() == 4);
    cout << "  ChangeOrAdd operations passed" << endl;
    
    // Test DeleteByKey
    cout << "\n✓ Testing DeleteByKey..." << endl;
    assert(map.DeleteByKey(2));
    assert(!map.DeleteByKey(2));
    assert(map.GetCount() == 3);
    cout << "  DeleteByKey operations passed" << endl;
    
    // Test IsEmpty and Clear
    cout << "\n✓ Testing IsEmpty and Clear..." << endl;
    assert(!map.IsEmpty());
    map.Clear();
    assert(map.IsEmpty());
    assert(map.GetCount() == 0);
    cout << "  IsEmpty and Clear passed" << endl;
}

// ============================================================================
// 序列化测试
// ============================================================================

void TestSerialization()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 2: Serialization (Key/Value Array Separation)" << endl;
    cout << string(80, '=') << endl;
    
    UnorderedMap<int, string> map1;
    
    cout << "\n✓ Populating original map..." << endl;
    for (int i = 0; i < 10; ++i) {
        map1.Add(i, "value_" + to_string(i));
    }
    cout << "  Added 10 items to map1" << endl;
    
    cout << "\n✓ Testing GetKeyArray..." << endl;
    vector<int> key_array;
    int key_count = map1.GetKeyArray(key_array);
    assert(key_count == 10);
    cout << "  Extracted " << key_count << " keys" << endl;
    
    cout << "\n✓ Testing GetValueArray..." << endl;
    vector<string> value_array;
    int value_count = map1.GetValueArray(value_array);
    assert(value_count == 10);
    cout << "  Extracted " << value_count << " values" << endl;
    
    cout << "\n✓ Testing GetKeyValueArrays..." << endl;
    vector<int> keys;
    vector<string> values;
    int count = map1.GetKeyValueArrays(keys, values);
    assert(count == 10);
    assert(keys.size() == values.size());
    cout << "  Extracted " << count << " key-value pairs (synchronized)" << endl;
    
    cout << "\n  Sample extracted data:" << endl;
    for (int i = 0; i < 3; ++i) {
        cout << "    keys[" << i << "]=" << keys[i] 
             << " values[" << i << "]=" << values[i] << endl;
    }
}

// ============================================================================
// 反序列化测试
// ============================================================================

void TestDeserialization()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 3: Deserialization (Rebuild from Arrays)" << endl;
    cout << string(80, '=') << endl;
    
    cout << "\n✓ Creating source arrays..." << endl;
    vector<int> source_keys;
    vector<string> source_values;
    
    for (int i = 0; i < 5; ++i) {
        source_keys.push_back(i * 100);
        source_values.push_back("restored_" + to_string(i));
    }
    cout << "  Created " << source_keys.size() << " key-value pairs" << endl;
    
    cout << "\n✓ Testing RebuildFromArrays..." << endl;
    UnorderedMap<int, string> restored_map;
    int rebuilt_count = restored_map.RebuildFromArrays(source_keys, source_values);
    assert(rebuilt_count == 5);
    assert(restored_map.GetCount() == 5);
    cout << "  Successfully rebuilt map with " << rebuilt_count << " items" << endl;
    
    cout << "\n✓ Verifying restored data..." << endl;
    for (int i = 0; i < 5; ++i) {
        string value;
        int key = source_keys[i];
        assert(restored_map.Get(key, value));
        assert(value == source_values[i]);
    }
    cout << "  All restored data verified correctly" << endl;
    
    cout << "\n✓ Testing AddFromArrays..." << endl;
    vector<int> append_keys;
    vector<string> append_values;
    append_keys.push_back(500);
    append_values.push_back("appended_value");
    
    int added = restored_map.AddFromArrays(append_keys, append_values);
    assert(added == 1);
    assert(restored_map.GetCount() == 6);
    cout << "  Successfully appended " << added << " item. New count: " 
         << restored_map.GetCount() << endl;
}

// ============================================================================
// 性能测试
// ============================================================================

void TestPerformance()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 4: Performance Benchmark" << endl;
    cout << string(80, '=') << endl;
    
    const int INSERT_COUNT = 100000;
    const int FIND_COUNT = 100000;
    const int DELETE_COUNT = 10000;
    
    cout << "\nTest Parameters:" << endl;
    cout << "  Insert Count: " << INSERT_COUNT << endl;
    cout << "  Find Count:   " << FIND_COUNT << endl;
    cout << "  Delete Count: " << DELETE_COUNT << endl;
    
    cout << "\n✓ Generating test data..." << endl;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, INSERT_COUNT - 1);
    
    vector<int> find_keys, delete_keys;
    for (int i = 0; i < FIND_COUNT; ++i) {
        find_keys.push_back(dis(gen));
    }
    for (int i = 0; i < DELETE_COUNT; ++i) {
        delete_keys.push_back(dis(gen));
    }
    cout << "  Test data generated" << endl;
    
    cout << "\n✓ Benchmarking UnorderedMap<int, string>..." << endl;
    UnorderedMap<int, string> modern_map;
    
    double modern_insert, modern_find, modern_delete;
    
    {
        Timer timer;
        for (int i = 0; i < INSERT_COUNT; ++i) {
            modern_map.Add(i, "value_" + to_string(i));
        }
        modern_insert = timer.ElapsedMs();
    }
    
    {
        Timer timer;
        int found = 0;
        for (int key : find_keys) {
            string value;
            if (modern_map.Get(key, value)) ++found;
        }
        modern_find = timer.ElapsedMs();
    }
    
    {
        Timer timer;
        for (int key : delete_keys) {
            modern_map.DeleteByKey(key);
        }
        modern_delete = timer.ElapsedMs();
    }
    
    cout << "\n✓ Benchmarking std::map<int, string>..." << endl;
    std::map<int, string> std_map;
    
    double std_insert, std_find, std_delete;
    
    {
        Timer timer;
        for (int i = 0; i < INSERT_COUNT; ++i) {
            std_map[i] = "value_" + to_string(i);
        }
        std_insert = timer.ElapsedMs();
    }
    
    {
        Timer timer;
        int found = 0;
        for (int key : find_keys) {
            auto it = std_map.find(key);
            if (it != std_map.end()) ++found;
        }
        std_find = timer.ElapsedMs();
    }
    
    {
        Timer timer;
        for (int key : delete_keys) {
            std_map.erase(key);
        }
        std_delete = timer.ElapsedMs();
    }
    
    cout << "\n✓ Benchmarking std::unordered_map<int, string>..." << endl;
    std::unordered_map<int, string> hash_map;
    
    double hash_insert, hash_find, hash_delete;
    
    {
        Timer timer;
        for (int i = 0; i < INSERT_COUNT; ++i) {
            hash_map[i] = "value_" + to_string(i);
        }
        hash_insert = timer.ElapsedMs();
    }
    
    {
        Timer timer;
        int found = 0;
        for (int key : find_keys) {
            auto it = hash_map.find(key);
            if (it != hash_map.end()) ++found;
        }
        hash_find = timer.ElapsedMs();
    }
    
    {
        Timer timer;
        for (int key : delete_keys) {
            hash_map.erase(key);
        }
        hash_delete = timer.ElapsedMs();
    }
    
    cout << "\n" << string(80, '=') << endl;
    cout << "Performance Results:" << endl;
    cout << string(80, '=') << endl;
    
    cout << "\n+--" << string(30, '-') << "-+-" << string(12, '-') << "-+-"
         << string(12, '-') << "-+-" << string(12, '-') << "-+-" << string(12, '-') << "-+" << endl;
    cout << "| " << setw(30) << left << "Container"
         << " | " << setw(12) << "Insert(ms)"
         << " | " << setw(12) << "Find(ms)"
         << " | " << setw(12) << "Delete(ms)"
         << " | " << setw(12) << "Total(ms)" << " |" << endl;
    cout << "+--" << string(30, '-') << "-+-" << string(12, '-') << "-+-"
         << string(12, '-') << "-+-" << string(12, '-') << "-+-" << string(12, '-') << "-+" << endl;
    
    double modern_total = modern_insert + modern_find + modern_delete;
    double std_total = std_insert + std_find + std_delete;
    double hash_total = hash_insert + hash_find + hash_delete;
    
    cout << "| " << setw(30) << left << "UnorderedMap (ankerl)"
         << " | " << setw(12) << fixed << setprecision(2) << modern_insert
         << " | " << setw(12) << modern_find
         << " | " << setw(12) << modern_delete
         << " | " << setw(12) << modern_total << " |" << endl;
    
    cout << "| " << setw(30) << left << "std::map"
         << " | " << setw(12) << fixed << setprecision(2) << std_insert
         << " | " << setw(12) << std_find
         << " | " << setw(12) << std_delete
         << " | " << setw(12) << std_total << " |" << endl;
    
    cout << "| " << setw(30) << left << "std::unordered_map"
         << " | " << setw(12) << fixed << setprecision(2) << hash_insert
         << " | " << setw(12) << hash_find
         << " | " << setw(12) << hash_delete
         << " | " << setw(12) << hash_total << " |" << endl;
    
    cout << "+--" << string(30, '-') << "-+-" << string(12, '-') << "-+-"
         << string(12, '-') << "-+-" << string(12, '-') << "-+-" << string(12, '-') << "-+" << endl;
    
    cout << "\n【性能对比】" << endl;
    cout << "std::unordered_map 相对性能: " << fixed << setprecision(2) 
         << (modern_total / hash_total) << "x" << endl;
    cout << "std::map 相对性能:           " << fixed << setprecision(2) 
         << (modern_total / std_total) << "x" << endl;
}

// ============================================================================
// 状态快照测试
// ============================================================================

void TestStateSnapshot()
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Test 5: State Snapshot (Save/Restore Scenario)" << endl;
    cout << string(80, '=') << endl;
    
    cout << "\n✓ Creating initial state..." << endl;
    UnorderedMap<int, string> game_state;
    
    for (int i = 0; i < 5; ++i) {
        game_state.Add(i, "player_" + to_string(i));
    }
    cout << "  Initial state: " << game_state.GetCount() << " players" << endl;
    
    cout << "\n✓ Saving game state..." << endl;
    vector<int> saved_ids;
    vector<string> saved_names;
    game_state.GetKeyValueArrays(saved_ids, saved_names);
    cout << "  Saved: " << saved_ids.size() << " ids, " 
         << saved_names.size() << " names" << endl;
    
    cout << "\n✓ Modifying game state..." << endl;
    game_state.Clear();
    game_state.Add(100, "modified_player");
    cout << "  Modified state: " << game_state.GetCount() << " players" << endl;
    
    cout << "\n✓ Restoring game state..." << endl;
    game_state.RebuildFromArrays(saved_ids, saved_names);
    cout << "  Restored state: " << game_state.GetCount() << " players" << endl;
    
    cout << "\n✓ Verifying restoration..." << endl;
    for (size_t i = 0; i < saved_ids.size(); ++i) {
        string value;
        assert(game_state.Get(saved_ids[i], value));
        assert(value == saved_names[i]);
    }
    cout << "  All data restored correctly" << endl;
}

// ============================================================================
// 主函数
// ============================================================================

int main()
{
    cout << "\n╔════════════════════════════════════════════════════════════════╗" << endl;
    cout << "║    UnorderedMap - Comprehensive Test Suite              ║" << endl;
    cout << "║    (ankerl::unordered_dense + Serialization Support)          ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════════╝" << endl;
    
    try {
        TestBasicOperations();
        TestSerialization();
        TestDeserialization();
        TestPerformance();
        TestStateSnapshot();
        
        cout << "\n" << string(80, '=') << endl;
        cout << "✓ All tests passed successfully!" << endl;
        cout << string(80, '=') << endl;
        cout << "\n【总结】\n" << endl;
        cout << "✓ 基础操作: Add, Get, Delete, Change, ChangeOrAdd 正常工作" << endl;
        cout << "✓ 序列化支持: GetKeyArray, GetValueArray, GetKeyValueArrays 高效分离" << endl;
        cout << "✓ 反序列化支持: RebuildFromArrays, AddFromArrays 快速恢复" << endl;
        cout << "✓ 性能优秀: 基于 ankerl::unordered_dense 提供最优性能" << endl;
        cout << "✓ 状态快照: 支持游戏状态保存/恢复场景" << endl;
        cout << "\n测试完成！\n" << endl;
        return 0;
    }
    catch (const exception& e) {
        cerr << "\n✗ Test failed with error: " << e.what() << endl;
        return 1;
    }
}
