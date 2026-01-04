# CPU/Memory Affinity Functions Usage Examples

本文档提供了CPU亲合性和内存亲合性功能的使用示例。

## 1. 获取CPU拓扑信息

```cpp
#include <hgl/platform/CpuAffinity.h>

void PrintCpuTopology()
{
    hgl::CpuTopology topology;
    
    if (hgl::GetCpuTopology(&topology))
    {
        printf("NUMA节点数量: %u\n", topology.numa_node_count);
        
        for (uint i = 0; i < topology.numa_node_count; ++i)
        {
            const hgl::NumaNode& node = topology.numa_nodes[i];
            printf("节点 %u:\n", node.node_id);
            printf("  CPU数量: %u\n", node.cpu_count);
            printf("  内存大小: %llu MB\n", node.memory_size / (1024 * 1024));
            printf("  可用内存: %llu MB\n", node.free_memory / (1024 * 1024));
        }
        
        hgl::FreeCpuTopology(&topology);
    }
}
```

## 注意事项

1. **NUMA节点编号**: NUMA节点编号从0开始，使用前请确认系统的NUMA拓扑。
2. **错误处理**: 所有函数都会返回成功/失败状态，请检查返回值。
3. **平台差异**: 
   - Windows: 使用Windows NUMA API
   - Linux: 优先使用libnuma，若不可用则使用sysfs接口

## 9. 获取NUMA+CCD分布信息（新增）

```cpp
#include <hgl/platform/CpuAffinity.h>
#include <stdio.h>

void PrintCpuDistribution()
{
    hgl::NumaCcdDistribution dist;
    
    if (hgl::GetNumaCcdDistribution(&dist))
    {
        printf("系统总CPU数量: %u\n", dist.total_cpus);
        printf("NUMA节点数量: %u\n", dist.numa_node_count);
        printf("CCD数量: %u\n", dist.ccd_count);
        printf("\n详细分布信息:\n");
        printf("CPU_ID | NUMA节点 | CCD_ID | 物理核心 | 超线程\n");
        printf("-------|---------|--------|----------|--------\n");
        
        for (uint i = 0; i < dist.total_cpus; ++i)
        {
            const hgl::CpuDistribution& cpu = dist.cpu_list[i];
            printf("  %3u  |    %2u   |   %3u  |    %3u   |   %s\n",
                   cpu.cpu_id,
                   cpu.numa_node,
                   cpu.ccd_id == (uint)-1 ? 999 : cpu.ccd_id,
                   cpu.physical_core,
                   cpu.is_hyperthread ? "是" : "否");
        }
        
        hgl::FreeNumaCcdDistribution(&dist);
    }
}

// 示例：根据NUMA节点分配线程
void AssignThreadsByNuma()
{
    hgl::NumaCcdDistribution dist;
    
    if (hgl::GetNumaCcdDistribution(&dist))
    {
        // 为每个NUMA节点分配工作线程
        for (uint node = 0; node < dist.numa_node_count; ++node)
        {
            printf("NUMA节点 %u 的CPU列表: ", node);
            
            for (uint i = 0; i < dist.total_cpus; ++i)
            {
                if (dist.cpu_list[i].numa_node == node)
                {
                    printf("%u ", dist.cpu_list[i].cpu_id);
                }
            }
            printf("\n");
        }
        
        hgl::FreeNumaCcdDistribution(&dist);
    }
}

// 示例：只使用物理核心，避免超线程
void UsePhysicalCoresOnly()
{
    hgl::NumaCcdDistribution dist;
    
    if (hgl::GetNumaCcdDistribution(&dist))
    {
        printf("物理核心（非超线程）CPU列表: ");
        
        for (uint i = 0; i < dist.total_cpus; ++i)
        {
            if (!dist.cpu_list[i].is_hyperthread)
            {
                printf("%u ", dist.cpu_list[i].cpu_id);
            }
        }
        printf("\n");
        
        hgl::FreeNumaCcdDistribution(&dist);
    }
}

// 示例：按CCD分配线程（AMD Ryzen/EPYC）
void AssignThreadsByCcd()
{
    hgl::NumaCcdDistribution dist;
    
    if (hgl::GetNumaCcdDistribution(&dist))
    {
        if (dist.ccd_count > 0)
        {
            std::map<uint, std::vector<uint>> ccd_to_cpus;
            
            for (uint i = 0; i < dist.total_cpus; ++i)
            {
                if (dist.cpu_list[i].ccd_id != (uint)-1)
                {
                    ccd_to_cpus[dist.cpu_list[i].ccd_id].push_back(dist.cpu_list[i].cpu_id);
                }
            }
            
            for (const auto& pair : ccd_to_cpus)
            {
                printf("CCD %u 的CPU列表: ", pair.first);
                for (uint cpu : pair.second)
                {
                    printf("%u ", cpu);
                }
                printf("\n");
            }
        }
        
        hgl::FreeNumaCcdDistribution(&dist);
    }
}
```

