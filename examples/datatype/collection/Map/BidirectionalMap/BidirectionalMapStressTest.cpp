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

// 详细日志开关
static const bool kVerboseOps = false;     // 每步操作输出（关闭以测试 Heisenbug）
static const bool kDumpOnFailure = true;   // 失败时Dump内部结构
static const bool kDumpEachOp = false;     // 每步都Dump（极其详细，输出量巨大）
static const bool kVerifyEachOp = true;    // 每步都做一致性验证

static void LogOpHeader(const char* title)
{
    cout << "\n---- " << title << " ----" << endl;
}

static void LogOp(int op_index, const char* op_type, int key, const string& value, bool result, int count, int expected_count)
{
    if (!kVerboseOps) return;
    cout << "  [op " << op_index << "] " << op_type
         << " key=" << key
         << " value=\"" << value << "\""
         << " result=" << (result ? "true" : "false")
         << " size=" << count
         << " expected=" << expected_count
         << endl;
}

template<typename K, typename V>
static void DumpIfNeeded(const BidirectionalMap<K, V>& bmap, const char* label)
{
    if (kDumpEachOp || kDumpOnFailure)
        bmap.DebugDump(label);
}

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

template<typename K, typename V>
static bool VerifyState(const BidirectionalMap<K, V>& bmap, const map<K, V>& expected, const char* label, int op_index)
{
    if (!kVerifyEachOp)
        return true;

    bool ok = true;

    if (bmap.GetCount() != (int)expected.size())
    {
        cout << "  ✗ INTEGRITY FAIL: count mismatch at op " << op_index
             << " (actual=" << bmap.GetCount() << ", expected=" << expected.size() << ")"
             << " label=" << label << endl;
        ok = false;
    }

    for (const auto& [k, v] : expected)
    {
        V got;
        if (!bmap.Get(k, got))
        {
            cout << "  ✗ INTEGRITY FAIL: Get missing key=" << k
                 << " at op " << op_index << " label=" << label << endl;
            ok = false;
            break;
        }
        if (got != v)
        {
            cout << "  ✗ INTEGRITY FAIL: value mismatch key=" << k
                 << " expected=\"" << v << "\" got=\"" << got << "\""
                 << " at op " << op_index << " label=" << label << endl;
            ok = false;
            break;
        }

        K back_key;
        if (!bmap.GetByValue(v, back_key))
        {
            cout << "  ✗ INTEGRITY FAIL: GetByValue missing value=\"" << v << "\""
                 << " at op " << op_index << " label=" << label << endl;
            ok = false;
            break;
        }
        if (back_key != k)
        {
            cout << "  ✗ INTEGRITY FAIL: reverse mismatch value=\"" << v << "\""
                 << " expected key=" << k << " got key=" << back_key
                 << " at op " << op_index << " label=" << label << endl;
            ok = false;
            break;
        }
    }

    return ok;
}

