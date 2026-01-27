#include<hgl/platform/CpuAffinity.h>
#include<pthread.h>
#include<sched.h>
#include<unistd.h>
#include<string.h>
#include<fstream>
#include<sstream>
#include<set>
#include<map>

// NUMA support (optional, may not be available on all systems)
#ifdef __has_include
#  if __has_include(<numa.h>)
#    define HGL_HAS_NUMA 1
#    include<numa.h>
#  endif
#  if __has_include(<numaif.h>)
#    include<numaif.h>
#  endif
#endif

namespace hgl
{
    namespace
    {
        /**
         * 从/sys/devices/system/node获取NUMA信息
         */
        bool GetNumaNodeInfoFromSys(uint node_id, NumaNode *node)
        {
            if (!node) return false;

            node->node_id = node_id;
            node->cpu_count = 0;
            node->memory_size = 0;
            node->free_memory = 0;

            // 读取CPU列表
            std::ostringstream path;
            path << "/sys/devices/system/node/node" << node_id << "/cpulist";

            std::ifstream cpulist(path.str());
            if (cpulist.is_open())
            {
                std::string line;
                if (std::getline(cpulist, line))
                {
                    // 解析CPU范围，例如 "0-3,8-11"
                    std::istringstream iss(line);
                    std::string token;
                    std::set<int> cpus;

                    while (std::getline(iss, token, ','))
                    {
                        size_t dash = token.find('-');
                        if (dash != std::string::npos)
                        {
                            int start = std::stoi(token.substr(0, dash));
                            int end = std::stoi(token.substr(dash + 1));
                            for (int i = start; i <= end; ++i)
                                cpus.insert(i);
                        }
                        else
                        {
                            cpus.insert(std::stoi(token));
                        }
                    }
                    node->cpu_count = cpus.size();
                }
            }

            // 读取内存信息
            path.str("");
            path << "/sys/devices/system/node/node" << node_id << "/meminfo";

            std::ifstream meminfo(path.str());
            if (meminfo.is_open())
            {
                std::string line;
                while (std::getline(meminfo, line))
                {
                    if (line.find("MemTotal:") != std::string::npos)
                    {
                        std::istringstream iss(line);
                        std::string key, unit;
                        uint64 value;
                        iss >> key >> key >> value >> unit;
                        node->memory_size = value * 1024; // 转换为字节
                    }
                    else if (line.find("MemFree:") != std::string::npos)
                    {
                        std::istringstream iss(line);
                        std::string key, unit;
                        uint64 value;
                        iss >> key >> key >> value >> unit;
                        node->free_memory = value * 1024; // 转换为字节
                    }
                }
            }

            return true;
        }
    }//namespace

    bool GetCpuTopology(CpuTopology *topology)
    {
        if (!topology) return false;

        memset(topology, 0, sizeof(CpuTopology));

#ifdef HGL_HAS_NUMA
        // 使用libnuma获取NUMA信息
        if (numa_available() == 0)
        {
            int max_node = numa_max_node();
            if (max_node >= 0)
            {
                topology->numa_node_count = max_node + 1;
                topology->numa_nodes = new NumaNode[topology->numa_node_count];

                for (int i = 0; i <= max_node; ++i)
                {
                    GetNumaNodeInfoFromSys(i, &topology->numa_nodes[i]);
                }

                topology->ccd_count = 0;
                topology->ccds = nullptr;

                return true;
            }
        }
#endif

        // 回退到从/sys读取
        topology->numa_node_count = 0;

        // 检测有多少个NUMA节点
        for (uint i = 0; i < 256; ++i)
        {
            std::ostringstream path;
            path << "/sys/devices/system/node/node" << i;

            std::ifstream test(path.str());
            if (test.good())
                topology->numa_node_count = i + 1;
            else
                break;
        }

        if (topology->numa_node_count == 0)
        {
            // 没有NUMA信息，假设单节点
            topology->numa_node_count = 1;
            topology->numa_nodes = new NumaNode[1];
            topology->numa_nodes[0].node_id = 0;
            topology->numa_nodes[0].cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
            topology->numa_nodes[0].memory_size = 0;
            topology->numa_nodes[0].free_memory = 0;
        }
        else
        {
            topology->numa_nodes = new NumaNode[topology->numa_node_count];
            for (uint i = 0; i < topology->numa_node_count; ++i)
            {
                GetNumaNodeInfoFromSys(i, &topology->numa_nodes[i]);
            }
        }

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

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        // 将掩码转换为cpu_set_t
        for (uint i = 0; i < mask_size && i < CPU_SETSIZE / 64; ++i)
        {
            uint64 mask = cpu_mask[i];
            for (uint bit = 0; bit < 64 && (i * 64 + bit) < CPU_SETSIZE; ++bit)
            {
                if (mask & (1ULL << bit))
                {
                    CPU_SET(i * 64 + bit, &cpuset);
                }
            }
        }

        return sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0;
    }