### 使用场景说明

**GetNumaCcdDistribution()** 函数返回完整的CPU到NUMA/CCD的映射关系，让开发者可以：

1. **智能线程分配**: 根据NUMA拓扑将线程分配到合适的CPU上
2. **避免跨节点访问**: 确保线程和数据在同一个NUMA节点
3. **CCD感知调度**: 在AMD处理器上利用CCD结构优化性能
4. **超线程控制**: 选择性地使用或避免超线程CPU
5. **性能分析**: 了解系统的硬件拓扑结构


## 10. ARM big.LITTLE 大小核支持（iOS/Android）

### 10.1 获取ARM大小核分布信息

```cpp
#include <hgl/platform/CpuAffinity.h>

void PrintArmCoreDistribution()
{
#if defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
    hgl::ArmBigLittleDistribution dist;
    
    if (hgl::GetArmBigLittleDistribution(&dist))
    {
        printf("ARM CPU信息:\n");
        printf("总核心数: %u\n", dist.total_cores);
        printf("大核(性能核)数量: %u\n", dist.big_core_count);
        printf("小核(效率核)数量: %u\n", dist.little_core_count);
        printf("\n详细核心信息:\n");
        printf("核心ID | 类型 | 最大频率(MHz) | Cluster\n");
        printf("-------|------|---------------|--------\n");
        
        for (uint i = 0; i < dist.total_cores; ++i)
        {
            const hgl::ArmCoreInfo& core = dist.core_list[i];
            printf("  %2u   | %s |    %4u      |   %u\n",
                   core.core_id,
                   core.is_big_core ? "大核" : "小核",
                   core.max_frequency,
                   core.cluster_id);
        }
        
        printf("\n大核ID列表: ");
        for (uint i = 0; i < dist.big_core_count; ++i)
            printf("%u ", dist.big_core_ids[i]);
        
        printf("\n小核ID列表: ");
        for (uint i = 0; i < dist.little_core_count; ++i)
            printf("%u ", dist.little_core_ids[i]);
        printf("\n");
        
        hgl::FreeArmBigLittleDistribution(&dist);
    }
#else
    printf("此功能仅在ARM架构上可用\n");
#endif
}
```

### 10.2 将线程绑定到性能核心（大核）

```cpp
#include <hgl/platform/CpuAffinity.h>

void RunComputeTaskOnBigCores()
{
#if defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
    // 将当前线程绑定到性能核心
    if (hgl::BindThreadToBigCores())
    {
        printf("成功绑定到大核，开始执行计算密集型任务\n");
        
        // 执行计算密集型任务
        PerformHeavyComputation();
    }
    else
    {
        printf("绑定大核失败，使用默认调度\n");
    }
#endif
}
```

### 10.3 将后台任务绑定到效率核心（小核）

```cpp
#include <hgl/platform/CpuAffinity.h>

void RunBackgroundTaskOnLittleCores()
{
#if defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
    // 将后台任务绑定到效率核心以节省电量
    if (hgl::BindThreadToLittleCores())
    {
        printf("成功绑定到小核，执行后台任务\n");
        
        // 执行不需要高性能的后台任务
        PerformBackgroundSync();
    }
#endif
}
```

### 10.4 iOS应用示例：根据任务类型分配核心

```cpp
// iOS/macOS使用QoS来影响核心分配
#include <hgl/platform/CpuAffinity.h>
#include <thread>

void iOSTaskScheduler()
{
#ifdef __APPLE__
    // 主渲染线程 - 使用性能核心
    std::thread render_thread([]() {
        hgl::BindThreadToBigCores();  // 设置为USER_INTERACTIVE QoS
        while (running)
        {
            RenderFrame();
        }
    });
    
    // 后台数据同步 - 使用效率核心
    std::thread sync_thread([]() {
        hgl::BindThreadToLittleCores();  // 设置为BACKGROUND QoS
        while (running)
        {
            SyncData();
            std::this_thread::sleep_for(std::chrono::seconds(60));
        }
    });
    
    render_thread.join();
    sync_thread.join();
#endif
}
```

