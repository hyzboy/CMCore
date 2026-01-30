/**
 * QueueBenchmark.cpp
 * 
 * 性能对比测试：hgl::Queue vs std::queue vs std::deque
 * 
 * 测试场景：
 * 1. 顺序 Push 性能
 * 2. 顺序 Pop 性能
 * 3. Push + Pop 混合操作
 * 4. FIFO 循环操作（生产者-消费者模式）
 * 5. 大批量操作
 */

#include<hgl/type/DataType.h>
#include<hgl/type/Queue.h>
#include<queue>
#include<deque>
#include<iostream>
#include<chrono>
#include<vector>
#include<iomanip>
#include<functional>

using namespace hgl;
using namespace std;
using namespace std::chrono;

// ==================== 计时工具 ====================

class Timer
{
    high_resolution_clock::time_point start_time;
    const char* name;

public:
    Timer(const char* n) : name(n)
    {
        start_time = high_resolution_clock::now();
    }

    ~Timer()
    {
        auto end_time = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end_time - start_time).count();
        cout << "  " << setw(40) << left << name 
             << ": " << setw(10) << right << duration << " μs" << endl;
    }
};

// ==================== 测试数据结构 ====================

struct TestData
{
    int id;
    double value;
    char padding[56];  // 总共 64 字节，模拟中等大小的对象

    TestData() : id(0), value(0.0) {}
    TestData(int i) : id(i), value(i * 1.5) {}
};

// ==================== 性能测试函数 ====================

void TestSequentialPush(int count)
{
    cout << "\n=== Test 1: Sequential Push (" << count << " items) ===" << endl;

    // hgl::Queue
    {
        Queue<TestData> q;
        Timer t("hgl::Queue<TestData>");
        for(int i = 0; i < count; i++)
            q.Push(TestData(i));
    }

    // std::queue
    {
        std::queue<TestData> q;
        Timer t("std::queue<TestData>");
        for(int i = 0; i < count; i++)
            q.push(TestData(i));
    }

    // std::deque (直接使用)
    {
        std::deque<TestData> q;
        Timer t("std::deque<TestData>");
        for(int i = 0; i < count; i++)
            q.push_back(TestData(i));
    }
}

void TestSequentialPop(int count)
{
    cout << "\n=== Test 2: Sequential Pop (" << count << " items) ===" << endl;

    // 准备数据
    Queue<TestData> hgl_q;
    std::queue<TestData> std_q;
    std::deque<TestData> std_dq;

    for(int i = 0; i < count; i++)
    {
        hgl_q.Push(TestData(i));
        std_q.push(TestData(i));
        std_dq.push_back(TestData(i));
    }

    // hgl::Queue
    {
        Timer t("hgl::Queue<TestData>");
        TestData data;
        for(int i = 0; i < count; i++)
            hgl_q.Pop(data);
    }

    // std::queue
    {
        Timer t("std::queue<TestData>");
        for(int i = 0; i < count; i++)
        {
            [[maybe_unused]] auto data = std_q.front();
            std_q.pop();
        }
    }

    // std::deque
    {
        Timer t("std::deque<TestData>");
        for(int i = 0; i < count; i++)
        {
            [[maybe_unused]] auto data = std_dq.front();
            std_dq.pop_front();
        }
    }
}

void TestPushPopMixed(int count)
{
    cout << "\n=== Test 3: Push + Pop Mixed (" << count << " ops) ===" << endl;
    cout << "    (Pattern: Push 3, Pop 2, repeat)" << endl;

    // hgl::Queue
    {
        Queue<TestData> q;
        Timer t("hgl::Queue<TestData>");
        TestData data;
        
        for(int i = 0; i < count / 5; i++)
        {
            q.Push(TestData(i * 3));
            q.Push(TestData(i * 3 + 1));
            q.Push(TestData(i * 3 + 2));
            
            if(!q.IsEmpty()) q.Pop(data);
            if(!q.IsEmpty()) q.Pop(data);
        }
    }

    // std::queue
    {
        std::queue<TestData> q;
        Timer t("std::queue<TestData>");
        
        for(int i = 0; i < count / 5; i++)
        {
            q.push(TestData(i * 3));
            q.push(TestData(i * 3 + 1));
            q.push(TestData(i * 3 + 2));
            
            if(!q.empty()) q.pop();
            if(!q.empty()) q.pop();
        }
    }

    // std::deque
    {
        std::deque<TestData> q;
        Timer t("std::deque<TestData>");
        
        for(int i = 0; i < count / 5; i++)
        {
            q.push_back(TestData(i * 3));
            q.push_back(TestData(i * 3 + 1));
            q.push_back(TestData(i * 3 + 2));
            
            if(!q.empty()) q.pop_front();
            if(!q.empty()) q.pop_front();
        }
    }
}

