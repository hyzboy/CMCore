#pragma once

#include<hgl/platform/Platform.h>
#include<hgl/type/BidirectionalMap.h>
#include<iostream>
#include<chrono>
#include<random>
#include<set>
#include<map>
#include<string>
#include<atomic>

using namespace hgl;
using namespace std;

// 测试计数器
static int test_passed = 0;
static int test_failed = 0;

// 详细日志开关 - 为了诊断 Heisenbug，默认关闭
static const bool kVerboseOps = false;      // 每步操作输出
static const bool kDumpOnFailure = true;    // 失败时Dump内部结构
static const bool kDumpEachOp = false;      // 每步都Dump（极其详细，输出量巨大）
static const bool kVerifyEachOp = true;     // 每步都做一致性验证

// 新增：内存屏障选项（用于诊断编译器优化问题）
static const bool kUseMemoryBarriers = false;  // 在关键操作后添加内存屏障

// 内存屏障辅助函数
inline void MemoryBarrier()
{
    if (kUseMemoryBarriers)
        std::atomic_thread_fence(std::memory_order_seq_cst);
}

// 用于防止编译器优化掉某些变量
template<typename T>
inline void PreventOptimization(T& value)
{
    // 使用 volatile 读取来防止优化
    volatile T temp = value;
    (void)temp;
}

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

    // 使用 volatile 防止编译器优化
    volatile int actual_count = bmap.GetCount();
    volatile int expected_count = (int)expected.size();
    
    bool ok = true;

    if (actual_count != expected_count)
    {
        cout << "  ✗ INTEGRITY FAIL: count mismatch at op " << op_index
             << " (actual=" << actual_count << ", expected=" << expected_count << ")"
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
        
        // 防止优化
        PreventOptimization(got);
        
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
        
        // 防止优化
        PreventOptimization(back_key);
        
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
