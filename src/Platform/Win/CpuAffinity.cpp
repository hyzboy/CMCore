#include<hgl/platform/CpuAffinity.h>
#include<hgl/type/Smart.h>
#include<windows.h>
#include<memory.h>

namespace hgl
{
    namespace
    {
        /**
         * 获取NUMA节点信息
         */
        bool GetNumaNodeInfo(uint node_id, NumaNode *node)
        {
            if (!node) return false;

            node->node_id = node_id;
            
            ULONGLONG available_memory = 0;
            if (GetNumaAvailableMemoryNodeEx(node_id, &available_memory))
            {
                node->free_memory = available_memory;
            }
            else
            {
                node->free_memory = 0;
            }

            // 获取该节点的处理器信息
            DWORD length = 0;
            GetLogicalProcessorInformationEx(RelationNumaNode, nullptr, &length);
            
            if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
                return false;

            AutoFree<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX> buffer;
            buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)malloc(length);
            
            if (!GetLogicalProcessorInformationEx(RelationNumaNode, buffer, &length))
                return false;

            char* ptr = (char*)(buffer.data());
            uint cpu_count = 0;
            uint64 total_memory = 0;

            while (length > 0)
            {
                auto* info = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX*)ptr;
                
                if (info->Relationship == RelationNumaNode && 
                    info->NumaNode.NodeNumber == node_id)
                {
                    // 计算该节点的CPU数量
                    for (WORD i = 0; i < info->NumaNode.GroupMask.Group + 1; ++i)
                    {
                        KAFFINITY mask = info->NumaNode.GroupMask.Mask;
                        while (mask)
                        {
                            if (mask & 1) cpu_count++;
                            mask >>= 1;
                        }
                    }
                    break;
                }
                
                ptr += info->Size;
                length -= info->Size;
            }

            node->cpu_count = cpu_count;
            node->memory_size = 0; // Windows API不直接提供此信息
            
            return true;
        }
    }//namespace

    bool GetCpuTopology(CpuTopology *topology)
    {
        if (!topology) return false;

        memset(topology, 0, sizeof(CpuTopology));

        // 获取NUMA节点数量
        ULONG highest_node = 0;
        if (!GetNumaHighestNodeNumber(&highest_node))
            return false;

        topology->numa_node_count = highest_node + 1;
        topology->numa_nodes = new NumaNode[topology->numa_node_count];

        // 获取每个NUMA节点的信息
        for (uint i = 0; i < topology->numa_node_count; ++i)
        {
            if (!GetNumaNodeInfo(i, &topology->numa_nodes[i]))
            {
                delete[] topology->numa_nodes;
                topology->numa_nodes = nullptr;
                topology->numa_node_count = 0;
                return false;
            }
        }

        // Windows API不直接提供CCD信息，设置为0
        topology->ccd_count = 0;
        topology->ccds = nullptr;

        return true;
    }

    void FreeCpuTopology(CpuTopology *topology)
    {
        if (!topology) return;

        if (topology->numa_nodes)
        {
            delete[] topology->numa_nodes;
            topology->numa_nodes = nullptr;
        }

        if (topology->ccds)
        {
            delete[] topology->ccds;
            topology->ccds = nullptr;
        }

        topology->numa_node_count = 0;
        topology->ccd_count = 0;
    }

    bool SetProcessAffinity(const uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        HANDLE process = GetCurrentProcess();
        
        // Windows使用DWORD_PTR作为亲合性掩码
        // 对于64位系统，一个DWORD_PTR可以表示64个CPU
        DWORD_PTR affinity_mask = 0;
        
        if (mask_size > 0)
        {
            affinity_mask = static_cast<DWORD_PTR>(cpu_mask[0]);
        }

        return SetProcessAffinityMask(process, affinity_mask) != 0;
    }

    bool SetThreadAffinity(const uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        HANDLE thread = GetCurrentThread();
        
        DWORD_PTR affinity_mask = 0;
        
        if (mask_size > 0)
        {
            affinity_mask = static_cast<DWORD_PTR>(cpu_mask[0]);
        }

        return SetThreadAffinityMask(thread, affinity_mask) != (DWORD_PTR)-1;
    }

    bool GetProcessAffinity(uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        HANDLE process = GetCurrentProcess();
        DWORD_PTR process_mask = 0;
        DWORD_PTR system_mask = 0;

        if (!GetProcessAffinityMask(process, &process_mask, &system_mask))
            return false;

        for (uint i = 0; i < mask_size; ++i)
        {
            cpu_mask[i] = 0;
        }

        cpu_mask[0] = static_cast<uint64>(process_mask);
        return true;
    }

    bool GetThreadAffinity(uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        // Windows没有直接获取线程亲合性的API
        // 使用临时设置并恢复的方法
        HANDLE thread = GetCurrentThread();
        DWORD_PTR old_mask = SetThreadAffinityMask(thread, (DWORD_PTR)-1);
        
        if (old_mask == 0)
            return false;

        // 恢复原来的掩码
        SetThreadAffinityMask(thread, old_mask);

        for (uint i = 0; i < mask_size; ++i)
        {
            cpu_mask[i] = 0;
        }

        cpu_mask[0] = static_cast<uint64>(old_mask);
        return true;
    }

    bool BindThreadToCpu(uint cpu_id)
    {
        HANDLE thread = GetCurrentThread();
        DWORD_PTR mask = static_cast<DWORD_PTR>(1) << cpu_id;
        
        return SetThreadAffinityMask(thread, mask) != (DWORD_PTR)-1;
    }

    bool BindProcessToNumaNode(uint numa_node)
    {
        HANDLE process = GetCurrentProcess();
        
        // 获取NUMA节点的处理器掩码
        ULONGLONG process_mask = 0;
        if (!GetNumaNodeProcessorMaskEx(numa_node, (PGROUP_AFFINITY)&process_mask))
            return false;

        return SetProcessAffinityMask(process, (DWORD_PTR)process_mask) != 0;
    }

    bool BindThreadToNumaNode(uint numa_node)
    {
        // 获取NUMA节点的处理器掩码
        GROUP_AFFINITY group_affinity;
        if (!GetNumaNodeProcessorMaskEx(numa_node, &group_affinity))
            return false;

        HANDLE thread = GetCurrentThread();
        GROUP_AFFINITY old_affinity;
        
        return SetThreadGroupAffinity(thread, &group_affinity, &old_affinity) != 0;
    }
}//namespace hgl
