#pragma once

#include<hgl/type/Constants.h>

namespace hgl
{
    //==================================================================================================
    // 整数极值模板函数 / Integer Limit Template Functions
    //==================================================================================================
    
    template<typename T> constexpr const T HGL_INTEGER_MAX();
    template<typename T> constexpr const T HGL_INTEGER_MIN();
    template<typename T> constexpr const T HGL_UINTEGER_HALF();

    template<> constexpr const uint8     HGL_INTEGER_MAX<uint8   >() { return HGL_U8_MAX; }
    template<> constexpr const uint16    HGL_INTEGER_MAX<uint16  >() { return HGL_U16_MAX; }
    template<> constexpr const uint32    HGL_INTEGER_MAX<uint32  >() { return HGL_U32_MAX; }
    template<> constexpr const uint64    HGL_INTEGER_MAX<uint64  >() { return HGL_U64_MAX; }

    template<> constexpr const uint8     HGL_UINTEGER_HALF<uint8 >() { return HGL_U8_MAX >> 1; }
    template<> constexpr const uint16    HGL_UINTEGER_HALF<uint16>() { return HGL_U16_MAX >> 1; }
    template<> constexpr const uint32    HGL_UINTEGER_HALF<uint32>() { return HGL_U32_MAX >> 1; }
    template<> constexpr const uint64    HGL_UINTEGER_HALF<uint64>() { return HGL_U64_MAX >> 1; }

    template<> constexpr const int8      HGL_INTEGER_MAX<int8    >() { return HGL_S8_MAX; }
    template<> constexpr const int16     HGL_INTEGER_MAX<int16   >() { return HGL_S16_MAX; }
    template<> constexpr const int32     HGL_INTEGER_MAX<int32   >() { return HGL_S32_MAX; }
    template<> constexpr const int64     HGL_INTEGER_MAX<int64   >() { return HGL_S64_MAX; }

    template<> constexpr const int8      HGL_INTEGER_MIN<int8    >() { return HGL_S8_MIN; }
    template<> constexpr const int16     HGL_INTEGER_MIN<int16   >() { return HGL_S16_MIN; }
    template<> constexpr const int32     HGL_INTEGER_MIN<int32   >() { return HGL_S32_MIN; }
    template<> constexpr const int64     HGL_INTEGER_MIN<int64   >() { return HGL_S64_MIN; }
}//namespace hgl
