#pragma once
//--------------------------------------------------------------------------------------------------
#define HGL_COMPILER_NAME       OS_TEXT("GNU C/C++")
#define HGL_LIB_COMPILER_NAME   OS_TEXT("GCC")

#if (__GNUC__<4)||(__GNUC__==4&&__GNUC_MINOR__<8)
    #error Please upgrade your compiler or development tools to GNU C/C++ 4.8 or later
#else

    #if __GNUC__ == 4
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("4")
    #elif __GNUC__ == 5
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("5")
    #elif __GNUC__ == 6
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("6")
    #elif __GNUC__ == 7
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("7")
    #elif __GNUC__ == 8
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("8")
    #elif __GNUC__ == 9
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("9")
    #elif __GNUC__ == 10
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("10")
    #elif __GNUC__ == 11
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("11")
    #elif __GNUC__ == 12
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("12")
    #elif __GNUC__ == 13
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("13")
    #elif __GNUC__ == 14
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("14")
    #elif __GNUC__ == 15
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("15")
    #elif __GNUC__ == 16
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("16")
    #elif __GNUC__ == 17
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("17")
    #elif __GNUC__ == 18
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("18")
    #elif __GNUC__ == 19
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("19")
    #elif __GNUC__ == 20
        #define HGL_LIB_COMPILER_MAIOR_VER  OS_TEXT("20")
    #endif//__GNUC__

    #if __GNUC_MINOR__ == 0
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("0")
    #elif __GNUC_MINOR__ == 1
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("1")
    #elif __GNUC_MINOR__ == 2
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("2")
    #elif __GNUC_MINOR__ == 3
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("3")
    #elif __GNUC_MINOR__ == 4
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("4")
    #elif __GNUC_MINOR__ == 5
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("5")
    #elif __GNUC_MINOR__ == 6
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("6")
    #elif __GNUC_MINOR__ == 7
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("7")
    #elif __GNUC_MINOR__ == 8
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("8")
    #elif __GNUC_MINOR__ == 9
        #define HGL_LIB_COMPILER_MINOR_VER  OS_TEXT("9")
    #endif//

    #if __GNUC_PATCHLEVEL__ == 0
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("0")
    #elif __GNUC_PATCHLEVEL__ == 1
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("1")
    #elif __GNUC_PATCHLEVEL__ == 2
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("2")
    #elif __GNUC_PATCHLEVEL__ == 3
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("3")
    #elif __GNUC_PATCHLEVEL__ == 4
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("4")
    #elif __GNUC_PATCHLEVEL__ == 5
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("5")
    #elif __GNUC_PATCHLEVEL__ == 6
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("6")
    #elif __GNUC_PATCHLEVEL__ == 7
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("7")
    #elif __GNUC_PATCHLEVEL__ == 8
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("8")
    #elif __GNUC_PATCHLEVEL__ == 9
        #define HGL_LIB_COMPILER_PATCH_LEVEL    OS_TEXT("9")
    #endif//

    #define HGL_LIB_COMPILER_VERSION HGL_LIB_COMPILER_MAIOR_VER OS_TEXT(".") HGL_LIB_COMPILER_MINOR_VER OS_TEXT(".") HGL_LIB_COMPILER_PATCH_LEVEL

#endif//__GNUC__

#define HGL_THREAD_LOCAL_STORAGE        __thread                                //线程本地储存
//--------------------------------------------------------------------------------------------------
#define HGL_LIB_FRONT   HGL_LIB_OS "_" HGL_LIB_COMPILER_NAME "_" HGL_LIB_DEBUG_NAME "_"

#define HGL_LIB_END     ".a"
//--------------------------------------------------------------------------------------------------
#include<hgl/BasicType.h>
#include<hgl/platform/compiler/Property.h>
//--------------------------------------------------------------------------------------------------
