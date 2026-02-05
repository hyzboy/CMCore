/**
 * @file Platform.h
 * @brief 平台基础定义 - 提供跨平台基本类型定义
 */
#pragma once

#include <cstdint>
#include <cstddef>

// 基本整数类型定义
#ifndef int8
    typedef int8_t int8;
#endif

#ifndef int16
    typedef int16_t int16;
#endif

#ifndef int32
    typedef int32_t int32;
#endif

#ifndef int64
    typedef int64_t int64;
#endif

#ifndef uint
    typedef unsigned int uint;
#endif

#ifndef uint8
    typedef uint8_t uint8;
#endif

#ifndef uint16
    typedef uint16_t uint16;
#endif

#ifndef uint32
    typedef uint32_t uint32;
#endif

#ifndef uint64
    typedef uint64_t uint64;
#endif

// os_char 和 os_main 平台相关定义
#ifdef _WIN32
    typedef wchar_t os_char;
    #define os_main wmain
#else
    typedef char os_char;
    #define os_main main
#endif
