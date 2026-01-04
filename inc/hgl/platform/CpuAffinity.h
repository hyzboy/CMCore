#pragma once

#include<stdint.h>
#include<stddef.h>

#ifndef uint
    typedef unsigned int uint;
#endif

#ifndef uint64
    typedef uint64_t uint64;
#endif

namespace hgl
{
    /**
     * NUMA节点信息
     */
    struct NumaNode
    {
        uint node_id;                   ///< NUMA节点ID
        uint cpu_count;                 ///< 该节点上的CPU数量
        uint64 memory_size;             ///< 该节点上的内存大小(字节)
        uint64 free_memory;             ///< 该节点上的可用内存大小(字节)
    };//struct NumaNode

    /**
     * CCD (Core Complex Die) 信息
     * 主要用于AMD Zen架构处理器
     */
    struct CCDInfo
    {
        uint ccd_id;                    ///< CCD ID
        uint core_count;                ///< 该CCD上的核心数量
        uint numa_node;                 ///< 所属的NUMA节点
        uint* cpu_ids;                  ///< 该CCD上的CPU ID列表
    };//struct CCDInfo

    /**
     * CPU分布信息 - 描述每个CPU核心的NUMA和CCD归属
     */
    struct CpuDistribution
    {
        uint cpu_id;                    ///< CPU逻辑ID
        uint numa_node;                 ///< 所属NUMA节点
        uint ccd_id;                    ///< 所属CCD ID (如果适用，否则为-1)
        uint physical_core;             ///< 物理核心ID
        bool is_hyperthread;            ///< 是否为超线程
    };//struct CpuDistribution

    /**
     * NUMA+CCD分布映射表
     */
    struct NumaCcdDistribution
    {
        uint total_cpus;                ///< 总CPU数量
        CpuDistribution* cpu_list;      ///< CPU分布列表
        
        uint numa_node_count;           ///< NUMA节点数量
        uint ccd_count;                 ///< CCD数量
    };//struct NumaCcdDistribution

    /**
     * CPU拓扑信息
     */
    struct CpuTopology
    {
        uint numa_node_count;           ///< NUMA节点数量
        NumaNode *numa_nodes;           ///< NUMA节点数组
        
        uint ccd_count;                 ///< CCD数量
        CCDInfo *ccds;                  ///< CCD信息数组
    };//struct CpuTopology

    /**
     * 获取CPU拓扑信息
     * @param topology 输出的拓扑信息，由调用者负责释放内存
     * @return 是否成功获取
     */
    bool GetCpuTopology(CpuTopology *topology);

    /**
     * 释放CPU拓扑信息
     * @param topology 需要释放的拓扑信息
     */
    void FreeCpuTopology(CpuTopology *topology);

    /**
     * 获取NUMA+CCD分布信息
     * 返回每个CPU核心所属的NUMA节点和CCD信息，方便开发者决定线程应该工作在哪些CPU上
     * @param distribution 输出的分布信息，由调用者负责释放内存
     * @return 是否成功获取
     */
    bool GetNumaCcdDistribution(NumaCcdDistribution *distribution);

    /**
     * 释放NUMA+CCD分布信息
     * @param distribution 需要释放的分布信息
     */
    void FreeNumaCcdDistribution(NumaCcdDistribution *distribution);

    /**
     * 设置当前进程的CPU亲合性
     * @param cpu_mask 要绑定的CPU掩码数组
     * @param mask_size 掩码数组大小(以uint64为单位)
     * @return 是否设置成功
     */
    bool SetProcessAffinity(const uint64 *cpu_mask, uint mask_size);

    /**
     * 设置当前线程的CPU亲合性
     * @param cpu_mask 要绑定的CPU掩码数组
     * @param mask_size 掩码数组大小(以uint64为单位)
     * @return 是否设置成功
     */
    bool SetThreadAffinity(const uint64 *cpu_mask, uint mask_size);

    /**
     * 获取当前进程的CPU亲合性
     * @param cpu_mask 输出的CPU掩码数组
     * @param mask_size 掩码数组大小(以uint64为单位)
     * @return 是否获取成功
     */
    bool GetProcessAffinity(uint64 *cpu_mask, uint mask_size);

    /**
     * 获取当前线程的CPU亲合性
     * @param cpu_mask 输出的CPU掩码数组
     * @param mask_size 掩码数组大小(以uint64为单位)
     * @return 是否获取成功
     */
    bool GetThreadAffinity(uint64 *cpu_mask, uint mask_size);

    /**
     * 将当前线程绑定到指定的CPU核心
     * @param cpu_id CPU核心ID
     * @return 是否绑定成功
     */
    bool BindThreadToCpu(uint cpu_id);

    /**
     * 将当前进程绑定到指定的NUMA节点
     * @param numa_node NUMA节点ID
     * @return 是否绑定成功
     */
    bool BindProcessToNumaNode(uint numa_node);

    /**
     * 将当前线程绑定到指定的NUMA节点
     * @param numa_node NUMA节点ID
     * @return 是否绑定成功
     */
    bool BindThreadToNumaNode(uint numa_node);
}//namespace hgl
