#include<hgl/type/IDName.h>
#include<iostream>
#include<chrono>
#include<vector>
#include<cstring>
#include<random>

using namespace hgl;
using namespace std;

// 定义测试用的 IDName 类型
HGL_DEFINE_ANSI_IDNAME(StressTestIDName)

// ==================== 1. 大规模添加测试 (10万条) ====================
bool TestLargeScaleAddition()
{
    std::cout << "\n========== Test 1: Large Scale Addition (100,000 strings) ==========" << std::endl;

    const int NUM_STRINGS = 100000;
    auto start = chrono::steady_clock::now();

    // 生成唯一字符串并添加
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "stress_unique_str_%d", i);
        StressTestIDName id(buffer, len);
        
        if(id.GetID() != i)
        {
            cerr << "ERROR: Expected ID " << i << " but got " << id.GetID() << endl;
            return false;
        }
        
        if(i % 10000 == 0)
        {
            cout << "  Added " << i << " strings..." << endl;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Added " << NUM_STRINGS << " unique strings in " << duration.count() << " ms" << endl;
    cout << "  Average time per creation: " << (double)duration.count() / NUM_STRINGS * 1000 << " us" << endl;
    
    return true;
}

// ==================== 2. 大规模去重测试 ====================
bool TestLargeScaleDeduplication()
{
    std::cout << "\n========== Test 2: Large Scale Deduplication ==========" << std::endl;

    const int NUM_UNIQUE = 100000;
    const int NUM_ATTEMPTS = 500000;

    auto start = chrono::steady_clock::now();

    // 记录不同字符串的ID
    vector<int> ids;
    ids.reserve(NUM_UNIQUE);

    // Phase 1: 添加100,000个唯一字符串
    cout << "Phase 1: Adding " << NUM_UNIQUE << " unique strings..." << endl;
    for(int i = 0; i < NUM_UNIQUE; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "dedup_unique_%d", i);
        StressTestIDName id(buffer, len);
        ids.push_back(id.GetID());
    }

    // Phase 2: 重复添加500,000次
    cout << "Phase 2: Attempting " << NUM_ATTEMPTS << " duplicate additions..." << endl;
    mt19937 rng(12345);
    uniform_int_distribution<int> dist(0, NUM_UNIQUE - 1);

    int dedup_count = 0;
    for(int i = 0; i < NUM_ATTEMPTS; i++)
    {
        int random_idx = dist(rng);
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "dedup_unique_%d", random_idx);
        StressTestIDName id(buffer, len);
        
        if(id.GetID() == ids[random_idx])
        {
            dedup_count++;
        }
        
        if(i % 50000 == 0)
        {
            cout << "  Processed " << i << " attempts..." << endl;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Deduplication verified: " << dedup_count << "/" << NUM_ATTEMPTS << " matched existing IDs" << endl;
    cout << "  Total time: " << duration.count() << " ms" << endl;
    
    return dedup_count == NUM_ATTEMPTS;
}

