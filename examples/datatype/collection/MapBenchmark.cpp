#include<iostream>
#include<chrono>
#include<random>
#include<vector>
#include<unordered_map>
#include<map>
#include<hgl/type/ValueKVMap.h>
#include<absl/container/flat_hash_map.h>
#include<absl/container/node_hash_map.h>
#include<absl/container/btree_map.h>

using namespace std;
using namespace std::chrono;
using namespace hgl;

// ============================================================================
// 测试数据结构
// ============================================================================

struct TestObject
{
    char data[1024];  // 1KB 对象，模拟实际中的大数据

    TestObject() { memset(data, 0, sizeof(data)); }
    TestObject(int val) { memset(data, val % 256, sizeof(data)); }
    
    bool operator==(const TestObject& other) const
    {
        return memcmp(data, other.data, sizeof(data)) == 0;
    }
};

// ============================================================================
// 计时器
// ============================================================================

class Timer
{
    high_resolution_clock::time_point start;
public:
    Timer() { start = high_resolution_clock::now(); }

    double ElapsedMs() const
    {
        auto end = high_resolution_clock::now();
        return duration_cast<duration<double, milli>>(end - start).count();
    }
};

// ============================================================================
// 基准测试框架
// ============================================================================

struct BenchmarkResult
{
    string name;
    double insert_time;
    double find_time;
    double erase_time;
    double total_time;
    double cold_find_time;  // 新增：冷缓存查找时间
    size_t memory_used;

    void Print() const
    {
        cout << "\n" << name << " Results:" << endl;
        cout << "  Insert:     " << insert_time << " ms" << endl;
        cout << "  Find:       " << find_time << " ms" << endl;
        if (cold_find_time > 0)
            cout << "  Cold Find:  " << cold_find_time << " ms" << endl;
        cout << "  Erase:      " << erase_time << " ms" << endl;
        cout << "  Total:      " << total_time << " ms" << endl;
        cout << "  Memory:     " << (memory_used / 1024.0 / 1024.0) << " MB" << endl;
    }
};

// ============================================================================
// 测试 std::unordered_map<const TestObject*, int>
// ============================================================================

BenchmarkResult BenchmarkUnorderedMap(const vector<TestObject> &objects,
                                      const vector<int> &find_indices,
                                      const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "std::unordered_map";

    unordered_map<const TestObject*, int> map;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map[&objects[i]] = (int)i;
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            auto it = map.find(&objects[idx]);
            if (it != map.end())
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map.erase(&objects[idx]);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.size() * 16 + 1024 * objects.size();  // 粗估

    return result;
}

// ============================================================================
// 测试 std::map<const TestObject*, int>
// ============================================================================

BenchmarkResult BenchmarkMap(const vector<TestObject> &objects,
                             const vector<int> &find_indices,
                             const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "std::map";

    map<const TestObject*, int> map_obj;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map_obj[&objects[i]] = (int)i;
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            auto it = map_obj.find(&objects[idx]);
            if (it != map_obj.end())
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map_obj.erase(&objects[idx]);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map_obj.size() * 40 + 1024 * objects.size();  // 粗估（平衡树节点更大）

    return result;
}

// ============================================================================
// 测试 ValueKVMap<const TestObject*, int> - 已禁用
// 原因：KeyValue<K,V> 不支持 operator==，无法用于 ValueArray 的 Find/Delete
// ============================================================================
/*
BenchmarkResult BenchmarkValueKVMap(const vector<TestObject> &objects,
                                    const vector<int> &find_indices,
                                    const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "ValueKVMap";

    ValueKVMap<const TestObject*, int> map;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map.Add(&objects[i], (int)i);
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            int* p = map.GetValuePointer(&objects[idx]);
            if (p)
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // ValueKVMap 不支持逐个删除，跳过此测试
    result.erase_time = 0.0;

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.GetCount() * 16 + 1024 * objects.size();

    return result;
}
*/

// ============================================================================
// 测试 absl::flat_hash_map<const TestObject*, int>
// ============================================================================

BenchmarkResult BenchmarkAbslFlatHashMap(const vector<TestObject> &objects,
                                         const vector<int> &find_indices,
                                         const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "absl::flat_hash_map";

    absl::flat_hash_map<const TestObject*, int> map;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map[&objects[i]] = (int)i;
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            auto it = map.find(&objects[idx]);
            if (it != map.end())
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map.erase(&objects[idx]);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.size() * 16 + 1024 * objects.size();

    return result;
}

