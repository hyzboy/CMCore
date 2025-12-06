#pragma once

#if defined(_M_AMD64) || defined(_M_X64) || defined(_M_IX86)
#   include <hgl/platform/CpuX86Features.h>
#elif defined(_M_ARM) || defined(_M_ARM64)
#   include <hgl/platform/CpuARMFeatures.h>
#endif

namespace hgl
{
    enum class CpuArch
    {
        x86_64,

        ARMv8,      //32+64
        ARMv9,      //64
    };//

    struct CpuInfo
    {
        uint cpu_count;             ///<处理器数量
        uint core_count;            ///<处理器核心数量
        uint logical_core_count;    ///<逻辑处理器核心数量

        CpuArch arch;               ///<处理器架构
        CpuFeatures features;       ///< 通用处理器特性
    };//struct CpuInfo

    /**
     * 取得CPU信息
     */
    bool GetCpuInfo(CpuInfo *);
}//namespace hgl
