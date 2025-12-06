#pragma once

#if defined(_M_ARM) || defined(_M_ARM64)

#include <hgl/platform/CpuInfo.h>
#include <functional>
#include <string>

namespace hgl
{
    /**
     * 大小核检测和优化工具
     */
    class BigLittleDetector
    {
    public:
        /**
         * 检测结果
         */
        struct DetectionResult
        {
            bool has_big_little;
            uint32 big_cores;
            uint32 little_cores;
            uint32 big_freq;
            uint32 little_freq;
            double performance_ratio;
            std::string recommendation;
        };

        /**
         * 检测大小核架构
         */
        static DetectionResult Detect(const CpuInfo& ci)
        {
            DetectionResult result = {};

            if (ci.arch != CpuArch::ARMv8 && ci.arch != CpuArch::ARMv9)
            {
                result.has_big_little = false;
                result.recommendation = "Not an ARM processor - Big.LITTLE detection not applicable";
                return result;
            }

            const CpuFeatures& arm = ci.features;
            result.has_big_little = arm.has_big_little;
            result.big_cores = arm.big_core_count;
            result.little_cores = arm.little_core_count;
            result.big_freq = arm.big_core_max_freq;
            result.little_freq = arm.little_core_max_freq;

            if (arm.has_big_little && arm.little_core_count > 0)
            {
                result.performance_ratio = (double)arm.big_core_max_freq * arm.big_core_count /
                                          (arm.little_core_max_freq * arm.little_core_count);

                if (result.performance_ratio > 2.0)
                {
                    result.recommendation = "High performance difference - use big cores for compute tasks";
                }
                else if (result.performance_ratio > 1.5)
                {
                    result.recommendation = "Moderate difference - balance load across cores";
                }
                else
                {
                    result.recommendation = "Low difference - treat as unified architecture";
                }
            }
            else
            {
                result.recommendation = "Unified architecture - no special handling needed";
            }

            return result;
        }

        /**
         * 检查是否应该使用大核
         */
        static bool ShouldUseBigCores(const CpuInfo& ci, bool is_compute_intensive, bool is_power_sensitive)
        {
            if (ci.arch != CpuArch::ARMv8 && ci.arch != CpuArch::ARMv9)
                return false;

            const CpuARMFeatures& arm = ci.arm;

            if (!arm.has_big_little)
                return false;

            // 计算密集型任务优先使用大核
            if (is_compute_intensive)
                return true;

            // 功耗敏感时避免使用大核
            if (is_power_sensitive)
                return false;

            // 默认策略：有大核就用
            return arm.big_core_count > 0;
        }

        /**
         * 获取推荐的核心分配
         */
        static void GetRecommendedCoreAllocation(
            const CpuInfo& ci,
            uint32& big_cores_for_compute,
            uint32& little_cores_for_background,
            uint32& unified_cores)
        {
            if (ci.arch != CpuArch::ARMv8 && ci.arch != CpuArch::ARMv9)
            {
                unified_cores = ci.logical_core_count;
                return;
            }

            const CpuARMFeatures& arm = ci.arm;

            if (arm.has_big_little)
            {
                big_cores_for_compute = arm.big_core_count;
                little_cores_for_background = arm.little_core_count;
                unified_cores = 0;
            }
            else
            {
                big_cores_for_compute = 0;
                little_cores_for_background = 0;
                unified_cores = arm.big_core_count; // 统一架构
            }
        }

        /**
         * 性能评估
         */
        static double CalculatePerformanceScore(const CpuInfo& ci)
        {
            if (ci.arch != CpuArch::ARMv8 && ci.arch != CpuArch::ARMv9)
            {
                // x86处理器给个基准分数
                return 100.0;
            }

            const CpuARMFeatures& arm = ci.arm;

            if (arm.has_big_little)
            {
                // 基于大小核配置计算性能分数
                double big_perf = arm.big_core_count * arm.big_core_max_freq;
                double little_perf = arm.little_core_count * arm.little_core_max_freq;
                return (big_perf * 1.2 + little_perf * 0.8) / 1000.0; // 加权计算
            }
            else
            {
                // 统一架构
                return (arm.big_core_count * arm.big_core_max_freq) / 1000.0;
            }
        }
    };

    /**
     * 便捷函数
     */
    inline bool HasBigLittleCores(const CpuInfo& ci)
    {
        return (ci.arch == CpuArch::ARMv8 || ci.arch == CpuArch::ARMv9) && ci.arm.has_big_little;
    }

    inline uint32 GetTotalBigCores(const CpuInfo& ci)
    {
        if (HasBigLittleCores(ci))
            return ci.arm.big_core_count;
        return 0;
    }

    inline uint32 GetTotalLittleCores(const CpuInfo& ci)
    {
        if (HasBigLittleCores(ci))
            return ci.arm.little_core_count;
        return 0;
    }
} // namespace hgl
#endif//defined(_M_ARM) || defined(_M_ARM64)