    bool SetThreadAffinity(const uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        // 将掩码转换为cpu_set_t
        for (uint i = 0; i < mask_size && i < CPU_SETSIZE / 64; ++i)
        {
            uint64 mask = cpu_mask[i];
            for (uint bit = 0; bit < 64 && (i * 64 + bit) < CPU_SETSIZE; ++bit)
            {
                if (mask & (1ULL << bit))
                {
                    CPU_SET(i * 64 + bit, &cpuset);
                }
            }
        }

        return pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0;
    }

    bool GetProcessAffinity(uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        if (sched_getaffinity(0, sizeof(cpuset), &cpuset) != 0)
            return false;

        // 清零输出数组
        for (uint i = 0; i < mask_size; ++i)
            cpu_mask[i] = 0;

        // 将cpu_set_t转换为掩码数组
        for (uint cpu = 0; cpu < CPU_SETSIZE; ++cpu)
        {
            if (CPU_ISSET(cpu, &cpuset))
            {
                uint idx = cpu / 64;
                uint bit = cpu % 64;
                if (idx < mask_size)
                {
                    cpu_mask[idx] |= (1ULL << bit);
                }
            }
        }

        return true;
    }

    bool GetThreadAffinity(uint64 *cpu_mask, uint mask_size)
    {
        if (!cpu_mask || mask_size == 0) return false;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        if (pthread_getaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) != 0)
            return false;

        // 清零输出数组
        for (uint i = 0; i < mask_size; ++i)
            cpu_mask[i] = 0;

        // 将cpu_set_t转换为掩码数组
        for (uint cpu = 0; cpu < CPU_SETSIZE; ++cpu)
        {
            if (CPU_ISSET(cpu, &cpuset))
            {
                uint idx = cpu / 64;
                uint bit = cpu % 64;
                if (idx < mask_size)
                {
                    cpu_mask[idx] |= (1ULL << bit);
                }
            }
        }

        return true;
    }

    bool BindThreadToCpu(uint cpu_id)
    {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(cpu_id, &cpuset);

        return pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0;
    }

    bool BindProcessToNumaNode(uint numa_node)
    {
#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            struct bitmask *nodemask = numa_allocate_nodemask();
            numa_bitmask_clearall(nodemask);
            numa_bitmask_setbit(nodemask, numa_node);

            numa_bind(nodemask);

            numa_free_nodemask(nodemask);
            return true;
        }
