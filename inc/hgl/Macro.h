/**
 * Macro.h - 常用宏定义
 * Common macro definitions
 * 
 * This file provides common utility macros used throughout the codebase
 */

#pragma once

// Common macros
#define HGL_STRINGIFY(x) #x
#define HGL_TOSTRING(x) HGL_STRINGIFY(x)

// Unused parameter macro
#define HGL_UNUSED(x) (void)(x)

// Platform-specific DLL export/import macros
#if defined(_WIN32) || defined(_WIN64)
    #ifdef HGL_DLL_EXPORT
        #define HGL_API __declspec(dllexport)
    #elif defined(HGL_DLL_IMPORT)
        #define HGL_API __declspec(dllimport)
    #else
        #define HGL_API
    #endif
#else
    #if defined(__GNUC__) && __GNUC__ >= 4
        #define HGL_API __attribute__ ((visibility ("default")))
    #else
        #define HGL_API
    #endif
#endif

// Alignment macro
#if defined(_MSC_VER)
    #define HGL_ALIGN(n) __declspec(align(n))
#else
    #define HGL_ALIGN(n) __attribute__((aligned(n)))
#endif

// Force inline macro
#if defined(_MSC_VER)
    #define HGL_FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
    #define HGL_FORCE_INLINE inline __attribute__((always_inline))
#else
    #define HGL_FORCE_INLINE inline
#endif

// Likely/unlikely branch prediction hints
#if defined(__GNUC__) || defined(__clang__)
    #define HGL_LIKELY(x) __builtin_expect(!!(x), 1)
    #define HGL_UNLIKELY(x) __builtin_expect(!!(x), 0)
#else
    #define HGL_LIKELY(x) (x)
    #define HGL_UNLIKELY(x) (x)
#endif
