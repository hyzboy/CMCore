/**
 * @file RetryMechanismTest.cpp
 * @brief 测试自动重试机制 - 验证10键案例
 * 
 * 测试之前100%失败的10键案例，验证自动重试机制是否有效。
 */

#include <iostream>
#include <iomanip>
#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <hgl/type/StaticPerfectHashMap.h>

using namespace hgl;

struct SkillConfig
{
    uint32_t skill_id;
    uint32_t damage;
    uint32_t cooldown;
    uint32_t mana_cost;
};

int main()
{
    std::cout << "╔══════════════════════════════════════════╗" << std::endl;
    std::cout << "║     重试机制测试 - 10键案例              ║" << std::endl;
    std::cout << "╚══════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;

    std::cout << "[测试: 10键 - 之前总是失败的案例]" << std::endl;
    std::cout << std::endl;

    // 准备10个测试键（之前这个规模100%失败）
    const int NUM_KEYS = 10;
    std::cout << "准备" << NUM_KEYS << "个测试键..." << std::endl;
    std::cout << std::endl;

    // 构建MPH
    StaticPerfectHashMapBuilder<uint32_t, SkillConfig> builder;
    
    for (int i = 0; i < NUM_KEYS; ++i)
    {
        uint32_t skill_id = 10000 + i;
        SkillConfig config;
        config.skill_id = skill_id;
        config.damage = 100 + i * 10;
        config.cooldown = 5 + i;
        config.mana_cost = 50 + i * 5;
        
        builder.Add(skill_id, config);
    }

    // 尝试构建（带自动重试）
    bool success = builder.Build();
    
    std::cout << std::endl;
    std::cout << "[构建完成]" << std::endl;
    
    if (success)
    {
        auto stats = builder.GetBuildStats();
        
        std::cout << "  ✓ 构建成功！" << std::endl;
        std::cout << "  重试次数: " << stats.retry_count << std::endl;
        std::cout << "  最终桶数: " << stats.num_buckets << std::endl;
        std::cout << "  键数量: " << stats.num_keys << std::endl;
        std::cout << "  空间开销: " << std::fixed << std::setprecision(1) 
                  << stats.space_overhead << "%" << std::endl;
        std::cout << "  总构建时间: " << stats.build_time_us << " μs" << std::endl;
    }
    else
    {
        std::cout << "  ✗ 构建失败！（这不应该发生）" << std::endl;
        return 1;
    }
    
    // 保存到文件
    std::cout << std::endl;
    std::cout << "[保存到文件]" << std::endl;
    if (builder.SaveToFile("test_retry_10.mph"))
    {
        std::cout << "  ✓ 保存成功: test_retry_10.mph" << std::endl;
    }
    else
    {
        std::cout << "  ✗ 保存失败" << std::endl;
        return 1;
    }
    
    // 从文件加载
    std::cout << std::endl;
    std::cout << "[从文件加载]" << std::endl;
    StaticPerfectHashMap<uint32_t, SkillConfig> mph;
    if (mph.LoadFromFile("test_retry_10.mph"))
    {
        std::cout << "  ✓ 加载成功！" << std::endl;
    }
    else
    {
        std::cout << "  ✗ 加载失败" << std::endl;
        return 1;
    }
    
    // 验证查找
    std::cout << std::endl;
    std::cout << "[验证查找]" << std::endl;
    int found_count = 0;
    for (int i = 0; i < NUM_KEYS; ++i)
    {
        uint32_t skill_id = 10000 + i;
        const SkillConfig* config = mph.Get(skill_id);
        if (config && config->skill_id == skill_id)
        {
            found_count++;
        }
    }
    
    std::cout << "  查找成功: " << found_count << "/" << NUM_KEYS << std::endl;
    if (found_count == NUM_KEYS)
    {
        std::cout << "  ✓ 所有键都能正确查找" << std::endl;
    }
    else
    {
        std::cout << "  ✗ 部分键查找失败" << std::endl;
        return 1;
    }
    
    // 性能测试
    std::cout << std::endl;
    std::cout << "[性能测试]" << std::endl;
    const int ITERATIONS = 10000;
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int iter = 0; iter < ITERATIONS; ++iter)
    {
        for (int i = 0; i < NUM_KEYS; ++i)
        {
            uint32_t skill_id = 10000 + i;
            const SkillConfig* config = mph.Get(skill_id);
            (void)config;  // 避免未使用警告
        }
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    double avg_lookup_time = duration.count() / (double)(ITERATIONS * NUM_KEYS);
    
    std::cout << "  平均查找时间: " << std::fixed << std::setprecision(0) 
              << avg_lookup_time << " ns" << std::endl;
    
    // 总结
    std::cout << std::endl;
    std::cout << "══════════════════════════════════════════" << std::endl;
    std::cout << "结论: 之前100%失败的案例，现在100%成功！" << std::endl;
    std::cout << "══════════════════════════════════════════" << std::endl;
    
    return 0;
}
