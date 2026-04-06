#pragma once

#include <hgl/type/DataType.h>
#include <cstddef>
#include <memory>

namespace hgl::hash
{
    template<typename HashType>
    struct FNV1aTraits;

    template<>
    struct FNV1aTraits<uint32>
    {
        static constexpr uint32 OffsetBasis = 2166136261u;
        static constexpr uint32 Prime = 16777619u;
    };

    template<>
    struct FNV1aTraits<uint64>
    {
        static constexpr uint64 OffsetBasis = 1469598103934665603ull;
        static constexpr uint64 Prime = 1099511628211ull;
    };

    template<typename HashType>
    constexpr HashType FNV1aInit(const HashType seed = FNV1aTraits<HashType>::OffsetBasis) noexcept
    {
        return seed;
    }

    template<typename HashType, typename ValueType>
    constexpr HashType FNV1aAppend(HashType hash, const ValueType value) noexcept
    {
        hash ^= static_cast<HashType>(value);
        hash *= FNV1aTraits<HashType>::Prime;
        return hash;
    }

    template<typename HashType>
    inline HashType FNV1aAppendBytes(HashType hash, const void *data, const size_t size) noexcept
    {
        if (!data || size == 0)
            return hash;

        const auto *bytes = reinterpret_cast<const uchar *>(data);

        for (size_t i = 0; i < size; ++i)
            hash = FNV1aAppend(hash, bytes[i]);

        return hash;
    }

    template<typename HashType, typename ValueType>
    inline HashType FNV1aAppendValueBytes(HashType hash, const ValueType &value) noexcept
    {
        return FNV1aAppendBytes(hash, std::addressof(value), sizeof(ValueType));
    }
}