// ============================================================================
// 测试 absl::node_hash_map<const TestObject*, int>
// ============================================================================

BenchmarkResult BenchmarkAbslNodeHashMap(const vector<TestObject> &objects,
                                         const vector<int> &find_indices,
                                         const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "absl::node_hash_map";

    absl::node_hash_map<const TestObject*, int> map;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map[&objects[i]] = (int)i;
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            auto it = map.find(&objects[idx]);
            if (it != map.end())
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map.erase(&objects[idx]);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.size() * 24 + 1024 * objects.size();  // node_hash_map 节点更大

    return result;
}

// ============================================================================
// 测试 absl::btree_map<const TestObject*, int>
// ============================================================================

BenchmarkResult BenchmarkAbslBtreeMap(const vector<TestObject> &objects,
                                      const vector<int> &find_indices,
                                      const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "absl::btree_map";

    absl::btree_map<const TestObject*, int> map;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map[&objects[i]] = (int)i;
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            auto it = map.find(&objects[idx]);
            if (it != map.end())
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map.erase(&objects[idx]);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.size() * 32 + 1024 * objects.size();  // B-tree 节点

    return result;
}

// ============================================================================
// 测试 hgl::UnorderedMap<int, TestObject*> - 已禁用
// 原因：Map 内部使用 ManagedPool，要求 KeyValue<K,V> 符合 RAII 要求
// ============================================================================
/*
BenchmarkResult BenchmarkHglMap(const vector<TestObject> &objects,
                                const vector<int> &find_indices,
                                const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "hgl::UnorderedMap<int,T*>";

    hgl::UnorderedMap<int, TestObject*> map;

    // 插入 - 创建对象指针
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            TestObject* obj = new TestObject((int)i);
            map.Add((int)i, obj);
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            TestObject* value;
            if (map.Get(idx, value))
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map.DeleteByKey(idx);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.GetCount() * 16;

    return result;
}
*/

// ============================================================================
// 测试 hgl::UnorderedValueMap<int, int> - 已禁用
// 原因：UnorderedValueMap 内部使用 ManagedPool，要求 KeyValue<K,V> 符合 RAII 要求
// ============================================================================
/*
BenchmarkResult BenchmarkHglUnorderedValueMap(const vector<TestObject> &objects,
                                              const vector<int> &find_indices,
                                              const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "hgl::UnorderedValueMap<int,int>";

    hgl::UnorderedValueMap<int, int> map;

    // 插入
    {
        Timer timer;
        for (size_t i = 0; i < objects.size(); ++i)
        {
            map.Add((int)i, (int)i);
        }
        result.insert_time = timer.ElapsedMs();
    }

    // 查找
    {
        Timer timer;
        int found = 0;
        for (int idx : find_indices)
        {
            int value;
            if (map.Get(idx, value))
                ++found;
        }
        result.find_time = timer.ElapsedMs();
    }

    // 删除
    {
        Timer timer;
        for (int idx : erase_indices)
        {
            map.DeleteByKey(idx);
        }
        result.erase_time = timer.ElapsedMs();
    }

    result.total_time = result.insert_time + result.find_time + result.erase_time;
    result.memory_used = map.GetCount() * 16 + 1024 * objects.size();

    return result;
}
*/

// ============================================================================
// 性能对比汇总
// ============================================================================

