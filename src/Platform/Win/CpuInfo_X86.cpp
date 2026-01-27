#if defined(_M_AMD64) || defined(_M_X64) || defined(_M_IX86)

#include<hgl/platform/CpuInfo.h>
#include<hgl/type/StrChar.h>
#include<hgl/type/Smart.h>
#include<sysinfoapi.h>
#include<intrin.h>
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

        void GetX86CpuId(int function, int subfunction, int* eax, int* ebx, int* ecx, int* edx)
        {
            int regs[4];
            __cpuidex(regs, function, subfunction);
            *eax = regs[0];
            *ebx = regs[1];
            *ecx = regs[2];
            *edx = regs[3];
        }

        void GetX86Features(CpuFeatures* features)
        {
            if (!features) return;

            int eax, ebx, ecx, edx;
            GetX86CpuId(0, 0, &eax, &ebx, &ecx, &edx);

            memcpy(features->vendor, &ebx, 4);
            memcpy(features->vendor + 4, &edx, 4);
            memcpy(features->vendor + 8, &ecx, 4);
            features->vendor[12] = '\0';

            GetX86CpuId(1, 0, &eax, &ebx, &ecx, &edx);

            features->family = ((eax >> 8) & 0xF) + ((eax >> 20) & 0xFF);
            features->model = ((eax >> 4) & 0xF) | ((eax >> 12) & 0xF0);
            features->stepping = (eax & 0xF);

            features->has_mmx = (edx & (1 << 23)) != 0;
            features->has_sse = (edx & (1 << 25)) != 0;
            features->has_sse2 = (edx & (1 << 26)) != 0;
            features->has_sse3 = (ecx & 1) != 0;
            features->has_ssse3 = (ecx & (1 << 9)) != 0;
            features->has_sse4_1 = (ecx & (1 << 19)) != 0;
            features->has_sse4_2 = (ecx & (1 << 20)) != 0;
            features->has_avx = (ecx & (1 << 28)) != 0;
            features->has_aes = (ecx & (1 << 25)) != 0;
            features->has_pclmulqdq = (ecx & 1) != 0;
            features->has_rdrand = (ecx & (1 << 30)) != 0;
            features->has_fma3 = (ecx & (1 << 12)) != 0;
            features->has_popcnt = (ecx & (1 << 23)) != 0;

            GetX86CpuId(7, 0, &eax, &ebx, &ecx, &edx);

            features->has_avx2 = (ebx & (1 << 5)) != 0;
            features->has_avx512_f = (ebx & (1 << 16)) != 0;
            features->has_avx512_dq = (ebx & (1 << 17)) != 0;
            features->has_avx512_ifma = (ebx & (1 << 21)) != 0;
            features->has_avx512_pf = (ebx & (1 << 26)) != 0;
            features->has_avx512_er = (ebx & (1 << 27)) != 0;
            features->has_avx512_cd = (ebx & (1 << 28)) != 0;
            features->has_avx512_bw = (ebx & (1 << 30)) != 0;
            features->has_avx512_vl = (ebx & (1 << 31)) != 0;
            features->has_bmi1 = (ebx & (1 << 3)) != 0;
            features->has_bmi2 = (ebx & (1 << 8)) != 0;
            features->has_adx = (ebx & (1 << 19)) != 0;
            features->has_sha = (ebx & (1 << 29)) != 0;
            features->has_rdseed = (ebx & (1 << 18)) != 0;
            features->has_prefetchw = (ecx & (1 << 8)) != 0;

            char brand[48] = {0};
            for (int i = 0; i < 3; ++i)
            {
                GetX86CpuId(0x80000002 + i, 0, &eax, &ebx, &ecx, &edx);
                memcpy(brand + i * 16, &eax, 4);
                memcpy(brand + i * 16 + 4, &ebx, 4);
                memcpy(brand + i * 16 + 8, &ecx, 4);
                memcpy(brand + i * 16 + 12, &edx, 4);
            }
            memcpy(features->brand, brand, 48);
            features->brand[48] = '\0';

            GetX86CpuId(0x80000005, 0, &eax, &ebx, &ecx, &edx);
            features->l1_data_cache_size = (ecx >> 24) & 0xFF;
            features->l1_inst_cache_size = (edx >> 24) & 0xFF;

            GetX86CpuId(0x80000006, 0, &eax, &ebx, &ecx, &edx);
            features->l2_cache_size = (ecx >> 16) & 0xFFFF;
            features->l3_cache_size = ((edx >> 18) & 0x3FFF) * 512;

            features->base_frequency = 0;
            features->max_frequency = 0;
            features->bus_frequency = 100;
        }

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
                    return CpuArch::ARMv8;

                default:
                    return CpuArch::x86_64;
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

        ci->arch = DetectCpuArch();

        if (ci->arch == CpuArch::x86_64)
            GetX86Features(&ci->features);

        return(true);
    }
}//namespace hgl

#endif // _M_AMD64 || _M_X64 || _M_IX86
