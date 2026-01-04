#if defined(__APPLE__) && (defined(__ARM_ARCH) || defined(__aarch64__))

#include<hgl/platform/CpuAffinity.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/sysctl.h>
#include<mach/mach.h>
#include<mach/thread_policy.h>
#include<mach/thread_act.h>
#include<string.h>
#include<vector>

namespace hgl
{
    namespace
    {
        /**
         * 获取CPU频率信息来判断大小核
         */
        bool GetCoreFrequencies(std::vector<uint>& frequencies)
        {
            // Apple不直接提供per-core频率信息
            // 我们需要通过sysctl获取可用信息
            
            int ncpu = 0;
            size_t size = sizeof(ncpu);
            if (sysctlbyname("hw.ncpu", &ncpu, &size, NULL, 0) != 0)
                return false;

            frequencies.resize(ncpu);
            
            // 尝试获取性能核心和效率核心的信息
            // Apple Silicon通常有P核(性能)和E核(效率)
            uint64_t perflevel0_count = 0;
            uint64_t perflevel1_count = 0;
            
            size = sizeof(perflevel0_count);
            sysctlbyname("hw.perflevel0.physicalcpu", &perflevel0_count, &size, NULL, 0);
            sysctlbyname("hw.perflevel1.physicalcpu", &perflevel1_count, &size, NULL, 0);
            
            // perflevel0通常是性能核，perflevel1是效率核
            // 为大核分配更高的频率标识
            for (int i = 0; i < ncpu; ++i)
            {
                if (i < (int)perflevel0_count)
                    frequencies[i] = 3200; // 性能核 ~3.2GHz
                else
                    frequencies[i] = 2000; // 效率核 ~2.0GHz
            }
            
            return true;
        }
    }//namespace

    bool GetArmBigLittleDistribution(ArmBigLittleDistribution *distribution)
    {
        if (!distribution) return false;

        memset(distribution, 0, sizeof(ArmBigLittleDistribution));

        // 获取CPU数量
        int ncpu = 0;
        size_t size = sizeof(ncpu);
        if (sysctlbyname("hw.ncpu", &ncpu, &size, NULL, 0) != 0)
            return false;

        distribution->total_cores = ncpu;
        distribution->core_list = new ArmCoreInfo[ncpu];

        // 获取性能级别信息
        uint64_t perflevel0_count = 0;
        uint64_t perflevel1_count = 0;
        
        size = sizeof(perflevel0_count);
        sysctlbyname("hw.perflevel0.physicalcpu", &perflevel0_count, &size, NULL, 0);
        sysctlbyname("hw.perflevel1.physicalcpu", &perflevel1_count, &size, NULL, 0);

        std::vector<uint> big_cores;
        std::vector<uint> little_cores;

        // 填充核心信息
        for (uint i = 0; i < distribution->total_cores; ++i)
        {
            distribution->core_list[i].core_id = i;
            distribution->core_list[i].cluster_id = (i < perflevel0_count) ? 0 : 1;
            
            if (i < perflevel0_count)
            {
                // 性能核
                distribution->core_list[i].is_big_core = true;
                distribution->core_list[i].max_frequency = 3200;
                big_cores.push_back(i);
            }
            else
            {
                // 效率核
                distribution->core_list[i].is_big_core = false;
                distribution->core_list[i].max_frequency = 2000;
                little_cores.push_back(i);
            }
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
        // macOS/iOS使用QoS (Quality of Service)来影响线程调度
        // 设置为用户交互级别会优先使用性能核心
        pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
        
        return true;
    }

    bool BindThreadToLittleCores()
    {
        // 设置为后台级别会优先使用效率核心
        pthread_set_qos_class_self_np(QOS_CLASS_BACKGROUND, 0);
        
        return true;
    }

    bool BindThreadToCoreType(bool use_big_cores)
    {
        if (use_big_cores)
            return BindThreadToBigCores();
        else
            return BindThreadToLittleCores();
    }
}//namespace hgl

#endif // __APPLE__ && ARM