void PrintComparison(const vector<BenchmarkResult> &results)
{
    cout << "\n" << string(80, '=') << endl;
    cout << "Performance Comparison Summary" << endl;
    cout << string(80, '=') << endl;

    // 找最快的
    int fastest_insert = 0, fastest_find = 0, fastest_total = 0;
    double min_insert = results[0].insert_time;
    double min_find = results[0].find_time;
    double min_total = results[0].total_time;

    for (size_t i = 1; i < results.size(); ++i)
    {
        if (results[i].insert_time < min_insert)
        {
            min_insert = results[i].insert_time;
            fastest_insert = i;
        }
        if (results[i].find_time < min_find)
        {
            min_find = results[i].find_time;
            fastest_find = i;
        }
        if (results[i].total_time < min_total)
        {
            min_total = results[i].total_time;
            fastest_total = i;
        }
    }

    cout << "\nFastest Insert: " << results[fastest_insert].name << endl;
    cout << "Fastest Find:   " << results[fastest_find].name << endl;
    cout << "Fastest Total:  " << results[fastest_total].name << endl;

    // 详细对比表
    cout << "\nDetailed Comparison Table:" << endl;
    cout << "+-" << string(20, '-') << "-+-" << string(15, '-') << "-+-"
         << string(15, '-') << "-+-" << string(15, '-') << "-+-"
         << string(15, '-') << "-+" << endl;
    cout << "| " << string(20, ' ') << " | Insert(ms) | Find(ms)   | Erase(ms)  | Total(ms)  |" << endl;
    cout << "+-" << string(20, '-') << "-+-" << string(15, '-') << "-+-"
         << string(15, '-') << "-+-" << string(15, '-') << "-+-"
         << string(15, '-') << "-+" << endl;

    for (const auto &result : results)
    {
        printf("| %-20s | %10.2f | %10.2f | %10.2f | %10.2f |\n",
               result.name.c_str(),
               result.insert_time,
               result.find_time,
               result.erase_time,
               result.total_time);
    }

    cout << "+-" << string(20, '-') << "-+-" << string(15, '-') << "-+-"
         << string(15, '-') << "-+-" << string(15, '-') << "-+-"
         << string(15, '-') << "-+" << endl;
}

// ============================================================================
// 主函数
// ============================================================================

