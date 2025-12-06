#include<hgl/platform/CpuInfo.h>
#include<hgl/type/StrChar.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<unistd.h>

namespace hgl
{
    namespace
    {
        /**
         * 从/proc/cpuinfo解析X86 CPU信息
         */
        void GetX86Features(CpuX86Features* features)
        {
            if (!features) return;

            std::ifstream cpuinfo("/proc/cpuinfo");
            if (!cpuinfo.is_open()) return;

            std::string line;
            bool found_vendor = false;
            bool found_model = false;

            while (std::getline(cpuinfo, line))
            {
                std::istringstream iss(line);
                std::string key, value;

                if (std::getline(iss, key, ':'))
                {
                    std::getline(iss, value);
                    // 移除前导空格
                    value.erase(value.begin(), std::find_if(value.begin(), value.end(),
                        [](unsigned char ch) { return !std::isspace(ch); }));

                    if (key == "vendor_id")
                    {
                        if (!found_vendor)
                        {
                            strncpy(features->vendor, value.c_str(), 12);
                            features->vendor[12] = '\0';
                            found_vendor = true;
                        }
                    }
                    else if (key == "model name")
                    {
                        if (!found_model)
                        {
                            strncpy(features->brand, value.c_str(), 48);
                            features->brand[48] = '\0';
                            found_model = true;
                        }
                    }
                    else if (key == "cpu family")
                    {
                        features->family = std::stoi(value);
                    }
                    else if (key == "model")
                    {
                        features->model = std::stoi(value);
                    }
                    else if (key == "stepping")
                    {
                        features->stepping = std::stoi(value);
                    }
                    else if (key == "flags")
                    {
                        // 解析CPU特性标志
                        std::istringstream flag_stream(value);
                        std::string flag;
                        while (flag_stream >> flag)
                        {
                            if (flag == "mmx") features->has_mmx = true;
                            else if (flag == "sse") features->has_sse = true;
                            else if (flag == "sse2") features->has_sse2 = true;
                            else if (flag == "sse3") features->has_sse3 = true;
                            else if (flag == "ssse3") features->has_ssse3 = true;
                            else if (flag == "sse4_1") features->has_sse4_1 = true;
                            else if (flag == "sse4_2") features->has_sse4_2 = true;
                            else if (flag == "avx") features->has_avx = true;
                            else if (flag == "avx2") features->has_avx2 = true;
                            else if (flag == "aes") features->has_aes = true;
                            else if (flag == "pclmulqdq") features->has_pclmulqdq = true;
                            else if (flag == "rdrand") features->has_rdrand = true;
                            else if (flag == "fma") features->has_fma3 = true;
                            else if (flag == "bmi1") features->has_bmi1 = true;
                            else if (flag == "bmi2") features->has_bmi2 = true;
                            else if (flag == "adx") features->has_adx = true;
                            else if (flag == "sha") features->has_sha = true;
                            else if (flag == "rdseed") features->has_rdseed = true;
                            else if (flag == "popcnt") features->has_popcnt = true;
                            else if (flag == "lzcnt") features->has_lzcnt = true;
                            else if (flag == "prefetchw") features->has_prefetchw = true;
                        }
                    }
                }
            }

            // 设置默认缓存大小 (可以通过其他方式获取更精确的值)
            features->l1_data_cache_size = 32;
            features->l1_inst_cache_size = 32;
            features->l2_cache_size = 256;
            features->l3_cache_size = 8192; // 8MB

            // 频率信息 (可以通过其他方式获取)
            features->base_frequency = 0;
            features->max_frequency = 0;
            features->bus_frequency = 100;
        }

        /**
         * 检测CPU架构 (UNIX)
         */
        CpuArch DetectCpuArch()
        {
            std::ifstream cpuinfo("/proc/cpuinfo");
            if (!cpuinfo.is_open()) return CpuArch::x86_64;

            std::string line;
            while (std::getline(cpuinfo, line))
            {
                if (line.find("CPU architecture") != std::string::npos ||
                    line.find("CPU part") != std::string::npos)
                {
                    // ARM处理器
                    return CpuArch::ARMv8;
                }
            }

            // 默认假设x86_64
            return CpuArch::x86_64;
        }

        /**
         * 获取CPU核心数量 (UNIX)
         */
        void GetCpuCounts(uint& cpu_count, uint& core_count, uint& logical_core_count)
        {
            cpu_count = core_count = logical_core_count = 0;

            // 尝试从/proc/cpuinfo获取信息
            std::ifstream cpuinfo("/proc/cpuinfo");
            if (cpuinfo.is_open())
            {
                std::string line;
                int physical_id = -1;
                int core_id = -1;
                std::set<std::pair<int, int>> unique_cpus;
                int processor_count = 0;

                while (std::getline(cpuinfo, line))
                {
                    if (line.find("processor") != std::string::npos)
                    {
                        processor_count++;
                    }
                    else if (line.find("physical id") != std::string::npos)
                    {
                        sscanf(line.c_str(), "physical id\t: %d", &physical_id);
                    }
                    else if (line.find("core id") != std::string::npos)
                    {
                        sscanf(line.c_str(), "core id\t: %d", &core_id);
                        if (physical_id >= 0 && core_id >= 0)
                        {
                            unique_cpus.insert(std::make_pair(physical_id, core_id));
                        }
                    }
                }

                logical_core_count = processor_count;
                core_count = unique_cpus.size();
                cpu_count = 1; // 在大多数情况下，我们假设单CPU系统
            }
            else
            {
                // 回退到sysconf
                logical_core_count = sysconf(_SC_NPROCESSORS_ONLN);
                core_count = logical_core_count; // 简化处理
                cpu_count = 1;
            }
        }
    }//namespace

    bool GetCpuInfo(CpuInfo *ci)
    {
        if(!ci)return(false);

        memset(ci,0,sizeof(CpuInfo));

        // 获取CPU数量信息
        GetCpuCounts(ci->cpu_count, ci->core_count, ci->logical_core_count);

        // 检测CPU架构并填充特性信息
        ci->arch = DetectCpuArch();

        if (ci->arch == CpuArch::x86_64)
            GetX86Features(&ci->x86);

        return(true);
    }
}//namespace hgl