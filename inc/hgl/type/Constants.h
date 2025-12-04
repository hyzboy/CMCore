/**
 * Constants.h - 常量定义
 * Constants definitions
 * 
 * This file provides common constants used throughout the codebase
 */

#pragma once

#include<hgl/platform/Platform.h>

namespace hgl
{
    // Character constants for u16char
    #define U16_TEXT(x) u##x
    #define U16_FULL_WIDTH_SPACE 0x3000

    // Character constants for u32char
    #define U32_TEXT(x) U##x
    #define U32_FULL_WIDTH_SPACE 0x3000

    // Size constants
    constexpr uint HGL_SIZE_1KB = 1024;
    constexpr uint HGL_SIZE_1MB = 1024 * 1024;
    constexpr uint HGL_SIZE_1GB = 1024 * 1024 * 1024;

    // Filename constants
    #if HGL_OS == HGL_OS_Windows
        constexpr uint HGL_MAX_PATH = 260;
        constexpr uint HGL_MAX_FILENAME = 260;
    #else
        constexpr uint HGL_MAX_PATH = 4096;
        constexpr uint HGL_MAX_FILENAME = 255;
    #endif

    // Support for char8_t in C++20
    #if __cplusplus >= 202002L
        #define HGL_SUPPORT_CHAR8_T
    #endif
}
