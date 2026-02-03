/**
 * FlatUnorderedSetPerformanceBenchmark.cpp
 * 性能基准测试 - 测试优化后的性能改进
 * Performance Benchmark - Test optimization improvements
 */

#include <hgl/type/FlatUnorderedSet.h>
#include <iostream>
#include <chrono>
#include <random>
#include <iomanip>

using namespace hgl;
using namespace std;

// ==================== 计时工具 ====================

class Timer
{
    chrono::high_resolution_clock::time_point start_time;
    const char* name;
    bool print_on_destruct;

public:
    Timer(const char* n, bool print = true) 
        : name(n), start_time(chrono::high_resolution_clock::now()), print_on_destruct(print) {}

    ~Timer()
    {
        if (print_on_destruct)
            print();
    }

    double elapsed_ms() const
    {
        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
        return duration.count() / 1000.0;
    }

    void print() const
    {
        cout << "  " << name << ": " << fixed << setprecision(3) << elapsed_ms() << " ms" << endl;
    }
};

// ==================== 测试配置 ====================

struct BenchmarkConfig
{
    int element_count;
    const char* scenario_name;
};

// ==================== 基准测试 ====================

void BenchmarkAdd(const BenchmarkConfig& config)
{
    cout << "\n=== " << config.scenario_name << " - Add 操作 ===" << endl;
    cout << "元素数量: " << config.element_count << endl;

    FlatUnorderedSet<int> set;
    set.Reserve(config.element_count);

    // 测试顺序插入（无碰撞场景）
    {
        set.Clear();
        Timer t("顺序插入 (无碰撞)");
        for (int i = 0; i < config.element_count; i++)
        {
            set.Add(i);
        }
    }

    // 测试随机插入（低碰撞场景）
    {
        set.Clear();
        vector<int> random_values(config.element_count);
        for (int i = 0; i < config.element_count; i++)
        {
            random_values[i] = rand();
        }

        Timer t("随机插入 (低碰撞)");
        for (int value : random_values)
        {
            set.Add(value);
        }
    }

    // 测试批量插入
    {
        set.Clear();
        vector<int> values(config.element_count);
        for (int i = 0; i < config.element_count; i++)
        {
            values[i] = i;
        }

        Timer t("批量插入");
        set.Add(values.data(), values.size());
    }

    cout << "最终集合大小: " << set.GetCount() << endl;
}

void BenchmarkFind(const BenchmarkConfig& config)
{
    cout << "\n=== " << config.scenario_name << " - Find 操作 ===" << endl;
    cout << "元素数量: " << config.element_count << endl;

    FlatUnorderedSet<int> set;
    set.Reserve(config.element_count);

    // 准备数据：顺序插入
    for (int i = 0; i < config.element_count; i++)
    {
        set.Add(i);
    }

    // 测试成功查找（所有元素都存在）
    {
        Timer t("成功查找 (所有元素)");
        int found_count = 0;
        for (int i = 0; i < config.element_count; i++)
        {
            if (set.Contains(i))
                found_count++;
        }
        cout << "  找到元素: " << found_count << endl;
    }

    // 测试失败查找（所有元素都不存在）
    {
        Timer t("失败查找 (所有元素)");
        int not_found_count = 0;
        for (int i = config.element_count; i < config.element_count * 2; i++)
        {
            if (!set.Contains(i))
                not_found_count++;
        }
        cout << "  未找到元素: " << not_found_count << endl;
    }

    // 测试混合查找（50% 成功 + 50% 失败）
    {
        Timer t("混合查找 (50% 成功)");
        int found = 0, not_found = 0;
        for (int i = 0; i < config.element_count * 2; i++)
        {
            if (set.Contains(i))
                found++;
            else
                not_found++;
        }
        cout << "  找到: " << found << ", 未找到: " << not_found << endl;
    }
}

