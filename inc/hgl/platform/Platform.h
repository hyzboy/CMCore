﻿#ifndef HGL_PLATFORM_INCLUDE
#define HGL_PLATFORM_INCLUDE

#include<hgl/platform/os/vsprintf.h>

#ifdef _DEBUG
    #define HGL_LIB_DEBUG_NAME  "Debug"
#else
    #define HGL_LIB_DEBUG_NAME  "Release"
#endif//

#define HGL_MERGE32(a,b,c,d)        (a|b<<8|c<<16|d<<24)

#define HGL_OS_Windows              HGL_MERGE32('W','i','n',' ')
#define HGL_OS_macOS                HGL_MERGE32('m','a','c',' ')
#define HGL_OS_iOS                  HGL_MERGE32('i','O','S',' ')
#define HGL_OS_iPadOS               HGL_MERGE32('i','P','O','S')
#define HGL_OS_tvOS                 HGL_MERGE32('t','v','O','S')
#define HGL_OS_FreeBSD              HGL_MERGE32('F','B','S','D')
#define HGL_OS_NetBSD               HGL_MERGE32('N','B','S','D')
#define HGL_OS_OpenBSD              HGL_MERGE32('O','B','S','D')
#define HGL_OS_Linux                HGL_MERGE32('L','i','n','u')
#define HGL_OS_Cygwin               HGL_MERGE32('C','y','g','w')
#define HGL_OS_PS4                  HGL_MERGE32('P','S','4',' ')
#define HGL_OS_PS5                  HGL_MERGE32('P','S','5',' ')
#define HGL_OS_Android              HGL_MERGE32('A','n','d','r')
#define HGL_OS_Wasm                 HGL_MERGE32('W','a','s','m')

#define HGL_CPU_X86_64              HGL_MERGE32('8','6','6','4')
#define HGL_CPU_MIPS                HGL_MERGE32('M','I','P','S')
#define HGL_CPU_MIPSel              HGL_MERGE32('M','I','e','l')
#define HGL_CPU_MIPS64              HGL_MERGE32('M','I','6','4')
#define HGL_CPU_PowerPC64           HGL_MERGE32('P','P','6','4')
#define HGL_CPU_ARMv8               HGL_MERGE32('A','R','M','8')
#define HGL_CPU_ARMv9               HGL_MERGE32('A','R','M','9')
#define HGL_CPU_Apple               HGL_MERGE32('A','P','P','L')

#define HGL_COMPILER_Microsoft      HGL_MERGE32('M','S','C',' ')
#define HGL_COMPILER_GNU            HGL_MERGE32('G','N','U',' ')
#define HGL_COMPILER_MinGW64        HGL_MERGE32('M','G','6','4')
#define HGL_COMPILER_Intel          HGL_MERGE32('I','n','t','e')
#define HGL_COMPILER_IBM            HGL_MERGE32('I','B','M',' ')
#define HGL_COMPILER_LLVM           HGL_MERGE32('L','L','V','M')

#define HGL_BIG_ENDIAN      'B'
#define HGL_LITTLE_ENDIAN   'L'

#if defined(__amd64__)  ||defined(__amd64)  ||  \
    defined(__x86_64__) ||defined(__x86_64) ||  \
    defined(_M_AMD64)   ||defined(_M_X64)
    #define HGL_CPU                     HGL_CPU_X86_64
    #define HGL_CPU_NAME                OS_TEXT("X86-64Bit")
    #define HGL_LIB_CPU_NAME            OS_TEXT("x64")
    #define HGL_MIN_MEMORY_ALLOC_BYTES  8
    #define HGL_ENDIAN                  HGL_LITTLE_ENDIAN
#elif defined(_M_MIPS)||defined(_MIPS_ARCH)||defined(__mips__)||defined(__mips)||defined(mips)||defined(__MIPS__)
    #define HGL_MIN_MEMORY_ALLOC_BYTES  4
    #ifdef __MIPSEL__
        #define HGL_CPU                     HGL_CPU_MIPSel
        #define HGL_CPU_NAME                OS_TEXT("MIPSel")
        #define HGL_LIB_CPU_NAME            OS_TEXT("MIPSel")
        #define HGL_ENDIAN                  HGL_LITTLE_ENDIAN
    #else
        #define HGL_CPU                     HGL_CPU_MIPS
        #define HGL_CPU_NAME                OS_TEXT("MIPS")
        #define HGL_LIB_CPU_NAME            OS_TEXT("MIPS")
        #define HGL_ENDIAN                  HGL_BIG_ENDIAN
    #endif//__MIPSEL__
#elif defined(__mips64__)||defined(mips64)
        #define HGL_MIN_MEMORY_ALLOC_BYTES  8
        #define HGL_CPU                     HGL_CPU_MIPS64
        #define HGL_CPU_NAME                OS_TEXT("MIPS64")
        #define HGL_LIB_CPU_NAME            OS_TEXT("MIPS64")
        #define HGL_ENDIAN                  HGL_BIG_ENDIAN