void TestFIFOCycle(int count)
{
    cout << "\n=== Test 4: FIFO Cycle (" << count << " cycles) ===" << endl;
    cout << "    (Producer-Consumer: Push 10, Pop 10)" << endl;

    const int batch_size = 10;

    // hgl::Queue
    {
        Queue<TestData> q;
        Timer t("hgl::Queue<TestData>");
        TestData data;
        
        for(int cycle = 0; cycle < count; cycle++)
        {
            // 生产
            for(int i = 0; i < batch_size; i++)
                q.Push(TestData(cycle * batch_size + i));
            
            // 消费
            for(int i = 0; i < batch_size; i++)
                q.Pop(data);
        }
    }

    // std::queue
    {
        std::queue<TestData> q;
        Timer t("std::queue<TestData>");
        
        for(int cycle = 0; cycle < count; cycle++)
        {
            // 生产
            for(int i = 0; i < batch_size; i++)
                q.push(TestData(cycle * batch_size + i));
            
            // 消费
            for(int i = 0; i < batch_size; i++)
                q.pop();
        }
    }

    // std::deque
    {
        std::deque<TestData> q;
        Timer t("std::deque<TestData>");
        
        for(int cycle = 0; cycle < count; cycle++)
        {
            // 生产
            for(int i = 0; i < batch_size; i++)
                q.push_back(TestData(cycle * batch_size + i));
            
            // 消费
            for(int i = 0; i < batch_size; i++)
                q.pop_front();
        }
    }
}

void TestBatchPush(int count)
{
    cout << "\n=== Test 5: Batch Push (" << count << " items) ===" << endl;

    // 准备批量数据
    vector<TestData> batch_data;
    batch_data.reserve(count);
    for(int i = 0; i < count; i++)
        batch_data.push_back(TestData(i));

    // hgl::Queue (批量 Push)
    {
        Queue<TestData> q;
        Timer t("hgl::Queue<TestData> (batch)");
        q.Push(batch_data.data(), count);
    }

    // hgl::Queue (逐个 Push)
    {
        Queue<TestData> q;
        Timer t("hgl::Queue<TestData> (single)");
        for(int i = 0; i < count; i++)
            q.Push(batch_data[i]);
    }

    // std::queue (逐个 push)
    {
        std::queue<TestData> q;
        Timer t("std::queue<TestData> (single)");
        for(int i = 0; i < count; i++)
            q.push(batch_data[i]);
    }

    // std::deque (批量 insert)
    {
        std::deque<TestData> q;
        Timer t("std::deque<TestData> (batch insert)");
        q.insert(q.end(), batch_data.begin(), batch_data.end());
    }

    // std::deque (逐个 push_back)
    {
        std::deque<TestData> q;
        Timer t("std::deque<TestData> (single)");
        for(int i = 0; i < count; i++)
            q.push_back(batch_data[i]);
    }
}

