SET(CMCORE_PLATFORM_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform)

## CPU 硬件信息
SET(CMCORE_PLATFORM_CPUINFO_HEADERS ${CMCORE_PLATFORM_INCLUDE_PATH}/CpuInfo.h
                                    ${CMCORE_PLATFORM_INCLUDE_PATH}/CpuX86Features.h
                                    ${CMCORE_PLATFORM_INCLUDE_PATH}/CpuARMFeatures.h
                                    ${CMCORE_PLATFORM_INCLUDE_PATH}/BigLittleDetector.h
                                    ${CMCORE_PLATFORM_INCLUDE_PATH}/CpuAffinity.h
                                    ${CMCORE_PLATFORM_INCLUDE_PATH}/MemoryAffinity.h)

IF(UNIX)
    # 根据处理器架构选择不同的CPU信息实现
    IF(APPLE)
        IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i386|i686|amd64")
            SET(CMCORE_PLATFORM_CPUINFO_SOURCES     Apple/CpuInfo_X86.cpp)
        ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|armv7|arm")
            SET(CMCORE_PLATFORM_CPUINFO_SOURCES     Apple/CpuInfo_ARM.cpp)
        ELSE()
            SET(CMCORE_PLATFORM_CPUINFO_SOURCES     Apple/CpuInfo_X86.cpp)
            message(WARNING "Unknown Apple processor architecture ${CMAKE_SYSTEM_PROCESSOR}, using X86 CPU info")
        ENDIF()
    ELSE()
        IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i386|i686|amd64")
            SET(CMCORE_PLATFORM_CPUINFO_SOURCES     UNIX/CpuInfo_X86.cpp)
        ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|armv7|arm")
            SET(CMCORE_PLATFORM_CPUINFO_SOURCES     UNIX/CpuInfo_ARM.cpp)
        ELSE()
            SET(CMCORE_PLATFORM_CPUINFO_SOURCES     UNIX/CpuInfo_X86.cpp)
            message(WARNING "Unknown processor architecture ${CMAKE_SYSTEM_PROCESSOR}, using X86 CPU info")
        ENDIF()
    ENDIF()

    # CPU/Memory Affinity 亲合性支持
    SET(CMCORE_PLATFORM_AFFINITY_SOURCES    UNIX/CpuAffinity.cpp
                                            UNIX/MemoryAffinity.cpp)

    SET(CMCORE_PLATFORM_MMAP_SOURCES        /MMapFile.cpp)

    SET(CMCORE_PLATFORM_FILE_SOURCES        UNIX/File.cpp
                                            UNIX/FileAccess.cpp
                                            UNIX/EnumFile.cpp)

    SET(CMCORE_PLATFORM_THREAD_SOURCES      UNIX/CondVar.cpp
                                            UNIX/RWLock.cpp
                                            UNIX/Thread.cpp
                                            UNIX/ThreadMutex.cpp)

    IF(ANDROID)
        SET(CMCORE_PLATFORM_LOG_SOURCES         Android/LogConsole.cpp)

        SET(CMCORE_PLATFORM_APPLICATION_SOURCES ${CMCORE_PLATFORM_APPLICATION_SOURCES}
                                                Android/JNISupport.cpp
                                                Android/NativeActivitySupport.cpp)

        SET(CMCORE_PLATFORM_FILE_SOURCES        ${CMCORE_PLATFORM_FILE_SOURCES}
                                                Android/ProgramPath.cpp
                                                Android/AssetManage.cpp)

        SET(CMCORE_PLATFORM_THREAD_SOURCES      ${CMCORE_PLATFORM_THREAD_SOURCES}
                                                UNIX/Semaphore.cpp)
    ELSE()
        SET(CMCORE_PLATFORM_CHARSET_SOURCES     UNIX/Charset.cpp)
        SET(CMCORE_PLATFORM_LOG_SOURCES         UNIX/LogConsole.cpp)

        IF(APPLE)
            SET(CMCORE_PLATFORM_THREAD_SOURCES  ${CMCORE_PLATFORM_THREAD_SOURCES}
                                                Apple/Semaphore.cpp)

            SET(CMCORE_PLATFORM_FILE_SOURCES    ${CMCORE_PLATFORM_FILE_SOURCES}
                                                Apple/ProgramPath.mm)
        ELSE()
            SET(CMCORE_PLATFORM_THREAD_SOURCES  ${CMCORE_PLATFORM_THREAD_SOURCES}
                                                UNIX/Semaphore.cpp)

            SET(CMCORE_PLATFORM_FILE_SOURCES    ${CMCORE_PLATFORM_FILE_SOURCES}
                                                UNIX/ProgramPath.cpp)

            SET(CMCORE_PLATFORM_WINDOW_SOURCES  UNIX/XCBWindow.cpp)
        ENDIF()
    ENDIF()

    SET(CMCORE_PLATFORM_PROCESS_SOURCES     UNIX/ProcMutex.cpp
                                            UNIX/Process.cpp
                                            UNIX/Pipe.cpp
                                            UNIX/Fifo.cpp

                                            UNIX/Exit.cpp)

    SET(CMCORE_PLATFORM_MODULE_SOURCES      UNIX/ExternalModule.cpp)

