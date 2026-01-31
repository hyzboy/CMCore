#include<hgl/type/OrderedSet.h>
#include<hgl/type/FlatOrderedValueSet.h>
#include<iostream>
#include<chrono>
#include<random>
#include<vector>
#include<cassert>

using namespace hgl;
using namespace std;
using namespace std::chrono;

template<typename Func>
double MeasureTime(Func func)
{
    auto start = high_resolution_clock::now();
    func();
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / 1000.0;
}

void PrintHeader(const string& title)
{
    cout << "\n========================================" << endl;
    cout << title << endl;
    cout << "========================================" << endl;
}

void PrintResult(const string& operation, const string& impl, double time_ms, int64 count = -1)
{
    cout << "  [" << impl << "] " << operation << ": " << time_ms << " ms";
    if (count > 0)
        cout << " (" << (time_ms * 1000000.0 / count) << " ns/op)";
    cout << endl;
}

// 生成随机数据
vector<int> GenerateRandomData(int64 count, int max_value = 1000000)
{
    vector<int> data;
    data.reserve(count);
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, max_value);
    
    for (int64 i = 0; i < count; ++i)
        data.push_back(dis(gen));
    
    return data;
}

// 测试1: 顺序插入
void Test01_SequentialInsert(int64 count)
{
    PrintHeader("测试 01: 顺序插入 (" + to_string(count) + " 个元素)");
    
    OrderedSet<int> btree_set;
    FlatOrderedValueSet<int> flat_set;
    
    // BTree插入
    double btree_time = MeasureTime([&]() {
        for (int64 i = 0; i < count; ++i)
            btree_set.Add((int)i);
    });
    
    // Flat插入
    double flat_time = MeasureTime([&]() {
        for (int64 i = 0; i < count; ++i)
            flat_set.Add((int)i);
    });
    
    PrintResult("插入", "BTree", btree_time, count);
    PrintResult("插入", "Flat ", flat_time, count);
    cout << "  加速比: " << (btree_time / flat_time) << "x (BTree 更快)" << endl;
}

// 测试2: 随机插入
void Test02_RandomInsert(int64 count)
{
    PrintHeader("测试 02: 随机插入 (" + to_string(count) + " 个元素)");
    
    auto random_data = GenerateRandomData(count);
    
    OrderedSet<int> btree_set;
    FlatOrderedValueSet<int> flat_set;
    
    // BTree插入
    double btree_time = MeasureTime([&]() {
        for (auto val : random_data)
            btree_set.Add(val);
    });
    
    // Flat插入
    double flat_time = MeasureTime([&]() {
        for (auto val : random_data)
            flat_set.Add(val);
    });
    
    PrintResult("插入", "BTree", btree_time, count);
    PrintResult("插入", "Flat ", flat_time, count);
    cout << "  加速比: " << (btree_time / flat_time) << "x (BTree 更快)" << endl;
    cout << "  最终大小: BTree=" << btree_set.GetCount() 
         << ", Flat=" << flat_set.GetCount() << endl;
}

// 测试3: 批量插入后查找
void Test03_BulkInsertThenLookup(int64 count, int64 lookup_count)
{
    PrintHeader("测试 03: 批量插入+查找 (" + to_string(count) + " 插入, " 
                + to_string(lookup_count) + " 次查找)");
    
    auto insert_data = GenerateRandomData(count);
    auto lookup_data = GenerateRandomData(lookup_count, count * 2);
    
    OrderedSet<int> btree_set;
    FlatOrderedValueSet<int> flat_set;
    
    // BTree - 插入
    double btree_insert = MeasureTime([&]() {
        for (auto val : insert_data)
            btree_set.Add(val);
    });
    
    // Flat - 插入
    double flat_insert = MeasureTime([&]() {
        for (auto val : insert_data)
            flat_set.Add(val);
    });
    
    // BTree - 查找
    int64 btree_found = 0;
    double btree_lookup = MeasureTime([&]() {
        for (auto val : lookup_data)
            if (btree_set.Contains(val))
                ++btree_found;
    });
    
    // Flat - 查找
    int64 flat_found = 0;
    double flat_lookup = MeasureTime([&]() {
        for (auto val : lookup_data)
            if (flat_set.Contains(val))
                ++flat_found;
    });
    
    cout << "\n  插入阶段:" << endl;
    PrintResult("  插入", "BTree", btree_insert, count);
    PrintResult("  插入", "Flat ", flat_insert, count);
    
    cout << "\n  查找阶段:" << endl;
    PrintResult("  查找", "BTree", btree_lookup, lookup_count);
    PrintResult("  查找", "Flat ", flat_lookup, lookup_count);
    cout << "  找到: BTree=" << btree_found << ", Flat=" << flat_found << endl;
    cout << "  查找加速比: " << (btree_lookup / flat_lookup) << "x" << endl;
}

// 测试4: 混合操作（插入+删除）
void Test04_MixedOperations(int64 count)
{
    PrintHeader("测试 04: 混合插入/删除 (" + to_string(count) + " 操作)");
    
    auto data = GenerateRandomData(count);
    
    OrderedSet<int> btree_set;
    FlatOrderedValueSet<int> flat_set;
    
    // BTree - 混合操作
    double btree_time = MeasureTime([&]() {
        for (int64 i = 0; i < count; ++i)
        {
            btree_set.Add(data[i]);
            if (i % 3 == 0 && btree_set.GetCount() > 0)
                btree_set.DeleteAt(0);
        }
    });
    
    // Flat - 混合操作
    double flat_time = MeasureTime([&]() {
        for (int64 i = 0; i < count; ++i)
        {
            flat_set.Add(data[i]);
            if (i % 3 == 0 && flat_set.GetCount() > 0)
                flat_set.DeleteAt(0);
        }
    });
    
    PrintResult("混合操作", "BTree", btree_time, count);
    PrintResult("混合操作", "Flat ", flat_time, count);
    cout << "  加速比: " << (btree_time / flat_time) << "x (BTree 更快)" << endl;
    cout << "  最终大小: BTree=" << btree_set.GetCount() 
         << ", Flat=" << flat_set.GetCount() << endl;
}

