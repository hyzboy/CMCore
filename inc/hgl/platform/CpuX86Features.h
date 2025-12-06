#pragma once

#include <hgl/CoreType.h>

namespace hgl
{
    /**
     * X86处理器特性标志
     *
     * 该结构用于保存在运行时探测到的X86/AMD64处理器信息。
     * 由平台特定的CPU探测代码（如CpuInfo_X86.cpp）填充。所有字段
     * 通常为只读，消费者应视为探测结果的快照。
     *
     * 注意：结构中使用了按位(bool)位域以节省内存并清晰表达功能支持标志。
     * 位域的布局依赖于编译器实现（例如最低位/最高位顺序），但对于
     * 仅用于逻辑判断（true/false）通常没有问题。如果对内存布局有严格要求，
     * 请不要依赖该结构的二进制表示。
     */
    struct CpuFeatures
    {
        // ------------------------ 基本信息 ------------------------
        /**
         * CPU厂商字符串（如"GenuineIntel"或"AuthenticAMD"）。
         * 数组长度为13：存储最多12个可打印字符+1个终止符('\0')。
         * 如果探测失败或未知，字符串应保证为以 '\0' 结尾的空串。
         */
        char vendor[13];               ///< CPU厂商字符串 (12字节 + null终止符)

        /**
         * CPU品牌字符串（通常更为可读，包含系列和频率信息），
         * 该字段长度为49：最多48个字符 + 1个终止符('\0')。
         * 例如："Intel(R) Core(TM) i7-8650U CPU @ 1.90GHz"。
         */
        char brand[49];                ///< CPU品牌字符串 (48字节 + null终止符)

        /**
         * CPU家族号（family）。来源于CPUID的family字段或扩展字段，
         * 用于区分CPU的大类（例如Intel/AMD的不同微架构世代）。
         * 值的具体含义应结合厂商ID和model字段解释。
         */
        uint32 family;                 ///< CPU家族

        /**
         * CPU型号号（model）。通常与family共同用于确定具体CPU微架构。
         * 例如，从CPUID获得的model号可能需要结合extended model位来计算。
         */
        uint32 model;                  ///< CPU型号

        /**
         * CPU步进（stepping）。表示处理器的修订版本或微代码修订号，
         * 对调试、微码兼容性或硬件漏洞缓解判断有用。
         */
        uint32 stepping;               ///< CPU步进


        // ------------------------ 指令集支持（按位标志） ------------------------
        // 这些标志表示该处理器是否支持对应的指令集或扩展。值为true表示支持，
        // false表示不支持或未被检测到。大多数依赖CPUID或OS支持来判断。

        bool has_mmx      : 1;         ///< 支持MMX指令集
        bool has_sse      : 1;         ///< 支持SSE (Streaming SIMD Extensions)
        bool has_sse2     : 1;         ///< 支持SSE2
        bool has_sse3     : 1;         ///< 支持SSE3
        bool has_ssse3    : 1;         ///< 支持SSSE3 (Supplemental SSE3)
        bool has_sse4_1   : 1;         ///< 支持SSE4.1
        bool has_sse4_2   : 1;         ///< 支持SSE4.2
        bool has_avx      : 1;         ///< 支持AVX (需要OS保存YMM寄存器)
        bool has_avx2     : 1;         ///< 支持AVX2

        /**
         * AVX-512 系列子集标志：AVX-512由多个子扩展组成，不同CPU厂商/型号
         * 支持不同的AVX-512特性集合。下面的字段分别表示常见的子集：
         * - avx512_f: Foundation 基础指令集（核心支持位）
         * - avx512_dq: Doubleword/Quadword 整数/浮点指令
         * - avx512_ifma: Integer FMA 指令
         * - avx512_pf: Prefetch 指令
         * - avx512_er: Exponential/Reciprocal 指令
         * - avx512_cd: Conflict Detection
         * - avx512_bw: Byte/Word 指令
         * - avx512_vl: Vector Length 指令（对128/256位向量的扩展）
         */
        bool has_avx512_f : 1;         ///< 支持AVX-512 Foundation
        bool has_avx512_dq: 1;         ///< 支持AVX-512 Doubleword and Quadword
        bool has_avx512_ifma: 1;       ///< 支持AVX-512 Integer FMA
        bool has_avx512_pf: 1;         ///< 支持AVX-512 Prefetch
        bool has_avx512_er: 1;         ///< 支持AVX-512 Exponential and Reciprocal
        bool has_avx512_cd: 1;         ///< 支持AVX-512 Conflict Detection
        bool has_avx512_bw: 1;         ///< 支持AVX-512 Byte and Word
        bool has_avx512_vl: 1;         ///< 支持AVX-512 Vector Length

