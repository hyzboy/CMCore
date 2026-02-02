#pragma once

#include <cstdint>

namespace hgl
{
    template<typename K,typename V> struct KeyValue
    {
        K key;
        V value;
        uint64_t hash; // pre-computed hash for faster comparison

        bool operator==(const KeyValue& other) const
        {
            return key == other.key && value == other.value;
        }

        bool operator!=(const KeyValue& other) const
        {
            return !(*this == other);
        }
    };
}//namespace hgl
