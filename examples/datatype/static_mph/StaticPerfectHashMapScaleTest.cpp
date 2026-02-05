/**
 * @file StaticPerfectHashMapScaleTest.cpp
 * @brief 静态完美哈希 - 规模测试
 * 
 * 测试不同数据规模下的成功率和性能
 */
#include <hgl/type/StaticPerfectHashMapBuilder.h>
#include <hgl/platform/Platform.h>
#include <iostream>
#include <chrono>
#include <iomanip>

using namespace hgl;
using namespace std;

struct TestData
{
    uint32_t value1;
    uint32_t value2;
    uint32_t value3;
    uint32_t value4;
};

bool TestScale(int num_keys, bool verbose = false)
{
    StaticPerfectHashMapBuilder<uint32_t, TestData> builder;
    
    // 添加数据
    for (int i = 0; i < num_keys; ++i)
    {
        uint32_t key = 10000 + i;
        TestData data;
        data.value1 = i * 100;
        data.value2 = i * 200;
        data.value3 = i * 300;
        data.value4 = i * 400;
        builder.Add(key, data);
    }
    
    // 构建
    auto build_start = chrono::high_resolution_clock::now();
    bool success = builder.Build();
    auto build_end = chrono::high_resolution_clock::now();
    auto build_us = chrono::duration_cast<chrono::microseconds>(build_end - build_start).count();
    
    if (verbose)
    {
        if (success)
        {
            auto stats = builder.GetStats();
            cout << "  ✓ " << num_keys << " 键: 成功! 构建时间=" << build_us << "μs, "
                 << "桶数=" << stats.num_buckets << ", "
                 << "空间开销=" << fixed << setprecision(1) 
                 << stats.space_overhead << "%" << endl;
        }
        else
        {
            cout << "  ✗ " << num_keys << " 键: 失败! (构建用时" << build_us << "μs)" << endl;
        }
    }
    
    return success;
}

int os_main(int, os_char**)
{
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║   静态完美哈希 - 规模测试                               ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝" << endl;
    
    cout << "\n[测试说明]" << endl;
    cout << "  当前实现使用简化的CHD算法" << endl;
    cout << "  哈希函数独立性限制了可支持的规模" << endl;
    cout << "  测试不同规模以找出实际边界...\n" << endl;
    
    // 测试不同规模
    vector<int> test_scales = {
        3, 5, 7, 8, 9, 10, 12, 15, 20, 25, 30, 40, 50, 
        75, 100, 150, 200, 500, 1000
    };
    
    cout << "[规模测试结果]" << endl;
    
    int max_success = 0;
    int first_failure = -1;
    
    for (int scale : test_scales)
    {
        bool success = TestScale(scale, true);
        
        if (success)
        {
            max_success = scale;
        }
        else
        {
            if (first_failure == -1)
            {
                first_failure = scale;
            }
        }
        
        // 连续3次失败就停止
        if (first_failure > 0 && scale >= first_failure + 30)
        {
            cout << "  ... (后续规模预计都会失败)" << endl;
            break;
        }
    }
    
    cout << "\n[测试总结]" << endl;
    cout << "  最大成功规模: " << max_success << " 键" << endl;
    if (first_failure > 0)
    {
        cout << "  首次失败规模: " << first_failure << " 键" << endl;
    }
    
    // 性能测试（使用最大成功规模）
    if (max_success >= 5)
    {
        cout << "\n[性能测试] 使用 " << max_success << " 键规模..." << endl;
        
        StaticPerfectHashMapBuilder<uint32_t, TestData> builder;
        for (int i = 0; i < max_success; ++i)
        {
            uint32_t key = 10000 + i;
            TestData data{i*100, i*200, i*300, i*400};
            builder.Add(key, data);
        }
        
        builder.Build();
        
        // 序列化
        vector<uint8_t> buffer;
        auto save_start = chrono::high_resolution_clock::now();
        builder.SaveToMemory(buffer);
        auto save_end = chrono::high_resolution_clock::now();
        auto save_us = chrono::duration_cast<chrono::microseconds>(save_end - save_start).count();
        
        // 加载
        StaticPerfectHashMap<uint32_t, TestData> map;
        auto load_start = chrono::high_resolution_clock::now();
        map.LoadFromMemory(buffer.data(), buffer.size());
        auto load_end = chrono::high_resolution_clock::now();
        auto load_us = chrono::duration_cast<chrono::microseconds>(load_end - load_start).count();
        
        cout << "  保存时间: " << save_us << "μs" << endl;
        cout << "  加载时间: " << load_us << "μs" << endl;
        cout << "  数据大小: " << fixed << setprecision(2) 
             << buffer.size() / 1024.0 << " KB" << endl;
        
        // 查找性能
        auto lookup_start = chrono::high_resolution_clock::now();
        int found_count = 0;
        const int LOOKUP_ITERATIONS = 10000;
        for (int iter = 0; iter < LOOKUP_ITERATIONS; ++iter)
        {
            for (int i = 0; i < max_success; ++i)
            {
                uint32_t key = 10000 + i;
                const TestData* data = map.Get(key);
                if (data != nullptr)
                {
                    found_count++;
                }
            }
        }
        auto lookup_end = chrono::high_resolution_clock::now();
        auto lookup_ns = chrono::duration_cast<chrono::nanoseconds>(lookup_end - lookup_start).count();
        auto avg_lookup_ns = lookup_ns / (LOOKUP_ITERATIONS * max_success);
        
        cout << "  查找测试: " << LOOKUP_ITERATIONS << "×" << max_success 
             << "次, 找到=" << found_count << endl;
        cout << "  平均查找: " << avg_lookup_ns << " ns/次" << endl;
    }
    
    cout << "\n[改进建议]" << endl;
    if (max_success < 20)
    {
        cout << "  ⚠️  当前规模过小，建议：" << endl;
        cout << "     1. 使用更独立的哈希函数（MurmurHash3）" << endl;
        cout << "     2. 或集成成熟的MPH库（PTHash, BBHash）" << endl;
    }
    else if (max_success < 100)
    {
        cout << "  ✓  中等规模可用，改进后可支持更大规模" << endl;
    }
    else
    {
        cout << "  ✓✓ 当前实现表现良好！" << endl;
    }
    
    cout << "\n╔════════════════════════════════════════════════════════╗" << endl;
    cout << "║   ✓ 规模测试完成                                        ║" << endl;
    cout << "╚════════════════════════════════════════════════════════╝\n" << endl;
    
    return 0;
}