#elif defined(__powerpc__) || defined(__powerpc)    \
     || defined(__POWERPC__) \
     || defined(__ppc__) || defined(__PPC__) || defined(__ppc) \
     || defined(_M_PPC) ||defined(_M_MPPC) \
     || defined(_ARCH_PPC) || defined(_ARCH_PWR)
        #ifdef defined(__ppc64__)||defined(__PPC64__)||defined(__powerpc64__)   \
             ||defined(_ARCH_PPC64)||defined(__64BIT__)
            #define HGL_CPU             HGL_CPU_PowerPC64
            #define HGL_CPU_NAME        OS_TEXT("PowerPC64")
            #define HGL_LIB_CPU_NAME    OS_TEXT("PowerPC64")
        #else
            #define HGL_CPU             HGL_CPU_PowerPC
            #define HGL_CPU_NAME        OS_TEXT("PowerPC")
            #define HGL_LIB_CPU_NAME    OS_TEXT("PowerPC")
        #endif//PowerPC64

    #define HGL_MIN_MEMORY_ALLOC_BYTES  8
    #define HGL_ENDIAN                  HGL_BIG_ENDIAN
#elif defined(__aarch64__) || defined(__arm64__) || defined(__ARM64__) || defined(_M_ARM64)
    #define HGL_CPU                     HGL_CPU_ARMv8
    #define HGL_CPU_NAME                OS_TEXT("ARMv8")
    #define HGL_LIB_CPU_NAME            OS_TEXT("ARMv8")
    #define HGL_MIN_MEMORY_ALLOC_BYTES  4
    #define HGL_ENDIAN                  HGL_LITTLE_ENDIAN
#elif defined(__ARM_ARCH_9__) || defined(_M_ARM_ARCH_9) || (defined(__ARM_ARCH) && __ARM_ARCH>=9)
    #define HGL_CPU                     HGL_CPU_ARMv9
    #define HGL_CPU_NAME                OS_TEXT("ARMv9")
    #define HGL_LIB_CPU_NAME            OS_TEXT("ARMv9")
    #define HGL_MIN_MEMORY_ALLOC_BYTES  4
    #define HGL_ENDIAN                  HGL_LITTLE_ENDIAN
#elif defined(__wasm__)

    #error Not support WebAssembly.

    #define HGL_OS          HGL_OS_Wasm
    #define HGL_COMPILER    HGL_COMPILER_LLVM
#else
    #error Not support the cpu.
#endif

#if HGL_ENDIAN == HGL_BIG_ENDIAN
    #define HGL_CPU_ENDIAN      OS_TEXT("Big Endian")
#else
    #define HGL_CPU_ENDIAN      OS_TEXT("Little Endian")
#endif//HGL_ENDIAN == HGL_BIG_ENDIAN

#if defined(__WIN32__)||defined(_WIN32)||defined(WIN32)||defined(__WINDOWS__)||defined(__WIN64__)||defined(_WIN64)||defined(WIN64)
    #define HGL_OS          HGL_OS_Windows
#elif defined(__APPLE__)||defined(__MAC__)||defined(macintosh)||defined(__APPLE_CC__)

    #define HGL_OS_BSD  1

    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        //#if TARGET_IPHONE_SIMULATOR
        //    #define HGL_OS  HGL_OS_iOS_Simulator
        //#else
            #define HGL_OS  HGL_OS_iOS
        //#endif//TARGET_IPHONE_SIMULATOR
    #elif TARGET_OS_MAC
        #define HGL_OS      HGL_OS_macOS
    #elif TARGET_OS_TV
        #define HGL_OS      HGL_OS_tvOS
    #endif//
#elif defined(__ANDROID__)
    #define HGL_OS          HGL_OS_Android
#elif defined(__FreeBSD)||defined(__FreeBSD__)
    #define HGL_OS          HGL_OS_FreeBSD
    #define HGL_OS_BSD      1
#elif defined(__NetBSD)||defined(__NetBSD__)
    #define HGL_OS          HGL_OS_NetBSD
    #define HGL_OS_BSD      1
#elif defined(__OPENBSD)||defined(__OpenBSD__)
    #define HGL_OS          HGL_OS_OpenBSD
    #define HGL_OS_BSD      1
#elif defined(__linux__)||defined(__LINUX__)||defined(linux)||defined(__linux)
    #define HGL_OS          HGL_OS_Linux
#elif defined(__CYGWIN__)
    #define HGL_OS          HGL_OS_Cygwin
#elif defined(SN_TARGET_ORBIS) || defined(__ORBIS__)
    #define HGL_OS          HGL_OS_PS4
#else
    #error Unknown OS.
#endif//

#if defined(__clang__)
    #define HGL_COMPILER    HGL_COMPILER_LLVM
#elif defined(__INTEL_COMPILER)||defined(__ECL)||defined(__ICL)||defined(__ICC)
    #define HGL_COMPILER    HGL_COMPILER_Intel
