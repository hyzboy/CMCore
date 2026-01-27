#include <hgl/platform/CpuInfo.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <string.h>
#include <iostream>

namespace hgl {
namespace {
    void GetAppleARMFeatures(CpuFeatures* features) {
        if (!features) return;
        memset(features, 0, sizeof(CpuFeatures));
        // 品牌
        char brand[128] = {0};
        size_t size = sizeof(brand);
        if (sysctlbyname("machdep.cpu.brand_string", &brand, &size, NULL, 0) == 0) {
            strncpy(features->brand, brand, sizeof(features->brand) - 1);
        } else {
            strncpy(features->brand, "Apple Silicon", sizeof(features->brand) - 1);
        }
        // 厂商
        strncpy(features->vendor, "Apple", sizeof(features->vendor) - 1);
        // 家族/型号/步进
        features->family = 8; // ARMv8
        features->model = 0;
        features->stepping = 0;
        // ARM常见特性
        features->has_neon = true;
        features->has_vfp = true;
        features->has_vfpv3 = true;
        features->has_vfpv4 = true;
        features->has_aes = true;
        features->has_sha1 = true;
        features->has_sha2 = true;
        features->has_crc32 = true;
        features->has_pmull = true;
        features->has_fp16 = true;
        features->has_dotprod = true;
        features->has_i8mm = true;
        features->has_bf16 = false;
        features->has_sve = false;
        // 大小核信息
        int ncpu = 0;
        size = sizeof(ncpu);
        sysctlbyname("hw.ncpu", &ncpu, &size, NULL, 0);
        features->has_big_little = true;
        features->big_core_count = ncpu / 2;
        features->little_core_count = ncpu - features->big_core_count;
        features->big_core_max_freq = 3200;
        features->little_core_max_freq = 2000;
        // 缓存信息
        int l1 = 0, l2 = 0, l3 = 0;
        size = sizeof(int);
        sysctlbyname("hw.l1icachesize", &l1, &size, NULL, 0);
        features->l1_inst_cache_size = l1 / 1024;
        sysctlbyname("hw.l1dcachesize", &l1, &size, NULL, 0);
        features->l1_data_cache_size = l1 / 1024;
        sysctlbyname("hw.l2cachesize", &l2, &size, NULL, 0);
        features->l2_cache_size = l2 / 1024;
        sysctlbyname("hw.l3cachesize", &l3, &size, NULL, 0);
        features->l3_cache_size = l3 / 1024;
    }
    CpuArch DetectCpuArch() { return CpuArch::ARMv8; }
    void GetCpuCounts(uint& cpu_count, uint& core_count, uint& logical_core_count) {
        int ncpu = 0;
        size_t size = sizeof(ncpu);
        sysctlbyname("hw.ncpu", &ncpu, &size, NULL, 0);
        cpu_count = 1;
        core_count = ncpu;
        logical_core_count = ncpu;
    }
}
bool GetCpuInfo(CpuInfo *ci) {
    if (!ci) return false;
    memset(ci, 0, sizeof(CpuInfo));
    GetCpuCounts(ci->cpu_count, ci->core_count, ci->logical_core_count);
    ci->arch = DetectCpuArch();
    GetAppleARMFeatures(&ci->features);
    return true;
}
} // namespace hgl
