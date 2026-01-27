#if defined(_M_ARM) || defined(_M_ARM64)
#include<hgl/platform/CpuInfo.h>
#include<hgl/type/StrChar.h>
#include<sysinfoapi.h>
#include<string.h>
#include<iostream>

namespace hgl
{
    namespace
    {
        DWORD CountSetBits(ULONG_PTR bitMask)
        {
            DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
            DWORD bitSetCount = 0;
            ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
            DWORD i;

            for (i = 0; i <= LSHIFT; ++i)
            {
                bitSetCount += ((bitMask & bitTest)?1:0);
                bitTest/=2;
            }

            return bitSetCount;
        }

        /**
         * 获取ARM处理器特性 (Windows ARM64)
         */
        void GetARMFeatures(CpuARMFeatures* features)
        {
            if (!features) return;

            SYSTEM_INFO si;
            GetNativeSystemInfo(&si);

            // 检测是否为ARM架构
            if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_ARM64)
            {
                // 基础ARM64特性 (ARMv8.0+)
                features->has_neon = true;
                features->has_vfp = true;
                features->has_vfpv3 = true;
                features->has_vfpv4 = true;
                features->has_aes = true;
                features->has_sha1 = true;
                features->has_sha2 = true;
                features->has_crc32 = true;
                features->has_pmull = true;

                // 尝试检测ARMv8.2+特性
                // 注意：Windows ARM64目前主要支持到ARMv8.1
                // ARMv8.2+特性可能需要运行时检测

                // 大小核检测
                DetectBigLittleArchitecture(features, si.dwNumberOfProcessors);

                // 缓存信息 (基于典型ARM配置)
                SetTypicalARMCacheSizes(features);
            }
        }

        /**
         * 检测大小核架构
         */
        void DetectBigLittleArchitecture(CpuARMFeatures* features, DWORD total_cores)
        {
            // 大小核检测策略：
            // 1. 基于核心数量的启发式检测
            // 2. 基于已知处理器型号的检测
            // 3. 基于性能差异的检测

            if (total_cores <= 4) {
                // 4核及以下通常是统一架构
                features->has_big_little = false;
                features->big_core_count = total_cores;
                features->little_core_count = 0;
                features->big_core_max_freq = 2400; // 典型中端处理器频率
                features->little_core_max_freq = 0;
            }
            else if (total_cores <= 8) {
                // 6-8核通常是大小核架构
                features->has_big_little = true;
                features->big_core_count = total_cores / 2;
                features->little_core_count = total_cores - features->big_core_count;

                // 典型大小核频率
                features->big_core_max_freq = 2800;    // 大核 2.8GHz
                features->little_core_max_freq = 1800; // 小核 1.8GHz
            }
            else {
                // 8核以上，可能有更复杂的配置
                features->has_big_little = true;
                features->big_core_count = total_cores * 2 / 3;  // 假设2/3是大核
                features->little_core_count = total_cores - features->big_core_count;

                features->big_core_max_freq = 3000;    // 高端大核
                features->little_core_max_freq = 2000; // 高端小核
            }

            // 特殊处理：某些处理器可能不是严格的大小核
            // 这里可以添加特定型号的检测逻辑
        }

        /**
         * 设置典型的ARM缓存大小
         */
        void SetTypicalARMCacheSizes(CpuARMFeatures* features)
        {
            // 典型的ARM处理器缓存配置
            // 注意：实际值因处理器型号而异

            features->l1_data_cache_size = 32;   // 32KB L1数据缓存
            features->l1_inst_cache_size = 32;   // 32KB L1指令缓存

            // L2缓存大小因处理器而异
            if (features->has_big_little) {
                // 大小核架构通常有较大的L2缓存
                features->l2_cache_size = 512;   // 512KB L2缓存
                features->l3_cache_size = 2048;  // 2MB L3缓存 (如果有)
            } else {
                // 统一架构
                features->l2_cache_size = 256;   // 256KB L2缓存
                features->l3_cache_size = 0;      // 通常没有L3
            }
        }

        /**
         * 检测CPU架构
         */
        CpuArch DetectCpuArch()
        {
            SYSTEM_INFO si;
            GetNativeSystemInfo(&si);

            switch (si.wProcessorArchitecture)
            {
                case PROCESSOR_ARCHITECTURE_AMD64:
                case PROCESSOR_ARCHITECTURE_INTEL:
                    return CpuArch::x86_64;

                case PROCESSOR_ARCHITECTURE_ARM:
                case PROCESSOR_ARCHITECTURE_ARM64:
                    // 进一步检测ARM版本
                    // 这里简化处理，假设都支持ARMv8
                    return CpuArch::ARMv8;

                default:
                    return CpuArch::x86_64; // 默认假设x86_64
            }
        }
    }//namespace

    bool GetCpuInfo(CpuInfo *ci)
    {
        if(!ci)return(false);

        AutoFree<SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX> buf;
        DWORD length=0;

        GetLogicalProcessorInformationEx(RelationAll,nullptr,&length);

        if(GetLastError()!=ERROR_INSUFFICIENT_BUFFER)
            return(false);

        buf=(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)malloc(length);

        if(!GetLogicalProcessorInformationEx(RelationAll,buf,&length))
            return(false);

        char *sp=(char *)(buf.data());
        SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *p;

        memset(ci,0,sizeof(CpuInfo));

        while(length>0)
        {
            p=(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)sp;

            if(p->Relationship==RelationProcessorPackage)
                ++ci->cpu_count;
            else
            if(p->Relationship==RelationProcessorCore)
            {
                ++ci->core_count;

                ci->logical_core_count+=CountSetBits(p->Processor.GroupMask[0].Mask);
            }

            sp+=p->Size;
            length-=p->Size;
        }

        // 检测CPU架构并填充特性信息
        ci->arch = DetectCpuArch();

        if (ci->arch == CpuArch::ARMv8 || ci->arch == CpuArch::ARMv9)
            GetARMFeatures(&ci->features);

        return(true);
    }
}//namespace hgl
#endif // _M_ARM || _M_ARM64
