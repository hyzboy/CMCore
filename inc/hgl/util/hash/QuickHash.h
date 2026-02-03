/**
 * @file QuickHash.h
 * @brief CN:高性能哈希函数库\nEN:High-performance hash function library
 * 
 * CN:提供针对不同数据类型优化的哈希函数实现
 * EN:Provides optimized hash function implementations for different data types
 */
#pragma once

#include <cstdint>
#include <cstring>
#include <type_traits>
#include <string_view>

namespace hgl
{
    // ==================== 基础哈希常量 ====================
    
    /**
     * @brief CN:FNV-1a 哈希算法的基础偏移量\nEN:FNV-1a hash algorithm basis offset
     */
    constexpr uint64_t FNV_OFFSET_BASIS_64 = 14695981039346656037ULL;
    
    /**
     * @brief CN:FNV-1a 哈希算法的质数\nEN:FNV-1a hash algorithm prime
     */
    constexpr uint64_t FNV_PRIME_64 = 1099511628211ULL;

    // ==================== 内部实现细节 ====================
    
    namespace detail
    {
        /**
         * @brief CN:FNV-1a 哈希算法实现\nEN:FNV-1a hash algorithm implementation
         * @note CN:对于小数据块性能优异\nEN:Excellent performance for small data blocks
         */
        inline uint64_t fnv1a_hash(const void* data, size_t size) noexcept
        {
            const uint8_t* bytes = static_cast<const uint8_t*>(data);
            uint64_t hash = FNV_OFFSET_BASIS_64;
            
            for (size_t i = 0; i < size; ++i)
            {
                hash ^= static_cast<uint64_t>(bytes[i]);
                hash *= FNV_PRIME_64;
            }
            
            return hash;
        }

        /**
         * @brief CN:64位整数混淆函数（MurmurHash3 finalizer）\nEN:64-bit integer mixing function (MurmurHash3 finalizer)
         * @note CN:提供良好的雪崩效应\nEN:Provides good avalanche effect
         */
        inline uint64_t mix64(uint64_t x) noexcept
        {
            x ^= x >> 33;
            x *= 0xff51afd7ed558ccdULL;
            x ^= x >> 33;
            x *= 0xc4ceb9fe1a85ec53ULL;
            x ^= x >> 33;
            return x;
        }

        /**
         * @brief CN:32位整数混淆函数\nEN:32-bit integer mixing function
         */
        inline uint32_t mix32(uint32_t x) noexcept
        {
            x ^= x >> 16;
            x *= 0x85ebca6b;
            x ^= x >> 13;
            x *= 0xc2b2ae35;
            x ^= x >> 16;
            return x;
        }
    } // namespace detail

    // ==================== 基础类型哈希（零开销） ====================

    /**
     * @brief CN:通用哈希函数（针对平凡可复制类型）\nEN:Generic hash function (for trivially copyable types)
     * @tparam T CN:数据类型，必须是平凡可复制类型\nEN:Data type, must be trivially copyable
     */
    template<typename T>
    inline uint64_t ComputeOptimalHash(const T& value) noexcept
    {
        static_assert(std::is_trivially_copyable_v<T>,
            "ComputeOptimalHash requires trivially copyable types");
        
        if constexpr (sizeof(T) == 8)
        {
            // 对于8字节类型（如int64_t, double, 指针等），直接使用混淆函数
            uint64_t bits;
            std::memcpy(&bits, &value, sizeof(T));
            return detail::mix64(bits);
        }
        else if constexpr (sizeof(T) == 4)
        {
            // 对于4字节类型（如int32_t, float），使用32位混淆并扩展
            uint32_t bits;
            std::memcpy(&bits, &value, sizeof(T));
            return static_cast<uint64_t>(detail::mix32(bits));
        }
        else if constexpr (sizeof(T) <= 2)
        {
            // 对于小类型（1-2字节），直接混淆
            uint64_t bits = 0;
            std::memcpy(&bits, &value, sizeof(T));
            return detail::mix64(bits);
        }
        else
        {
            // 对于其他大小，使用FNV-1a
            return detail::fnv1a_hash(&value, sizeof(T));
        }
    }

    // ==================== 整数类型特化（零开销） ====================

    /**
     * @brief CN:int8_t 特化\nEN:int8_t specialization
     */
    inline uint64_t ComputeOptimalHash(int8_t value) noexcept
    {
        return detail::mix64(static_cast<uint64_t>(value));
    }

    /**
     * @brief CN:uint8_t 特化\nEN:uint8_t specialization
     */
    inline uint64_t ComputeOptimalHash(uint8_t value) noexcept
    {
        return detail::mix64(static_cast<uint64_t>(value));
    }

