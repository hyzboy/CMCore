/**
 * @file StringSetMPHTest.cpp
 * @brief 字符串集合MPH测试 - 从文件读取、去重、构建、保存、加载
 * 
 * 演示完整的MPH工作流程：
 * 1. 从文本文件读取字符串
 * 2. 使用HashSet去重
 * 3. 构建静态完美哈希表
 * 4. 保存到文件
 * 5. 从文件加载
 * 6. 验证查找
 */

#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_set>
#include <chrono>

using namespace hgl;

// 字符串数据结构（示例）
struct StringData
{
    uint32_t id;
    uint32_t level;
    float power;
};

int main(int argc, char* argv[])
{
    const char* input_file = argc > 1 ? argv[1] : "testdata/skills.txt";
    const char* output_file = "test_strings.mph";

    std::cout << "=== 字符串集合MPH测试 ===\n\n";

    // ========================================
    // 步骤1: 读取文件并去重
    // ========================================
    std::cout << "[步骤1: 读取文件并去重]\n";
    
    std::ifstream file(input_file);
    if (!file.is_open())
    {
        std::cerr << "✗ 无法打开文件: " << input_file << std::endl;
        return 1;
    }

    // 使用HashSet去重
    std::unordered_set<std::string> unique_strings;
    std::string line;
    int line_count = 0;
    
    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            unique_strings.insert(line);
            line_count++;
        }
    }
    file.close();

    std::cout << "  原始行数: " << line_count << std::endl;
    std::cout << "  去重后: " << unique_strings.size() << " 个唯一字符串\n\n";

    if (unique_strings.empty())
    {
        std::cout << "✗ 没有数据\n";
        return 1;
    }

    // ========================================
    // 步骤2: 构建MPH
    // ========================================
    std::cout << "[步骤2: 构建MPH]\n";

    StaticPerfectHashMapBuilder<uint32_t, StringData> builder;
    
    // 将字符串转换为哈希键
    uint32_t id_counter = 1;
    for (const auto& str : unique_strings)
    {
        // 使用标准库的hash函数
        uint32_t key = std::hash<std::string>{}(str) & 0xFFFFFFFF;
        
        StringData data;
        data.id = id_counter++;
        data.level = 1;
        data.power = 100.0f;
        
        builder.Add(key, data);
    }

    auto build_start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto build_end = std::chrono::high_resolution_clock::now();
    auto build_time = std::chrono::duration_cast<std::chrono::microseconds>(build_end - build_start).count();

    if (!success)
    {
        std::cout << "✗ MPH构建失败！\n";
        std::cout << "  可能原因: 数据规模超出当前简化实现能力\n";
        std::cout << "  建议: 减少数据量或改进哈希函数\n";
        return 1;
    }

    auto stats = builder.GetStats();
    std::cout << "  ✓ 构建成功！\n";
    std::cout << "  构建时间: " << build_time << " μs\n";
    std::cout << "  键数量: " << stats.num_keys << std::endl;
    std::cout << "  桶数量: " << stats.num_buckets << std::endl;
    std::cout << "  总大小: " << stats.total_size << " 字节\n";
    std::cout << "  空间开销: " << stats.space_overhead << "%\n\n";

    // ========================================
    // 步骤3: 保存到文件
    // ========================================
    std::cout << "[步骤3: 保存到文件]\n";

    if (!builder.SaveToFile(output_file))
    {
        std::cout << "✗ 保存失败\n";
        return 1;
    }

    std::cout << "  ✓ 保存成功: " << output_file << std::endl;
    std::cout << "  文件大小: " << stats.total_size << " 字节\n\n";

    // ========================================
    // 步骤4: 从文件加载
    // ========================================
    std::cout << "[步骤4: 从文件加载]\n";

    StaticPerfectHashMap<uint32_t, StringData> mph;
    
    auto load_start = std::chrono::high_resolution_clock::now();
    if (!mph.LoadFromFile(output_file))
    {
        std::cout << "✗ 加载失败\n";
        return 1;
    }
    auto load_end = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start).count();

    std::cout << "  ✓ 加载成功！\n";
    std::cout << "  加载时间: " << load_time << " μs\n";
    std::cout << "  加载的键数: " << mph.GetCount() << "\n\n";

    // ========================================
    // 步骤5: 验证查找
    // ========================================
    std::cout << "[步骤5: 验证查找]\n";

    int found_count = 0;
    int total_count = 0;

    for (const auto& str : unique_strings)
    {
        uint32_t key = std::hash<std::string>{}(str) & 0xFFFFFFFF;
        const StringData* data = mph.Get(key);
        
        total_count++;
        if (data)
        {
            found_count++;
        }
        else
        {
            std::cout << "  ✗ 未找到: " << str << " (key=" << key << ")\n";
        }
    }

    std::cout << "  查找成功: " << found_count << "/" << total_count << std::endl;
    
    if (found_count == total_count)
    {
        std::cout << "  ✓ 所有键验证通过！\n";
    }
    else
    {
        std::cout << "  ✗ 部分键查找失败\n";
    }

    std::cout << "\n=== 测试完成 ===\n";

    return (found_count == total_count) ? 0 : 1;
}
