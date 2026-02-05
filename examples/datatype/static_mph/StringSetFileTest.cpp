/**
 * @file StringSetFileTest.cpp
 * @brief 字符串集合MPH测试 - 从文件读取，去重，构建集合
 */

#include <hgl/type/StaticPerfectHashSetBuilder.h>
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <string>
#include <chrono>
#include <iomanip>

using namespace hgl;

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

int main(int argc, char* argv[])
{
    PrintSeparator("字符串集合MPH测试");

    // 确定输入文件
    const char* input_file = "testdata/skills.txt";
    if (argc > 1)
    {
        input_file = argv[1];
    }

    // 步骤1：读取文件并去重
    std::cout << "\n[步骤1: 读取文件并去重]\n";
    
    std::ifstream file(input_file);
    if (!file.is_open())
    {
        std::cout << "  ✗ 无法打开文件: " << input_file << "\n";
        std::cout << "  用法: " << argv[0] << " [文件路径]\n";
        return 1;
    }

    std::unordered_set<std::string> unique_strings;
    std::string line;
    uint32_t line_count = 0;

    while (std::getline(file, line))
    {
        // 去除空行和空白
        if (!line.empty() && line.find_first_not_of(" \t\r\n") != std::string::npos)
        {
            unique_strings.insert(line);
            line_count++;
        }
    }
    file.close();

    std::cout << "  读取文件: " << input_file << "\n";
    std::cout << "  原始行数: " << line_count << "\n";
    std::cout << "  去重后: " << unique_strings.size() << " 个唯一字符串\n";

    if (unique_strings.empty())
    {
        std::cout << "  ✗ 没有读取到任何字符串！\n";
        return 1;
    }

    // 步骤2：构建MPH集合（字符串哈希）
    std::cout << "\n[步骤2: 构建MPH集合]\n";
    
    StaticPerfectHashSetBuilder<uint32_t> builder;
    std::hash<std::string> string_hasher;
    
    // 为调试保存字符串到哈希的映射
    std::vector<std::pair<std::string, uint32_t>> string_to_hash;
    
    for (const auto& str : unique_strings)
    {
        uint32_t hash = static_cast<uint32_t>(string_hasher(str));
        builder.Add(hash);
        string_to_hash.push_back({str, hash});
        
        std::cout << "    " << str << " -> " << hash << "\n";
    }

    auto build_start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto build_end = std::chrono::high_resolution_clock::now();
    auto build_time = std::chrono::duration_cast<std::chrono::microseconds>(build_end - build_start).count();

    if (!success)
    {
        std::cout << "  ✗ 构建失败！\n";
        std::cout << "  说明: 当前规模 (" << unique_strings.size() 
                  << " 键) 可能超出简化实现的能力\n";
        return 1;
    }

    std::cout << "  ✓ 构建成功！\n";
    std::cout << "  构建时间: " << build_time << " μs\n";

    auto stats = builder.GetStats();
    std::cout << "  键数量: " << stats.num_keys << "\n";
    std::cout << "  桶数量: " << stats.num_buckets << "\n";
    std::cout << "  数据大小: " << stats.total_size << " 字节\n";
    std::cout << "  空间开销: " << std::fixed << std::setprecision(1) 
              << stats.space_overhead << "%\n";

    // 步骤3：保存到文件
    std::cout << "\n[步骤3: 保存到文件]\n";
    
    const char* output_file = "test_string_set.mphs";
    if (!builder.SaveToFile(output_file))
    {
        std::cout << "  ✗ 保存失败！\n";
        return 1;
    }
    
    std::cout << "  ✓ 保存成功: " << output_file << "\n";
    std::cout << "  文件大小: " << stats.total_size << " 字节\n";

    // 步骤4：从文件加载
    std::cout << "\n[步骤4: 从文件加载]\n";
    
    StaticPerfectHashSet<uint32_t> mph_set;
    
    auto load_start = std::chrono::high_resolution_clock::now();
    if (!mph_set.LoadFromFile(output_file))
    {
        std::cout << "  ✗ 加载失败！\n";
        return 1;
    }
    auto load_end = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start).count();

    std::cout << "  ✓ 加载成功！\n";
    std::cout << "  加载时间: " << load_time << " μs\n";

    // 步骤5：验证查找
    std::cout << "\n[步骤5: 验证查找]\n";
    
    uint32_t found_count = 0;
    for (const auto& [str, hash] : string_to_hash)
    {
        if (mph_set.Contains(hash))
        {
            found_count++;
        }
        else
        {
            std::cout << "  ✗ 未找到: " << str << " (hash: " << hash << ")\n";
        }
    }

    std::cout << "  查找成功: " << found_count << "/" << string_to_hash.size() << "\n";

    if (found_count == string_to_hash.size())
    {
        std::cout << "  ✓ 所有字符串都能正确查找！\n";
    }
    else
    {
        std::cout << "  ✗ 有字符串查找失败！\n";
        return 1;
    }

    // 总结
    PrintSeparator();
    std::cout << "测试总结\n";
    std::cout << "══════════════════════════════════════════\n";
    std::cout << "  输入文件: " << input_file << "\n";
    std::cout << "  字符串数: " << unique_strings.size() << "\n";
    std::cout << "  ✓ 去重: std::unordered_set\n";
    std::cout << "  ✓ 构建: " << build_time << " μs\n";
    std::cout << "  ✓ 保存: " << output_file << "\n";
    std::cout << "  ✓ 加载: " << load_time << " μs\n";
    std::cout << "  ✓ 验证: 100%\n";
    std::cout << "\n✓ 字符串集合测试通过！\n\n";

    return 0;
}