    /**
     * @brief CN:int16_t 特化\nEN:int16_t specialization
     */
    inline uint64_t ComputeOptimalHash(int16_t value) noexcept
    {
        return detail::mix64(static_cast<uint64_t>(value));
    }

    /**
     * @brief CN:uint16_t 特化\nEN:uint16_t specialization
     */
    inline uint64_t ComputeOptimalHash(uint16_t value) noexcept
    {
        return detail::mix64(static_cast<uint64_t>(value));
    }

    /**
     * @brief CN:int32_t 特化\nEN:int32_t specialization
     */
    inline uint64_t ComputeOptimalHash(int32_t value) noexcept
    {
        return static_cast<uint64_t>(detail::mix32(static_cast<uint32_t>(value)));
    }

    /**
     * @brief CN:uint32_t 特化\nEN:uint32_t specialization
     */
    inline uint64_t ComputeOptimalHash(uint32_t value) noexcept
    {
        return static_cast<uint64_t>(detail::mix32(value));
    }

    /**
     * @brief CN:int64_t 特化\nEN:int64_t specialization
     */
    inline uint64_t ComputeOptimalHash(int64_t value) noexcept
    {
        return detail::mix64(static_cast<uint64_t>(value));
    }

    /**
     * @brief CN:uint64_t 特化\nEN:uint64_t specialization
     */
    inline uint64_t ComputeOptimalHash(uint64_t value) noexcept
    {
        return detail::mix64(value);
    }

    /**
     * @brief CN:指针类型特化\nEN:Pointer type specialization
     */
    template<typename T>
    inline uint64_t ComputeOptimalHash(T* ptr) noexcept
    {
        return detail::mix64(reinterpret_cast<uint64_t>(ptr));
    }

    /**
     * @brief CN:float 特化\nEN:float specialization
     */
    inline uint64_t ComputeOptimalHash(float value) noexcept
    {
        uint32_t bits;
        std::memcpy(&bits, &value, sizeof(float));
        return static_cast<uint64_t>(detail::mix32(bits));
    }

    /**
     * @brief CN:double 特化\nEN:double specialization
     */
    inline uint64_t ComputeOptimalHash(double value) noexcept
    {
        uint64_t bits;
        std::memcpy(&bits, &value, sizeof(double));
        return detail::mix64(bits);
    }

    // ==================== 字符串哈希 ====================

    /**
     * @brief CN:字符串哈希（C风格字符串，带长度）\nEN:String hash (C-style string with length)
     * @param str CN:字符串指针\nEN:String pointer
     * @param length CN:字符串长度\nEN:String length
     */
    template<typename CharType>
    inline uint64_t ComputeOptimalHash(const CharType* str, size_t length) noexcept
    {
        static_assert(std::is_trivially_copyable_v<CharType>,
            "CharType must be trivially copyable");
        
        if (!str || length == 0)
            return 0;
        
        // 对于字符串，使用FNV-1a算法
        return detail::fnv1a_hash(str, length * sizeof(CharType));
    }

    /**
     * @brief CN:std::string_view 特化\nEN:std::string_view specialization
     */
    inline uint64_t ComputeOptimalHash(std::string_view sv) noexcept
    {
        return ComputeOptimalHash(sv.data(), sv.size());
    }

    /**
     * @brief CN:std::wstring_view 特化\nEN:std::wstring_view specialization
     */
    inline uint64_t ComputeOptimalHash(std::wstring_view sv) noexcept
    {
        return ComputeOptimalHash(sv.data(), sv.size());
    }

    /**
     * @brief CN:std::u16string_view 特化\nEN:std::u16string_view specialization
     */
    inline uint64_t ComputeOptimalHash(std::u16string_view sv) noexcept
    {
        return ComputeOptimalHash(sv.data(), sv.size());
    }

    /**
     * @brief CN:std::u32string_view 特化\nEN:std::u32string_view specialization
     */
    inline uint64_t ComputeOptimalHash(std::u32string_view sv) noexcept
    {
        return ComputeOptimalHash(sv.data(), sv.size());
    }

    // ==================== 性能优化建议 ====================
    
    /**
     * CN:使用建议：
     * 1. 对于整数/指针类型：零开销，直接位操作
     * 2. 对于小结构体（≤8字节）：使用 memcpy + mix，性能优异
     * 3. 对于大结构体：考虑只哈希关键字段，或使用预计算哈希
     * 4. 对于字符串：使用FNV-1a，对短字符串性能优秀
     * 
     * EN:Usage recommendations:
     * 1. For integers/pointers: Zero overhead, direct bit manipulation
     * 2. For small structs (≤8 bytes): Use memcpy + mix, excellent performance
     * 3. For large structs: Consider hashing only key fields, or use pre-computed hash
     * 4. For strings: Use FNV-1a, excellent performance for short strings
     */

} // namespace hgl
