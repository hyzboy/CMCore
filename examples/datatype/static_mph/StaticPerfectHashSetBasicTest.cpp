/**
 * @file StaticPerfectHashSetBasicTest.cpp
 * @brief 静态完美哈希集合 - 基础测试
 */

#include <hgl/type/StaticPerfectHashSetBuilder.h>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace hgl;

// 简单的测试数据
struct TestData
{
    static constexpr uint32_t test_keys[] = {
        10001, 10002, 10003, 10004, 10005
    };
    static constexpr size_t num_keys = 5;
};

constexpr uint32_t TestData::test_keys[];

void PrintSeparator(const char* title = nullptr)
{
    std::cout << "\n";
    if (title)
    {
        std::cout << "╔══════════════════════════════════════════╗\n";
        std::cout << "║  " << std::left << std::setw(38) << title << "  ║\n";
        std::cout << "╚══════════════════════════════════════════╝\n";
    }
    else
    {
        std::cout << "══════════════════════════════════════════\n";
    }
}

int main()
{
    PrintSeparator("静态完美哈希集合 - 基础测试");

    // 步骤1：构建MPH集合
    std::cout << "\n[步骤1: 构建MPH集合]\n";
    
    StaticPerfectHashSetBuilder<uint32_t> builder;
    
    // 添加测试键
    for (size_t i = 0; i < TestData::num_keys; ++i)
    {
        builder.Add(TestData::test_keys[i]);
    }
    
    std::cout << "  添加了 " << builder.GetCount() << " 个键\n";

    // 构建
    auto build_start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto build_end = std::chrono::high_resolution_clock::now();
    auto build_time = std::chrono::duration_cast<std::chrono::microseconds>(build_end - build_start).count();

    if (!success)
    {
        std::cout << "  ✗ 构建失败！\n";
        return 1;
    }

    std::cout << "  ✓ 构建成功！\n";
    std::cout << "  构建时间: " << build_time << " μs\n";

    // 获取统计信息
    auto stats = builder.GetStats();
    std::cout << "  键数量: " << stats.num_keys << "\n";
    std::cout << "  桶数量: " << stats.num_buckets << "\n";
    std::cout << "  数据大小: " << stats.total_size << " 字节\n";
    std::cout << "  空间开销: " << std::fixed << std::setprecision(1) 
              << stats.space_overhead << "%\n";

    // 步骤2：保存到文件
    std::cout << "\n[步骤2: 保存到文件]\n";
    
    const char* filename = "test_set.mphs";
    if (!builder.SaveToFile(filename))
    {
        std::cout << "  ✗ 保存失败！\n";
        return 1;
    }
    
    std::cout << "  ✓ 保存成功: " << filename << "\n";
    std::cout << "  文件大小: " << stats.total_size << " 字节\n";

    // 步骤3：从文件加载
    std::cout << "\n[步骤3: 从文件加载]\n";
    
    StaticPerfectHashSet<uint32_t> mph_set;
    
    auto load_start = std::chrono::high_resolution_clock::now();
    if (!mph_set.LoadFromFile(filename))
    {
        std::cout << "  ✗ 加载失败！\n";
        return 1;
    }
    auto load_end = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start).count();

    std::cout << "  ✓ 加载成功！\n";
    std::cout << "  加载时间: " << load_time << " μs\n";
    std::cout << "  键数量: " << mph_set.GetCount() << "\n";

    // 步骤4：验证查找
    std::cout << "\n[步骤4: 验证查找]\n";
    
    uint32_t found_count = 0;
    uint32_t total_tests = 0;

    // 测试已知键
    for (size_t i = 0; i < TestData::num_keys; ++i)
    {
        uint32_t key = TestData::test_keys[i];
        if (mph_set.Contains(key))
        {
            found_count++;
        }
        else
        {
            std::cout << "  ✗ 键 " << key << " 未找到！\n";
        }
        total_tests++;
    }

    // 测试不存在的键
    uint32_t non_existent_keys[] = {9999, 20000, 30000};
    for (uint32_t key : non_existent_keys)
    {
        if (!mph_set.Contains(key))
        {
            found_count++;  // 正确地没找到
        }
        else
        {
            std::cout << "  ✗ 假阳性：键 " << key << " 不应该存在！\n";
        }
        total_tests++;
    }

    std::cout << "  测试通过: " << found_count << "/" << total_tests << "\n";

    if (found_count == total_tests)
    {
        std::cout << "  ✓ 所有查找测试通过！\n";
    }
    else
    {
        std::cout << "  ✗ 有查找失败！\n";
        return 1;
    }

    // 步骤5：性能测试
    std::cout << "\n[步骤5: 性能测试]\n";
    
    const int num_iterations = 100000;
    auto perf_start = std::chrono::high_resolution_clock::now();
    
    uint32_t check_count = 0;
    for (int i = 0; i < num_iterations; ++i)
    {
        for (size_t j = 0; j < TestData::num_keys; ++j)
        {
            if (mph_set.Contains(TestData::test_keys[j]))
                check_count++;
        }
    }
    
    auto perf_end = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::nanoseconds>(perf_end - perf_start).count();
    
    uint64_t total_lookups = (uint64_t)num_iterations * TestData::num_keys;
    double avg_time = (double)total_time / total_lookups;
    
    std::cout << "  总查找次数: " << total_lookups << "\n";
    std::cout << "  成功次数: " << check_count << "\n";
    std::cout << "  平均查找时间: " << std::fixed << std::setprecision(1) 
              << avg_time << " ns\n";

    // 总结
    PrintSeparator();
    std::cout << "测试总结\n";
    std::cout << "══════════════════════════════════════════\n";
    std::cout << "  ✓ 构建: " << build_time << " μs\n";
    std::cout << "  ✓ 保存: 成功\n";
    std::cout << "  ✓ 加载: " << load_time << " μs\n";
    std::cout << "  ✓ 查找: " << std::fixed << std::setprecision(1) << avg_time << " ns\n";
    std::cout << "  ✓ 正确性: 100%\n";
    std::cout << "\n✓ 所有测试通过！\n\n";

    return 0;
}
