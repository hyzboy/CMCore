SET(PLATFORM_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform)

SET(CPU_INFO_HEADER ${PLATFORM_INCLUDE_PATH}/CpuInfo.h
                    ${PLATFORM_INCLUDE_PATH}/CpuX86Features.h
                    ${PLATFORM_INCLUDE_PATH}/CpuARMFeatures.h
                    ${PLATFORM_INCLUDE_PATH}/BigLittleDetector.h)

IF(UNIX)
    # 根据处理器架构选择不同的CPU信息实现
    IF(APPLE)
        IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i386|i686|amd64")
            SET(CPU_INFO_SOURCE             Apple/CpuInfo_X86.cpp)
        ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|armv7|arm")
            SET(CPU_INFO_SOURCE             Apple/CpuInfo_ARM.cpp)
        ELSE()
            SET(CPU_INFO_SOURCE             Apple/CpuInfo_X86.cpp)
            message(WARNING "Unknown Apple processor architecture ${CMAKE_SYSTEM_PROCESSOR}, using X86 CPU info")
        ENDIF()
    ELSE()
        IF(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|i386|i686|amd64")
            SET(CPU_INFO_SOURCE             UNIX/CpuInfo_X86.cpp)
        ELSEIF(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|armv7|arm")
            SET(CPU_INFO_SOURCE             UNIX/CpuInfo_ARM.cpp)
        ELSE()
            SET(CPU_INFO_SOURCE             UNIX/CpuInfo_X86.cpp)
            message(WARNING "Unknown processor architecture ${CMAKE_SYSTEM_PROCESSOR}, using X86 CPU info")
        ENDIF()
    ENDIF()

    SET(PLATFORM_MMAP_FILE_SOURCE       /MMapFile.cpp)

    SET(PLATFORM_FILE_SOURCE            UNIX/File.cpp
                                        UNIX/FileAccess.cpp
                                        UNIX/EnumFile.cpp)

    SET(PLATFORM_MULTI_THREAD_SOURCE    UNIX/CondVar.cpp
                                        UNIX/RWLock.cpp
                                        UNIX/Thread.cpp
                                        UNIX/ThreadMutex.cpp)

    IF(ANDROID)
        SET(PLATFORM_LOG_SOURCE             Android/LogConsole.cpp)

        SET(PLATFORM_APPLICATION_SOURCE     ${PLATFORM_APPLICATION_SOURCE}
                                            Android/JNISupport.cpp
                                            Android/NativeActivitySupport.cpp)

        SET(PLATFORM_FILE_SOURCE            ${PLATFORM_FILE_SOURCE}
                                            Android/ProgramPath.cpp
                                            Android/AssetManage.cpp)

        SET(PLATFORM_MULTI_THREAD_SOURCE    ${PLATFORM_MULTI_THREAD_SOURCE}
                                            UNIX/Semaphore.cpp)
    ELSE()
        SET(PLATFORM_CODEPAGE_SOURCE        UNIX/Charset.cpp)
        SET(PLATFORM_LOG_SOURCE             UNIX/LogConsole.cpp)

        IF(APPLE)
            SET(PLATFORM_MULTI_THREAD_SOURCE    ${PLATFORM_MULTI_THREAD_SOURCE}
                                                Apple/Semaphore.cpp)

            SET(PLATFORM_FILE_SOURCE            ${PLATFORM_FILE_SOURCE}
                                                Apple/ProgramPath.mm)
        ELSE()
            SET(PLATFORM_MULTI_THREAD_SOURCE    ${PLATFORM_MULTI_THREAD_SOURCE}
                                                UNIX/Semaphore.cpp)

            SET(PLATFORM_FILE_SOURCE            ${PLATFORM_FILE_SOURCE}
                                                UNIX/ProgramPath.cpp)

            SET(PLATFORM_WINDOW_SOURCE          UNIX/XCBWindow.cpp)
        ENDIF()
    ENDIF()

    SET(PLATFORM_MULTI_PROCESS_SOURCE   UNIX/ProcMutex.cpp
                                        UNIX/Process.cpp
                                        UNIX/Pipe.cpp
                                        UNIX/Fifo.cpp

                                        UNIX/Exit.cpp)

    SET(PLATFORM_EXTERNAL_MODULE_SOURCE UNIX/ExternalModule.cpp)

ENDIF()

IF(WIN32)
    # 包含两种实现：X86 和 ARM
    # 通过源文件内部的编译时宏来区分具体编译哪一个实现，避免在CMake中基于处理器字符串做选择。
    set(CPU_INFO_SOURCE                 Win/CpuInfo_X86.cpp
                                        Win/CpuInfo_ARM.cpp)

    SET(PLATFORM_MMAP_FILE_SOURCE       Win/MMapFile.cpp)

    SET(PLATFORM_FILE_SOURCE            Win/File.cpp
                                        Win/FileAccess.cpp
                                        Win/EnumFile.cpp
                                        Win/EnumVolume.cpp
                                        Win/ProgramPath.cpp)

    SET(PLATFORM_CODEPAGE_SOURCE        Win/CodePage.cpp
                                        Win/vsprintf.cpp)

    SET(PLATFORM_SYSTEM_INFO_SOURCE     Win/SystemInfo.cpp)

    SET(PLATFORM_LOG_SOURCE             Win/LogConsole.cpp
                                        Win/LogDialog.cpp)

    SET(PLATFORM_MULTI_THREAD_SOURCE
                                        Win/CondVar.cpp
                                        Win/RWLock.cpp
                                        Win/Semaphore.cpp
                                        Win/Thread.cpp
                                        Win/ThreadMutex.cpp)

    SET(PLATFORM_MULTI_PROCESS_SOURCE   Win/ProcMutex.cpp
                                        Win/Pipe.cpp
                                        Win/Fifo.cpp)

    SET(PLATFORM_EXTERNAL_MODULE_SOURCE Win/ExternalModule.cpp)

    SET(PLATFORM_DESKTOP_SOURCE         ${PLATFORM_INCLUDE_PATH}/DisplayMode.h
                                        ${PLATFORM_INCLUDE_PATH}/Desktop.h
                                        Win/Clipboard.cpp
                                        Win/Desktop.cpp
                                        Win/DisplayMode.cpp)

    SET(PLATFORM_WINDOW_SOURCE          ${PLATFORM_INCLUDE_PATH}/WinWindow.h
                                        ${PLATFORM_INCLUDE_PATH}/Window.h
                                        Win/WinWindow.cpp
                                        Win/WinMessage.cpp
                                        Win/EnumWinFonts.cpp)
ENDIF(WIN32)

SET(PLATFORM_WINDOW_SOURCE ${PLATFORM_WINDOW_SOURCE} Window.cpp )

SOURCE_GROUP("MMapFile"     FILES ${PLATFORM_MMAP_FILE_SOURCE})
SOURCE_GROUP("InputDevice"  FILES ${PLATFORM_INPUT_DEVICE_SOURCE})
SOURCE_GROUP("CodePage"     FILES ${PLATFORM_CODEPAGE_SOURCE})
SOURCE_GROUP("File"         FILES ${PLATFORM_FILE_SOURCE})
SOURCE_GROUP("SystemInfo"   FILES ${PLATFORM_SYSTEM_INFO_SOURCE})
SOURCE_GROUP("Application"  FILES ${PLATFORM_APPLICATION_SOURCE})
SOURCE_GROUP("LogInfo"      FILES ${PLATFORM_LOG_SOURCE})
SOURCE_GROUP("MultiThread"  FILES ${PLATFORM_MULTI_THREAD_SOURCE})
SOURCE_GROUP("MultiProcess" FILES ${PLATFORM_MULTI_PROCESS_SOURCE})
SOURCE_GROUP("Time"         FILES ${PLATFORM_TIME_SOURCE})
SOURCE_GROUP("Desktop"      FILES ${PLATFORM_DESKTOP_SOURCE})
SOURCE_GROUP("Window"       FILES ${PLATFORM_WINDOW_SOURCE})

SOURCE_GROUP("Hardware\\Cpuinfo" FILES ${CPU_INFO_HEADER} ${CPU_INFO_SOURCE})

SET(ANDROID_ABOUT_SOURCE    ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform/Android.h
                            Android/AndroidVersion.cpp)

SOURCE_GROUP("Android\\Version" FILES ${ANDROID_ABOUT_SOURCE})

OPTION(CM_PLATFORM_SOC OFF)

IF(CM_PLATFORM_SOC)

    SET(SOC_SOURCE  ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform/SOC.h
                    SOC/SOC.cpp
                    SOC/ARMCpuInfo.cpp
    )

    SOURCE_GROUP("SOC" FILES ${SOC_SOURCE})

ENDIF(CM_PLATFORM_SOC)

SET(PLATFORM_SOURCE ${PLATFORM_SOURCE}
                    ${PLATFORM_FILE_SOURCE}
                    ${PLATFORM_MMAP_FILE_SOURCE}
                    ${PLATFORM_CODEPAGE_SOURCE}
                    ${PLATFORM_SYSTEM_INFO_SOURCE}
                    ${PLATFORM_APPLICATION_SOURCE}
                    ${PLATFORM_LOG_SOURCE}
                    ${PLATFORM_MULTI_THREAD_SOURCE}
                    ${PLATFORM_MULTI_PROCESS_SOURCE}
                    ${PLATFORM_EXTERNAL_MODULE_SOURCE}
                    ${PLATFORM_INPUT_DEVICE_SOURCE}
                    ${PLATFORM_WINDOW_SOURCE}

                    #${PLATFORM_DESKTOP_SOURCE}

                    ${CPU_INFO_HEADER}
                    ${CPU_INFO_SOURCE}
                    ${ANDROID_ABOUT_SOURCE}
                    ${SOC_SOURCE}
)

add_cm_library(CMPlatform "CM" ${PLATFORM_SOURCE})

target_link_libraries(CMCore PUBLIC CMPlatform)
