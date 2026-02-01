#include<hgl/type/ConstStringSet.h>
#include<iostream>
#include<chrono>
#include<cstring>

using namespace hgl;
using namespace std;

// 测试辅助宏
#define TEST_ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::cerr << "❌ FAILED: " << message << std::endl; \
            return false; \
        } \
    } while(0)

#define TEST_PASS(message) \
    std::cout << "✓ PASSED: " << message << std::endl

// ==================== 1. 大规模添加测试 (1万条) ====================
bool TestLargeScaleAddition()
{
    std::cout << "\n========== Test 1: Large Scale Addition (10,000 strings) ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_STRINGS = 10000;

    auto start = chrono::steady_clock::now();

    // 生成唯一字符串并添加
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "string_unique_id_%d_data_%d", i, i * 2);
        int id = css.Add(buffer, len);
        
        if(id != i)
        {
            cerr << "ERROR: Expected ID " << i << " but got " << id << endl;
            return false;
        }
        
        if(i % 1000 == 0)
        {
            cout << "  Added " << i << " strings..." << endl;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    TEST_ASSERT(css.GetCount() == NUM_STRINGS, "Should have all 10000 strings");
    cout << "✓ Added " << NUM_STRINGS << " unique strings in " << duration.count() << " ms" << endl;
    cout << "  Total length: " << css.GetTotalLength() << " bytes" << endl;
    TEST_PASS("Large scale addition");

    return true;
}

// ==================== 2. 大规模去重测试 ====================
bool TestLargeScaleDeduplication()
{
    std::cout << "\n========== Test 2: Large Scale Deduplication ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_UNIQUE = 10000;
    const int NUM_DUPLICATES = 50000;

    auto start = chrono::steady_clock::now();

    // 先添加10000个唯一字符串
    for(int i = 0; i < NUM_UNIQUE; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "unique_string_%d", i);
        css.Add(buffer, len);
    }

    // 然后重复添加50000次（随机索引）
    for(int i = 0; i < NUM_DUPLICATES; i++)
    {
        int random_idx = i % NUM_UNIQUE;  // 循环使用0-9999的索引
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "unique_string_%d", random_idx);
        int id = css.Add(buffer, len);
        
        // 每次添加都应该返回相同的ID
        TEST_ASSERT(id == random_idx, "Duplicate should return same ID");
        
        if(i % 10000 == 0)
        {
            cout << "  Processed " << i << " duplicate attempts..." << endl;
        }
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    TEST_ASSERT(css.GetCount() == NUM_UNIQUE, "Count should still be 10000");
    cout << "✓ Processed " << NUM_DUPLICATES << " duplicate attempts in " << duration.count() << " ms" << endl;
    TEST_PASS("Large scale deduplication");

    return true;
}

// ==================== 3. 大规模查询测试 ====================
bool TestLargeScaleQuery()
{
    std::cout << "\n========== Test 3: Large Scale Query ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_STRINGS = 10000;

    // 准备数据
    cout << "Preparing data..." << endl;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "query_test_string_%d", i);
        css.Add(buffer, len);
    }

    auto start = chrono::steady_clock::now();

    // 查询所有字符串
    int found_count = 0;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "query_test_string_%d", i);
        
        if(css.Contains(buffer, len))
        {
            found_count++;
        }
        
        // 验证GetID
        int id = css.GetID(buffer, len);
        TEST_ASSERT(id == i, "GetID should return correct index");
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    TEST_ASSERT(found_count == NUM_STRINGS, "Should find all 10000 strings");
    cout << "✓ Queried " << NUM_STRINGS << " strings in " << duration.count() << " ms" << endl;
    cout << "  Average query time: " << (double)duration.count() / NUM_STRINGS << " ms/query" << endl;
    TEST_PASS("Large scale query");

    return true;
}

// ==================== 4. 大规模迭代器测试 ====================
bool TestLargeScaleIterator()
{
    std::cout << "\n========== Test 4: Large Scale Iterator ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_STRINGS = 10000;

    // 准备数据
    cout << "Preparing data..." << endl;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "iter_test_string_%d", i);
        css.Add(buffer, len);
    }

    auto start = chrono::steady_clock::now();

    // 通过迭代器遍历
    int count = 0;
    for(auto it = css.begin(); it != css.end(); ++it)
    {
        TEST_ASSERT(it->id == count, "Iterator ID should match order");
        count++;
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    TEST_ASSERT(count == NUM_STRINGS, "Should iterate over all 10000 strings");
    cout << "✓ Iterated " << NUM_STRINGS << " strings in " << duration.count() << " ms" << endl;
    TEST_PASS("Large scale iterator");

    return true;
}