// ==================== 3. 大规模比较测试 ====================
bool TestLargeScaleComparison()
{
    std::cout << "\n========== Test 3: Large Scale Comparison (100,000 objects) ==========" << std::endl;

    const int NUM_OBJECTS = 100000;
    vector<StressTestIDName> objects;
    objects.reserve(NUM_OBJECTS);

    // 创建100,000个对象
    cout << "Creating " << NUM_OBJECTS << " IDName objects..." << endl;
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "compare_test_%d", i);
        objects.emplace_back(buffer, len);
    }

    auto start = chrono::steady_clock::now();

    // 进行大量比较操作
    int comparison_count = 0;
    for(int i = 0; i < NUM_OBJECTS; i++)
    {
        for(int j = i + 1; j < NUM_OBJECTS; j += 1000)  // 每1000个比较一次
        {
            if(objects[i] < objects[j])
            {
                comparison_count++;
            }
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Performed " << comparison_count << " comparisons in " << duration.count() << " ms" << endl;
    cout << "  Average comparison time: " << (double)duration.count() / comparison_count * 1000 << " us" << endl;
    
    return true;
}

// ==================== 4. 大规模赋值测试 ====================
bool TestLargeScaleAssignment()
{
    std::cout << "\n========== Test 4: Large Scale Assignment ==========" << std::endl;

    const int NUM_UNIQUE = 50000;
    const int NUM_ASSIGNMENTS = 500000;

    // 预先创建50,000个唯一ID
    vector<StressTestIDName> templates;
    templates.reserve(NUM_UNIQUE);

    cout << "Creating " << NUM_UNIQUE << " template objects..." << endl;
    for(int i = 0; i < NUM_UNIQUE; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "template_%d", i);
        templates.emplace_back(buffer, len);
    }

    auto start = chrono::steady_clock::now();

    // 进行大量赋值操作
    cout << "Performing " << NUM_ASSIGNMENTS << " assignments..." << endl;
    mt19937 rng(54321);
    uniform_int_distribution<int> dist(0, NUM_UNIQUE - 1);

    vector<StressTestIDName> assignments;
    for(int i = 0; i < NUM_ASSIGNMENTS; i++)
    {
        int idx = dist(rng);
        StressTestIDName id;
        id = templates[idx];
        assignments.push_back(id);
        
        if(i % 50000 == 0)
        {
            cout << "  Completed " << i << " assignments..." << endl;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Performed " << NUM_ASSIGNMENTS << " assignments in " << duration.count() << " ms" << endl;
    cout << "  Average assignment time: " << (double)duration.count() / NUM_ASSIGNMENTS * 1000 << " us" << endl;
    
    return true;
}

// ==================== 5. 大规模查询测试 ====================
bool TestLargeScaleQuery()
{
    std::cout << "\n========== Test 5: Large Scale Query (100,000 strings) ==========" << std::endl;

    const int NUM_STRINGS = 100000;
    using Registry = IDNameRegistry<IDName_StressTestIDName_Manager, char>;

    // 准备数据 - 先清空Registry确保测试隔离
    cout << "Clearing registry and preparing " << NUM_STRINGS << " strings..." << endl;
    Registry::Clear();
    
    vector<int> registered_ids;
    registered_ids.reserve(NUM_STRINGS);
    
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "query_test_string_%d", i);
        StressTestIDName id(buffer, len);
        registered_ids.push_back(id.GetID());
    }

    auto start = chrono::steady_clock::now();

    // 查询所有字符串
    cout << "Querying all strings..." << endl;
    int found_count = 0;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "query_test_string_%d", i);
        
        int id = Registry::GetID(buffer, len);
        
        // 验证ID存在且与注册时相同
        if(id >= 0 && id == registered_ids[i])
        {
            found_count++;
        }
        
        if(i % 10000 == 0)
        {
            cout << "  Queried " << i << " strings..." << endl;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Found " << found_count << "/" << NUM_STRINGS << " strings in " << duration.count() << " ms" << endl;
    cout << "  Average query time: " << (double)duration.count() / NUM_STRINGS * 1000 << " us" << endl;
    
    return found_count == NUM_STRINGS;
}

