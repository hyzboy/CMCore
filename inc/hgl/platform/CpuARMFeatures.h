#pragma once

#include <hgl/CoreType.h>

namespace hgl
{
    /**
     * ARM处理器特性标志
     *
     * 该结构保存运行时探测到的ARM/ARM64处理器信息与功能位。
     * 平台特定代码（例如CpuInfo_ARM.cpp）负责填充这些字段。
     * 所有字段应视为只读探测结果快照。
     *
     * 注意事项：
     * - 结构中对布尔值使用了按位位域，以节省内存并将功能标志组合在一起。
     *   位域的底层内存布局依赖于编译器实现，不应对其二进制表示做假定。
     * - 某些功能不仅依赖硬件支持，也需要操作系统提供相应的上下文保存/管理
     *  （例如向量寄存器的上下文切换），探测代码可能会同时检查OS支持。
     */
    struct CpuFeatures
    {
        // ------------------------ 基本信息 ------------------------
        /**
         * 实现商（implementer）。通常为2个ASCII字符的厂商缩写，
         * 外加终止符，总长度为3字节（2字符 + '\0'）。常见值示例：
         * - '0x41' (ASCII 'A') 表示ARM Ltd. (在某些编码中为十六进制)
         * 实际字符串语义依赖探测实现，若未知应保证为以 '\0' 结尾的空串。
         */
        char implementer[3];           ///< 实现商 (2字节 + null终止符)

        /**
         * 部件号（part number），表示具体CPU核心/产品编号，来源于
         * 主机寄存器或系统描述（例如CPUID等），用于区分不同微架构。
         */
        uint32 part_number;            ///< 部件号

        /**
         * 变体（variant），通常用于标识芯片变体号（如工厂定制或小幅改动）。
         */
        uint32 variant;                ///< 变体

        /**
         * 修订号（revision），表示该核的版本或硅片修订。
         */
        uint32 revision;               ///< 修订版


        // ------------------------ 指令集/功能支持（按位标志） ------------------------
        // 这些标志表示CPU是否支持特定指令集或扩展。值为true表示支持，
        // false表示不支持或尚未检测到。

        bool has_neon     : 1;         ///< 支持NEON SIMD 扩展
        bool has_vfp      : 1;         ///< 支持VFP 浮点协处理器
        bool has_vfpv3    : 1;         ///< 支持VFPv3（VFP第3版）
        bool has_vfpv4    : 1;         ///< 支持VFPv4（VFP第4版）
        bool has_aes      : 1;         ///< 支持AES 指令集加速
        bool has_sha1     : 1;         ///< 支持SHA1 指令
        bool has_sha2     : 1;         ///< 支持SHA2 指令
        bool has_crc32    : 1;         ///< 支持CRC32 校验指令
        bool has_pmull    : 1;         ///< 支持PMULL（多项式乘法）
        bool has_fp16     : 1;         ///< 支持半精度浮点（FP16）指令/运算
        bool has_dotprod  : 1;         ///< 支持点积（dot product）指令
        bool has_i8mm     : 1;         ///< 支持8-bit整数矩阵乘法（I8MM）
        bool has_bf16     : 1;         ///< 支持BF16 半精度浮点格式加速
        bool has_sve      : 1;         ///< 支持SVE（可伸缩向量扩展）

        // ------------------------ 大小核（Big.Little）信息 ------------------------
        /**
         * 是否为大小核异构架构（big.LITTLE）。如果为true，则下面的
         * 核心计数与频率字段对描述大/小核有效；如果为false，
         * 则通常只有little_core_count表示物理核心数量。
         */
        bool has_big_little : 1;       ///< 是否为大小核架构

        /**
         * 大核数量（big cluster core count）。在非大小核平台上可为0或与
         * little_core_count相等（探测实现可能在不区分时将全部记为little）。
         */
        uint32 big_core_count;         ///< 大核数量

        /**
         * 小核数量（little cluster core count）。表示效率核的数量。
         */
        uint32 little_core_count;      ///< 小核数量

        /**
         * 大核最大频率，单位为MHz。表示大核在最大性能状态下的标称/测得频率。
         * 该值可能来自设备树、/sys 文件系统或厂商接口，探测失败时可能为0。
         */
        uint32 big_core_max_freq;      ///< 大核最大频率(MHz)

        /**
         * 小核最大频率，单位为MHz。表示小核在最大性能状态下的标称/测得频率。
         */
        uint32 little_core_max_freq;   ///< 小核最大频率(MHz)


        // ------------------------ 缓存信息（单位：KB） ------------------------
        /**
         * L1 数据缓存大小，单位KB。可能为每核大小或每簇大小，具体语义依赖
         * 于探测实现以及CPU架构（在ARM中，L1通常为每核的私有缓存）。
         */
        uint32 l1_data_cache_size;     ///< L1数据缓存大小(KB)

        /**
         * L1 指令缓存大小，单位KB。
         */
        uint32 l1_inst_cache_size;     ///< L1指令缓存大小(KB)

        /**
         * L2 缓存大小，单位KB。常见为每簇或每集群共享。
         */
        uint32 l2_cache_size;          ///< L2缓存大小(KB)

        /**
         * L3 缓存大小，单位KB。并非所有ARM平台都有L3缓存，若无则为0。
         */
        uint32 l3_cache_size;          ///< L3缓存大小(KB)
    };
} // namespace hgl