### 10.5 Android应用示例：游戏性能优化

```cpp
#include <hgl/platform/CpuAffinity.h>
#include <thread>

void AndroidGameOptimization()
{
#ifdef __ANDROID__
    hgl::ArmBigLittleDistribution dist;
    
    if (hgl::GetArmBigLittleDistribution(&dist))
    {
        // 游戏主循环 - 绑定到所有大核
        std::thread game_thread([]() {
            if (hgl::BindThreadToBigCores())
            {
                printf("游戏线程运行在性能核心上\n");
                GameMainLoop();
            }
        });
        
        // 音频处理 - 可以在小核上运行
        std::thread audio_thread([]() {
            if (hgl::BindThreadToLittleCores())
            {
                printf("音频线程运行在效率核心上\n");
                AudioProcessingLoop();
            }
        });
        
        // 网络通信 - 使用小核
        std::thread network_thread([]() {
            hgl::BindThreadToLittleCores();
            NetworkUpdateLoop();
        });
        
        game_thread.join();
        audio_thread.join();
        network_thread.join();
        
        hgl::FreeArmBigLittleDistribution(&dist);
    }
#endif
}
```

### 10.6 动态核心选择

```cpp
#include <hgl/platform/CpuAffinity.h>

void DynamicCoreSelection(bool is_battery_low, bool is_performance_mode)
{
#if defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
    if (is_battery_low)
    {
        // 省电模式：使用小核
        printf("电量低，切换到效率核心\n");
        hgl::BindThreadToLittleCores();
    }
    else if (is_performance_mode)
    {
        // 性能模式：使用大核
        printf("性能模式，使用性能核心\n");
        hgl::BindThreadToBigCores();
    }
    else
    {
        // 平衡模式：让系统自动调度
        printf("平衡模式，使用系统默认调度\n");
        // 不设置亲和性，使用系统默认调度
    }
#endif
}
```

### 10.7 核心类型切换

```cpp
#include <hgl/platform/CpuAffinity.h>

class AdaptiveTaskScheduler
{
private:
    bool using_big_cores = false;
    
public:
    void SwitchToPerformanceMode()
    {
#if defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
        if (!using_big_cores)
        {
            if (hgl::BindThreadToCoreType(true))
            {
                using_big_cores = true;
                printf("切换到性能核心\n");
            }
        }
#endif
    }
    
    void SwitchToEfficiencyMode()
    {
#if defined(__ARM_ARCH) || defined(__aarch64__) || defined(_M_ARM) || defined(_M_ARM64)
        if (using_big_cores)
        {
            if (hgl::BindThreadToCoreType(false))
            {
                using_big_cores = false;
                printf("切换到效率核心\n");
            }
        }
#endif
    }
    
    void AdaptToWorkload(double cpu_usage)
    {
        if (cpu_usage > 0.8)
            SwitchToPerformanceMode();
        else if (cpu_usage < 0.3)
            SwitchToEfficiencyMode();
    }
};
```

### ARM big.LITTLE 注意事项

1. **平台差异**:
   - **iOS/macOS**: 使用QoS (Quality of Service)间接影响核心分配，系统保留最终决定权
   - **Android**: 直接使用CPU亲和性绑定到特定核心，控制更精确

2. **核心检测**:
   - **iOS**: 通过`hw.perflevel0/perflevel1`系统参数识别性能/效率核心
   - **Android**: 通过`/sys/devices/system/cpu/cpufreq`读取频率信息判断核心类型

3. **电池影响**:
   - 长时间使用大核会显著增加功耗
   - 建议根据任务类型和电量状态动态调整

4. **系统权限**:
   - Android可能需要特定权限才能设置CPU亲和性
   - iOS的QoS设置通常无需特殊权限

5. **性能建议**:
   - 游戏渲染、视频编码等高负载任务使用大核
   - 网络通信、文件I/O等I/O密集任务使用小核
   - 后台同步、日志记录等低优先级任务使用小核