ENDIF()

IF(WIN32)
    # 包含两种实现：X86 和 ARM
    # 通过源文件内部的编译时宏来区分具体编译哪一个实现，避免在CMake中基于处理器字符串做选择。
    set(CMCORE_PLATFORM_CPUINFO_SOURCES     Win/CpuInfo_X86.cpp
                                            Win/CpuInfo_ARM.cpp)

    # CPU/Memory Affinity 亲合性支持
    SET(CMCORE_PLATFORM_AFFINITY_SOURCES    Win/CpuAffinity.cpp
                                            Win/MemoryAffinity.cpp)

    SET(CMCORE_PLATFORM_MMAP_SOURCES        Win/MMapFile.cpp)

    SET(CMCORE_PLATFORM_FILE_SOURCES        Win/File.cpp
                                            Win/FileAccess.cpp
                                            Win/EnumFile.cpp
                                            Win/EnumVolume.cpp
                                            Win/ProgramPath.cpp)

    SET(CMCORE_PLATFORM_CHARSET_SOURCES     Win/CodePage.cpp
                                            Win/vsprintf.cpp)

    SET(CMCORE_PLATFORM_SYSTEMINFO_SOURCES  Win/SystemInfo.cpp)

    SET(CMCORE_PLATFORM_LOG_SOURCES         Win/LogConsole.cpp
                                            Win/LogDialog.cpp)

    SET(CMCORE_PLATFORM_THREAD_SOURCES
                                            Win/CondVar.cpp
                                            Win/RWLock.cpp
                                            Win/Semaphore.cpp
                                            Win/Thread.cpp
                                            Win/ThreadMutex.cpp)

    SET(CMCORE_PLATFORM_PROCESS_SOURCES     Win/ProcMutex.cpp
                                            Win/Pipe.cpp
                                            Win/Fifo.cpp)

    SET(CMCORE_PLATFORM_MODULE_SOURCES      Win/ExternalModule.cpp)

    SET(CMCORE_PLATFORM_DESKTOP_HEADERS     ${CMCORE_PLATFORM_INCLUDE_PATH}/DisplayMode.h
                                            ${CMCORE_PLATFORM_INCLUDE_PATH}/Desktop.h)

    SET(CMCORE_PLATFORM_DESKTOP_SOURCES     Win/Clipboard.cpp
                                            Win/Desktop.cpp
                                            Win/DisplayMode.cpp)

    SET(CMCORE_PLATFORM_WINDOW_HEADERS      ${CMCORE_PLATFORM_INCLUDE_PATH}/WinWindow.h
                                            ${CMCORE_PLATFORM_INCLUDE_PATH}/Window.h)

    SET(CMCORE_PLATFORM_WINDOW_SOURCES      Win/WinWindow.cpp
                                            Win/WinMessage.cpp
                                            Win/EnumWinFonts.cpp)
ENDIF(WIN32)

SET(CMCORE_PLATFORM_WINDOW_SOURCES ${CMCORE_PLATFORM_WINDOW_SOURCES} Window.cpp )