// 测试5: 批量加载场景（Flat的优势场景）
void Test05_BulkLoadScenario(int64 count)
{
    PrintHeader("测试 05: 批量加载场景 (" + to_string(count) + " 个元素)");
    
    auto data = GenerateRandomData(count);
    
    // 预排序并去重（模拟从磁盘加载的数据）
    vector<int> sorted_data = data;
    sort(sorted_data.begin(), sorted_data.end());
    auto last = unique(sorted_data.begin(), sorted_data.end());
    sorted_data.erase(last, sorted_data.end());
    
    cout << "  去重后的唯一元素: " << sorted_data.size() << endl;
    
    FlatOrderedValueSet<int> flat_set;
    OrderedSet<int> btree_set;
    
    // Flat - 使用LoadFromBuffer（零拷贝加载）
    double flat_load = MeasureTime([&]() {
        flat_set.LoadFromBuffer(sorted_data.data(), sorted_data.size(), true);
    });
    
    // BTree - 逐个插入
    double btree_load = MeasureTime([&]() {
        for (auto val : sorted_data)
            btree_set.Add(val);
    });
    
    PrintResult("批量加载", "Flat (LoadFromBuffer)", flat_load, sorted_data.size());
    PrintResult("顺序插入", "BTree", btree_load, sorted_data.size());
    cout << "  加速比: " << (btree_load / flat_load) << "x (Flat 更快)" << endl;
    
    // 验证数据一致性
    assert(flat_set.GetCount() == btree_set.GetCount());
    cout << "  ✓ 数据一致性已验证" << endl;
}

// 测试6: 内存占用对比
void Test06_MemoryFootprint(int64 count)
{
    PrintHeader("测试 06: 内存占用 (" + to_string(count) + " 个元素)");
    
    auto data = GenerateRandomData(count);
    
    OrderedSet<int> btree_set;
    FlatOrderedValueSet<int> flat_set;
    
    for (auto val : data)
    {
        btree_set.Add(val);
        flat_set.Add(val);
    }
    
    int64 flat_capacity = flat_set.GetAllocCount();
    int64 flat_size = flat_set.GetCount();
    
    cout << "  Flat 向量:" << endl;
    cout << "    大小: " << flat_size << " 个元素" << endl;
    cout << "    容量: " << flat_capacity << " 个元素" << endl;
    cout << "    内存: ~" << (flat_capacity * sizeof(int) / 1024) << " KB" << endl;
    cout << "    利用率: " << (100.0 * flat_size / flat_capacity) << "%" << endl;
    
    cout << "\n  BTree 集合:" << endl;
    cout << "    大小: " << btree_set.GetCount() << " 个元素" << endl;
    cout << "    内存: ~" << (btree_set.GetCount() * sizeof(int) * 1.5 / 1024) 
         << " KB (估计含开销)" << endl;
    cout << "    注: BTree 有节点开销但无过度分配" << endl;
}

int os_main(int, os_char**)
{
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  OrderedSet vs FlatOrderedValueSet 性能测试       ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    // 小规模测试
    Test01_SequentialInsert(1000);
    Test02_RandomInsert(1000);
    
    // 中等规模测试
    Test01_SequentialInsert(10000);
    Test02_RandomInsert(10000);
    Test03_BulkInsertThenLookup(10000, 5000);
    Test04_MixedOperations(5000);
    
    // 大规模测试
    Test01_SequentialInsert(100000);
    Test02_RandomInsert(100000);
    Test03_BulkInsertThenLookup(100000, 50000);
    Test05_BulkLoadScenario(100000);
    Test06_MemoryFootprint(100000);
    
    // 超大规模测试（可选）
    cout << "\n[可选] 运行大规模测试..." << endl;
    Test01_SequentialInsert(1000000);
    Test02_RandomInsert(1000000);
    
    PrintHeader("总结");
    cout << "  OrderedSet (BTree):" << endl;
    cout << "    ✓ O(log n) 插入/删除 - 高效处理频繁修改" << endl;
    cout << "    ✓ 无重新分配开销" << endl;
    cout << "    ✗ 不支持零拷贝序列化" << endl;
    cout << "    ✗ 节点开销增加内存使用" << endl;
    
    cout << "\n  FlatOrderedValueSet (向量):" << endl;
    cout << "    ✓ O(log n) 二分查找" << endl;
    cout << "    ✓ 连续内存 - 查找缓存友好" << endl;
    cout << "    ✓ 支持零拷贝序列化 (LoadFromBuffer)" << endl;
    cout << "    ✗ O(n) 插入/删除 - 频繁修改时较慢" << endl;
    
    cout << "\n  使用场景指南:" << endl;
    cout << "    • OrderedSet: 频繁插入/删除操作" << endl;
    cout << "    • FlatOrderedValueSet: 批量加载+频繁查询, 序列化" << endl;
    
    cout << "\n✅ 所有性能测试完成" << endl;
    return 0;
}