#elif defined(__IBMC__)||defined(__IBMCPP__)
    #define HGL_COMPILER    HGL_COMPILER_IBM
#elif defined(__DMC__)||defined(__SC__)||defined(__ZTC__)
    #define HGL_COMPILER    HGL_COMPILER_DigitalMars
#elif defined(_MSC_VER)
    #define HGL_COMPILER    HGL_COMPILER_Microsoft
#elif defined(__PGI)
    #define HGL_COMPILER    HGL_COMPILER_PGI
#elif defined(__MINGW32__)
    #define HGL_COMPILER    HGL_COMPILER_MinGW32
#elif defined(__MINGW64__)
    #define HGL_COMPILER    HGL_COMPILER_MinGW64
#elif defined(__GNUC__)
    #define HGL_COMPILER    HGL_COMPILER_GNU
#else
    #error Can not support the Compiler.
#endif//

#if defined(SetEventCall)||defined(SafeCallEvent)||defined(CallEvent)||defined(DefEvent)
    #error SetEventCall,SafeCallEvent,CallEvent,DefEvent 其中之一已经定义
#endif//

#if defined(Property)||defined(PropertyRead)||defined(PropertyWrite)
    #error Property,PropertyRead,PropertyWrite 其中之一已经定义
#endif//

// #ifndef UNICODE
//  #define UNICODE                     //定义使用UNICODE编码
// #endif//UNICODE

#if HGL_OS == HGL_OS_Windows

    #include<hgl/platform/os/MSWindows.h>

    #if HGL_COMPILER == HGL_COMPILER_Microsoft
        #include<hgl/platform/compiler/Microsoft.h>
    #elif HGL_COMPILER == HGL_COMPILER_LLVM
        #include<hgl/platform/compiler/LLVM.h>
    #elif HGL_COMPILER == HGL_COMPILER_Intel
        #include<hgl/platform/compiler/Intel.h>
    #elif defined(HGL_COMPILER == HGL_COMPILER_MinGW64)||defined(HGL_COMPILER == HGL_COMPILER_GNU)
        #include<hgl/platform/compiler/GNU.h>
    #else
        #error Unrecognized compiler
    #endif

#elif HGL_OS == HGL_OS_Cygwin

    #include<hgl/platform/os/Cygwin.h>

    #if HGL_COMPILER == HGL_COMPILER_LLVM
        #include<hgl/platform/compiler/LLVM.h>
    #elif HGL_COMPILER == HGL_COMPILER_GNU
        #include<hgl/platform/compiler/GNU.h>
    #else
        #error Unrecognized compiler
    #endif//GNU

#elif (HGL_OS == HGL_OS_FreeBSD)||(HGL_OS == HGL_OS_NetBSD)||(HGL_OS == HGL_OS_OpenBSD)

    #include<hgl/platform/os/BSD.h>

    #if HGL_COMPILER == HGL_COMPILER_LLVM
        #include<hgl/platform/compiler/LLVM.h>
    #elif HGL_COMPILER == HGL_COMPILER_GNU
        #include<hgl/platform/compiler/GNU.h>
    #else
        #error Unrecognized compiler
    #endif//GNU

#elif HGL_OS == HGL_OS_Linux

    #include<hgl/platform/os/Linux.h>

    #if HGL_COMPILER == HGL_COMPILER_Intel
        #include<hgl/platform/compiler/Intel.h>
    #elif HGL_COMPILER == HGL_COMPILER_IBM
        #include<hgl/platform/compiler/IBM.h>
    #elif HGL_COMPILER == HGL_COMPILER_LLVM
        #include<hgl/platform/compiler/LLVM.h>
    #elif HGL_COMPILER == HGL_COMPILER_GNU
        #include<hgl/platform/compiler/GNU.h>
    #else
        #error Unrecognized compiler
    #endif

#elif defined(HGL_OS == HGL_OS_macOS)||defined(HGL_OS == HGL_OS_iOS)||defined(HGL_OS == HGL_OS_tvOS)

    #include<hgl/platform/os/Apple.h>
    #include<hgl/platform/compiler/LLVM.h>

#elif HGL_OS == HGL_OS_Android

    #include<hgl/platform/os/Android.h>
    #include<hgl/platform/compiler/LLVM.h>
#endif//HGL_OS

namespace hgl
{
    #define HGL_WINDOW_DIRECTORY_SEPARATOR  OS_TEXT('\\')
    #define HGL_UNIX_DIRECTORY_SPEARATOR    OS_TEXT('/')

    #if HGL_DIRECTORY_SEPARATOR_RAWCHAR=='\\'
        #define HGL_INCORRECT_DIRECTORY_SEPARATOR   OS_TEXT('/')
    #else
        #define HGL_INCORRECT_DIRECTORY_SEPARATOR   OS_TEXT('\\')
    #endif//
}//namespace hgl
#endif//HGL_PLATFORM_INCLUDE
