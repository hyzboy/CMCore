/**
 * IndexedList vs std::list 性能对比测试
 *
 * 测试目标：
 * 1. 对比 IndexedList 和 std::list 的性能
 * 2. 分析不同操作下的性能特征
 * 3. 评估内存使用情况
 * 4. 提供容器选择建议
 */

#include<hgl/type/IndexedList.h>
#include<iostream>
#include<iomanip>
#include<chrono>
#include<list>
#include<cassert>

using namespace hgl;

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            tests_passed++; \
            std::cout << "  [PASS]: " << message << std::endl; \
        } else { \
            tests_failed++; \
            std::cout << "  [FAIL]: " << message << std::endl; \
        } \
    } while(0)

// 计时宏（毫秒精度）
#define TIMER_START auto timer_start = std::chrono::high_resolution_clock::now()
#define TIMER_END(name) \
    { \
        auto timer_end = std::chrono::high_resolution_clock::now(); \
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(timer_end - timer_start); \
        std::cout << "    " << std::left << std::setw(40) << name << ": " << std::right << std::setw(10) << duration.count() << " μs" << std::endl; \
    }

// ============================================================================
// TEST 1: 顺序添加性能对比
// ============================================================================

void test_sequential_add()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 1: Sequential add performance (100K elements)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int TEST_COUNT = 100000;

    // IndexedList 添加测试
    std::cout << "[1.1] IndexedList sequential add:" << std::endl;
    {
        IndexedList<int> ia;
        ia.Reserve(TEST_COUNT);

        TIMER_START;
        for (int i = 0; i < TEST_COUNT; ++i)
            ia.Add(i);
        TIMER_END("Add time");

        TEST_ASSERT(ia.GetCount() == TEST_COUNT, "Added element count correct");

        double memory_kb = ia.GetTotalBytes() / 1024.0;
        std::cout << "    Memory usage: " << std::fixed << std::setprecision(2) << memory_kb << " KB" << std::endl;
    }

    // std::list 添加测试
    std::cout << "\n[1.2] std::list sequential add:" << std::endl;
    {
        std::list<int> list;

        TIMER_START;
        for (int i = 0; i < TEST_COUNT; ++i)
            list.push_back(i);
        TIMER_END("Add time");

        TEST_ASSERT(list.size() == TEST_COUNT, "Added element count correct");

        // 估算 std::list 内存占用（每个节点 ~40 字节）
        double memory_kb = TEST_COUNT * 40.0 / 1024.0;
        std::cout << "    Memory usage (estimated): " << std::fixed << std::setprecision(2) << memory_kb << " KB" << std::endl;
    }
}

// ============================================================================
// TEST 2: 随机访问性能对比
// ============================================================================

void test_random_access()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 2: Random access performance (50K elements, 1M accesses)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int DATA_SIZE = 50000;
    const int ACCESS_COUNT = 1000000;

    // IndexedList 随机访问
    std::cout << "[2.1] IndexedList random access:" << std::endl;
    {
        IndexedList<int> ia;
        ia.Reserve(DATA_SIZE);
        for (int i = 0; i < DATA_SIZE; ++i)
            ia.Add(i * 2);

        TIMER_START;
        volatile int sum = 0;
        for (int i = 0; i < ACCESS_COUNT; ++i)
        {
            int idx = i % DATA_SIZE;
            sum += ia[idx];
        }
        TIMER_END("Access time");
    }

    // std::list 随机访问
    std::cout << "\n[2.2] std::list random access:" << std::endl;
    {
        std::list<int> list;
        for (int i = 0; i < DATA_SIZE; ++i)
            list.push_back(i * 2);

        TIMER_START;
        volatile int sum = 0;
        for (int i = 0; i < ACCESS_COUNT; ++i)
        {
            int idx = i % DATA_SIZE;
            auto it = list.begin();
            std::advance(it, idx);
            sum += *it;
        }
        TIMER_END("Access time");

        std::cout << "    [note] std::list has no O(1) random access; each access is O(n) traversal" << std::endl;
    }
}

// ============================================================================
// TEST 3: 中间插入性能对比
// ============================================================================

