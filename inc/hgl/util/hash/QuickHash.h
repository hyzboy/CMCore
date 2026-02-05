/**
 * @file QuickHash.h
 * @brief CN:快速哈希函数 - 为平凡类型提供优化的哈希计算\nEN:Quick hash functions - Optimized hashing for trivial types
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>

namespace hgl
{
    /**
     * @brief CN:计算优化哈希（针对平凡类型）\nEN:Compute optimal hash (for trivial types)
     * 
     * 根据类型大小选择最优的哈希算法：
     * - 1-8字节：直接使用值或FNV-1a
     * - 更大：FNV-1a哈希
     * 
     * @tparam T CN:必须是平凡可复制类型\nEN:Must be trivially copyable type
     * @param value CN:要计算哈希的值\nEN:Value to hash
     * @return CN:64位哈希值\nEN:64-bit hash value
     */
    template<typename T>
    inline uint64_t ComputeOptimalHash(const T& value)
    {
        static_assert(std::is_trivially_copyable_v<T>,
                      "ComputeOptimalHash requires trivially copyable types");

        constexpr size_t size = sizeof(T);

        // 1字节：直接转换
        if constexpr (size == 1)
        {
            uint8_t val;
            std::memcpy(&val, &value, 1);
            return static_cast<uint64_t>(val);
        }
        // 2字节：直接转换
        else if constexpr (size == 2)
        {
            uint16_t val;
            std::memcpy(&val, &value, 2);
            return static_cast<uint64_t>(val);
        }
        // 4字节：直接转换
        else if constexpr (size == 4)
        {
            uint32_t val;
            std::memcpy(&val, &value, 4);
            return static_cast<uint64_t>(val);
        }
        // 8字节：直接使用
        else if constexpr (size == 8)
        {
            uint64_t val;
            std::memcpy(&val, &value, 8);
            return val;
        }
        // 更大的类型：使用FNV-1a哈希
        else
        {
            // FNV-1a hash constants
            constexpr uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
            constexpr uint64_t FNV_PRIME = 1099511628211ULL;

            const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&value);
            uint64_t hash = FNV_OFFSET_BASIS;

            for (size_t i = 0; i < size; ++i)
            {
                hash ^= static_cast<uint64_t>(bytes[i]);
                hash *= FNV_PRIME;
            }

            return hash;
        }
    }

    /**
     * @brief CN:指针类型的哈希（特化）\nEN:Hash for pointer types (specialization)
     */
    template<typename T>
    inline uint64_t ComputeOptimalHash(T* ptr)
    {
        return reinterpret_cast<uint64_t>(ptr);
    }

} // namespace hgl