int os_main(int, os_char**)
{
    cout << "\n========================================" << endl;
    cout << "TEST: BidirectionalMap Stress Test" << endl;
    cout << "========================================" << endl;

    // [1] 顺序添加 50000 个元素
    cout << "\n[1] Sequential Add (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int COUNT = 50000;
        LogOpHeader("[1] Sequential Add");
        
        for (int i = 0; i < COUNT; i++)
        {
            const string value = "value_" + to_string(i);
            const bool result = bmap.Add(i, value);
            if (result)
                expected[i] = value;

            LogOp(i, "Add", i, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "SeqAdd", i);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("SeqAdd Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == COUNT, "Count is 50000");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    // [2] 顺序删除 50000 个元素
    cout << "\n[2] Sequential Delete (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        const int COUNT = 50000;
        
        // 先添加
        for (int i = 0; i < COUNT; i++)
        {
            const string value = "value_" + to_string(i);
            bmap.Add(i, value);
            expected[i] = value;
        }
        
        Timer timer;
        LogOpHeader("[2] Sequential Delete");
        // 顺序删除
        for (int i = 0; i < COUNT; i++)
        {
            const bool result = bmap.DeleteByKey(i);
            if (result)
                expected.erase(i);

            LogOp(i, "Delete", i, "", result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "SeqDelete", i);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("SeqDelete Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.IsEmpty(), "Map is empty after all deletes");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    // [3] 顺序修改 50000 个元素
    cout << "\n[3] Sequential Change (50000 items):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        const int COUNT = 50000;
        
        // 先添加
        for (int i = 0; i < COUNT; i++)
        {
            const string value = "value_" + to_string(i);
            bmap.Add(i, value);
            expected[i] = value;
        }
        
        Timer timer;
        LogOpHeader("[3] Sequential Change");
        // 顺序修改
        for (int i = 0; i < COUNT; i++)
        {
            const string new_value = "changed_" + to_string(i);
            const bool result = bmap.Change(i, new_value);
            if (result)
                expected[i] = new_value;

            LogOp(i, "Change", i, new_value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "SeqChange", i);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("SeqChange Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == COUNT, "Count remains 50000");
        double elapsed = timer.elapsed_ms();
        cout << "  Time: " << elapsed << " ms (" << (elapsed / COUNT * 1000) << " us/op)" << endl;
    }

    // [4] 随机操作压力测试
    cout << "\n[4] Random Operations (100000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        mt19937 rng(12345);  // 固定种子保证可复现
        uniform_int_distribution<int> op_dist(0, 2);  // 0:Add, 1:Delete, 2:Change
        uniform_int_distribution<int> key_dist(0, 9999);
        
        Timer timer;
        const int OPS = 100000;
        LogOpHeader("[4] Random Ops");
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            string value;
            bool result = false;
            const char* op_name = "";
            
            if (op_type == 0)  // Add
            {
                op_name = "Add";
                value = "random_" + to_string(key);
                result = bmap.Add(key, value);
                if (result)
                    expected[key] = value;
            }
            else if (op_type == 1)  // Delete
            {
                op_name = "Delete";
                value = "";
                result = bmap.DeleteByKey(key);
                if (result)
                    expected.erase(key);
            }
            else  // Change
            {
                op_name = "Change";
                value = "modified_" + to_string(key);
                result = bmap.Change(key, value);
                if (result)
                    expected[key] = value;
            }

            LogOp(op, op_name, key, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "RandomOps", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("RandomOps Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == (int)expected.size(), "Count matches expected");
        double elapsed = timer.elapsed_ms();
        cout << "  Operations: " << OPS << endl;
        cout << "  Final count: " << bmap.GetCount() << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / OPS * 1000) << " us/op)" << endl;
    }

    // [5] 交替Add-Delete操作
    cout << "\n[5] Alternating Add-Delete (20000 rounds):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int ROUNDS = 20000;
        LogOpHeader("[5] Alternating Add-Delete");
        
        for (int round = 0; round < ROUNDS; round++)
        {
            // 添加
            const string value = "value_" + to_string(round);
            bool result = bmap.Add(round, value);
            if (result)
                expected[round] = value;

            LogOp(round * 2, "Add", round, value, result, bmap.GetCount(), (int)expected.size());

            bool ok = VerifyState(bmap, expected, "AltAdd", round * 2);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("AltAdd Integrity Failure");
                return 1;
            }
            
            // 立即删除
            result = bmap.DeleteByKey(round);
            if (result)
                expected.erase(round);

            LogOp(round * 2 + 1, "Delete", round, "", result, bmap.GetCount(), (int)expected.size());

            ok = VerifyState(bmap, expected, "AltDelete", round * 2 + 1);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("AltDelete Integrity Failure");
                return 1;
            }
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
        map<int, string> expected;
        Timer timer;
        const int ROUNDS = 10000;
        LogOpHeader("[6] Batch Add-Change-Delete");
        
        for (int round = 0; round < ROUNDS; round++)
        {
            // Add
            string value = "v" + to_string(round);
            bool result = bmap.Add(round, value);
            if (result)
                expected[round] = value;
            LogOp(round * 3, "Add", round, value, result, bmap.GetCount(), (int)expected.size());
            bool ok = VerifyState(bmap, expected, "BatchAdd", round * 3);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BatchAdd Integrity Failure");
                return 1;
            }
            
            // Change
            value = "modified_v" + to_string(round);
            result = bmap.Change(round, value);
            if (result)
                expected[round] = value;
            LogOp(round * 3 + 1, "Change", round, value, result, bmap.GetCount(), (int)expected.size());
            ok = VerifyState(bmap, expected, "BatchChange", round * 3 + 1);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BatchChange Integrity Failure");
                return 1;
            }
            
            // Delete
            result = bmap.DeleteByKey(round);
            if (result)
                expected.erase(round);
            LogOp(round * 3 + 2, "Delete", round, "", result, bmap.GetCount(), (int)expected.size());
            ok = VerifyState(bmap, expected, "BatchDelete", round * 3 + 2);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BatchDelete Integrity Failure");
                return 1;
            }
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
        map<int, string> expected;
        const int ITEMS = 10000;
        
        // 添加10000个元素
        for (int i = 0; i < ITEMS; i++)
        {
            const string value = "value_" + to_string(i);
            bmap.Add(i, value);
            expected[i] = value;
        }
        
        Timer timer;
        const int QUERIES = 100000;
        string dummy;
        int key_dummy;
        LogOpHeader("[7] Query-Heavy Ops");
        
        for (int q = 0; q < QUERIES; q++)
        {
            int key = q % ITEMS;
            bool result = false;
            string value = "value_" + to_string(key);
            
            // 交替进行正向和反向查询
            if (q % 2 == 0)
            {
                result = bmap.Get(key, dummy);
                LogOp(q, "Get", key, value, result, bmap.GetCount(), (int)expected.size());
            }
            else
            {
                result = bmap.GetByValue(value, key_dummy);
                LogOp(q, "GetByValue", key, value, result, bmap.GetCount(), (int)expected.size());
            }

            const bool ok = VerifyState(bmap, expected, "QueryHeavy", q);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("QueryHeavy Integrity Failure");
                return 1;
            }
        }
        
        double elapsed = timer.elapsed_ms();
        cout << "  Total queries: " << QUERIES << endl;
        cout << "  Time: " << elapsed << " ms (" << (elapsed / QUERIES * 1000) << " us/op)" << endl;
    }

    // [8] ChangeOrAdd压力测试
    cout << "\n[8] ChangeOrAdd Stress (50000 operations):" << endl;
    {
        BidirectionalMap<int, string> bmap;
        map<int, string> expected;
        Timer timer;
        const int OPS = 50000;
        mt19937 rng(54321);
        uniform_int_distribution<int> key_dist(0, 4999);  // 较小范围造成冲突
        LogOpHeader("[8] ChangeOrAdd Stress");
        
        for (int op = 0; op < OPS; op++)
        {
            int key = key_dist(rng);
            string value = "v" + to_string(key) + "_" + to_string(op);
            bmap.ChangeOrAdd(key, value);
            expected[key] = value;

            LogOp(op, "ChangeOrAdd", key, value, true, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "ChangeOrAdd", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("ChangeOrAdd Integrity Failure");
                return 1;
            }
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
        map<int, string> expected;
        const int OPS = 50000;
        int add_count = 0, delete_count = 0;
        
        mt19937 rng(88888);
        uniform_int_distribution<int> op_dist(0, 2);
        uniform_int_distribution<int> key_dist(0, 4999);
        
        Timer timer;
        LogOpHeader("[9] Basic Consistency");
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            string value;
            bool result = false;
            const char* op_name = "";
            
            if (op_type == 0)  // Add
            {
                op_name = "Add";
                value = "v" + to_string(key);
                result = bmap.Add(key, value);
                if (result)
                {
                    expected[key] = value;
                    add_count++;
                }
            }
            else if (op_type == 1)  // Delete
            {
                op_name = "Delete";
                value = "";
                result = bmap.DeleteByKey(key);
                if (result)
                {
                    expected.erase(key);
                    delete_count++;
                }
            }
            else  // Query
            {
                op_name = "Get";
                value = "v" + to_string(key);
                string dummy;
                result = bmap.Get(key, dummy);
            }

            LogOp(op, op_name, key, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "BasicConsistency", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("BasicConsistency Integrity Failure");
                return 1;
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
        map<int, string> expected;
        int next_id = 0;
        Timer timer;
        const int ROUNDS = 10000;
        LogOpHeader("[11] Delete Pattern Stress");
        int op_index = 0;
        
        for (int round = 0; round < ROUNDS; round++)
        {
            // Add 5
            for (int i = 0; i < 5; i++)
            {
                const int key = next_id;
                const string value = "v" + to_string(next_id);
                const bool result = bmap.Add(key, value);
                if (result)
                    expected[key] = value;

                LogOp(op_index++, "Add", key, value, result, bmap.GetCount(), (int)expected.size());
                const bool ok = VerifyState(bmap, expected, "DeletePatternAdd", op_index);
                if (!ok)
                {
                    if (kDumpOnFailure)
                        bmap.DebugDump("DeletePattern Add Integrity Failure");
                    return 1;
                }

                next_id++;
            }
            
            // Delete 4 (oldest)
            for (int i = 0; i < 4; i++)
            {
                int del_id = (round * 5) + i;
                if (del_id < next_id)
                {
                    const bool result = bmap.DeleteByKey(del_id);
                    if (result)
                        expected.erase(del_id);

                    LogOp(op_index++, "Delete", del_id, "", result, bmap.GetCount(), (int)expected.size());
                    const bool ok = VerifyState(bmap, expected, "DeletePatternDelete", op_index);
                    if (!ok)
                    {
                        if (kDumpOnFailure)
                            bmap.DebugDump("DeletePattern Delete Integrity Failure");
                        return 1;
                    }
                }
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
        map<int, string> expected;
        
        mt19937 rng(777);
        uniform_int_distribution<int> op_dist(0, 2);
        uniform_int_distribution<int> key_dist(0, 999);  // 仅1000个KEY
        
        Timer timer;
        const int OPS = 20000;
        LogOpHeader("[12] Dense Random Ops");
        
        for (int op = 0; op < OPS; op++)
        {
            int op_type = op_dist(rng);
            int key = key_dist(rng);
            string value;
            bool result = false;
            const char* op_name = "";
            
            if (op_type == 0)  // Add
            {
                op_name = "Add";
                value = "v" + to_string(key);
                result = bmap.Add(key, value);
                if (result)
                    expected[key] = value;
            }
            else if (op_type == 1)  // Delete
            {
                op_name = "Delete";
                value = "";
                result = bmap.DeleteByKey(key);
                if (result)
                    expected.erase(key);
            }
            else  // Query
            {
                op_name = "Get";
                value = "v" + to_string(key);
                string dummy;
                result = bmap.Get(key, dummy);
            }

            LogOp(op, op_name, key, value, result, bmap.GetCount(), (int)expected.size());

            const bool ok = VerifyState(bmap, expected, "DenseRandom", op);
            if (!ok)
            {
                if (kDumpOnFailure)
                    bmap.DebugDump("DenseRandom Integrity Failure");
                return 1;
            }
        }
        
        TEST_ASSERT(bmap.GetCount() == (int)expected.size(), "Consistency maintained");
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
