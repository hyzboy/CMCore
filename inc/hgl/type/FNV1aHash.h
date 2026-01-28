#pragma once

#include<hgl/CoreType.h>

namespace hgl
{
    // ==================== 通用哈希函数（FNV-1a） ====================

    // 计算字符串的哈希值（FNV-1a 算法）
    template<typename T>
    inline uint64 ComputeFNV1aHash(const T *str, int length)
    {
        uint64 hash = 14695981039346656037ULL;  // FNV offset basis

        for(int i = 0; i < length; i++)
        {
            hash ^= (uint64)str[i];
            hash *= 1099511628211ULL;  // FNV prime
        }

        return hash;
    }

    /**
     * 计算数据的哈希值（使用 FNV-1a 算法）
     * @tparam T 数据类型
     * @param value 要计算哈希的数据
     * @return 64位哈希值
     */
    template<typename T>
    inline uint64 ComputeFNV1aHash(const T& value)
    {
        return ComputeFNV1aHash((const u8char *)&value,sizeof(T));
    }
    
    // ==================== 优化的哈希函数 ====================

    /**
     * @brief CN:针对类型优化的哈希函数\nEN:Type-optimized hash function
     * @tparam T CN:数据类型\nEN:Data type
     *
     * CN:自动选择最优哈希策略：\nEN:Automatically selects optimal hash strategy:
     * - CN:整数/枚举：身份哈希（零开销）\nEN:Integer/enum: identity hash (zero overhead)
     * - CN:指针：直接使用地址\nEN:Pointer: use address directly
     * - CN:其他类型：FNV1a 哈希\nEN:Other types: FNV1a hash
     */
    template<typename T>
    inline uint64 ComputeOptimalHash(const T& value)
    {
        if constexpr (std::is_integral_v<T> || std::is_enum_v<T>)
        {
            // CN:整数/枚举类型：直接转换（零开销）
            // EN:Integer/enum types: direct conversion (zero overhead)
            return static_cast<uint64>(value);
        }
        else if constexpr (std::is_pointer_v<T>)
        {
            // CN:指针类型：使用地址作为哈希
            // EN:Pointer types: use address as hash
            return reinterpret_cast<uint64>(value);
        }
        else
        {
            // CN:复杂类型：使用 FNV1a 哈希保证质量
            // EN:Complex types: use FNV1a hash for quality
            return ComputeFNV1aHash(value);
        }
    }
}//namespace hgl
