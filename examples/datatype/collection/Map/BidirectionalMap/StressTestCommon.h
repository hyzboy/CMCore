#pragma once

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
static const bool kVerboseOps = true;      // 每步操作输出（设为false以减少输出）
static const bool kDumpOnFailure = true;    // 失败时Dump内部结构
static const bool kDumpEachOp = false;      // 每步都Dump（极其详细，输出量巨大）
static const bool kVerifyEachOp = false;     // 每步都做一致性验证

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
