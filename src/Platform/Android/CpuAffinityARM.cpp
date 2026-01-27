#if defined(__ANDROID__) && (defined(__ARM_ARCH) || defined(__aarch64__))

#include<hgl/platform/CpuAffinity.h>
#include<pthread.h>
#include<sched.h>
#include<unistd.h>
#include<string.h>
#include<fstream>
#include<sstream>
#include<vector>
#include<map>
#include<algorithm>

namespace hgl
{
    namespace
    {
        /**
         * 从/sys/devices/system/cpu读取核心频率信息
         */
        uint GetCoreMaxFrequency(uint cpu_id)
        {
            std::ostringstream path;
            path << "/sys/devices/system/cpu/cpu" << cpu_id << "/cpufreq/cpuinfo_max_freq";

            std::ifstream freq_file(path.str());
            if (freq_file.is_open())
            {
                uint freq_khz;
                if (freq_file >> freq_khz)
                {
                    return freq_khz / 1000; // 转换为MHz
                }
            }

            return 0;
        }

        /**
         * 判断核心类型：通过频率判断大小核
         */
        bool IsBigCore(uint frequency, uint threshold_mhz)
        {
            return frequency >= threshold_mhz;
        }

        /**
         * 获取CPU拓扑信息来识别cluster
         */
        uint GetCoreClusterId(uint cpu_id)
        {
            std::ostringstream path;
            path << "/sys/devices/system/cpu/cpu" << cpu_id << "/topology/physical_package_id";

            std::ifstream cluster_file(path.str());
            if (cluster_file.is_open())
            {
                uint cluster_id;
                if (cluster_file >> cluster_id)
                {
                    return cluster_id;
                }
            }

            // 回退：尝试从core_id推断
            path.str("");
            path << "/sys/devices/system/cpu/cpu" << cpu_id << "/topology/core_id";

            std::ifstream core_file(path.str());
            if (core_file.is_open())
            {
                uint core_id;
                if (core_file >> core_id)
                {
                    // 假设相邻的core_id属于同一cluster
                    return core_id / 4; // 粗略估计
                }
            }

            return 0;
        }
    }//namespace

    bool GetArmBigLittleDistribution(ArmBigLittleDistribution *distribution)
    {
        if (!distribution) return false;

        memset(distribution, 0, sizeof(ArmBigLittleDistribution));

        // 获取系统CPU数量
        long num_cpus = sysconf(_SC_NPROCESSORS_CONF);
        if (num_cpus <= 0) return false;

        distribution->total_cores = static_cast<uint>(num_cpus);
        distribution->core_list = new ArmCoreInfo[distribution->total_cores];

        // 收集所有核心的频率信息
        std::vector<uint> frequencies;
        std::vector<uint> big_cores;
        std::vector<uint> little_cores;

        for (uint i = 0; i < distribution->total_cores; ++i)
        {
            uint freq = GetCoreMaxFrequency(i);
            frequencies.push_back(freq);
        }

        // 找出频率的中位数作为区分大小核的阈值
        std::vector<uint> sorted_freqs = frequencies;
        std::sort(sorted_freqs.begin(), sorted_freqs.end());

        uint threshold = 0;
        if (!sorted_freqs.empty())
        {
            // 如果有明显的频率差异，使用中值偏上作为阈值
            size_t mid = sorted_freqs.size() / 2;
            uint median = sorted_freqs[mid];
            uint max_freq = sorted_freqs.back();

            // 如果最大频率和中位数差异超过20%，认为有大小核
            if (max_freq > median * 1.2)
            {
                threshold = median + (max_freq - median) / 3;
            }
            else
            {
                // 频率相似，可能是统一架构
                threshold = max_freq + 1; // 使所有核心都归为小核
            }
        }

        // 填充核心信息
        for (uint i = 0; i < distribution->total_cores; ++i)
        {
            distribution->core_list[i].core_id = i;
            distribution->core_list[i].max_frequency = frequencies[i];
            distribution->core_list[i].cluster_id = GetCoreClusterId(i);
            distribution->core_list[i].is_big_core = IsBigCore(frequencies[i], threshold);

            if (distribution->core_list[i].is_big_core)
                big_cores.push_back(i);
            else
                little_cores.push_back(i);
        }

        // 设置大小核计数和ID列表
        distribution->big_core_count = big_cores.size();
        distribution->little_core_count = little_cores.size();

        if (distribution->big_core_count > 0)
        {
            distribution->big_core_ids = new uint[distribution->big_core_count];
            for (uint i = 0; i < distribution->big_core_count; ++i)
                distribution->big_core_ids[i] = big_cores[i];
        }

        if (distribution->little_core_count > 0)
        {
            distribution->little_core_ids = new uint[distribution->little_core_count];
            for (uint i = 0; i < distribution->little_core_count; ++i)
                distribution->little_core_ids[i] = little_cores[i];
        }

        return true;
    }

    void FreeArmBigLittleDistribution(ArmBigLittleDistribution *distribution)
    {
        if (!distribution) return;

        if (distribution->core_list)
        {
            delete[] distribution->core_list;
            distribution->core_list = nullptr;
        }

        if (distribution->big_core_ids)
        {
            delete[] distribution->big_core_ids;
            distribution->big_core_ids = nullptr;
        }

        if (distribution->little_core_ids)
        {
            delete[] distribution->little_core_ids;
            distribution->little_core_ids = nullptr;
        }

        distribution->total_cores = 0;
        distribution->big_core_count = 0;
        distribution->little_core_count = 0;
    }

    bool BindThreadToBigCores()
    {
        ArmBigLittleDistribution dist;
        if (!GetArmBigLittleDistribution(&dist))
            return false;

        if (dist.big_core_count == 0)
        {
            FreeArmBigLittleDistribution(&dist);
            return false;
        }

        // 设置CPU亲和性到大核
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        for (uint i = 0; i < dist.big_core_count; ++i)
        {
            CPU_SET(dist.big_core_ids[i], &cpuset);
        }

        bool result = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0;

        FreeArmBigLittleDistribution(&dist);
        return result;
    }

    bool BindThreadToLittleCores()
    {
        ArmBigLittleDistribution dist;
        if (!GetArmBigLittleDistribution(&dist))
            return false;

        if (dist.little_core_count == 0)
        {
            FreeArmBigLittleDistribution(&dist);
            return false;
        }

        // 设置CPU亲和性到小核
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);

        for (uint i = 0; i < dist.little_core_count; ++i)
        {
            CPU_SET(dist.little_core_ids[i], &cpuset);
        }

        bool result = pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) == 0;

        FreeArmBigLittleDistribution(&dist);
        return result;
    }

    bool BindThreadToCoreType(bool use_big_cores)
    {
        if (use_big_cores)
            return BindThreadToBigCores();
        else
            return BindThreadToLittleCores();
    }
}//namespace hgl

#endif // __ANDROID__ && ARM
