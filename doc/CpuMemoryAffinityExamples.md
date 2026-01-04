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