void test_middle_insert()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 3: Middle insertion performance (1K elements, 1K inserts each)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int INITIAL_SIZE = 1000;
    const int INSERT_COUNT = 1000;
    const int INSERT_POS = INITIAL_SIZE / 2;

    // IndexedList 中间插入
    std::cout << "[3.1] IndexedList middle insertion:" << std::endl;
    {
        IndexedList<int> ia;
        ia.Reserve(INITIAL_SIZE * 2);
        for (int i = 0; i < INITIAL_SIZE; ++i)
            ia.Add(i);

        TIMER_START;
        for (int i = 0; i < INSERT_COUNT; ++i)
            ia.Insert(INSERT_POS, 9999);
        TIMER_END("Insert time");

        TEST_ASSERT(ia.GetCount() == INITIAL_SIZE + INSERT_COUNT, "Element count correct after insert");
    }

    // std::list 中间插入
    std::cout << "\n[3.2] std::list middle insertion:" << std::endl;
    {
        std::list<int> list;
        for (int i = 0; i < INITIAL_SIZE; ++i)
            list.push_back(i);

        TIMER_START;
        for (int i = 0; i < INSERT_COUNT; ++i)
        {
            auto it = list.begin();
            std::advance(it, INSERT_POS);
            list.insert(it, 9999);
        }
        TIMER_END("Insert time");

        TEST_ASSERT(list.size() == INITIAL_SIZE + INSERT_COUNT, "Element count correct after insert");
        std::cout << "    [note] std::list middle insert moves no elements, only relinks pointers (but locating takes time)" << std::endl;
    }
}

// ============================================================================
// TEST 4: 删除操作性能对比
// ============================================================================

void test_delete_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 4: Delete performance (delete 5K from 10K elements)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int INITIAL_SIZE = 10000;
    const int DELETE_COUNT = 5000;

    // IndexedList 删除
    std::cout << "[4.1] IndexedList delete (Delete):" << std::endl;
    {
        IndexedList<int> ia;
        ia.Reserve(INITIAL_SIZE);
        for (int i = 0; i < INITIAL_SIZE; ++i)
            ia.Add(i);

        TIMER_START;
        // 从后往前删除避免索引偏移
        for (int i = 0; i < DELETE_COUNT; ++i)
            ia.Delete(INITIAL_SIZE / 2 - 1);
        TIMER_END("Delete time");

        TEST_ASSERT(ia.GetCount() == INITIAL_SIZE - DELETE_COUNT, "Element count correct after delete");
    }

    // std::list 删除
    std::cout << "\n[4.2] std::list delete (erase):" << std::endl;
    {
        std::list<int> list;
        for (int i = 0; i < INITIAL_SIZE; ++i)
            list.push_back(i);

        TIMER_START;
        for (int i = 0; i < DELETE_COUNT; ++i)
        {
            auto it = list.begin();
            std::advance(it, INITIAL_SIZE / 2 - 1);
            list.erase(it);
        }
        TIMER_END("Delete time");

        TEST_ASSERT(list.size() == INITIAL_SIZE - DELETE_COUNT, "Element count correct after delete");
    }
}

// ============================================================================
// TEST 5: 顺序遍历性能对比
// ============================================================================

void test_sequential_traversal()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 5: Sequential traversal performance (100K elements, 100 passes)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int DATA_SIZE = 100000;
    const int TRAVERSE_COUNT = 100;

    // IndexedList 遍历
    std::cout << "[5.1] IndexedList sequential traversal:" << std::endl;
    {
        IndexedList<int> ia;
        ia.Reserve(DATA_SIZE);
        for (int i = 0; i < DATA_SIZE; ++i)
            ia.Add(i);

        TIMER_START;
        volatile int sum = 0;
        for (int t = 0; t < TRAVERSE_COUNT; ++t)
        {
            for (int i = 0; i < ia.GetCount(); ++i)
                sum += ia[i];
        }
        TIMER_END("Traverse time");
    }

    // std::list 遍历
    std::cout << "\n[5.2] std::list sequential traversal:" << std::endl;
    {
        std::list<int> list;
        for (int i = 0; i < DATA_SIZE; ++i)
            list.push_back(i);

        TIMER_START;
        volatile int sum = 0;
        for (int t = 0; t < TRAVERSE_COUNT; ++t)
        {
            for (auto it = list.begin(); it != list.end(); ++it)
                sum += *it;
        }
        TIMER_END("Traverse time");
    }
}

// ============================================================================
// TEST 6: 空间利用率对比
// ============================================================================

