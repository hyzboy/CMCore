# CPU/内存亲合性功能说明

本文档详细说明CMCore库中新增的CPU亲合性和内存亲合性功能。

## 功能概述

本次更新为CMCore添加了全面的CPU和内存亲合性支持，主要功能包括：

1. **CPU拓扑信息获取**: 获取系统的NUMA节点和CCD信息
2. **CPU亲合性控制**: 设置进程和线程的CPU绑定
3. **NUMA感知的内存分配**: 在指定NUMA节点上分配内存
4. **内存策略管理**: 控制内存分配的NUMA策略

## 支持的平台

- **Windows**: 使用Windows NUMA API (VirtualAllocExNuma, GetNumaHighestNodeNumber等)
- **Linux/UNIX**: 
  - 优先使用libnuma库（如果可用）
  - 回退到sysfs接口和标准pthread API
- **macOS**: 基础线程亲合性支持

## 新增的头文件

### 1. CpuAffinity.h

提供CPU亲合性相关的结构和函数：

**数据结构**:
- `NumaNode`: NUMA节点信息（节点ID、CPU数量、内存大小）
- `CCDInfo`: CCD信息（主要用于AMD Zen架构）
- `CpuTopology`: CPU拓扑信息（包含所有NUMA节点和CCD信息）

**函数**:
- `GetCpuTopology()`: 获取CPU拓扑信息
- `FreeCpuTopology()`: 释放拓扑信息
- `SetProcessAffinity()`: 设置进程CPU亲合性
- `SetThreadAffinity()`: 设置线程CPU亲合性
- `GetProcessAffinity()`: 获取进程CPU亲合性
- `GetThreadAffinity()`: 获取线程CPU亲合性
- `BindThreadToCpu()`: 将线程绑定到特定CPU核心
- `BindProcessToNumaNode()`: 将进程绑定到NUMA节点
- `BindThreadToNumaNode()`: 将线程绑定到NUMA节点

### 2. MemoryAffinity.h

提供NUMA感知的内存分配函数：

**基础内存操作**:
- `NumaAlloc()`: 在指定NUMA节点分配内存
- `NumaFree()`: 释放NUMA内存

**模板函数 - 对象分配**:
- `NumaNew<T>()`: 在指定NUMA节点创建对象
- `NumaDelete<T>()`: 删除NUMA对象

**模板函数 - 数组分配**:
- `NumaNewArray<T>()`: 在指定NUMA节点分配数组
- `NumaDeleteArray<T>()`: 删除NUMA数组

**内存策略**:
- `SetMemoryPolicy()`: 设置内存分配策略
- `ResetMemoryPolicy()`: 恢复默认策略
- `GetMemoryNode()`: 查询内存所在节点
- `MigrateMemory()`: 迁移内存到其他节点

## 实现细节

### Windows实现

**CPU亲合性**:
- 使用`GetLogicalProcessorInformationEx()`获取NUMA拓扑
- 使用`SetProcessAffinityMask()`和`SetThreadAffinityMask()`设置亲合性
- 使用`SetThreadGroupAffinity()`支持超过64个逻辑处理器的系统

**内存亲合性**:
- 使用`VirtualAllocExNuma()`在指定节点分配内存
- 使用`GetNumaAvailableMemoryNodeEx()`查询节点信息

### Linux/UNIX实现

**CPU亲合性**:
- 使用`numa_available()`检测libnuma支持
- 使用`pthread_setaffinity_np()`设置线程亲合性
- 使用`sched_setaffinity()`设置进程亲合性
- 从`/sys/devices/system/node/`读取NUMA信息（回退方案）

**内存亲合性**:
- 优先使用`numa_alloc_onnode()`（libnuma）
- 回退到`mmap()`分配
- 使用`numa_set_membind()`设置内存策略
- 使用`numa_migrate_pages()`迁移内存

## 使用建议

### 1. 高性能计算场景

```cpp
// 为每个NUMA节点创建一个工作线程
for (int node = 0; node < numa_node_count; ++node)
{
    std::thread worker([node]() {
        // 绑定线程到NUMA节点
        hgl::BindThreadToNumaNode(node);
        
        // 在本地节点分配数据
        void* data = hgl::NumaAlloc(DATA_SIZE, node);
        
        // 处理数据（最小化跨节点访问）
        process_data(data);
        
        hgl::NumaFree(data, DATA_SIZE);
    });
}
```

### 2. 缓存优化

```cpp
// 将关键线程绑定到同一个物理核心
hgl::BindThreadToCpu(0);  // 主线程在CPU 0
hgl::BindThreadToCpu(1);  // 辅助线程在CPU 1（假设1和0共享缓存）
```

### 3. 内存密集型应用

```cpp
// 设置内存策略后，后续malloc也会受影响
hgl::SetMemoryPolicy(preferred_node);

// 标准分配也会考虑NUMA
void* data = malloc(size);

// 恢复默认策略
hgl::ResetMemoryPolicy();
```

## 性能注意事项

1. **跨节点访问延迟**: 在NUMA系统上，跨节点内存访问比本地访问慢30-50%
2. **线程迁移开销**: 频繁改变线程亲合性会导致缓存失效
3. **内存迁移成本**: 内存迁移会产生显著开销，应谨慎使用
4. **负载均衡**: 过度的亲合性绑定可能导致负载不均衡

## 编译要求

**Linux/UNIX**:
- 可选：libnuma-dev（推荐，提供完整NUMA支持）
- pthread库（必需）
- C++11或更高版本

**Windows**:
- Windows Vista或更高版本（NUMA API支持）
- Visual Studio 2015或更高版本

## 兼容性

- 在不支持NUMA的系统上，函数会回退到标准实现
- 单NUMA节点系统也能正常工作
- 所有函数都有错误返回值，便于处理不支持的情况

## 文件列表

### 头文件
- `inc/hgl/platform/CpuAffinity.h` - CPU亲合性接口
- `inc/hgl/platform/MemoryAffinity.h` - 内存亲合性接口

### 实现文件
- `src/Platform/Win/CpuAffinity.cpp` - Windows实现
- `src/Platform/Win/MemoryAffinity.cpp` - Windows实现
- `src/Platform/UNIX/CpuAffinity.cpp` - Linux/UNIX实现
- `src/Platform/UNIX/MemoryAffinity.cpp` - Linux/UNIX实现

### 文档
- `doc/CpuMemoryAffinityExamples.md` - 使用示例
- `doc/CPU_Memory_Affinity_README.md` - 本文档

## 未来改进方向

1. 支持更精细的CCD检测（AMD Zen架构）
2. 增加性能监控和统计功能
3. 支持动态负载均衡
4. 增加对Apple Silicon的优化支持
