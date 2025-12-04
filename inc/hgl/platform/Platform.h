/**
 * Platform.h - 平台定义和基础类型定义
 * Platform definitions and basic type definitions
 * 
 * This file provides basic type definitions and platform detection macros
 */

#pragma once

#include <cstdint>
#include <cstddef>

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define HGL_OS_Windows 1
    #define HGL_OS HGL_OS_Windows
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define HGL_OS_iOS 2
        #define HGL_OS HGL_OS_iOS
    #else
        #define HGL_OS_macOS 3
        #define HGL_OS HGL_OS_macOS
    #endif
#elif defined(__ANDROID__)
    #define HGL_OS_Android 4
    #define HGL_OS HGL_OS_Android
#elif defined(__linux__)
    #define HGL_OS_Linux 5
    #define HGL_OS HGL_OS_Linux
#elif defined(__FreeBSD__)
    #define HGL_OS_FreeBSD 6
    #define HGL_OS_BSD 6
    #define HGL_OS HGL_OS_FreeBSD
#else
    #define HGL_OS_Unknown 0
    #define HGL_OS HGL_OS_Unknown
#endif

// Endianness detection
#define HGL_BIG_ENDIAN 1
#define HGL_LITTLE_ENDIAN 0

#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #define HGL_ENDIAN HGL_BIG_ENDIAN
#else
    #define HGL_ENDIAN HGL_LITTLE_ENDIAN
#endif

// Basic type definitions
namespace hgl
{
    using int8 = int8_t;
    using int16 = int16_t;
    using int32 = int32_t;
    using int64 = int64_t;

    using uint8 = uint8_t;
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;

    using uint = unsigned int;

    // Character types
    using u8char = char;
    using u16char = char16_t;
    using u32char = char32_t;

    #if defined(_WIN32) || defined(_WIN64)
        using os_char = wchar_t;
    #else
        using os_char = char;
    #endif

    // Platform-specific pointer type
    #if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
        using HGL_POINTER_UINT = uint64;
    #else
        using HGL_POINTER_UINT = uint32;
    #endif
}

// Bring types into global namespace for compatibility
using hgl::int8;
using hgl::int16;
using hgl::int32;
using hgl::int64;
using hgl::uint8;
using hgl::uint16;
using hgl::uint32;
using hgl::uint64;
using hgl::uint;
using hgl::u8char;
using hgl::u16char;
using hgl::u32char;
using hgl::os_char;
using hgl::HGL_POINTER_UINT;