#endif

        // 如果没有libnuma，尝试手动设置CPU亲合性
        NumaNode node;
        if (!GetNumaNodeInfoFromSys(numa_node, &node))
            return false;

        // 读取该节点的CPU列表并设置亲合性
        std::ostringstream path;
        path << "/sys/devices/system/node/node" << numa_node << "/cpulist";

        std::ifstream cpulist(path.str());
        if (!cpulist.is_open())
            return false;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        std::string line;
        if (std::getline(cpulist, line))
        {
            std::istringstream iss(line);
            std::string token;

            while (std::getline(iss, token, ','))
            {
                size_t dash = token.find('-');
                if (dash != std::string::npos)
                {
                    int start = std::stoi(token.substr(0, dash));
                    int end = std::stoi(token.substr(dash + 1));
                    for (int i = start; i <= end; ++i)
                        CPU_SET(i, &cpuset);
                }
                else
                {
                    CPU_SET(std::stoi(token), &cpuset);
                }
            }
        }

        return sched_setaffinity(0, sizeof(cpuset), &cpuset) == 0;
    }

    bool BindThreadToNumaNode(uint numa_node)
    {
#ifdef HGL_HAS_NUMA
        if (numa_available() == 0)
        {
            struct bitmask *nodemask = numa_allocate_nodemask();
            numa_bitmask_clearall(nodemask);
            numa_bitmask_setbit(nodemask, numa_node);

            numa_run_on_node_mask(nodemask);

            numa_free_nodemask(nodemask);
            return true;
        }
#endif

        // 如果没有libnuma，尝试手动设置CPU亲合性
        std::ostringstream path;
        path << "/sys/devices/system/node/node" << numa_node << "/cpulist";

        std::ifstream cpulist(path.str());
        if (!cpulist.is_open())
            return false;

        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        std::string line;
        if (std::getline(cpulist, line))
        {
            std::istringstream iss(line);
            std::string token;

            while (std::getline(iss, token, ','))
            {
                size_t dash = token.find('-');
                if (dash != std::string::npos)
                {
                    int start = std::stoi(token.substr(0, dash));
                    int end = std::stoi(token.substr(dash + 1));
                    for (int i = start; i <= end; ++i)
                        CPU_SET(i, &cpuset);
                }
                else
                {
                    CPU_SET(std::stoi(token), &cpuset);
                }
            }
        }

        return pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0;
    }

    bool GetNumaCcdDistribution(NumaCcdDistribution *distribution)
    {
        if (!distribution) return false;

        memset(distribution, 0, sizeof(NumaCcdDistribution));

        // 获取系统CPU数量
        long num_cpus = sysconf(_SC_NPROCESSORS_ONLN);
        if (num_cpus <= 0) return false;

        distribution->total_cpus = static_cast<uint>(num_cpus);
        distribution->cpu_list = new CpuDistribution[distribution->total_cpus];

        // 初始化CPU列表
        for (uint i = 0; i < distribution->total_cpus; ++i)
        {
            distribution->cpu_list[i].cpu_id = i;
            distribution->cpu_list[i].numa_node = 0;
            distribution->cpu_list[i].ccd_id = (uint)-1;
            distribution->cpu_list[i].physical_core = i;
            distribution->cpu_list[i].is_hyperthread = false;
        }

        std::set<uint> numa_nodes;
        std::map<uint, uint> physical_to_first_logical;

        // 从/sys/devices/system/cpu读取CPU信息
        for (uint cpu_id = 0; cpu_id < distribution->total_cpus; ++cpu_id)
        {
            // 读取NUMA节点
            std::ostringstream numa_path;
            numa_path << "/sys/devices/system/cpu/cpu" << cpu_id << "/node";

            std::ifstream numa_file(numa_path.str());
            if (numa_file.is_open())
            {
                std::string line;
                if (std::getline(numa_file, line))
                {
                    // 解析 "node0" 或者直接是数字
                    size_t node_pos = line.find("node");
                    if (node_pos != std::string::npos)
                    {
                        uint node_num = std::stoi(line.substr(node_pos + 4));
                        distribution->cpu_list[cpu_id].numa_node = node_num;
                        numa_nodes.insert(node_num);
                    }
                }
            }

            // 读取物理核心ID
            std::ostringstream core_path;
            core_path << "/sys/devices/system/cpu/cpu" << cpu_id << "/topology/core_id";

            std::ifstream core_file(core_path.str());
            if (core_file.is_open())
            {
                uint physical_core;
                if (core_file >> physical_core)
                {
                    distribution->cpu_list[cpu_id].physical_core = physical_core;

                    // 检测超线程：如果这个物理核心已经有对应的逻辑CPU，则当前是超线程
                    if (physical_to_first_logical.find(physical_core) != physical_to_first_logical.end())
                    {
                        distribution->cpu_list[cpu_id].is_hyperthread = true;
                    }
                    else
                    {
                        physical_to_first_logical[physical_core] = cpu_id;
                    }
                }
            }

            // 尝试读取CCD信息（仅AMD处理器）
            // 检查 /sys/devices/system/cpu/cpu*/cache/index3/id 来推断CCD
            std::ostringstream cache_path;
            cache_path << "/sys/devices/system/cpu/cpu" << cpu_id << "/cache/index3/id";

            std::ifstream cache_file(cache_path.str());
            if (cache_file.is_open())
            {
                uint cache_id;
                if (cache_file >> cache_id)
                {
                    // L3缓存ID可以作为CCD的近似标识
                    distribution->cpu_list[cpu_id].ccd_id = cache_id;
                }
            }
        }

        distribution->numa_node_count = numa_nodes.empty() ? 1 : numa_nodes.size();

        // 计算唯一的CCD数量
        std::set<uint> unique_ccds;
        for (uint i = 0; i < distribution->total_cpus; ++i)
        {
            if (distribution->cpu_list[i].ccd_id != (uint)-1)
            {
                unique_ccds.insert(distribution->cpu_list[i].ccd_id);
            }
        }
        distribution->ccd_count = unique_ccds.size();

        return true;
    }

    void FreeNumaCcdDistribution(NumaCcdDistribution *distribution)
    {
        if (!distribution) return;

        if (distribution->cpu_list)
        {
            delete[] distribution->cpu_list;
            distribution->cpu_list = nullptr;
        }

        distribution->total_cpus = 0;
        distribution->numa_node_count = 0;
        distribution->ccd_count = 0;
    }
}//namespace hgl
