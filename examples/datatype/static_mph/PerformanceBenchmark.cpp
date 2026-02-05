/**
 * Performance Benchmark for StaticPerfectHash
 * Comprehensive performance testing and comparison
 */

#include <hgl/type/StaticPerfectHashMap.h>
#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <hgl/type/StaticPerfectHashSet.h>
#include <hgl/type/StaticPerfectHashSetBuilder.h>
#include <iostream>
#include <chrono>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace hgl;

struct BenchmarkResult {
    int64_t build_time_us;
    int64_t load_time_us;
    int64_t lookup_time_ns;
    int64_t miss_lookup_time_ns;
    float space_overhead;
    int retry_count;
};

BenchmarkResult BenchmarkMap(int num_keys) {
    BenchmarkResult result = {0};
    
    // Generate keys
    std::mt19937 gen(42);
    std::uniform_int_distribution<uint32_t> dis(1, 1000000000);
    std::unordered_set<uint32_t> unique_keys;
    while (unique_keys.size() < (size_t)num_keys) {
        unique_keys.insert(dis(gen));
    }
    
    // Build
    StaticPerfectHashMapBuilder<uint32_t, uint64_t> builder;
    for (uint32_t key : unique_keys) {
        builder.Add(key, (uint64_t)key * 2);
    }
    
    auto build_start = std::chrono::high_resolution_clock::now();
    builder.Build();
    auto build_end = std::chrono::high_resolution_clock::now();
    result.build_time_us = std::chrono::duration_cast<std::chrono::microseconds>(build_end - build_start).count();
    
    auto stats = builder.GetBuildStats();
    result.space_overhead = stats.space_overhead;
    result.retry_count = stats.retry_count;
    
    // Save/Load
    builder.SaveToFile("benchmark_map.mph");
    StaticPerfectHashMap<uint32_t, uint64_t> map;
    
    auto load_start = std::chrono::high_resolution_clock::now();
    map.LoadFromFile("benchmark_map.mph");
    auto load_end = std::chrono::high_resolution_clock::now();
    result.load_time_us = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start).count();
    
    // Benchmark lookups (hits)
    std::vector<uint32_t> test_keys(unique_keys.begin(), unique_keys.end());
    const int iterations = 1000000;
    
    auto lookup_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        uint32_t key = test_keys[i % test_keys.size()];
        const uint64_t* val = map.Get(key);
        (void)val;
    }
    auto lookup_end = std::chrono::high_resolution_clock::now();
    result.lookup_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(lookup_end - lookup_start).count() / iterations;
    
    // Benchmark lookups (misses)
    auto miss_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        uint32_t key = dis(gen);
        if (unique_keys.find(key) == unique_keys.end()) {
            const uint64_t* val = map.Get(key);
            (void)val;
        }
    }
    auto miss_end = std::chrono::high_resolution_clock::now();
    result.miss_lookup_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(miss_end - miss_start).count() / iterations;
    
    return result;
}

BenchmarkResult BenchmarkSet(int num_keys) {
    BenchmarkResult result = {0};
    
    // Generate keys
    std::mt19937 gen(42);
    std::uniform_int_distribution<uint32_t> dis(1, 1000000000);
    std::unordered_set<uint32_t> unique_keys;
    while (unique_keys.size() < (size_t)num_keys) {
        unique_keys.insert(dis(gen));
    }
    
    // Build
    StaticPerfectHashSetBuilder<uint32_t> builder;
    for (uint32_t key : unique_keys) {
        builder.Add(key);
    }
    
    auto build_start = std::chrono::high_resolution_clock::now();
    builder.Build();
    auto build_end = std::chrono::high_resolution_clock::now();
    result.build_time_us = std::chrono::duration_cast<std::chrono::microseconds>(build_end - build_start).count();
    
    auto stats = builder.GetBuildStats();
    result.space_overhead = stats.space_overhead;
    result.retry_count = stats.retry_count;
    
    // Save/Load
    builder.SaveToFile("benchmark_set.mphs");
    StaticPerfectHashSet<uint32_t> set;
    
    auto load_start = std::chrono::high_resolution_clock::now();
    set.LoadFromFile("benchmark_set.mphs");
    auto load_end = std::chrono::high_resolution_clock::now();
    result.load_time_us = std::chrono::duration_cast<std::chrono::microseconds>(load_end - load_start).count();
    
    // Benchmark lookups (hits)
    std::vector<uint32_t> test_keys(unique_keys.begin(), unique_keys.end());
    const int iterations = 1000000;
    
    auto lookup_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        uint32_t key = test_keys[i % test_keys.size()];
        bool contains = set.Contains(key);
        (void)contains;
    }
    auto lookup_end = std::chrono::high_resolution_clock::now();
    result.lookup_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(lookup_end - lookup_start).count() / iterations;
    
    // Benchmark lookups (misses)
    auto miss_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        uint32_t key = dis(gen);
        if (unique_keys.find(key) == unique_keys.end()) {
            bool contains = set.Contains(key);
            (void)contains;
        }
    }
    auto miss_end = std::chrono::high_resolution_clock::now();
    result.miss_lookup_time_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(miss_end - miss_start).count() / iterations;
    
    return result;
}

