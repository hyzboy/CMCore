/**
 * Stress Test for StaticPerfectHash
 * Tests edge cases, boundary conditions, and error handling
 */

#include <hgl/type/StaticPerfectHashMap.h>
#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <hgl/type/StaticPerfectHashSet.h>
#include <hgl/type/StaticPerfectHashSetBuilder.h>
#include <iostream>
#include <chrono>
#include <random>

using namespace hgl;

// Test with sequential keys (potential hash collision pattern)
void TestSequentialKeys() {
    printf("\n[测试: 顺序键模式]\n");
    
    StaticPerfectHashMapBuilder<uint32_t, uint32_t> builder;
    const int n = 100;
    
    for (int i = 0; i < n; i++) {
        builder.Add(i, i * 10);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    if (success) {
        auto stats = builder.GetBuildStats();
        printf("  ✓ 顺序键 0-%d: 成功\n", n-1);
        printf("    构建时间: %lld μs\n", duration);
        printf("    重试次数: %d\n", stats.retry_count);
        printf("    空间开销: %.1f%%\n", stats.space_overhead);
    } else {
        printf("  ✗ 顺序键测试失败\n");
    }
}

// Test with power-of-2 keys (potential modulo collision)
void TestPowerOf2Keys() {
    printf("\n[测试: 2的幂次键模式]\n");
    
    StaticPerfectHashMapBuilder<uint32_t, uint32_t> builder;
    
    for (int i = 0; i < 20; i++) {
        uint32_t key = 1u << i;
        builder.Add(key, i);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    if (success) {
        auto stats = builder.GetBuildStats();
        printf("  ✓ 2的幂次键: 成功\n");
        printf("    构建时间: %lld μs\n", duration);
        printf("    重试次数: %d\n", stats.retry_count);
        printf("    空间开销: %.1f%%\n", stats.space_overhead);
    } else {
        printf("  ✗ 2的幂次键测试失败\n");
    }
}

// Test with clustered keys
void TestClusteredKeys() {
    printf("\n[测试: 聚集键模式]\n");
    
    StaticPerfectHashMapBuilder<uint32_t, uint32_t> builder;
    
    // Add clusters of keys
    for (int cluster = 0; cluster < 10; cluster++) {
        uint32_t base = cluster * 1000000;
        for (int i = 0; i < 10; i++) {
            builder.Add(base + i, cluster * 10 + i);
        }
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    if (success) {
        auto stats = builder.GetBuildStats();
        printf("  ✓ 聚集键: 成功\n");
        printf("    构建时间: %lld μs\n", duration);
        printf("    重试次数: %d\n", stats.retry_count);
        printf("    空间开销: %.1f%%\n", stats.space_overhead);
    } else {
        printf("  ✗ 聚集键测试失败\n");
    }
}

// Test with sparse keys
void TestSparseKeys() {
    printf("\n[测试: 稀疏键模式]\n");
    
    StaticPerfectHashMapBuilder<uint32_t, uint32_t> builder;
    std::mt19937 gen(123);
    std::uniform_int_distribution<uint32_t> dis(0, 0xFFFFFFFF);
    
    std::unordered_set<uint32_t> keys;
    while (keys.size() < 100) {
        keys.insert(dis(gen));
    }
    
    for (uint32_t key : keys) {
        builder.Add(key, key);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    if (success) {
        auto stats = builder.GetBuildStats();
        printf("  ✓ 稀疏键: 成功\n");
        printf("    构建时间: %lld μs\n", duration);
        printf("    重试次数: %d\n", stats.retry_count);
        printf("    空间开销: %.1f%%\n", stats.space_overhead);
    } else {
        printf("  ✗ 稀疏键测试失败\n");
    }
}

// Test with max values
void TestMaxValues() {
    printf("\n[测试: 极值键]\n");
    
    StaticPerfectHashMapBuilder<uint32_t, uint32_t> builder;
    
    // Add extreme values
    builder.Add(0, 0);
    builder.Add(1, 1);
    builder.Add(0xFFFFFFFF, 2);
    builder.Add(0xFFFFFFFE, 3);
    builder.Add(0x80000000, 4);
    builder.Add(0x7FFFFFFF, 5);
    
    auto start = std::chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    if (success) {
        auto stats = builder.GetBuildStats();
        printf("  ✓ 极值键: 成功\n");
        printf("    构建时间: %lld μs\n", duration);
        printf("    重试次数: %d\n", stats.retry_count);
        printf("    空间开销: %.1f%%\n", stats.space_overhead);
        
        // Verify
        builder.SaveToFile("test_maxval.mph");
        StaticPerfectHashMap<uint32_t, uint32_t> map;
        map.LoadFromFile("test_maxval.mph");
        
        bool all_found = true;
        all_found &= (map.Get(0) != nullptr && *map.Get(0) == 0);
        all_found &= (map.Get(1) != nullptr && *map.Get(1) == 1);
        all_found &= (map.Get(0xFFFFFFFF) != nullptr && *map.Get(0xFFFFFFFF) == 2);
        all_found &= (map.Get(0xFFFFFFFE) != nullptr && *map.Get(0xFFFFFFFE) == 3);
        all_found &= (map.Get(0x80000000) != nullptr && *map.Get(0x80000000) == 4);
        all_found &= (map.Get(0x7FFFFFFF) != nullptr && *map.Get(0x7FFFFFFF) == 5);
        
        printf("    验证: %s\n", all_found ? "✓ 通过" : "✗ 失败");
    } else {
        printf("  ✗ 极值键测试失败\n");
    }
}

// Test multiple build/save/load cycles
void TestMultipleCycles() {
    printf("\n[测试: 多次构建/加载循环]\n");
    
    const int cycles = 10;
    const int keys_per_cycle = 50;
    
    for (int cycle = 0; cycle < cycles; cycle++) {
        StaticPerfectHashSetBuilder<uint32_t> builder;
        
        for (int i = 0; i < keys_per_cycle; i++) {
            builder.Add(cycle * 1000 + i);
        }
        
        if (!builder.Build()) {
            printf("  ✗ 循环 %d: 构建失败\n", cycle);
            return;
        }
        
        char filename[64];
        snprintf(filename, sizeof(filename), "test_cycle_%d.mphs", cycle);
        builder.SaveToFile(filename);
        
        StaticPerfectHashSet<uint32_t> set;
        set.LoadFromFile(filename);
        
        // Verify
        bool all_found = true;
        for (int i = 0; i < keys_per_cycle; i++) {
            if (!set.Contains(cycle * 1000 + i)) {
                all_found = false;
                break;
            }
        }
        
        if (!all_found) {
            printf("  ✗ 循环 %d: 验证失败\n", cycle);
            return;
        }
    }
    
    printf("  ✓ %d 个循环全部成功\n", cycles);
}

// Test concurrent builds (simulate multiple builders)
void TestConcurrentBuilds() {
    printf("\n[测试: 并发构建模拟]\n");
    
    const int num_builders = 5;
    bool all_success = true;
    
    for (int b = 0; b < num_builders; b++) {
        StaticPerfectHashMapBuilder<uint32_t, uint32_t> builder;
        
        for (int i = 0; i < 100; i++) {
            builder.Add(b * 10000 + i, i);
        }
        
        if (!builder.Build()) {
            printf("  ✗ 构建器 %d 失败\n", b);
            all_success = false;
        }
    }
    
    if (all_success) {
        printf("  ✓ %d 个独立构建器全部成功\n", num_builders);
    }
}

int main() {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  StaticPerfectHash 压力测试              ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    
    TestSequentialKeys();
    TestPowerOf2Keys();
    TestClusteredKeys();
    TestSparseKeys();
    TestMaxValues();
    TestMultipleCycles();
    TestConcurrentBuilds();
    
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  压力测试完成！                          ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    
    return 0;
}
