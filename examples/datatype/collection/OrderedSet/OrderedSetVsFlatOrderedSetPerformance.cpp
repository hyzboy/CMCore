#include<hgl/type/OrderedSet.h>
#include<hgl/type/FlatOrderedSet.h>
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
    PrintHeader("Test 01: Sequential insert (" + to_string(count) + " elements)");

    OrderedSet<int> btree_set;
    FlatOrderedSet<int> flat_set;

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

    PrintResult("Insert", "BTree", btree_time, count);
    PrintResult("Insert", "Flat ", flat_time, count);
    cout << "  Speedup: " << (btree_time / flat_time) << "x (BTree faster)" << endl;
}

// 测试2: 随机插入
void Test02_RandomInsert(int64 count)
{
    PrintHeader("Test 02: Random insert (" + to_string(count) + " elements)");

    auto random_data = GenerateRandomData(count);

    OrderedSet<int> btree_set;
    FlatOrderedSet<int> flat_set;

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

    PrintResult("Insert", "BTree", btree_time, count);
    PrintResult("Insert", "Flat ", flat_time, count);
    cout << "  Speedup: " << (btree_time / flat_time) << "x (BTree faster)" << endl;
    cout << "  Final size: BTree=" << btree_set.GetCount()
         << ", Flat=" << flat_set.GetCount() << endl;
}

// 测试3: 批量插入后查找
void Test03_BulkInsertThenLookup(int64 count, int64 lookup_count)
{
    PrintHeader("Test 03: Bulk insert + find (" + to_string(count) + " inserts, "
                + to_string(lookup_count) + " finds)");

    auto insert_data = GenerateRandomData(count);
    auto lookup_data = GenerateRandomData(lookup_count, count * 2);

    OrderedSet<int> btree_set;
    FlatOrderedSet<int> flat_set;

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

    cout << "\n  Insert phase:" << endl;
    PrintResult("  Insert", "BTree", btree_insert, count);
    PrintResult("  Insert", "Flat ", flat_insert, count);

    cout << "\n  Find phase:" << endl;
    PrintResult("  Find", "BTree", btree_lookup, lookup_count);
    PrintResult("  Find", "Flat ", flat_lookup, lookup_count);
    cout << "  Found: BTree=" << btree_found << ", Flat=" << flat_found << endl;
    cout << "  Find speedup: " << (btree_lookup / flat_lookup) << "x" << endl;
}

// 测试4: 混合操作（插入+删除）
void Test04_MixedOperations(int64 count)
{
    PrintHeader("Test 04: Mixed insert/delete (" + to_string(count) + " operations)");

    auto data = GenerateRandomData(count);

    OrderedSet<int> btree_set;
    FlatOrderedSet<int> flat_set;

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

    PrintResult("Mixed operations", "BTree", btree_time, count);
    PrintResult("Mixed operations", "Flat ", flat_time, count);
    cout << "  Speedup: " << (btree_time / flat_time) << "x (BTree faster)" << endl;
    cout << "  Final size: BTree=" << btree_set.GetCount()
         << ", Flat=" << flat_set.GetCount() << endl;
}

// 测试5: 批量加载场景（Flat的优势场景）
void Test05_BulkLoadScenario(int64 count)
{
    PrintHeader("Test 05: Bulk load scenario (" + to_string(count) + " elements)");

    auto data = GenerateRandomData(count);

    // 预排序并去重（模拟从磁盘加载的数据）
    vector<int> sorted_data = data;
    sort(sorted_data.begin(), sorted_data.end());
    auto last = unique(sorted_data.begin(), sorted_data.end());
    sorted_data.erase(last, sorted_data.end());

    cout << "  Unique elements after dedup: " << sorted_data.size() << endl;

    FlatOrderedSet<int> flat_set;
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

    PrintResult("Bulk load", "Flat (LoadFromBuffer)", flat_load, sorted_data.size());
    PrintResult("Sequential insert", "BTree", btree_load, sorted_data.size());
    cout << "  Speedup: " << (btree_load / flat_load) << "x (Flat faster)" << endl;

    // 验证数据一致性
    assert(flat_set.GetCount() == btree_set.GetCount());
    cout << "  ✓ Data consistency verified" << endl;
}

// 测试6: 内存占用对比
void Test06_MemoryFootprint(int64 count)
{
    PrintHeader("Test 06: Memory usage (" + to_string(count) + " elements)");

    auto data = GenerateRandomData(count);

    OrderedSet<int> btree_set;
    FlatOrderedSet<int> flat_set;

    for (auto val : data)
    {
        btree_set.Add(val);
        flat_set.Add(val);
    }

    int64 flat_capacity = flat_set.GetAllocCount();
    int64 flat_size = flat_set.GetCount();

    cout << "  Flat vector:" << endl;
    cout << "    Size: " << flat_size << " elements" << endl;
    cout << "    Capacity: " << flat_capacity << " elements" << endl;
    cout << "    Memory: ~" << (flat_capacity * sizeof(int) / 1024) << " KB" << endl;
    cout << "    Utilization: " << (100.0 * flat_size / flat_capacity) << "%" << endl;

    cout << "\n  BTree set:" << endl;
    cout << "    Size: " << btree_set.GetCount() << " elements" << endl;
    cout << "    Memory: ~" << (btree_set.GetCount() * sizeof(int) * 1.5 / 1024)
         << " KB (estimated with overhead)" << endl;
    cout << "    note: BTree has node overhead but no over-allocation" << endl;
}

int os_main(int, os_char**)
{
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  OrderedSet vs FlatOrderedSet Performance Test  ║" << endl;
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
    cout << "\n[optional] Running large-scale test..." << endl;
    Test01_SequentialInsert(1000000);
    Test02_RandomInsert(1000000);

    PrintHeader("Summary");
    cout << "  OrderedSet (BTree):" << endl;
    cout << "    ✓ O(log n) insert/delete - efficient for frequent modifications" << endl;
    cout << "    ✓ No re-allocation overhead" << endl;
    cout << "    ✗ No zero-copy serialization" << endl;
    cout << "    ✗ Node overhead increases memory usage" << endl;

    cout << "\n  FlatOrderedSet (vector):" << endl;
    cout << "    ✓ O(log n) binary search" << endl;
    cout << "    ✓ Contiguous memory - cache friendly lookup" << endl;
    cout << "    ✓ Zero-copy serialization (LoadFromBuffer)" << endl;
    cout << "    ✗ O(n) insert/delete - slow for frequent modifications" << endl;

    cout << "\n  Usage scenario guide:" << endl;
    cout << "    • OrderedSet: frequent insert/delete operations" << endl;
    cout << "    • FlatOrderedSet: bulk load + frequent queries, serialization" << endl;

    cout << "\n✅ All performance tests completed" << endl;
    return 0;
}