SOURCE_GROUP("Platform\\MMapFile\\Sources"      FILES ${CMCORE_PLATFORM_MMAP_SOURCES})
SOURCE_GROUP("Platform\\InputDevice\\Sources"   FILES ${CMCORE_PLATFORM_INPUTDEVICE_SOURCES})
SOURCE_GROUP("Platform\\Charset\\Sources"       FILES ${CMCORE_PLATFORM_CHARSET_SOURCES})
SOURCE_GROUP("Platform\\File\\Sources"          FILES ${CMCORE_PLATFORM_FILE_SOURCES})
SOURCE_GROUP("Platform\\SystemInfo\\Sources"    FILES ${CMCORE_PLATFORM_SYSTEMINFO_SOURCES})
SOURCE_GROUP("Platform\\Application\\Sources"   FILES ${CMCORE_PLATFORM_APPLICATION_SOURCES})
SOURCE_GROUP("Platform\\Log\\Sources"           FILES ${CMCORE_PLATFORM_LOG_SOURCES})
SOURCE_GROUP("Platform\\Thread\\Sources"        FILES ${CMCORE_PLATFORM_THREAD_SOURCES})
SOURCE_GROUP("Platform\\Process\\Sources"       FILES ${CMCORE_PLATFORM_PROCESS_SOURCES})
SOURCE_GROUP("Platform\\Time\\Sources"          FILES ${CMCORE_PLATFORM_TIME_SOURCES})
SOURCE_GROUP("Platform\\Desktop\\Headers"       FILES ${CMCORE_PLATFORM_DESKTOP_HEADERS})
SOURCE_GROUP("Platform\\Desktop\\Sources"       FILES ${CMCORE_PLATFORM_DESKTOP_SOURCES})
SOURCE_GROUP("Platform\\Window\\Headers"        FILES ${CMCORE_PLATFORM_WINDOW_HEADERS})
SOURCE_GROUP("Platform\\Window\\Sources"        FILES ${CMCORE_PLATFORM_WINDOW_SOURCES})

SOURCE_GROUP("Hardware\\CpuInfo\\Headers"       FILES ${CMCORE_PLATFORM_CPUINFO_HEADERS})
SOURCE_GROUP("Hardware\\CpuInfo\\Sources"       FILES ${CMCORE_PLATFORM_CPUINFO_SOURCES})
SOURCE_GROUP("Hardware\\Affinity\\Sources"      FILES ${CMCORE_PLATFORM_AFFINITY_SOURCES})

## Android 安卓平台
SET(CMCORE_PLATFORM_ANDROID_HEADERS ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform/Android.h)
SET(CMCORE_PLATFORM_ANDROID_SOURCES Android/AndroidVersion.cpp)

SOURCE_GROUP("Android\\Headers" FILES ${CMCORE_PLATFORM_ANDROID_HEADERS})
SOURCE_GROUP("Android\\Sources" FILES ${CMCORE_PLATFORM_ANDROID_SOURCES})

OPTION(CM_PLATFORM_SOC OFF)

IF(CM_PLATFORM_SOC)

    SET(CMCORE_PLATFORM_SOC_HEADERS ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform/SOC.h)
    SET(CMCORE_PLATFORM_SOC_SOURCES SOC/SOC.cpp
                                    SOC/ARMCpuInfo.cpp)

    SOURCE_GROUP("SOC\\Headers" FILES ${CMCORE_PLATFORM_SOC_HEADERS})
    SOURCE_GROUP("SOC\\Sources" FILES ${CMCORE_PLATFORM_SOC_SOURCES})

ENDIF(CM_PLATFORM_SOC)

SET(CMCORE_PLATFORM_ALL_SOURCES ${CMCORE_PLATFORM_ALL_SOURCES}
                                ${CMCORE_PLATFORM_FILE_SOURCES}
                                ${CMCORE_PLATFORM_MMAP_SOURCES}
                                ${CMCORE_PLATFORM_CHARSET_SOURCES}
                                ${CMCORE_PLATFORM_SYSTEMINFO_SOURCES}
                                ${CMCORE_PLATFORM_APPLICATION_SOURCES}
                                ${CMCORE_PLATFORM_LOG_SOURCES}
                                ${CMCORE_PLATFORM_THREAD_SOURCES}
                                ${CMCORE_PLATFORM_PROCESS_SOURCES}
                                ${CMCORE_PLATFORM_MODULE_SOURCES}
                                ${CMCORE_PLATFORM_INPUTDEVICE_SOURCES}
                                ${CMCORE_PLATFORM_WINDOW_HEADERS}
                                ${CMCORE_PLATFORM_WINDOW_SOURCES}

                                #${CMCORE_PLATFORM_DESKTOP_HEADERS}
                                #${CMCORE_PLATFORM_DESKTOP_SOURCES}

                                ${CMCORE_PLATFORM_CPUINFO_HEADERS}
                                ${CMCORE_PLATFORM_CPUINFO_SOURCES}
                                ${CMCORE_PLATFORM_AFFINITY_SOURCES}
                                ${CMCORE_PLATFORM_ANDROID_HEADERS}
                                ${CMCORE_PLATFORM_ANDROID_SOURCES}
                                ${CMCORE_PLATFORM_SOC_HEADERS}
                                ${CMCORE_PLATFORM_SOC_SOURCES}
)

add_cm_library(CMPlatform "CM" ${CMCORE_PLATFORM_ALL_SOURCES})

target_link_libraries(CMCore PUBLIC CMPlatform)
