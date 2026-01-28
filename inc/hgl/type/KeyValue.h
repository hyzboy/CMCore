#pragma once

#include <cstdint>

namespace hgl
{
    template<typename K,typename V> struct KeyValue
    {
        K key;
        V value;
        uint64_t hash; // pre-computed hash for faster comparison
    };
}//namespace hgl