int main(int, char **)
{
    cout << "========================================" << endl;
    cout << "Map Container Benchmark" << endl;
    cout << "========================================" << endl;

    cout << "\nBuild Mode: "
#ifdef NDEBUG
         << "Release (Optimized)"
#else
         << "Debug (Unoptimized)"
#endif
         << endl;

    const int OBJECT_COUNT = 100000;      // 10万个1KB对象
    const int FIND_COUNT = OBJECT_COUNT;  // 进行同样数量的查找
    const int ERASE_COUNT = OBJECT_COUNT / 10;  // 删除10%

    cout << "\nTest Parameters:" << endl;
    cout << "  Object Count:    " << OBJECT_COUNT << endl;
    cout << "  Object Size:     1 KB" << endl;
    cout << "  Total Data Size: " << (OBJECT_COUNT / 1024.0) << " MB" << endl;
    cout << "  Find Operations: " << FIND_COUNT << endl;
    cout << "  Erase Count:     " << ERASE_COUNT << endl;

    // 创建测试数据
    cout << "\nGenerating test data..." << endl;
    Timer data_timer;
    vector<TestObject> objects;
    objects.reserve(OBJECT_COUNT);
    for (int i = 0; i < OBJECT_COUNT; ++i)
    {
        objects.emplace_back(i);
    }
    cout << "Data generation took: " << data_timer.ElapsedMs() << " ms" << endl;

    // 生成随机查找和删除索引
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, OBJECT_COUNT - 1);

    vector<int> find_indices;
    vector<int> erase_indices;

    for (int i = 0; i < FIND_COUNT; ++i)
        find_indices.push_back(dis(gen));

    for (int i = 0; i < ERASE_COUNT; ++i)
        erase_indices.push_back(dis(gen));

    // 执行基准测试
    vector<BenchmarkResult> results;

    cout << "\n" << string(80, '=') << endl;
    cout << "Running benchmarks..." << endl;
    cout << string(80, '=') << endl;

    Timer test_timer;

    cout << "\n[1/4] Testing std::unordered_map..." << endl;
    results.push_back(BenchmarkUnorderedMap(objects, find_indices, erase_indices));

    cout << "[2/4] Testing std::map..." << endl;
    results.push_back(BenchmarkMap(objects, find_indices, erase_indices));

    // cout << "[3/6] Testing ValueKVMap..." << endl;
    // results.push_back(BenchmarkValueKVMap(objects, find_indices, erase_indices));

    cout << "[3/6] Testing absl::flat_hash_map..." << endl;
    results.push_back(BenchmarkAbslFlatHashMap(objects, find_indices, erase_indices));

    cout << "[4/6] Testing absl::node_hash_map..." << endl;
    results.push_back(BenchmarkAbslNodeHashMap(objects, find_indices, erase_indices));

    cout << "[5/6] Testing absl::btree_map..." << endl;
    results.push_back(BenchmarkAbslBtreeMap(objects, find_indices, erase_indices));

    // hgl 容器都要求使用 ManagedPool，对于指针值而言很复杂
    // 详见注释掉的 BenchmarkHglOrderedManagedMap 和 BenchmarkHglUnorderedMangedMap

    // cout << "[6/8] Testing hgl::OrderedManagedMap..." << endl;
    // results.push_back(BenchmarkHglOrderedManagedMap(find_indices, erase_indices));

    // cout << "[7/8] Testing hgl::UnorderedMangedMap..." << endl;
    // results.push_back(BenchmarkHglUnorderedMangedMap(find_indices, erase_indices));

    cout << "\nAll benchmarks completed in " << test_timer.ElapsedMs() << " ms" << endl;

    // 打印详细结果
    for (const auto &result : results)
    {
        result.Print();
    }

    // 性能对比
    PrintComparison(results);

    // 性能排名
    cout << "\n" << string(80, '=') << endl;
    cout << "Performance Ranking (by total time):" << endl;
    cout << string(80, '=') << endl;

    vector<BenchmarkResult> sorted_results = results;
    sort(sorted_results.begin(), sorted_results.end(),
         [](const BenchmarkResult &a, const BenchmarkResult &b) {
             return a.total_time < b.total_time;
         });

    for (size_t i = 0; i < sorted_results.size(); ++i)
    {
        cout << (i + 1) << ". " << sorted_results[i].name
             << " - " << sorted_results[i].total_time << " ms" << endl;
    }

    cout << "\n" << string(80, '=') << endl;
    cout << "Benchmark completed!" << endl;
    cout << string(80, '=') << endl;

    // ========================================================================
    // 纯查找性能测试（排除插入/删除干扰）
    // ========================================================================
    cout << "\n" << string(80, '=') << endl;
    cout << "Pure Find Performance Test (1 million lookups)" << endl;
    cout << string(80, '=') << endl;

    const int PURE_FIND_COUNT = 1000000;
    vector<int> pure_find_indices;
    for (int i = 0; i < PURE_FIND_COUNT; ++i)
        pure_find_indices.push_back(dis(gen));

    // 预先构建所有容器
    unordered_map<const TestObject*, int> umap;
    map<const TestObject*, int> stdmap;
    absl::flat_hash_map<const TestObject*, int> fmap;
    absl::node_hash_map<const TestObject*, int> nmap;
    
    for (size_t i = 0; i < objects.size(); ++i) {
        const TestObject* ptr = &objects[i];
        umap[ptr] = (int)i;
        stdmap[ptr] = (int)i;
        fmap[ptr] = (int)i;
        nmap[ptr] = (int)i;
    }

    cout << "\n容器已预填充 " << objects.size() << " 个元素，开始纯查找测试...\n" << endl;

    // 测试 std::unordered_map
    {
        Timer timer;
        volatile int found = 0;
        for (int idx : pure_find_indices) {
            auto it = umap.find(&objects[idx]);
            if (it != umap.end()) ++found;
        }
        cout << "std::unordered_map:   " << timer.ElapsedMs() << " ms" << endl;
    }

    // 测试 std::map
    {
        Timer timer;
        volatile int found = 0;
        for (int idx : pure_find_indices) {
            auto it = stdmap.find(&objects[idx]);
            if (it != stdmap.end()) ++found;
        }
        cout << "std::map:             " << timer.ElapsedMs() << " ms" << endl;
    }

    // 测试 absl::flat_hash_map
    {
        Timer timer;
        volatile int found = 0;
        for (int idx : pure_find_indices) {
            auto it = fmap.find(&objects[idx]);
            if (it != fmap.end()) ++found;
        }
        cout << "absl::flat_hash_map:  " << timer.ElapsedMs() << " ms" << endl;
    }

    // 测试 absl::node_hash_map
    {
        Timer timer;
        volatile int found = 0;
        for (int idx : pure_find_indices) {
            auto it = nmap.find(&objects[idx]);
            if (it != nmap.end()) ++found;
        }
        cout << "absl::node_hash_map:  " << timer.ElapsedMs() << " ms" << endl;
    }

    cout << "\n" << string(80, '=') << endl;

    // ========================================================================
    // 压力测试场景：频繁插入/删除/查询混合
    // ========================================================================
    cout << "\n" << string(80, '=') << endl;
    cout << "Stress Test Scenarios" << endl;
    cout << string(80, '=') << endl;

    const int STRESS_OPERATIONS = 100000;
    
    // 场景1：频繁插入删除后查询（碎片化测试）
    cout << "\n[Scenario 1] 碎片化测试：删除50%元素后查询性能" << endl;
    cout << string(60, '-') << endl;
    {
        // 预先构建容器
        unordered_map<const TestObject*, int> umap1;
        absl::flat_hash_map<const TestObject*, int> fmap1;
        absl::node_hash_map<const TestObject*, int> nmap1;
        
        for (size_t i = 0; i < objects.size(); ++i) {
            const TestObject* ptr = &objects[i];
            umap1[ptr] = (int)i;
            fmap1[ptr] = (int)i;
            nmap1[ptr] = (int)i;
        }
        
        // 删除50%的元素（制造碎片）
        for (size_t i = 0; i < objects.size(); i += 2) {
            const TestObject* ptr = &objects[i];
            umap1.erase(ptr);
            fmap1.erase(ptr);
            nmap1.erase(ptr);
        }
        
        cout << "已删除50%元素，当前元素数量：" << umap1.size() << endl;
        cout << "查找100万次（50%命中，50%未命中）...\n" << endl;
        
        // 测试查询性能（50%命中，50%未命中）
        {
            Timer timer;
            volatile int found = 0;
            for (int i = 0; i < STRESS_OPERATIONS; ++i) {
                auto it = umap1.find(&objects[i % objects.size()]);
                if (it != umap1.end()) ++found;
            }
            cout << "std::unordered_map:   " << timer.ElapsedMs() << " ms (found: " << found << ")" << endl;
        }
        
        {
            Timer timer;
            volatile int found = 0;
            for (int i = 0; i < STRESS_OPERATIONS; ++i) {
                auto it = fmap1.find(&objects[i % objects.size()]);
                if (it != fmap1.end()) ++found;
            }
            cout << "absl::flat_hash_map:  " << timer.ElapsedMs() << " ms (found: " << found << ")" << endl;
        }
        
        {
            Timer timer;
            volatile int found = 0;
            for (int i = 0; i < STRESS_OPERATIONS; ++i) {
                auto it = nmap1.find(&objects[i % objects.size()]);
                if (it != nmap1.end()) ++found;
            }
            cout << "absl::node_hash_map:  " << timer.ElapsedMs() << " ms (found: " << found << ")" << endl;
        }
    }
    
    // 场景2：插入删除查询混合操作
    cout << "\n[Scenario 2] 混合操作：随机插入/删除/查询" << endl;
    cout << string(60, '-') << endl;
    cout << "执行10万次随机操作（40%插入，30%删除，30%查询）...\n" << endl;
    {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> op_dist(0, 9);  // 0-3插入，4-6删除，7-9查询
        uniform_int_distribution<> idx_dist(0, objects.size() - 1);
        
        // std::unordered_map
        {
            unordered_map<const TestObject*, int> map;
            Timer timer;
            int inserts = 0, deletes = 0, finds = 0;
            
            for (int i = 0; i < STRESS_OPERATIONS; ++i) {
                int op = op_dist(gen);
                int idx = idx_dist(gen);
                const TestObject* ptr = &objects[idx];
                
                if (op < 4) {  // 插入
                    map[ptr] = idx;
                    ++inserts;
                } else if (op < 7) {  // 删除
                    map.erase(ptr);
                    ++deletes;
                } else {  // 查询
                    auto it = map.find(ptr);
                    ++finds;
                }
            }
            cout << "std::unordered_map:   " << timer.ElapsedMs() << " ms "
                 << "(I:" << inserts << " D:" << deletes << " F:" << finds 
                 << " Final:" << map.size() << ")" << endl;
        }
        
        // absl::flat_hash_map
        {
            absl::flat_hash_map<const TestObject*, int> map;
            Timer timer;
            int inserts = 0, deletes = 0, finds = 0;
            
            for (int i = 0; i < STRESS_OPERATIONS; ++i) {
                int op = op_dist(gen);
                int idx = idx_dist(gen);
                const TestObject* ptr = &objects[idx];
                
                if (op < 4) {
                    map[ptr] = idx;
                    ++inserts;
                } else if (op < 7) {
                    map.erase(ptr);
                    ++deletes;
                } else {
                    auto it = map.find(ptr);
                    ++finds;
                }
            }
            cout << "absl::flat_hash_map:  " << timer.ElapsedMs() << " ms "
                 << "(I:" << inserts << " D:" << deletes << " F:" << finds 
                 << " Final:" << map.size() << ")" << endl;
        }
        
        // absl::node_hash_map
        {
            absl::node_hash_map<const TestObject*, int> map;
            Timer timer;
            int inserts = 0, deletes = 0, finds = 0;
            
            for (int i = 0; i < STRESS_OPERATIONS; ++i) {
                int op = op_dist(gen);
                int idx = idx_dist(gen);
                const TestObject* ptr = &objects[idx];
                
                if (op < 4) {
                    map[ptr] = idx;
                    ++inserts;
                } else if (op < 7) {
                    map.erase(ptr);
                    ++deletes;
                } else {
                    auto it = map.find(ptr);
                    ++finds;
                }
            }
            cout << "absl::node_hash_map:  " << timer.ElapsedMs() << " ms "
                 << "(I:" << inserts << " D:" << deletes << " F:" << finds 
                 << " Final:" << map.size() << ")" << endl;
        }
    }
    
    // 场景3：频繁扩容测试
    cout << "\n[Scenario 3] 扩容测试：从空表逐步插入10万元素" << endl;
    cout << string(60, '-') << endl;
    cout << "测试多次rehash的性能影响...\n" << endl;
    {
        {
            Timer timer;
            unordered_map<const TestObject*, int> map;
            for (size_t i = 0; i < objects.size(); ++i) {
                map[&objects[i]] = (int)i;
            }
            cout << "std::unordered_map:   " << timer.ElapsedMs() << " ms" << endl;
        }
        
        {
            Timer timer;
            absl::flat_hash_map<const TestObject*, int> map;
            for (size_t i = 0; i < objects.size(); ++i) {
                map[&objects[i]] = (int)i;
            }
            cout << "absl::flat_hash_map:  " << timer.ElapsedMs() << " ms" << endl;
        }
        
        {
            Timer timer;
            absl::node_hash_map<const TestObject*, int> map;
            for (size_t i = 0; i < objects.size(); ++i) {
                map[&objects[i]] = (int)i;
            }
            cout << "absl::node_hash_map:  " << timer.ElapsedMs() << " ms" << endl;
        }
    }
    
    // 场景4：查询未命中测试
    cout << "\n[Scenario 4] 未命中查询：查询不存在的元素" << endl;
    cout << string(60, '-') << endl;
    {
        // 创建额外的测试对象用于未命中查询
        vector<TestObject> miss_objects;
        miss_objects.reserve(1000);
        for (int i = 0; i < 1000; ++i) {
            miss_objects.emplace_back(99999 + i);
        }
        
        cout << "查找10万次不存在的元素...\n" << endl;
        
        {
            Timer timer;
            volatile int found = 0;
            for (int i = 0; i < 100000; ++i) {
                auto it = umap.find(&miss_objects[i % 1000]);
                if (it != umap.end()) ++found;
            }
            cout << "std::unordered_map:   " << timer.ElapsedMs() << " ms (found: " << found << ")" << endl;
        }
                
        {
            Timer timer;
            volatile int found = 0;
            for (int i = 0; i < 100000; ++i) {
                auto it = fmap.find(&miss_objects[i % 1000]);
                if (it != fmap.end()) ++found;
            }
            cout << "absl::flat_hash_map:  " << timer.ElapsedMs() << " ms (found: " << found << ")" << endl;
        }
        
        {
            Timer timer;
            volatile int found = 0;
            for (int i = 0; i < 100000; ++i) {
                auto it = nmap.find(&miss_objects[i % 1000]);
                if (it != nmap.end()) ++found;
            }
            cout << "absl::node_hash_map:  " << timer.ElapsedMs() << " ms (found: " << found << ")" << endl;
        }
    }

    cout << "\n" << string(80, '=') << endl;

    return 0;
}
