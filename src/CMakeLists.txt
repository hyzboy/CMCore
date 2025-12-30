#==================================================================================================
## DataType 数据类型基础
##==================================================================================================
SET(TYPE_INCLUDE_PATH   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/type)

SET(TYPE_INFO_FILE ${TYPE_INCLUDE_PATH}/TypeInfo.h)

SOURCE_GROUP("DataType" FILES ${TYPE_INFO_FILE})

## BitsArray 位数组
SET(BITS_ARRAY_FILES ${TYPE_INCLUDE_PATH}/BitsArray.h
                    Type/BitsArray.cpp)

SOURCE_GROUP("DataType\\BitsArray" FILES ${BITS_ARRAY_FILES})

## Object 对象基类
SET(BASE_OBJECT_HEADER_FILES    ${TYPE_INCLUDE_PATH}/object/ObjectBaseInfo.h
                                ${TYPE_INCLUDE_PATH}/object/Object.h)

SET(BASE_OBJECT_SOURCE_FILES    Object/ObjectBaseInfo.cpp
                                Object/Object.cpp
                                Object/TickObject.cpp)

SOURCE_GROUP("DataType\\Object" FILES ${BASE_OBJECT_HEADER_FILES}
                                      ${BASE_OBJECT_SOURCE_FILES})

## IDName 标识名称
SET(TYPE_IDNAME_HEADER_FILES    ${TYPE_INCLUDE_PATH}/ConstStringSet.h
                                ${TYPE_INCLUDE_PATH}/IDName.h)

SET(TYPE_IDNAME_SOURCE_FILES    Text/ConstStringSetSaveToTextStream.cpp)

SOURCE_GROUP("DataType\\IDName" FILES ${TYPE_IDNAME_HEADER_FILES} ${TYPE_IDNAME_SOURCE_FILES})

## Collection 集合
SET(TYPE_COLLECTION_SOURCE  ${TYPE_INCLUDE_PATH}/Collection.h
                            ${TYPE_INCLUDE_PATH}/ElementCollection.h
                            Type/Collection.cpp)

SOURCE_GROUP("DataType\\Collection" FILES ${TYPE_COLLECTION_SOURCE})

## DataChain 数据链
SET(TYPE_DATA_CHAIN_SOURCE  ${TYPE_INCLUDE_PATH}/DataChain.h
                            Type/DataChain.cpp)

SOURCE_GROUP("DataType\\DataChain" FILES ${TYPE_DATA_CHAIN_SOURCE})

## ActiveManager 活跃管理器
SET(ACTIVE_MANAGER_FILES    ${TYPE_INCLUDE_PATH}/ActiveIDManager.h
                            ${TYPE_INCLUDE_PATH}/ActiveMemoryBlockManager.h
                            ${TYPE_INCLUDE_PATH}/ActiveDataManager.h
                            Type/ActiveIDManager.cpp
                            Type/ActiveMemoryBlockManager.cpp)

SOURCE_GROUP("DataType\\ActiveManager" FILES ${ACTIVE_MANAGER_FILES})