// ==================== 5. 大规模Range-for循环测试 ====================
bool TestLargeScaleRangeFor()
{
    std::cout << "\n========== Test 5: Large Scale Range-for Loop ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_STRINGS = 10000;

    // 准备数据
    cout << "Preparing data..." << endl;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "rangefor_test_%d", i);
        css.Add(buffer, len);
    }

    auto start = chrono::steady_clock::now();

    // 使用Range-for循环
    int count = 0;
    for(const auto& view : css)
    {
        TEST_ASSERT(view.id == count, "Range-for ID should match order");
        count++;
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    TEST_ASSERT(count == NUM_STRINGS, "Should iterate over all 10000 strings");
    cout << "✓ Range-for looped " << NUM_STRINGS << " strings in " << duration.count() << " ms" << endl;
    TEST_PASS("Large scale range-for loop");

    return true;
}

// ==================== 6. 混合操作压力测试 ====================
bool TestMixedOperationsStress()
{
    std::cout << "\n========== Test 6: Mixed Operations Stress Test ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_UNIQUE = 5000;
    const int NUM_DUPLICATE_ADDS = 10000;
    const int NUM_QUERIES = 5000;

    auto start = chrono::steady_clock::now();

    // Phase 1: 添加5000个唯一字符串
    cout << "Phase 1: Adding 5000 unique strings..." << endl;
    for(int i = 0; i < NUM_UNIQUE; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "mixed_stress_%d", i);
        css.Add(buffer, len);
    }

    // Phase 2: 重复添加10000次
    cout << "Phase 2: Adding 10000 duplicates..." << endl;
    for(int i = 0; i < NUM_DUPLICATE_ADDS; i++)
    {
        int idx = i % NUM_UNIQUE;
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "mixed_stress_%d", idx);
        css.Add(buffer, len);
    }

    // Phase 3: 查询5000次
    cout << "Phase 3: Querying 5000 times..." << endl;
    for(int i = 0; i < NUM_QUERIES; i++)
    {
        int idx = i % NUM_UNIQUE;
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "mixed_stress_%d", idx);
        int id = css.GetID(buffer, len);
        TEST_ASSERT(id == idx, "Query should return correct ID");
    }

    // Phase 4: 遍历所有元素
    cout << "Phase 4: Iterating all elements..." << endl;
    int count = 0;
    for(const auto& view : css)
    {
        count++;
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    TEST_ASSERT(css.GetCount() == NUM_UNIQUE, "Should have 5000 strings");
    TEST_ASSERT(count == NUM_UNIQUE, "Should iterate 5000 strings");
    cout << "✓ All operations completed in " << duration.count() << " ms" << endl;
    TEST_PASS("Mixed operations stress test");

    return true;
}

// ==================== 7. GetStringView大规模测试 ====================
bool TestLargeScaleGetStringView()
{
    std::cout << "\n========== Test 7: Large Scale GetStringView ==========" << std::endl;

    ConstAnsiStringSet css;
    const int NUM_STRINGS = 10000;

    // 准备数据
    cout << "Preparing data..." << endl;
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        char buffer[64];
        int len = snprintf(buffer, sizeof(buffer), "view_test_%d", i);
        css.Add(buffer, len);
    }

    auto start = chrono::steady_clock::now();

    // 获取所有StringView
    for(int i = 0; i < NUM_STRINGS; i++)
    {
        const ConstAnsiStringView* view = css.GetStringView(i);
        TEST_ASSERT(view != nullptr, "GetStringView should not return null");
        TEST_ASSERT(view->id == i, "View ID should match");
    }

    auto end = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);

    cout << "✓ GetStringView called " << NUM_STRINGS << " times in " << duration.count() << " ms" << endl;
    TEST_PASS("Large scale GetStringView");

    return true;
}

// ==================== 主测试运行器 ====================
int main(int, char**)
{
    std::cout << "╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  ConstStringSet Large Scale Test Suite             ║" << std::endl;
    std::cout << "║  Testing with 10,000+ strings                      ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;

    int passed = 0;
    int failed = 0;

    struct Test {
        const char* name;
        bool (*func)();
    };

    Test tests[] = {
        {"Large Scale Addition (10k)", TestLargeScaleAddition},
        {"Large Scale Deduplication", TestLargeScaleDeduplication},
        {"Large Scale Query", TestLargeScaleQuery},
        {"Large Scale Iterator", TestLargeScaleIterator},
        {"Large Scale Range-for", TestLargeScaleRangeFor},
        {"Mixed Operations Stress", TestMixedOperationsStress},
        {"Large Scale GetStringView", TestLargeScaleGetStringView}
    };

    for(const auto& test : tests)
    {
        try
        {
            if(test.func())
            {
                passed++;
                std::cout << "✓ " << test.name << " - ALL PASSED" << std::endl;
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

    std::cout << "\n╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║             Large Scale Test Summary               ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════════╣" << std::endl;
    std::cout << "  Total Tests:  " << (passed + failed) << std::endl;
    std::cout << "  Passed:       " << passed << " ✓" << std::endl;
    std::cout << "  Failed:       " << failed << " ✗" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl;

    return (failed == 0) ? 0 : 1;
}
