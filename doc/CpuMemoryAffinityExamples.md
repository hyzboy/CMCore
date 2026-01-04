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
