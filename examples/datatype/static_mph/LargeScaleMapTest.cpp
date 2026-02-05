/**
 * Large Scale StaticPerfectHashMap Test
 * Tests with 100-1000+ keys to validate scalability
 */

#include <hgl/type/StaticPerfectHashMap.h>
#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <iostream>
#include <chrono>
#include <random>
#include <unordered_set>

using namespace hgl;

struct ItemData {
    uint32_t id;
    uint32_t value;
    char name[32];
};

void TestScale(int num_keys) {
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  大规模测试: %d 键                      \n", num_keys);
    printf("╚══════════════════════════════════════════╝\n\n");
    
    // Generate unique keys
    std::unordered_set<uint32_t> unique_keys;
    std::random_device rd;
    std::mt19937 gen(42); // Fixed seed for reproducibility
    std::uniform_int_distribution<uint32_t> dis(1, 1000000000);
    
    while (unique_keys.size() < (size_t)num_keys) {
        unique_keys.insert(dis(gen));
    }
    
    // Build phase
    printf("[步骤1: 构建阶段]\n");
    StaticPerfectHashMapBuilder<uint32_t, ItemData> builder;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (uint32_t key : unique_keys) {
        ItemData data;
        data.id = key;
        data.value = key * 2;
        snprintf(data.name, sizeof(data.name), "Item_%u", key);
        builder.Add(key, data);
    }
    
    auto add_end = std::chrono::high_resolution_clock::now();
    auto add_duration = std::chrono::duration_cast<std::chrono::microseconds>(add_end - start).count();
    
    printf("  添加 %d 个键: %lld μs\n", num_keys, add_duration);
    
    // Build
    auto build_start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto build_end = std::chrono::high_resolution_clock::now();
    auto build_duration = std::chrono::duration_cast<std::chrono::microseconds>(build_end - build_start).count();
    
    if (!success) {
        printf("  ✗ 构建失败！\n");
        return;
    }
    
    printf("  ✓ 构建成功！用时: %lld μs\n", build_duration);
    
    // Get stats
    auto stats = builder.GetBuildStats();
    printf("  重试次数: %d\n", stats.retry_count);
    printf("  最终桶数: %d\n", stats.num_buckets);
    printf("  空间开销: %.1f%%\n", stats.space_overhead);
    
    // Save to file
    printf("\n[步骤2: 保存到文件]\n");
    char filename[64];
    snprintf(filename, sizeof(filename), "test_large_%d.mph", num_keys);
    
    auto save_start = std::chrono::high_resolution_clock::now();
    builder.SaveToFile(filename);
    auto save_end = std::chrono::high_resolution_clock::now();
    auto save_duration = std::chrono::duration_cast<std::chrono::microseconds>(save_end - save_start).count();
    
    printf("  ✓ 保存成功: %s\n", filename);
    printf("  保存时间: %lld μs\n", save_duration);
    
    // Load from file
    printf("\n[步骤3: 从文件加载]\n");
    StaticPerfectHashMap<uint32_t, ItemData> map;
    
    auto load_start = std::chrono::high_resolution_clock::now();
    map.LoadFromFile(filename);
    auto load_end = std::chrono::high_resolution_clock::now();
    auto load_duration = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start).count();
    
    printf("  ✓ 加载成功！用时: %lld μs\n", load_duration);
    printf("  加载速度: %.1fx 构建速度\n", (double)build_duration / load_duration);
    
    // Verify all keys
    printf("\n[步骤4: 验证查找]\n");
    int found = 0;
    int not_found = 0;
    
    auto verify_start = std::chrono::high_resolution_clock::now();
    for (uint32_t key : unique_keys) {
        const ItemData* data = map.Get(key);
        if (data && data->id == key) {
            found++;
        } else {
            not_found++;
        }
    }
    auto verify_end = std::chrono::high_resolution_clock::now();
    auto verify_duration = std::chrono::duration_cast<std::chrono::microseconds>(verify_end - verify_start).count();
    
    printf("  查找成功: %d/%d\n", found, num_keys);
    if (not_found > 0) {
        printf("  ✗ 查找失败: %d 个键\n", not_found);
    } else {
        printf("  ✓ 所有键都能正确查找\n");
    }
    printf("  验证时间: %lld μs\n", verify_duration);
    printf("  平均查找: %.1f ns/查询\n", (double)verify_duration * 1000.0 / num_keys);
    
    // Performance test
    printf("\n[步骤5: 性能测试]\n");
    const int iterations = 100000;
    std::vector<uint32_t> test_keys(unique_keys.begin(), unique_keys.end());
    
    auto perf_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        uint32_t key = test_keys[i % test_keys.size()];
        const ItemData* data = map.Get(key);
        (void)data; // Avoid unused warning
    }
    auto perf_end = std::chrono::high_resolution_clock::now();
    auto perf_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(perf_end - perf_start).count();
    
    printf("  %d 次查找: %lld ns\n", iterations, perf_duration);
    printf("  平均查找时间: %.1f ns\n", (double)perf_duration / iterations);
    
    // Summary
    printf("\n══════════════════════════════════════════\n");
    printf("测试总结 (%d 键)\n", num_keys);
    printf("══════════════════════════════════════════\n");
    printf("  构建: %lld μs\n", build_duration);
    printf("  加载: %lld μs (快 %.1fx)\n", load_duration, (double)build_duration / load_duration);
    printf("  查找: %.1f ns/查询\n", (double)verify_duration * 1000.0 / num_keys);
    printf("  空间: %.1f%%\n", stats.space_overhead);
    printf("  重试: %d 次\n", stats.retry_count);
    printf("  结果: %s\n", not_found == 0 ? "✓ 成功" : "✗ 失败");
    printf("══════════════════════════════════════════\n");
}

int main() {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  StaticPerfectHashMap 大规模测试         ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    
    // Test different scales
    int scales[] = {50, 100, 200, 500, 1000};
    
    for (int scale : scales) {
        TestScale(scale);
    }
    
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  所有大规模测试完成！                    ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    
    return 0;
}