void TestMemoryEfficiency()
{
    cout << "\n=== Test 6: Memory Efficiency ===" << endl;
    cout << "    (10 cycles: Push 1000, Pop 1000)" << endl;

    const int cycle_count = 10;
    const int items_per_cycle = 1000;

    // hgl::Queue
    {
        Queue<TestData> q;
        TestData data;
        
        auto start = high_resolution_clock::now();
        
        for(int cycle = 0; cycle < cycle_count; cycle++)
        {
            for(int i = 0; i < items_per_cycle; i++)
                q.Push(TestData(i));
            
            for(int i = 0; i < items_per_cycle; i++)
                q.Pop(data);
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "  " << setw(40) << left << "hgl::Queue<TestData>" 
             << ": " << setw(10) << right << duration << " μs" 
             << " (final count: " << q.GetCount() << ")" << endl;
    }

    // std::queue
    {
        std::queue<TestData> q;
        
        auto start = high_resolution_clock::now();
        
        for(int cycle = 0; cycle < cycle_count; cycle++)
        {
            for(int i = 0; i < items_per_cycle; i++)
                q.push(TestData(i));
            
            for(int i = 0; i < items_per_cycle; i++)
                q.pop();
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "  " << setw(40) << left << "std::queue<TestData>" 
             << ": " << setw(10) << right << duration << " μs" 
             << " (final count: " << q.size() << ")" << endl;
    }

    // std::deque
    {
        std::deque<TestData> q;
        
        auto start = high_resolution_clock::now();
        
        for(int cycle = 0; cycle < cycle_count; cycle++)
        {
            for(int i = 0; i < items_per_cycle; i++)
                q.push_back(TestData(i));
            
            for(int i = 0; i < items_per_cycle; i++)
                q.pop_front();
        }
        
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<microseconds>(end - start).count();
        
        cout << "  " << setw(40) << left << "std::deque<TestData>" 
             << ": " << setw(10) << right << duration << " μs" 
             << " (final count: " << q.size() << ")" << endl;
    }
}

void TestSmallDataType()
{
    cout << "\n=== Test 7: Small Data Type (int, 100K ops) ===" << endl;

    const int count = 100000;

    // hgl::Queue<int>
    {
        Queue<int> q;
        Timer t("hgl::Queue<int>");
        for(int i = 0; i < count; i++)
            q.Push(i);
        
        int val;
        for(int i = 0; i < count; i++)
            q.Pop(val);
    }

    // std::queue<int>
    {
        std::queue<int> q;
        Timer t("std::queue<int>");
        for(int i = 0; i < count; i++)
            q.push(i);
        
        for(int i = 0; i < count; i++)
            q.pop();
    }

    // std::deque<int>
    {
        std::deque<int> q;
        Timer t("std::deque<int>");
        for(int i = 0; i < count; i++)
            q.push_back(i);
        
        for(int i = 0; i < count; i++)
            q.pop_front();
    }
}

void TestIteratorPerformance()
{
    cout << "\n=== Test 8: Iterator Performance (50K items) ===" << endl;

    const int count = 50000;

    // 准备测试数据
    Queue<int> hgl_q;
    std::deque<int> std_dq;

    for(int i = 0; i < count; i++)
    {
        hgl_q.Push(i);
        std_dq.push_back(i);
    }

    // 触发 hgl::Queue 的双缓冲状态（部分在 read，部分在 write）
    int dummy;
    for(int i = 0; i < count / 4; i++)
        hgl_q.Pop(dummy);
    
    for(int i = count; i < count + count / 4; i++)
        hgl_q.Push(i);

    // hgl::Queue 迭代
    {
        Timer t("hgl::Queue<int> iteration");
        long long sum = 0;
        for(const auto& val : hgl_q)
            sum += val;
    }

    // std::deque 迭代
    {
        Timer t("std::deque<int> iteration");
        long long sum = 0;
        for(const auto& val : std_dq)
            sum += val;
    }
}

// ==================== 主程序 ====================

int os_main(int, os_char**)
{
    cout << "========================================" << endl;
    cout << "Queue Performance Benchmark" << endl;
    cout << "========================================" << endl;
    cout << "\nComparing:" << endl;
    cout << "  - hgl::Queue<T>   (dual-buffer design)" << endl;
    cout << "  - std::queue<T>   (adapter over std::deque)" << endl;
    cout << "  - std::deque<T>   (STL deque)" << endl;
    cout << "\n----------------------------------------" << endl;

    try
    {
        TestSequentialPush(100000);
        TestSequentialPop(100000);
        TestPushPopMixed(100000);
        TestFIFOCycle(10000);
        TestBatchPush(50000);
        TestMemoryEfficiency();
        TestSmallDataType();
        TestIteratorPerformance();

        cout << "\n========================================" << endl;
        cout << "Benchmark Complete" << endl;
        cout << "========================================" << endl;
        cout << "\n分析总结 / Analysis Summary:" << endl;
        cout << "  - hgl::Queue: 双缓冲设计，适合批量操作和 FIFO 场景" << endl;
        cout << "  - std::queue: 标准库适配器，通用性好" << endl;
        cout << "  - std::deque: 底层实现，灵活性最高" << endl;
        cout << "\n建议 / Recommendations:" << endl;
        cout << "  • 批量操作频繁 → 优先 hgl::Queue (批量 Push)" << endl;
        cout << "  • 严格 FIFO 场景 → hgl::Queue 或 std::deque" << endl;
        cout << "  • 通用队列需求 → std::queue (标准且可靠)" << endl;
        cout << "  • 需要双端操作 → std::deque (支持两端访问)" << endl;
        cout << "========================================" << endl;
    }
    catch(const exception& e)
    {
        cerr << "Exception: " << e.what() << endl;
        return 1;
    }

    return 0;
}