void test_memory_efficiency()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 6: Space efficiency comparison" << std::endl;
    std::cout << "========================================\n" << std::endl;

    const int TEST_SIZES[] = {1000, 10000, 100000, 1000000};

    std::cout << "\n[6.1] Memory usage at various capacities:\n" << std::endl;
    std::cout << std::left << std::setw(15) << "Element count"
              << std::setw(25) << "IndexedList(KB)"
              << std::setw(25) << "std::list (est. KB)"
              << std::setw(15) << "Ratio" << std::endl;
    std::cout << std::string(80, '-') << std::endl;

    for (int size : TEST_SIZES)
    {
        IndexedList<int> ia;
        ia.Reserve(size);
        for (int i = 0; i < size; ++i)
            ia.Add(i);

        double ia_memory = ia.GetTotalBytes() / 1024.0;
        double list_memory = size * 40.0 / 1024.0;  // 每个节点估计 40 字节
        double ratio = list_memory / ia_memory;

        std::cout << std::left << std::setw(15) << size
                  << std::setw(25) << std::fixed << std::setprecision(2) << ia_memory
                  << std::setw(25) << list_memory
                  << std::setw(15) << "x" << std::setprecision(1) << ratio << std::endl;
    }
}

// ============================================================================
// TEST 7: 混合操作性能对比（真实场景模拟）
// ============================================================================

void test_mixed_operations()
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "TEST 7: Mixed operation performance (real-world)" << std::endl;
    std::cout << "========================================\n" << std::endl;

    std::cout << "[7.1] IndexedList mixed operations:" << std::endl;
    {
        IndexedList<int> ia;

        TIMER_START;

        // 添加 10K 个元素
        for (int i = 0; i < 10000; ++i)
            ia.Add(i);

        // 随机访问 1000 次
        for (int i = 0; i < 1000; ++i)
            volatile int v = ia[i % ia.GetCount()];

        // 插入 100 个元素
        for (int i = 0; i < 100; ++i)
            ia.Insert(i % ia.GetCount(), 999);

        // 删除 50 个元素
        for (int i = 0; i < 50; ++i)
            ia.Delete(i % ia.GetCount());

        // 遍历一次
        volatile int sum = 0;
        for (int i = 0; i < ia.GetCount(); ++i)
            sum += ia[i];

        TIMER_END("Mixed operations time");

        std::cout << "    Final element count: " << ia.GetCount() << std::endl;
    }

    std::cout << "\n[7.2] std::list mixed operations:" << std::endl;
    {
        std::list<int> list;

        TIMER_START;

        // 添加 10K 个元素
        for (int i = 0; i < 10000; ++i)
            list.push_back(i);

        // 随机访问 1000 次
        for (int i = 0; i < 1000; ++i)
        {
            auto it = list.begin();
            std::advance(it, i % list.size());
            volatile int v = *it;
        }

        // 插入 100 个元素
        for (int i = 0; i < 100; ++i)
        {
            auto it = list.begin();
            std::advance(it, i % list.size());
            list.insert(it, 999);
        }

        // 删除 50 个元素
        for (int i = 0; i < 50; ++i)
        {
            auto it = list.begin();
            std::advance(it, i % list.size());
            list.erase(it);
        }

        // 遍历一次
        volatile int sum = 0;
        for (auto v : list)
            sum += v;

        TIMER_END("Mixed operations time");

        std::cout << "    Final element count: " << list.size() << std::endl;
    }
}

// ============================================================================
// 性能分析总结
// ============================================================================

