#include<hgl/platform/Platform.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<chrono>
#include<random>
#include<set>
#include<map>
#include<string>

using namespace hgl;
using namespace std;

// 测试计数器
static int test_passed = 0;
static int test_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            test_passed++; \
        } else { \
            cout << "  ✗ FAIL: " << message << endl; \
            test_failed++; \
        } \
    } while(0)

// 计时工具
class Timer
{
private:
    chrono::high_resolution_clock::time_point start_time;
    
public:
    Timer() { start_time = chrono::high_resolution_clock::now(); }
    
    double elapsed_ms() const
    {
        auto end = chrono::high_resolution_clock::now();
        return chrono::duration<double, milli>(end - start_time).count();
    }
};

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress Test" << endl;
    cout << "========================================" << endl;

    // [1] 顺序添加 50000 个元素
    cout << "\n[1] Sequential Add (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        Timer timer;
        const int COUNT = 50000;
        
        for (int i = 0; i < COUNT; i++)
        {
            TEST_ASSERT(bmap.Add(i, "value_" + to_string(i)), "Add item");
        }
        
        TEST_ASSERT(bmap.GetCount() == COUNT, "Count is 50000");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    // [2] 顺序删除 50000 个元素
    cout << "\n[2] Sequential Delete (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        const int COUNT = 50000;
        
        // 先添加
        for (int i = 0; i < COUNT; i++)
            bmap.Add(i, "value_" + to_string(i));
        
        Timer timer;
        // 顺序删除
        for (int i = 0; i < COUNT; i++)
        {
            TEST_ASSERT(bmap.DeleteByKey(i), "Delete item");
        }
        
        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after all deletes");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    // [3] 顺序修改 50000 个元素
    cout << "\n[3] Sequential Change (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        const int COUNT = 50000;
        
        // 先添加
        for (int i = 0; i < COUNT; i++)
            bmap.Add(i, "value_" + to_string(i));
        
        Timer timer;
        // 顺序修改
        for (int i = 0; i < COUNT; i++)
        {
            TEST_ASSERT(bmap.Change(i, "changed_" + to_string(i)), "Change item");
        }
        
        TEST_ASSERT(bmap.GetCount() == COUNT, "Count remains 50000");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    // [4] 随机操作压力测试
    cout << "\n[4] Random Operations (100000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        set<int> valid_keys;
        mt19937 rng(12345);  // 固定种子保证可复现
        uniform_int_distribution<int> op_dist(0, 2);  // 0:Add, 1:Delete, 2:Change
        uniform_int_distribution<int> key_dist(0, 9999);
        
        Timer timer;
        const int OPS = 100000;
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            
            if (op_type == 0)  // Add
            {
                if (valid_keys.find(key) == valid_keys.end())
                {
                    if (bmap.Add(key, "random_" + to_string(key)))
                        valid_keys.insert(key);
                }
            }
            else if (op_type == 1)  // Delete
            {
                if (valid_keys.find(key) != valid_keys.end())
                {
                    if (bmap.DeleteByKey(key))
                        valid_keys.erase(key);
                }
            }
            else  // Change
            {
                if (valid_keys.find(key) != valid_keys.end())
                {
                    bmap.Change(key, "modified_" + to_string(key));
                }
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == (int)valid_keys.size(), "Count matches valid keys");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final count: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    // [5] 交替Add-Delete操作
    cout << "\n[5] Alternating Add-Delete (20000 rounds):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        Timer timer;
        const int ROUNDS = 20000;
        
        for (int round = 0; round < ROUNDS; round++)
        {
            // 添加
            TEST_ASSERT(bmap.Add(round, "value_" + to_string(round)), "Add");
            
            // 立即删除
            TEST_ASSERT(bmap.DeleteByKey(round), "Delete");
        }
        
        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after all add-delete pairs");
        double elapsed = timer.elapsed_ms();
        cout << "  Total operations: " << (ROUNDS * 2) << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / (ROUNDS * 2) * 1000) << " us/op)" << endl;
    }

    // [6] 批量操作：Add-Change-Delete循环
    cout << "\n[6] Batch Operations (Add-Change-Delete cycle, 10000 rounds):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        Timer timer;
        const int ROUNDS = 10000;
        
        for (int round = 0; round < ROUNDS; round++)
        {
            // Add
            TEST_ASSERT(bmap.Add(round, "v" + to_string(round)), "Batch Add");
            
            // Change
            TEST_ASSERT(bmap.Change(round, "modified_v" + to_string(round)), "Batch Change");
            
            // Delete
            TEST_ASSERT(bmap.DeleteByKey(round), "Batch Delete");
        }
        
        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after batch operations");
        double elapsed = timer.elapsed_ms();
        cout << "  Total operations: " << (ROUNDS * 3) << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / (ROUNDS * 3) * 1000) << " us/op)" << endl;
    }

    // [7] 查询密集型操作
    cout << "\n[7] Query-Heavy Operations (100000 queries on 10000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        const int ITEMS = 10000;
        
        // 添加10000个元素
        for (int i = 0; i < ITEMS; i++)
            bmap.Add(i, "value_" + to_string(i));
        
        Timer timer;
        const int QUERIES = 100000;
        string dummy;
        int key_dummy;
        
        for (int q = 0; q < QUERIES; q++)
        {
            int key = q % ITEMS;
            
            // 交替进行正向和反向查询
            if (q % 2 == 0)
                bmap.Get(key, dummy);
            else
                bmap.GetByValue("value_" + to_string(key), key_dummy);
        }
        
        double elapsed = timer.elapsed_ms();
        cout << "  Total queries: " << QUERIES << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / QUERIES * 1000) << " us/op)" << endl;
    }

    // [8] ChangeOrAdd压力测试
    cout << "\n[8] ChangeOrAdd Stress (50000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        Timer timer;
        const int OPS = 50000;
        mt19937 rng(54321);
        uniform_int_distribution<int> key_dist(0, 4999);  // 较小范围造成冲突
        
        for (int op = 0; op < OPS; op++)
        {
            int key = key_dist(rng);
            bmap.ChangeOrAdd(key, "v" + to_string(key) + "_" + to_string(op));
        }
        
        TEST_ASSERT(bmap.GetCount() <= 5000, "Map size bounded by key range");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final unique keys: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    // [9] 简化的一致性测试
    cout << "\n[9] Basic Consistency Check (50000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        const int OPS = 50000;
        int add_count = 0, delete_count = 0;
        
        mt19937 rng(88888);
        uniform_int_distribution<int> op_dist(0, 2);
        uniform_int_distribution<int> key_dist(0, 4999);
        
        Timer timer;
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            
            if (op_type == 0)  // Add
            {
                if (bmap.Add(key, "v" + to_string(key)))
                    add_count++;
            }
            else if (op_type == 1)  // Delete
            {
                if (bmap.DeleteByKey(key))
                    delete_count++;
            }
            else  // Query
            {
                string dummy;
                bmap.Get(key, dummy);
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == add_count - delete_count, "Count = adds - deletes");
        double elapsed = timer.elapsed_ms();
        cout << "  Adds: " << add_count << ", Deletes: " << delete_count << endl;
        cout << "  Final size: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    // [10] 迭代器性能测试
    cout << "\n[10] Iterator Performance (50000 items, 1000 iterations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        const int ITEMS = 50000;
        
        for (int i = 0; i < ITEMS; i++)
            bmap.Add(i, "value_" + to_string(i));
        
        Timer timer;
        const int ITERATIONS = 1000;
        long long sum = 0;
        
        for (int iter = 0; iter < ITERATIONS; iter++)
        {
            for (auto [k, v] : bmap)
                sum += k;
        }
        
        double elapsed = timer.elapsed_ms();
        long long total_items = (long long)ITEMS * ITERATIONS;
        cout << "  Total iterations: " << total_items << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / total_items * 1000) << " us/item)" << endl;
    }

    // [11] 删除模式压力测试
    cout << "\n[11] Delete Pattern Stress (5 add, 4 delete pattern):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        int next_id = 0;
        Timer timer;
        const int ROUNDS = 10000;
        
        for (int round = 0; round < ROUNDS; round++)
        {
            // Add 5
            for (int i = 0; i < 5; i++)
            {
                bmap.Add(next_id, "v" + to_string(next_id));
                next_id++;
            }
            
            // Delete 4 (oldest)
            for (int i = 0; i < 4; i++)
            {
                int del_id = (round * 5) + i;
                if (del_id < next_id)
                    bmap.DeleteByKey(del_id);
            }
        }
        
        int expected_count = ROUNDS * 5 - ROUNDS * 4;
        TEST_ASSERT(bmap.GetCount() == expected_count, "Count matches pattern");
        
        double elapsed = timer.elapsed_ms();
        cout << "  Rounds: " << ROUNDS << endl;
        cout << "  Final count: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms" << endl;
    }

    // [12] 随机Key范围的密集操作
    cout << "\n[12] Dense Random Operations in Limited Range (20000 ops, 1000 keys):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        set<int> present_keys;
        
        mt19937 rng(777);
        uniform_int_distribution<int> op_dist(0, 2);
        uniform_int_distribution<int> key_dist(0, 999);  // 仅1000个KEY
        
        Timer timer;
        const int OPS = 20000;
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            
            if (op_type == 0)  // Add
            {
                if (present_keys.find(key) == present_keys.end())
                {
                    if (bmap.Add(key, "v" + to_string(key)))
                        present_keys.insert(key);
                }
            }
            else if (op_type == 1)  // Delete
            {
                if (present_keys.find(key) != present_keys.end())
                {
                    if (bmap.DeleteByKey(key))
                        present_keys.erase(key);
                }
            }
            else  // Query
            {
                string dummy;
                bmap.Get(key, dummy);
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == (int)present_keys.size(), "Consistency maintained");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final size: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    cout << "\n========================================" << endl;
    cout << "Results: " << test_passed << " passed, " << test_failed << " failed" << endl;
    cout << "========================================\n" << endl;

    return test_failed == 0 ? 0 : 1;
}
