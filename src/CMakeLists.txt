SET(TYPE_INCLUDE_PATH   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/type)

file(GLOB TYPE_TEMPLATE_HEADER ${TYPE_INCLUDE_PATH}/*.h)

SET(SYSTEM_INFO_SOURCE   SystemInfo.cpp)

##Color--------------------------------------------------------
SET(COLOR_HEADER_FILES  ${TYPE_INCLUDE_PATH}/Color.h
                        ${TYPE_INCLUDE_PATH}/Color3f.h
                        ${TYPE_INCLUDE_PATH}/Color4f.h)

SET(COLOR_SOURCE_FILES  Color/Color.cpp
                        Color/Color3f.cpp
                        Color/Color4f.cpp)

SOURCE_GROUP("Datatype\\Color\\Header Files" FILES ${COLOR_HEADER_FILES})
SOURCE_GROUP("Datatype\\Color\\Source Files" FILES ${COLOR_SOURCE_FILES})

##Text--------------------------------------------------------
SET(STRING_HEADER_FILES ${TYPE_INCLUDE_PATH}/String.h
                        ${TYPE_INCLUDE_PATH}/StringInstance.h
                        ${TYPE_INCLUDE_PATH}/StringList.h
                        ${TYPE_INCLUDE_PATH}/StdString.h)

SET(TEXT_HEADER_FILES   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Endian.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/CodePage.h
                        ${TYPE_INCLUDE_PATH}/UnicodeBlocks.h
                        ${TYPE_INCLUDE_PATH}/StrChar.h)

SET(TEXT_SOURCE_FILES   Text/Endian.cpp
                        Text/CodePage.cpp
                        Text/UnicodeBlocks.cpp
                        Text/StringList.cpp)

SOURCE_GROUP("Text\\String" FILES   ${STRING_HEADER_FILES}
                                    Text/StringList.cpp)

SOURCE_GROUP("Text\\Header Files" FILES ${TEXT_HEADER_FILES})
SOURCE_GROUP("Text\\Source Files" FILES ${TEXT_SOURCE_FILES})

SOURCE_GROUP("Datatype\\Template" FILES ${TYPE_TEMPLATE_HEADER})

##I/O------------------------------------------------------------
SET(IO_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io)

SET(IO_BASE_FILES   ${IO_INCLUDE_PATH}/InputStream.h
                    ${IO_INCLUDE_PATH}/IOType.h
                    ${IO_INCLUDE_PATH}/OutputStream.h
                    ${IO_INCLUDE_PATH}/SeekAccess.h
                    IO/IOType.cpp)

SET(IO_DATA_FILES   ${IO_INCLUDE_PATH}/DataInputStream.h
                    ${IO_INCLUDE_PATH}/DataOutputStream.h
                    ${IO_INCLUDE_PATH}/EndianDataInputStream.h
                    ${IO_INCLUDE_PATH}/EndianDataOutputStream.h
                    IO/InputStream.cpp
                    IO/DataInputStream.cpp
                    IO/DataOutputStream.cpp)

SET(IO_MEMORY_FILES ${IO_INCLUDE_PATH}/MemoryInputStream.h
                    ${IO_INCLUDE_PATH}/MemoryOutputStream.h)

SET(IO_FILE_FILES   ${IO_INCLUDE_PATH}/FileAccess.h
                    ${IO_INCLUDE_PATH}/FileInputStream.h
                    ${IO_INCLUDE_PATH}/FileOutputStream.h
                    ${IO_INCLUDE_PATH}/RandomAccessFile.h
                    ${IO_INCLUDE_PATH}/TextOutputStream.h
                    IO/FileAccess.cpp
                    IO/FileInputStream.cpp
                    IO/FileOutputStream.cpp
                    IO/RandomAccessFile.cpp)


SET(IO_JAVA_FILES   ${IO_INCLUDE_PATH}/JavaInputStream.h
                    ${IO_INCLUDE_PATH}/JavaOutputStream.h)

SET(IO_TEXT_FILES   IO/TextOutputStream.cpp)

SOURCE_GROUP("IO\\Base"    FILES ${IO_BASE_FILES})
SOURCE_GROUP("IO\\Data"    FILES ${IO_DATA_FILES})
SOURCE_GROUP("IO\\Memory"  FILES ${IO_MEMORY_FILES})
SOURCE_GROUP("IO\\File"    FILES ${IO_FILE_FILES})
SOURCE_GROUP("IO\\Jave"    FILES ${IO_JAVA_FILES})
SOURCE_GROUP("IO\\Text"    FILES ${IO_TEXT_FILES})

SET(IO_SOURCE_FILES ${IO_BASE_FILES}
                    ${IO_DATA_FILES}
                    ${IO_MEMORY_FILES}
                    ${IO_FILE_FILES}
                    ${IO_JAVA_FILES}
                    ${IO_TEXT_FILES})

SET(FILESYSTEM_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/filesystem)

SET(FILESYSTEM_HEADER_FILES ${FILESYSTEM_INCLUDE_PATH}/EnumFile.h
                            ${FILESYSTEM_INCLUDE_PATH}/EnumVolume.h
                            ${FILESYSTEM_INCLUDE_PATH}/FileSystem.h)

SET(FILESYSTEM_SOURCE_FILES FileSystem/FileSystem.cpp
                            FileSystem/EnumFile.cpp)

SOURCE_GROUP("FileSystem" FILES ${FILESYSTEM_HEADER_FILES} ${FILESYSTEM_SOURCE_FILES})

SET(BASE_TIME_HEADER_FILES  ${TYPE_INCLUDE_PATH}/DateTime.h
                            ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Time.h)

SET(BASE_TIME_SOURCE_FILES  Time/DateTime.cpp
                            Time/TimeCount.cpp
                            Time/TimeVal.cpp)

SOURCE_GROUP("Time\\Header Files" FILES ${BASE_TIME_HEADER_FILES})
SOURCE_GROUP("Time\\Source Files" FILES ${BASE_TIME_SOURCE_FILES})

SET(BASE_THREAD_SOURCE   Other/ThreadFunc.cpp)

SOURCE_GROUP("Thread" FILES ${BASE_OTHER_SOURCE})

file(GLOB BASE_PLUG_IN_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/*.h)
file(GLOB BASE_PLUG_IN_SOURCE PlugIn/*.cpp)

SOURCE_GROUP("PlugIn\\Header Files" FILES ${BASE_PLUG_IN_HEADER})
SOURCE_GROUP("PlugIn\\Source Files" FILES ${BASE_PLUG_IN_SOURCE})

file(GLOB BASE_LOG_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/*.h)
file(GLOB BASE_LOG_SOURCE Log/*.*)

SOURCE_GROUP("Log\\Header Files" FILES ${BASE_LOG_HEADER})
SOURCE_GROUP("Log\\Source Files" FILES ${BASE_LOG_SOURCE})

add_cm_library(CMCore "CM"  #${SYSTEM_INFO_SOURCE}

                            ${COLOR_HEADER_FILES}
                            ${COLOR_SOURCE_FILES}

                            ${STRING_HEADER_FILES}
                            ${TEXT_HEADER_FILES}
                            ${TEXT_SOURCE_FILES}

                            ${TYPE_TEMPLATE_HEADER}

                            ${IO_SOURCE_FILES}

                            ${FILESYSTEM_HEADER_FILES}
                            ${FILESYSTEM_SOURCE_FILES}

                            ${BASE_TIME_HEADER_FILES}
                            ${BASE_TIME_SOURCE_FILES}

                            ${BASE_THREAD_SOURCE}

                            ${BASE_PLUG_IN_HEADER}
                            ${BASE_PLUG_IN_SOURCE}

                            ${BASE_LOG_HEADER}
                            ${BASE_LOG_SOURCE}
                            )