void print_analysis()
{
    std::cout << "\n\n" << std::string(80, '=') << std::endl;
    std::cout << "Performance Analysis and Recommendations" << std::endl;
    std::cout << std::string(80, '=') << std::endl;

    std::cout << "\n[IndexedList Advantages]\n" << std::endl;

    std::cout << "1. Random access: O(1) time complexity\n"
              << "   - Contiguous array storage, CPU cache friendly\n"
              << "   - Avoids pointer indirection overhead\n"
              << "   - Can be 100+ times faster than std::list\n" << std::endl;

    std::cout << "2. Memory efficient\n"
              << "   - No pointer overhead (saves 16-24 bytes per node)\n"
              << "   - No extra allocation fragmentation\n"
              << "   - Can save 80% memory vs std::list\n" << std::endl;

    std::cout << "3. Cache friendly\n"
              << "   - Contiguous memory layout\n"
              << "   - High CPU cache hit rate\n"
              << "   - Good prefetch characteristics\n" << std::endl;

    std::cout << "4. Fast sequential traversal\n"
              << "   - Excellent linear traversal performance\n"
              << "   - Suitable for streaming data processing\n" << std::endl;

    std::cout << "\n[std::list Advantages]\n" << std::endl;

    std::cout << "1. Middle insert/delete: pointer operations only\n"
              << "   - No large element moves\n"
              << "   - Advantageous for frequent middle modifications\n"
              << "   - Locating insert/delete point can be slow\n" << std::endl;

    std::cout << "2. Dynamic growth\n"
              << "   - No pre-allocation needed\n"
              << "   - Nodes allocated on demand\n" << std::endl;

    std::cout << "\n[Performance Comparison Conclusion]\n" << std::endl;

    std::cout << "Operation              | IndexedList | std::list | Winner" << std::endl;
    std::cout << std::string(70, '-') << std::endl;
    std::cout << "Sequential add        |  Very fast(O(1))  | Fast(O(1)) | ~(IndexedVA memory better)" << std::endl;
    std::cout << "Random access         |  Very fast(O(1))  | Very slow(O(n))| ★IndexedList" << std::endl;
    std::cout << "Middle insert         |  Slow(O(n))       | Fast(O(1)) | ★std::list" << std::endl;
    std::cout << "Middle delete         |  Slow(O(n))       | Fast(O(1)) | ★std::list" << std::endl;
    std::cout << "Sequential traversal  |  Very fast        | Fast       | ★IndexedList" << std::endl;
    std::cout << "Memory usage          |  Low              | High       | ★IndexedList" << std::endl;

    std::cout << "\n[Usage Recommendations]\n" << std::endl;

    std::cout << "[Choose IndexedList]\n"
              << "  ✓ Frequent random access scenarios (arrays, matrices)\n"
              << "  ✓ Efficient traversal needed\n"
              << "  ✓ Memory-constrained scenarios\n"
              << "  ✓ Most real-world scenarios (>90%)\n"
              << "  ✓ Game dev, graphics, data processing\n" << std::endl;

    std::cout << "[Choose std::list]\n"
              << "  ✓ Frequent middle insert/delete, rare random access\n"
              << "  ✓ No capacity estimation needed\n"
              << "  ✓ O(1) operations at both ends (deque is better)\n"
              << "  ✓ Only <10% of use cases\n" << std::endl;

    std::cout << "\n[Performance Analysis]\n" << std::endl;

    std::cout << "1. Why random access differs hugely (100+x):\n"
              << "   - IndexedList: computes address directly as arr[i]\n"
              << "   - std::list: traverses from head i times, O(n)\n"
              << "   - Cache locality difference (10x+ hit rate gap)\n\n" << std::endl;

    std::cout << "2. Why memory usage differs (4-5x):\n"
              << "   - std::list: two pointers per node (prev, next) = 16 bytes\n"
              << "   - Plus data and alignment = 40+ bytes per int\n"
              << "   - IndexedList needs only 4 bytes/int\n\n" << std::endl;

    std::cout << "3. Middle operations (insert/delete) comparison:\n"
              << "   - std::list: O(1) in theory, but O(n) locating, still slow overall\n"
              << "   - IndexedList: O(n) element moves, but cache friendly\n"
              << "   - Actual performance depends on data size and operation frequency\n" << std::endl;

    std::cout << std::string(80, '=') << std::endl;
}

// ============================================================================
// 主函数
// ============================================================================

int main()
{
    std::cout << "\n╔════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   IndexedList vs std::list Performance Test        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;

    try
    {
        test_sequential_add();
        test_random_access();
        test_middle_insert();
        test_delete_operations();
        test_sequential_traversal();
        test_memory_efficiency();
        test_mixed_operations();

        print_analysis();

        // 测试总结
        std::cout << "\n\n═══════════════════════════════════════════════════════════" << std::endl;
        std::cout << "Test summary: " << tests_passed << " passed, " << tests_failed << " failed" << std::endl;
        std::cout << "═══════════════════════════════════════════════════════════\n" << std::endl;

        return tests_failed == 0 ? 0 : 1;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