// ==================== 6. 混合操作压力测试 ====================
bool TestMixedOperationsStress()
{
    std::cout << "\n========== Test 6: Mixed Operations Stress Test ==========" << std::endl;

    const int PHASE1_COUNT = 50000;   // 添加50,000个唯一字符串
    const int PHASE2_COUNT = 200000;  // 尝试200,000次重复添加
    const int PHASE3_COUNT = 100000;  // 进行100,000次比较

    auto start = chrono::steady_clock::now();

    // Phase 1: 添加50,000个唯一字符串
    cout << "Phase 1: Adding " << PHASE1_COUNT << " unique strings..." << endl;
    vector<StressTestIDName> objects;
    objects.reserve(PHASE1_COUNT);
    
    for(int i = 0; i < PHASE1_COUNT; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "mixed_stress_%d", i);
        objects.emplace_back(buffer, len);
    }

    // Phase 2: 重复添加200,000次
    cout << "Phase 2: Attempting " << PHASE2_COUNT << " duplicate additions..." << endl;
    mt19937 rng(99999);
    uniform_int_distribution<int> dist(0, PHASE1_COUNT - 1);

    int dedup_verified = 0;
    for(int i = 0; i < PHASE2_COUNT; i++)
    {
        int idx = dist(rng);
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "mixed_stress_%d", idx);
        StressTestIDName id(buffer, len);
        
        if(id.GetID() == objects[idx].GetID())
        {
            dedup_verified++;
        }
        
        if(i % 50000 == 0)
        {
            cout << "  Processed " << i << " attempts..." << endl;
        }
    }

    // Phase 3: 进行100,000次比较
    cout << "Phase 3: Performing " << PHASE3_COUNT << " comparisons..." << endl;
    int comparison_count = 0;
    for(int i = 0; i < PHASE3_COUNT; i++)
    {
        int idx1 = dist(rng);
        int idx2 = dist(rng);
        
        if(idx1 != idx2 && objects[idx1] != objects[idx2])
        {
            comparison_count++;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ All operations completed in " << duration.count() << " ms" << endl;
    cout << "  Deduplication verified: " << dedup_verified << "/" << PHASE2_COUNT << endl;
    cout << "  Comparisons performed: " << comparison_count << endl;
    
    return true;
}

// ==================== 7. 大规模容器测试 ====================
bool TestLargeScaleContainer()
{
    std::cout << "\n========== Test 7: Large Scale Container (OrderedSet) ==========" << std::endl;

    const int NUM_ITEMS = 10000;
    using IDNameSet = StressTestIDNameSet;

    auto start = chrono::steady_clock::now();

    // 创建并添加100,000个IDName对象
    cout << "Creating OrderedSet with " << NUM_ITEMS << " items..." << endl;
    IDNameSet set;
    
    for(int i = 0; i < NUM_ITEMS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "container_item_%d", i);
        StressTestIDName id(buffer, len);
        set.Add(id);
        
        if(i % (NUM_ITEMS/10) == 0)
        {
            cout << "  Added " << i << " items..." << endl;
        }
    }

    // 验证容器大小
    if(set.GetCount() != NUM_ITEMS)
    {
        cerr << "ERROR: Expected " << NUM_ITEMS << " items but got " << set.GetCount() << endl;
        return false;
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ Container operations completed in " << duration.count() << " ms" << endl;
    cout << "  Final container size: " << set.GetCount() << " items" << endl;
    
    return true;
}

// ==================== 主测试运行器 ====================
int main(int, char**)
{
    std::cout << "╔═════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  OrderedIDName Large Scale Stress Test Suite        ║" << std::endl;
    std::cout << "║  Testing with 100,000+ strings and operations       ║" << std::endl;
    std::cout << "╚═════════════════════════════════════════════════════╝" << std::endl;

    int passed = 0;
    int failed = 0;

    struct Test {
        const char* name;
        bool (*func)();
    };

    Test tests[] = {
        {"Large Scale Addition (100k)", TestLargeScaleAddition},
        {"Large Scale Deduplication", TestLargeScaleDeduplication},
        {"Large Scale Comparison", TestLargeScaleComparison},
        {"Large Scale Assignment", TestLargeScaleAssignment},
        {"Large Scale Query", TestLargeScaleQuery},
        {"Mixed Operations Stress", TestMixedOperationsStress},
        {"Large Scale Container", TestLargeScaleContainer}
    };

    for(const auto& test : tests)
    {
        try
        {
            if(test.func())
            {
                passed++;
                std::cout << "✓ " << test.name << " - PASSED" << std::endl;
            }
            else
            {
                failed++;
                std::cout << "✗ " << test.name << " - FAILED" << std::endl;
            }
        }
        catch(const std::exception& e)
        {
            failed++;
            std::cout << "✗ " << test.name << " - EXCEPTION: " << e.what() << std::endl;
        }
    }

    std::cout << "\n╔═════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          Stress Test Summary                         ║" << std::endl;
    std::cout << "╠═════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "  Total Tests:  " << (passed + failed) << std::endl;
    std::cout << "  Passed:       " << passed << " ✓" << std::endl;
    std::cout << "  Failed:       " << failed << " ✗" << std::endl;
    std::cout << "╚═════════════════════════════════════════════════════╝" << std::endl;

    return (failed == 0) ? 0 : 1;
}