        // ------------------------ 其他CPU功能标志 ------------------------
        // 用于表示常见的算术、加密、随机数指令及位操作扩展支持。

        bool has_fma3     : 1;         ///< 支持FMA3 (三元浮点乘加)
        bool has_fma4     : 1;         ///< 支持FMA4 (AMD的四操作数FMA，较少见)
        bool has_aes      : 1;         ///< 支持AES指令集硬件加密加速
        bool has_sha      : 1;         ///< 支持SHA指令集硬件加速
        bool has_pclmulqdq: 1;         ///< 支持PCLMULQDQ（多项式乘法）
        bool has_rdrand   : 1;         ///< 支持RDRAND（硬件随机数）
        bool has_rdseed   : 1;         ///< 支持RDSEED（种子随机数）
        bool has_adx      : 1;         ///< 支持ADX（算术扩展，用于大整数）
        bool has_bmi1     : 1;         ///< 支持BMI1 (位操作指令集1)
        bool has_bmi2     : 1;         ///< 支持BMI2 (位操作指令集2)
        bool has_popcnt   : 1;         ///< 支持POPCNT（按位计数）
        bool has_lzcnt    : 1;         ///< 支持LZCNT（前导零计数）
        bool has_prefetchw: 1;         ///< 支持PREFETCHW（写入预取）


        // ------------------------ 缓存信息（单位：KB） ------------------------
        /**
         * L1 数据缓存大小，单位KB。若探测失败，值可能为0。
         * 注意：L1通常分为数据缓存和指令缓存，下面分别给出。
         */
        uint32 l1_data_cache_size;     ///< L1数据缓存大小(KB)

        /**
         * L1 指令缓存大小，单位KB。若探测失败，值可能为0。
         */
        uint32 l1_inst_cache_size;     ///< L1指令缓存大小(KB)

        /**
         * L2 缓存大小，单位KB。通常为每个核心或每个核心对的共享缓存，
         * 具体语义依赖于CPU架构。
         */
        uint32 l2_cache_size;          ///< L2缓存大小(KB)

        /**
         * L3 缓存大小，单位KB。常见于多核CPU的共享缓存，可能为0表示无L3。
         */
        uint32 l3_cache_size;          ///< L3缓存大小(KB)


        // ------------------------ 频率信息（单位：MHz） ------------------------
        /**
         * 标称基础频率（base frequency），单位为MHz。该值通常来自
         * CPUID、ACPI或厂商提供的信息，代表单核在标准条件下的标称频率。
         * 请注意动态频率缩放（如Turbo Boost）会使实际运行频率高于此值。
         */
        uint32 base_frequency;         ///< 基础频率(MHz)

        /**
         * 最大频率（max frequency），单位为MHz。表示该处理器在可用功耗/热
         * 预算和OS允许的情况下的最大单核/全核频率估计。并非在所有平台
         * 上都能精确探测到，失败时可能为0。
         */
        uint32 max_frequency;          ///< 最大频率(MHz)

        /**
         * 总线频率（bus frequency），单位为MHz。用于表示与CPU核心通信的
         * 外部总线或参考时钟频率。在许多现代系统中此值的意义已被弱化，
         * 但仍可用于某些时钟/比例计算。
         */
        uint32 bus_frequency;          ///< 总线频率(MHz)
    };
} // namespace hgl
