#pragma once

#include<cstdint>

namespace hgl
{
    using int8   = std::int8_t;
    using uint8  = std::uint8_t;
    using int16  = std::int16_t;
    using uint16 = std::uint16_t;
    using int32  = std::int32_t;
    using uint32 = std::uint32_t;
    using int64  = std::int64_t;
    using uint64 = std::uint64_t;
    
    using half_float = uint16;
    using float32    = float;
    using float64    = double;

#if defined(__cpp_char8_t)||defined(__cpp_lib_char8_t)
    #define HGL_SUPPORT_CHAR8_T
    using u8char = char8_t;
#else
    using u8char = char;
#endif
    
    using i8=int8;
    using i16=int16;
    using i32=int32;
    using i64=int64;

    using u8=uint8;
    using u16=uint16;
    using u32=uint32;
    using u64=uint64;

    using f16=half_float;
    using f32=float;
    using f64=double;

    #define enum_int(name)  enum name:int
    #define enum_uint(name) enum name:uint

    using void_pointer=void *;

    using uchar     = unsigned char;    ///< 无符号字符型
    using ushort    = unsigned short;   ///< 无符号短整型
    using uint      = unsigned int;     ///< 无符号整型
    using ulong     = unsigned long;    ///< 无符号长整型
}//namespace hgl