## Template 模板类
file(GLOB TYPE_TEMPLATE_HEADER ${TYPE_INCLUDE_PATH}/*.h)

SOURCE_GROUP("DataType\\Template" FILES ${TYPE_TEMPLATE_HEADER})

## Memory 内存管理
SET(BASE_OTHER_SOURCE   ${TYPE_INCLUDE_PATH}/MemoryBlock.h
                        ${TYPE_INCLUDE_PATH}/MemoryAllocator.h
                        Other/MemoryBlock.cpp
                        Other/MemoryAllocator.cpp)

SOURCE_GROUP("DataType\\Memory" FILES ${BASE_OTHER_SOURCE})

##==================================================================================================
## Math 数学库
##==================================================================================================
SET(MATH_INCLUDE_PATH   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/math)

SET(MATH_HEADER_FILES   ${MATH_INCLUDE_PATH}/Transform.h
                        ${MATH_INCLUDE_PATH}/TransformAction.h)

SET(MATH_SOURCE_FILES   Math/Transform.cpp)

SOURCE_GROUP("Math\\Header Files" FILES ${MATH_HEADER_FILES})
SOURCE_GROUP("Math\\Source Files" FILES ${MATH_SOURCE_FILES})

##==================================================================================================
## String 字符串类
##==================================================================================================
SET(STRING_HEADER_FILES ${TYPE_INCLUDE_PATH}/String.h
                        ${TYPE_INCLUDE_PATH}/StringView.h
                        ${TYPE_INCLUDE_PATH}/StringViewList.h
                        ${TYPE_INCLUDE_PATH}/StringList.h
                        ${TYPE_INCLUDE_PATH}/SplitString.h
                        ${TYPE_INCLUDE_PATH}/MergeString.h
                        ${TYPE_INCLUDE_PATH}/StdString.h
)

SOURCE_GROUP("Text\\String" FILES   ${STRING_HEADER_FILES}
                                    Text/StringList.cpp)

## Text 文本编码
SET(TEXT_HEADER_FILES   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Endian.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/utf.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Charset.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/text/FontInfo.h
                        ${TYPE_INCLUDE_PATH}/UnicodeBlocks.h)

SET(TEXT_SOURCE_FILES   Text/Endian.cpp
                        Text/utf.cpp
                        Text/CodePage.cpp
                        Text/UnicodeBlocks.cpp)

SOURCE_GROUP("Text\\Header Files" FILES ${TEXT_HEADER_FILES})
SOURCE_GROUP("Text\\Source Files" FILES ${TEXT_SOURCE_FILES})

##==================================================================================================
## IO 输入输出
##==================================================================================================
SET(IO_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io)

## IO Base 基础
SET(IO_BASE_FILES ${IO_INCLUDE_PATH}/InputStream.h
                  ${IO_INCLUDE_PATH}/IOType.h
                  ${IO_INCLUDE_PATH}/OutputStream.h
                  ${IO_INCLUDE_PATH}/SeekAccess.h
                  ${IO_INCLUDE_PATH}/MMapFile.h
                  IO/IOType.cpp)

SOURCE_GROUP("IO\\Base" FILES ${IO_BASE_FILES})

## IO DataStream 数据流
SET(IO_DATA_FILES ${IO_INCLUDE_PATH}/DataInputStream.h
                  ${IO_INCLUDE_PATH}/DataOutputStream.h
                  ${IO_INCLUDE_PATH}/EndianDataInputStream.h
                  ${IO_INCLUDE_PATH}/EndianDataOutputStream.h
                  IO/InputStream.cpp
                  IO/DataInputStream.cpp
                  IO/DataOutputStream.cpp)

SOURCE_GROUP("IO\\DataIOStream" FILES ${IO_DATA_FILES})

## IO MemoryStream 内存流
SET(IO_MEMORY_STREAM_FILES  ${IO_INCLUDE_PATH}/MemoryInputStream.h
                            ${IO_INCLUDE_PATH}/MemoryOutputStream.h)

SOURCE_GROUP("IO\\MemoryStream" FILES ${IO_MEMORY_STREAM_FILES})

## IO DataArray 数据数组
SET(IO_DATA_ARRAY_FILES ${IO_INCLUDE_PATH}/LoadDataArray.h
                        ${IO_INCLUDE_PATH}/SaveDataArray.h)

SOURCE_GROUP("IO\\DataArray" FILES ${IO_DATA_ARRAY_FILES})

## IO File 文件访问
SET(IO_FILE_FILES ${IO_INCLUDE_PATH}/FileAccess.h
                  ${IO_INCLUDE_PATH}/FileInputStream.h
                  ${IO_INCLUDE_PATH}/FileOutputStream.h
                  ${IO_INCLUDE_PATH}/RandomAccessFile.h
                  IO/FileAccess.cpp
                  IO/FileInputStream.cpp
                  IO/FileOutputStream.cpp
                  IO/RandomAccessFile.cpp)

SOURCE_GROUP("IO\\File" FILES ${IO_FILE_FILES})

## IO Java Java风格IO
SET(IO_JAVA_FILES   ${IO_INCLUDE_PATH}/JavaInputStream.h
                    ${IO_INCLUDE_PATH}/JavaOutputStream.h)

SOURCE_GROUP("IO\\Java" FILES ${IO_JAVA_FILES})

## IO Text 文本IO
SET(IO_TEXT_FILES   IO/TextOutputStream.cpp
                    IO/TextInputStream.cpp
                    ${IO_INCLUDE_PATH}/TextInputStream.h
                    ${IO_INCLUDE_PATH}/TextOutputStream.h
                    Text/LoadString.cpp
                    Text/LoadStringList.cpp
                    Text/LoadStringViewList.cpp

                    ${IO_INCLUDE_PATH}/LoadStringList.h
                    ${IO_INCLUDE_PATH}/SaveStringList.h)

SOURCE_GROUP("IO\\Text" FILES ${IO_TEXT_FILES})

## IO MiniPack 迷你打包
SET(IO_MINI_PACK_FILES  ${IO_INCLUDE_PATH}/MiniPack.h
                        IO/MiniPackInfoBlock.h
                        IO/MiniPackInfoBlock.cpp
                        IO/MiniPackReader.cpp
                        IO/MiniPackMemory.cpp)

SOURCE_GROUP("IO\\MiniPack" FILES ${IO_MINI_PACK_FILES})

## IO Event 输入事件
SET(INPUT_EVENT_FILES   ${IO_INCLUDE_PATH}/event/KeyboardEvent.h
                        ${IO_INCLUDE_PATH}/event/MouseEvent.h
                        ${IO_INCLUDE_PATH}/event/JoystickEvent.h
                        ${IO_INCLUDE_PATH}/event/EventDispatcher.h
                        ${IO_INCLUDE_PATH}/event/InputMapping.h
                        IO/Event/MouseEvent.cpp
                        IO/Event/EventDispatcher.cpp
)

SOURCE_GROUP("IO\\Event" FILES ${INPUT_EVENT_FILES})

## IO Pipe 管道
SET(IO_PIPE_FILES   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/proc/Pipe.h
                    ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io/PipeStream.h
                    IO/PipeStream.cpp)

SOURCE_GROUP("IO\\Pipe" FILES ${IO_PIPE_FILES})

## IO Fifo 先进先出
SET(IO_FIFO_FILES   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/proc/Fifo.h
                    ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io/FifoStream.h
                    IO/FifoStream.cpp)

SOURCE_GROUP("IO\\Fifo" FILES ${IO_FIFO_FILES})

## IO 汇总
SET(IO_SOURCE_FILES ${IO_BASE_FILES}
                    ${IO_DATA_FILES}
                    ${IO_DATA_ARRAY_FILES}
                    ${IO_MEMORY_STREAM_FILES}
                    ${IO_FILE_FILES}
                    ${IO_JAVA_FILES}
                    ${IO_TEXT_FILES}
                    ${INPUT_EVENT_FILES}
                    ${IO_PIPE_FILES}
                    ${IO_FIFO_FILES}
                    ${IO_MINI_PACK_FILES})

##==================================================================================================
## FileSystem 文件系统
##==================================================================================================
SET(FILESYSTEM_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/filesystem)

SET(FILESYSTEM_HEADER_FILES ${FILESYSTEM_INCLUDE_PATH}/EnumFile.h
                            ${FILESYSTEM_INCLUDE_PATH}/EnumVolume.h
                            ${FILESYSTEM_INCLUDE_PATH}/FileSystem.h
                            ${FILESYSTEM_INCLUDE_PATH}/Filename.h
                            ${FILESYSTEM_INCLUDE_PATH}/Path.h
                            ${FILESYSTEM_INCLUDE_PATH}/CollectFiles.h
)

SET(FILESYSTEM_SOURCE_FILES FileSystem/FileSystem.cpp
                            FileSystem/EnumFile.cpp
                            FileSystem/Path.cpp
                            FileSystem/CollectFiles.cpp)

SOURCE_GROUP("FileSystem" FILES ${FILESYSTEM_HEADER_FILES} ${FILESYSTEM_SOURCE_FILES})

##==================================================================================================
## Time 时间
##==================================================================================================
SET(TIME_HEADER_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/time)

SET(BASE_TIME_HEADER_FILES  ${TIME_HEADER_PATH}/DateTime.h
                            ${TIME_HEADER_PATH}/Time.h
                            ${TIME_HEADER_PATH}/TimeID.h)

SET(BASE_TIME_SOURCE_FILES  Time/DateTime.cpp
                            Time/TimeCount.cpp
                            Time/TimeVal.cpp
                            Time/Time.cpp
                            Time/Date.cpp
                            Time/TimeID.cpp)

SOURCE_GROUP("Time\\Header Files" FILES ${BASE_TIME_HEADER_FILES})
SOURCE_GROUP("Time\\Source Files" FILES ${BASE_TIME_SOURCE_FILES})

##==================================================================================================
## Thread 线程
##==================================================================================================
FILE(GLOB BASE_THREAD_HEADER_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/*.h)
SET(BASE_THREAD_SOURCE_FILES   Other/ThreadFunc.cpp)

SOURCE_GROUP("Thread" FILES ${BASE_THREAD_HEADER_FILES}
                            ${BASE_THREAD_SOURCE_FILES})

##==================================================================================================
## Event 事件
##==================================================================================================
FILE(GLOB BASE_EVENT_HEADER_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/event/*.*)

SOURCE_GROUP("Event" FILES  ${BASE_EVENT_HEADER_FILES})

##==================================================================================================
## PlugIn 插件
##==================================================================================================
file(GLOB BASE_PLUG_IN_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/*.h)
file(GLOB BASE_PLUG_IN_SOURCE PlugIn/*.cpp)

SOURCE_GROUP("PlugIn\\Header Files" FILES ${BASE_PLUG_IN_HEADER})
SOURCE_GROUP("PlugIn\\Source Files" FILES ${BASE_PLUG_IN_SOURCE})

##==================================================================================================
## Log 日志
##==================================================================================================
file(GLOB BASE_LOG_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/*.h)
file(GLOB BASE_LOG_SOURCE Log/*.*)

SOURCE_GROUP("Log\\Header Files" FILES ${BASE_LOG_HEADER})
SOURCE_GROUP("Log\\Source Files" FILES ${BASE_LOG_SOURCE})

##==================================================================================================
## Library 库定义
##==================================================================================================
add_cm_library(CMCore "CM"
#                        ${SYSTEM_INFO_SOURCE}

                        ${TYPE_INFO_FILE}
                        ${BASE_OBJECT_HEADER_FILES}
                        ${BASE_OBJECT_SOURCE_FILES}

                        ${BITS_ARRAY_FILES}

                        ${STRING_HEADER_FILES}
                        ${TEXT_HEADER_FILES}
                        ${TEXT_SOURCE_FILES}

                        ${ACTIVE_MANAGER_FILES}

                        ${TYPE_TEMPLATE_HEADER}

                        ${TYPE_COLLECTION_SOURCE}
                        ${TYPE_DATA_CHAIN_SOURCE}

                        ${BASE_OTHER_SOURCE}

                        ${IO_SOURCE_FILES}

                        ${TYPE_IDNAME_HEADER_FILES}
                        ${TYPE_IDNAME_SOURCE_FILES}

                        ${MATH_HEADER_FILES}
                        ${MATH_SOURCE_FILES}

                        ${FILESYSTEM_HEADER_FILES}
                        ${FILESYSTEM_SOURCE_FILES}

                        ${BASE_TIME_HEADER_FILES}
                        ${BASE_TIME_SOURCE_FILES}
                        
                        ${BASE_THREAD_HEADER_FILES}
                        ${BASE_THREAD_SOURCE_FILES}
                        
                        ${BASE_EVENT_HEADER_FILES}

                        ${BASE_PLUG_IN_HEADER}
                        ${BASE_PLUG_IN_SOURCE}

                        ${BASE_LOG_HEADER}
                        ${BASE_LOG_SOURCE}
                        )

target_link_libraries(CMCore PRIVATE CMCoreType)

##==================================================================================================
## Platform 平台相关（从CMPlatform合并）
##==================================================================================================
add_subdirectory(Platform)