void CompareWithSTL(int num_keys) {
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  与STL对比 (%d 键)                      \n", num_keys);
    printf("╚══════════════════════════════════════════╝\n\n");
    
    // Generate keys
    std::mt19937 gen(42);
    std::uniform_int_distribution<uint32_t> dis(1, 1000000000);
    std::unordered_set<uint32_t> unique_keys;
    while (unique_keys.size() < (size_t)num_keys) {
        unique_keys.insert(dis(gen));
    }
    
    // Benchmark std::unordered_map
    std::unordered_map<uint32_t, uint64_t> std_map;
    auto std_build_start = std::chrono::high_resolution_clock::now();
    for (uint32_t key : unique_keys) {
        std_map[key] = (uint64_t)key * 2;
    }
    auto std_build_end = std::chrono::high_resolution_clock::now();
    auto std_build_us = std::chrono::duration_cast<std::chrono::microseconds>(std_build_end - std_build_start).count();
    
    std::vector<uint32_t> test_keys(unique_keys.begin(), unique_keys.end());
    const int iterations = 1000000;
    
    auto std_lookup_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        uint32_t key = test_keys[i % test_keys.size()];
        auto it = std_map.find(key);
        (void)it;
    }
    auto std_lookup_end = std::chrono::high_resolution_clock::now();
    auto std_lookup_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(std_lookup_end - std_lookup_start).count() / iterations;
    
    // Benchmark StaticPerfectHashMap
    auto mph_result = BenchmarkMap(num_keys);
    
    printf("[std::unordered_map]\n");
    printf("  构建时间: %lld μs\n", std_build_us);
    printf("  查找时间: %lld ns\n", std_lookup_ns);
    
    printf("\n[StaticPerfectHashMap]\n");
    printf("  构建时间: %lld μs\n", mph_result.build_time_us);
    printf("  加载时间: %lld μs\n", mph_result.load_time_us);
    printf("  查找时间: %lld ns (命中)\n", mph_result.lookup_time_ns);
    printf("  查找时间: %lld ns (未命中)\n", mph_result.miss_lookup_time_ns);
    printf("  空间开销: %.1f%%\n", mph_result.space_overhead);
    printf("  重试次数: %d\n", mph_result.retry_count);
    
    printf("\n[性能对比]\n");
    printf("  构建速度: %.2fx (STL 更快)\n", (double)mph_result.build_time_us / std_build_us);
    printf("  查找速度: %.2fx (StaticMPH %s)\n", 
           (double)std_lookup_ns / mph_result.lookup_time_ns,
           mph_result.lookup_time_ns < std_lookup_ns ? "更快" : "更慢");
    printf("  加载优势: %.0fx (StaticMPH 极快)\n", (double)std_build_us / mph_result.load_time_us);
}

int main() {
    printf("╔══════════════════════════════════════════╗\n");
    printf("║  StaticPerfectHash 性能基准测试          ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    
    // Test different scales
    int scales[] = {50, 100, 200, 500, 1000};
    
    printf("\n═══════════════════════════════════════════\n");
    printf("Map 性能测试\n");
    printf("═══════════════════════════════════════════\n");
    printf("%-10s %-12s %-12s %-12s %-12s %-10s %s\n", 
           "键数", "构建(μs)", "加载(μs)", "查找(ns)", "未中(ns)", "空间(%)", "重试");
    printf("───────────────────────────────────────────\n");
    
    for (int scale : scales) {
        auto result = BenchmarkMap(scale);
        printf("%-10d %-12lld %-12lld %-12lld %-12lld %-10.1f %d\n",
               scale, result.build_time_us, result.load_time_us,
               result.lookup_time_ns, result.miss_lookup_time_ns,
               result.space_overhead, result.retry_count);
    }
    
    printf("\n═══════════════════════════════════════════\n");
    printf("Set 性能测试\n");
    printf("═══════════════════════════════════════════\n");
    printf("%-10s %-12s %-12s %-12s %-12s %-10s %s\n", 
           "键数", "构建(μs)", "加载(μs)", "查找(ns)", "未中(ns)", "空间(%)", "重试");
    printf("───────────────────────────────────────────\n");
    
    for (int scale : scales) {
        auto result = BenchmarkSet(scale);
        printf("%-10d %-12lld %-12lld %-12lld %-12lld %-10.1f %d\n",
               scale, result.build_time_us, result.load_time_us,
               result.lookup_time_ns, result.miss_lookup_time_ns,
               result.space_overhead, result.retry_count);
    }
    
    // Compare with STL
    CompareWithSTL(100);
    CompareWithSTL(500);
    
    printf("\n╔══════════════════════════════════════════╗\n");
    printf("║  性能基准测试完成！                      ║\n");
    printf("╚══════════════════════════════════════════╝\n");
    
    return 0;
}