void BenchmarkDelete(const BenchmarkConfig& config)
{
    cout << "\n=== " << config.scenario_name << " - Delete 操作 ===" << endl;
    cout << "元素数量: " << config.element_count << endl;

    FlatUnorderedSet<int> set;
    set.Reserve(config.element_count);

    // 准备数据
    for (int i = 0; i < config.element_count; i++)
    {
        set.Add(i);
    }

    // 测试顺序删除
    {
        FlatUnorderedSet<int> temp = set;  // 复制一份
        Timer t("顺序删除 (前50%)");
        int deleted = 0;
        for (int i = 0; i < config.element_count / 2; i++)
        {
            if (temp.Delete(i))
                deleted++;
        }
        cout << "  删除元素: " << deleted << endl;
    }

    // 测试随机删除
    {
        FlatUnorderedSet<int> temp = set;  // 复制一份
        vector<int> indices(config.element_count);
        for (int i = 0; i < config.element_count; i++)
            indices[i] = i;
        random_shuffle(indices.begin(), indices.end());

        Timer t("随机删除 (前50%)");
        int deleted = 0;
        for (int i = 0; i < config.element_count / 2; i++)
        {
            if (temp.Delete(indices[i]))
                deleted++;
        }
        cout << "  删除元素: " << deleted << endl;
    }
}

void BenchmarkMemoryUsage(const BenchmarkConfig& config)
{
    cout << "\n=== " << config.scenario_name << " - 内存使用 ===" << endl;
    cout << "元素数量: " << config.element_count << endl;

    FlatUnorderedSet<int> set;
    
    // 填充数据
    for (int i = 0; i < config.element_count; i++)
    {
        set.Add(i);
    }

    // 估算内存使用（简化版）
    size_t estimated_memory = 0;
    
    // 数据管理器内存
    estimated_memory += config.element_count * sizeof(int);
    
    // 哈希表内存（hash_to_id）
    estimated_memory += config.element_count * (sizeof(uint64) + sizeof(int));
    
    // 碰撞链内存（假设5%碰撞率）
    int collision_count = config.element_count * 0.05;
    estimated_memory += collision_count * sizeof(vector<int>);
    estimated_memory += collision_count * 2 * sizeof(int);  // 平均每条链2个元素
    
    cout << "  估算内存使用: " << (estimated_memory / 1024.0) << " KB" << endl;
    cout << "  平均每元素: " << (estimated_memory / (double)config.element_count) << " bytes" << endl;
}

void BenchmarkIteration(const BenchmarkConfig& config)
{
    cout << "\n=== " << config.scenario_name << " - 迭代器性能 ===" << endl;
    cout << "元素数量: " << config.element_count << endl;

    FlatUnorderedSet<int> set;
    for (int i = 0; i < config.element_count; i++)
    {
        set.Add(i);
    }

    // 测试 Enum 迭代
    {
        Timer t("Enum 迭代");
        long long sum = 0;
        set.Enum([&](int value) {
            sum += value;
        });
        cout << "  总和: " << sum << endl;
    }

    // 测试 range-for 迭代
    {
        Timer t("range-for 迭代");
        long long sum = 0;
        for (int value : set)
        {
            sum += value;
        }
        cout << "  总和: " << sum << endl;
    }

    // 测试手动迭代器
    {
        Timer t("手动迭代器");
        long long sum = 0;
        for (auto it = set.cbegin(); it != set.cend(); ++it)
        {
            sum += *it;
        }
        cout << "  总和: " << sum << endl;
    }
}

// ==================== 综合性能测试 ====================

void RunComprehensiveBenchmark()
{
    cout << "\n" << string(70, '=') << endl;
    cout << "FlatUnorderedSet 性能基准测试" << endl;
    cout << "Performance Benchmark for FlatUnorderedSet" << endl;
    cout << string(70, '=') << endl;

    // 测试不同规模
    BenchmarkConfig configs[] = {
        { 1000,    "小规模 (1K)" },
        { 10000,   "中规模 (10K)" },
        { 100000,  "大规模 (100K)" },
        { 1000000, "超大规模 (1M)" }
    };

    for (const auto& config : configs)
    {
        BenchmarkAdd(config);
        BenchmarkFind(config);
        BenchmarkDelete(config);
        BenchmarkMemoryUsage(config);
        BenchmarkIteration(config);
    }

    cout << "\n" << string(70, '=') << endl;
    cout << "测试完成！" << endl;
    cout << string(70, '=') << endl;
}

// ==================== 主函数 ====================

int main()
{
    cout << "FlatUnorderedSet 优化性能基准测试" << endl;
    cout << "======================================" << endl;
    cout << "\n优化特性:" << endl;
    cout << "  1. 高性能哈希函数 (QuickHash)" << endl;
    cout << "  2. 两级存储结构（减少内存）" << endl;
    cout << "  3. 快速路径优化（无碰撞情况）" << endl;
    cout << "\n开始测试..." << endl;

    // 设置随机种子
    srand(42);

    // 运行综合基准测试
    RunComprehensiveBenchmark();

    return 0;
}
