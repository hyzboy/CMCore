#include<hgl/platform/CpuInfo.h>
#include<hgl/type/StrChar.h>
#include<string.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<set>
#include<unistd.h>

namespace hgl
{
    namespace
    {
        /**
         * 从/proc/cpuinfo解析ARM CPU信息
         */
        void GetARMFeatures(CpuARMFeatures* features)
        {
            if (!features) return;

            std::ifstream cpuinfo("/proc/cpuinfo");
            if (!cpuinfo.is_open()) return;

            std::string line;
            std::set<std::string> features_set;
            bool found_implementer = false;
            bool found_part = false;

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

                    if (key == "CPU implementer")
                    {
                        if (!found_implementer)
                        {
                            // 将十六进制值转换为字符串
                            unsigned int impl;
                            sscanf(value.c_str(), "0x%x", &impl);
                            sprintf(features->implementer, "%02X", impl);
                            found_implementer = true;
                        }
                    }
                    else if (key == "CPU part")
                    {
                        if (!found_part)
                        {
                            sscanf(value.c_str(), "0x%x", &features->part_number);
                            found_part = true;
                        }
                    }
                    else if (key == "CPU variant")
                    {
                        features->variant = std::stoi(value, nullptr, 16);
                    }
                    else if (key == "CPU revision")
                    {
                        features->revision = std::stoi(value, nullptr, 16);
                    }
                    else if (key == "Features")
                    {
                        // 解析ARM特性标志
                        std::istringstream feature_stream(value);
                        std::string feature;
                        while (feature_stream >> feature)
                        {
                            features_set.insert(feature);
                        }
                    }
                }
            }

            // 解析特性标志
            for (const auto& feature : features_set)
            {
                if (feature == "neon") features->has_neon = true;
                else if (feature == "vfp") features->has_vfp = true;
                else if (feature == "vfpv3") features->has_vfpv3 = true;
                else if (feature == "vfpv4") features->has_vfpv4 = true;
                else if (feature == "aes") features->has_aes = true;
                else if (feature == "sha1") features->has_sha1 = true;
                else if (feature == "sha2") features->has_sha2 = true;
                else if (feature == "crc32") features->has_crc32 = true;
                else if (feature == "pmull") features->has_pmull = true;
                else if (feature == "fp16") features->has_fp16 = true;
                else if (feature == "dotprod") features->has_dotprod = true;
                else if (feature == "i8mm") features->has_i8mm = true;
                else if (feature == "bf16") features->has_bf16 = true;
                else if (feature == "sve") features->has_sve = true;
            }

            // 大小核检测 (基于启发式方法)
            // 在Linux上，更精确的检测可能需要读取/sys/devices/system/cpu/cpu*/cpufreq/等目录
            DetectBigLittleArchitecture(features);

            // 设置缓存大小 (可以通过读取/sys/devices/system/cpu/cpu0/cache/获取更精确的值)
            SetTypicalARMCacheSizes(features);
        }

        /**
         * 检测大小核架构 (UNIX)
         */
        void DetectBigLittleArchitecture(CpuARMFeatures* features)
        {
            // 尝试从/proc/cpuinfo或其他系统文件检测大小核
            // 这里使用简化的启发式方法

            std::ifstream cpuinfo("/proc/cpuinfo");
            if (!cpuinfo.is_open()) return;

            std::string line;
            std::set<std::string> cpu_parts;
            int processor_count = 0;

            while (std::getline(cpuinfo, line))
            {
                if (line.find("processor") != std::string::npos)
                {
                    processor_count++;
                }
                else if (line.find("CPU part") != std::string::npos)
                {
                    std::istringstream iss(line);
                    std::string key, value;
                    if (std::getline(iss, key, ':'))
                    {
                        std::getline(iss, value);
                        value.erase(value.begin(), std::find_if(value.begin(), value.end(),
                            [](unsigned char ch) { return !std::isspace(ch); }));
                        cpu_parts.insert(value);
                    }
                }
            }

            // 如果有多个不同的CPU部件号，可能存在大小核
            if (cpu_parts.size() > 1 || processor_count > 4)
            {
                features->has_big_little = true;

                if (processor_count <= 8)
                {
                    features->big_core_count = processor_count / 2;
                    features->little_core_count = processor_count - features->big_core_count;
                    features->big_core_max_freq = 2800;
                    features->little_core_max_freq = 1800;
                }
                else
                {
                    features->big_core_count = processor_count * 2 / 3;
                    features->little_core_count = processor_count - features->big_core_count;
                    features->big_core_max_freq = 3000;
                    features->little_core_max_freq = 2000;
                }
            }
            else
            {
                features->has_big_little = false;
                features->big_core_count = processor_count;
                features->little_core_count = 0;
                features->big_core_max_freq = 2400;
            }
        }

        /**
         * 设置典型的ARM缓存大小 (UNIX)
         */
        void SetTypicalARMCacheSizes(CpuARMFeatures* features)
        {
            features->l1_data_cache_size = 32;
            features->l1_inst_cache_size = 32;

            if (features->has_big_little)
            {
                features->l2_cache_size = 512;
                features->l3_cache_size = 2048;
            }
            else
            {
                features->l2_cache_size = 256;
                features->l3_cache_size = 0;
            }
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
                    line.find("CPU part") != std::string::npos ||
                    line.find("AArch64") != std::string::npos)
                {
                    return CpuArch::ARMv8;
                }
            }

            return CpuArch::x86_64;
        }

        /**
         * 获取CPU核心数量 (UNIX)
         */
        void GetCpuCounts(uint& cpu_count, uint& core_count, uint& logical_core_count)
        {
            cpu_count = core_count = logical_core_count = 0;

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
                cpu_count = 1; // 简化处理
            }
            else
            {
                logical_core_count = sysconf(_SC_NPROCESSORS_ONLN);
                core_count = logical_core_count;
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

        if (ci->arch == CpuArch::ARMv8 || ci->arch == CpuArch::ARMv9)
            GetARMFeatures(&ci->arm);

        return(true);
    }
}//namespace hgl
