#include<iostream>
#include<chrono>
#include<random>
#include<vector>
#include<unordered_map>
#include<map>
#include<hgl/type/ValueKVMap.h>
#include<tsl/robin_map.h>

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
    size_t memory_used;
    
    void Print() const
    {
        cout << "\n" << name << " Results:" << endl;
        cout << "  Insert:  " << insert_time << " ms" << endl;
        cout << "  Find:    " << find_time << " ms" << endl;
        cout << "  Erase:   " << erase_time << " ms" << endl;
        cout << "  Total:   " << total_time << " ms" << endl;
        cout << "  Memory:  " << (memory_used / 1024.0 / 1024.0) << " MB" << endl;
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
// 测试 tsl::robin_map<const TestObject*, int>
// ============================================================================

BenchmarkResult BenchmarkRobinMap(const vector<TestObject> &objects,
                                  const vector<int> &find_indices,
                                  const vector<int> &erase_indices)
{
    BenchmarkResult result;
    result.name = "tsl::robin_map";
    
    tsl::robin_map<const TestObject*, int> map;
    
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
// 测试 ValueKVMap<const TestObject*, int>
// ============================================================================

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
    
    cout << "[3/4] Testing tsl::robin_map..." << endl;
    results.push_back(BenchmarkRobinMap(objects, find_indices, erase_indices));
    
    cout << "[4/4] Testing ValueKVMap..." << endl;
    results.push_back(BenchmarkValueKVMap(objects, find_indices, erase_indices));
    
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
    
    return 0;
}
