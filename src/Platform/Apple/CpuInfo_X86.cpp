#include <hgl/platform/CpuInfo.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <string.h>
#include <iostream>

namespace hgl {
namespace {
    void GetAppleX86Features(CpuFeatures* features) {
        if (!features) return;
        memset(features, 0, sizeof(CpuFeatures));
        // 获取CPU品牌
        char brand[128] = {0};
        size_t size = sizeof(brand);
        if (sysctlbyname("machdep.cpu.brand_string", &brand, &size, NULL, 0) == 0) {
            strncpy(features->brand, brand, sizeof(features->brand) - 1);
        }
        // 获取厂商
        char vendor[32] = {0};
        size = sizeof(vendor);
        if (sysctlbyname("machdep.cpu.vendor", &vendor, &size, NULL, 0) == 0) {
            strncpy(features->vendor, vendor, sizeof(features->vendor) - 1);
        }
        // 获取家族、型号、步进
        int family = 0, model = 0, stepping = 0;
        size = sizeof(family);
        sysctlbyname("machdep.cpu.family", &family, &size, NULL, 0);
        size = sizeof(model);
        sysctlbyname("machdep.cpu.model", &model, &size, NULL, 0);
        size = sizeof(stepping);
        sysctlbyname("machdep.cpu.stepping", &stepping, &size, NULL, 0);
        features->family = family;
        features->model = model;
        features->stepping = stepping;
        // 指令集特性
        int sse = 0, sse2 = 0, sse3 = 0, ssse3 = 0, sse4_1 = 0, sse4_2 = 0, avx1_0 = 0, avx2_0 = 0;
        size = sizeof(int);
        sysctlbyname("hw.optional.sse", &sse, &size, NULL, 0);
        sysctlbyname("hw.optional.sse2", &sse2, &size, NULL, 0);
        sysctlbyname("hw.optional.sse3", &sse3, &size, NULL, 0);
        sysctlbyname("hw.optional.ssse3", &ssse3, &size, NULL, 0);
        sysctlbyname("hw.optional.sse4_1", &sse4_1, &size, NULL, 0);
        sysctlbyname("hw.optional.sse4_2", &sse4_2, &size, NULL, 0);
        sysctlbyname("hw.optional.avx1_0", &avx1_0, &size, NULL, 0);
        sysctlbyname("hw.optional.avx2_0", &avx2_0, &size, NULL, 0);
        features->has_sse = sse;
        features->has_sse2 = sse2;
        features->has_sse3 = sse3;
        features->has_ssse3 = ssse3;
        features->has_sse4_1 = sse4_1;
        features->has_sse4_2 = sse4_2;
        features->has_avx = avx1_0;
        features->has_avx2 = avx2_0;
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
    CpuArch DetectCpuArch() { return CpuArch::x86_64; }
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
    GetAppleX86Features(&ci->features);
    return true;
}
} // namespace hgl
