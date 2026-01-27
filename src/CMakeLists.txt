#==================================================================================================
## DataType 数据类型基础
##==================================================================================================
SET(CMCORE_TYPE_INCLUDE_PATH   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/type)

## TypeInfo 类型信息
SET(CMCORE_TYPE_INFO_FILES ${CMCORE_TYPE_INCLUDE_PATH}/TypeInfo.h)
SOURCE_GROUP("DataType\\TypeInfo" FILES ${CMCORE_TYPE_INFO_FILES})

## BitsArray 位数组
SET(CMCORE_TYPE_BITSARRAY_FILES ${CMCORE_TYPE_INCLUDE_PATH}/BitsArray.h
                                Type/BitsArray.cpp)
SOURCE_GROUP("DataType\\BitsArray" FILES ${CMCORE_TYPE_BITSARRAY_FILES})

## Object 对象基类
SET(CMCORE_TYPE_OBJECT_FILES ${CMCORE_TYPE_INCLUDE_PATH}/object/ObjectBaseInfo.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/object/Object.h
                             Object/ObjectBaseInfo.cpp
                             Object/Object.cpp
                             Object/TickObject.cpp)
SOURCE_GROUP("DataType\\Object" FILES ${CMCORE_TYPE_OBJECT_FILES})

## IDName 标识名称
SET(CMCORE_TYPE_IDNAME_FILES ${CMCORE_TYPE_INCLUDE_PATH}/ConstStringSet.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/IDName.h
                             Text/ConstStringSetSaveToTextStream.cpp)
SOURCE_GROUP("DataType\\IDName" FILES ${CMCORE_TYPE_IDNAME_FILES})

## Collection 集合
SET(CMCORE_TYPE_COLLECTION_FILES ${CMCORE_TYPE_INCLUDE_PATH}/Collection.h
                                 ${CMCORE_TYPE_INCLUDE_PATH}/TypedCollection.h
                                 Type/Collection.cpp)
SOURCE_GROUP("DataType\\Collection" FILES ${CMCORE_TYPE_COLLECTION_FILES})

## BlockAllocator 数据链
SET(CMCORE_TYPE_DATACHAIN_FILES ${CMCORE_TYPE_INCLUDE_PATH}/BlockAllocator.h
                                Type/BlockAllocator.cpp)
SOURCE_GROUP("DataType\\BlockAllocator" FILES ${CMCORE_TYPE_DATACHAIN_FILES})

## ActiveManager 活跃管理器
SET(CMCORE_TYPE_ACTIVEMANAGER_FILES ${CMCORE_TYPE_INCLUDE_PATH}/ActiveIDManager.h
                                    ${CMCORE_TYPE_INCLUDE_PATH}/ActiveMemoryBlockManager.h
                                    ${CMCORE_TYPE_INCLUDE_PATH}/ActiveDataManager.h
                                    Type/ActiveIDManager.cpp
                                    Type/ActiveMemoryBlockManager.cpp)
SOURCE_GROUP("DataType\\ActiveManager" FILES ${CMCORE_TYPE_ACTIVEMANAGER_FILES})

## Template 模板类 - 智能指针和引用管理
SET(CMCORE_TYPE_SMARTPTR_FILES ${CMCORE_TYPE_INCLUDE_PATH}/Smart.h
                               ${CMCORE_TYPE_INCLUDE_PATH}/RefObjectPool.h)
SOURCE_GROUP("DataType\\Template\\SmartPtr" FILES ${CMCORE_TYPE_SMARTPTR_FILES})

## Template 模板类 - 内存管理
SET(CMCORE_TYPE_MEMORY_MGR_FILES ${CMCORE_TYPE_INCLUDE_PATH}/AccumMemoryManager.h
                                 ${CMCORE_TYPE_INCLUDE_PATH}/ActiveDataManager.h
                                 ${CMCORE_TYPE_INCLUDE_PATH}/ActiveIDManager.h
                                 ${CMCORE_TYPE_INCLUDE_PATH}/ActiveMemoryBlockManager.h
                                 ${CMCORE_TYPE_INCLUDE_PATH}/ActiveObjectManager.h
                                 ${CMCORE_TYPE_INCLUDE_PATH}/ObjectManager.h)
SOURCE_GROUP("DataType\\Template\\MemoryMgr" FILES ${CMCORE_TYPE_MEMORY_MGR_FILES})

## Template 模板类 - 容器集合
SET(CMCORE_TYPE_CONTAINER_FILES ${CMCORE_TYPE_INCLUDE_PATH}/BitsArray.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/BlockAllocator.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/Collection.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/TypedCollection.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/PtrArray.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/Queue.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/Stack.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/Pool.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/SimpleValuePool.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/SeriesPool.h
                                ${CMCORE_TYPE_INCLUDE_PATH}/ManagedArray.h)
SOURCE_GROUP("DataType\\Template\\Container" FILES ${CMCORE_TYPE_CONTAINER_FILES})

## Template 模板类 - 映射关键字
SET(CMCORE_TYPE_MAP_FILES ${CMCORE_TYPE_INCLUDE_PATH}/Map.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/OrderedMap.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/UnorderedMap.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/HashIDMap.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/KeyValue.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/Pair.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/OrderedManagedSet.h
                          ${CMCORE_TYPE_INCLUDE_PATH}/UnorderedManagedSet.h)
SOURCE_GROUP("DataType\\Template\\Map" FILES ${CMCORE_TYPE_MAP_FILES})

## Template 模板类 - 基本类型
SET(CMCORE_TYPE_BASIC_FILES ${CMCORE_TYPE_INCLUDE_PATH}/Extent.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/Size2.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/Gradient.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/RectScope.h)
SOURCE_GROUP("DataType\\Template\\Basic" FILES ${CMCORE_TYPE_BASIC_FILES})

## Template 模板类 - 字符串处理
SET(CMCORE_TYPE_STRING_FILES ${CMCORE_TYPE_INCLUDE_PATH}/String.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StringView.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StringViewList.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StringList.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/MergeString.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StdString.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/SplitString.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/ConstStringSet.h)
SOURCE_GROUP("DataType\\Template\\String" FILES ${CMCORE_TYPE_STRING_FILES})

## Template 模板类 - 索引与ID管理
SET(CMCORE_TYPE_INDEX_FILES ${CMCORE_TYPE_INCLUDE_PATH}/IDName.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/MonotonicIDList.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/IndexedManagedArray.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/IndexedValueArray.h)
SOURCE_GROUP("DataType\\Template\\Index" FILES ${CMCORE_TYPE_INDEX_FILES})

## Template 模板类 - 缓存与优化
SET(CMCORE_TYPE_CACHE_FILES ${CMCORE_TYPE_INCLUDE_PATH}/LRUCache.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/FlatTree.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/OrderedValueSet.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/UnorderedValueSet.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/ValueArray.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/ValueBuffer.h
                            ${CMCORE_TYPE_INCLUDE_PATH}/ValueKVMap.h)
SOURCE_GROUP("DataType\\Template\\Cache" FILES ${CMCORE_TYPE_CACHE_FILES})

## Template 模板类 - 核心类型
SET(CMCORE_TYPE_CORE_FILES ${CMCORE_TYPE_INCLUDE_PATH}/Inherit.h
                           ${CMCORE_TYPE_INCLUDE_PATH}/TypeInfo.h
                           ${CMCORE_TYPE_INCLUDE_PATH}/DataType.h
                           ${CMCORE_TYPE_INCLUDE_PATH}/VersionData.h
                           ${CMCORE_TYPE_INCLUDE_PATH}/TickObject.h
                           ${CMCORE_TYPE_INCLUDE_PATH}/UnicodeBlocks.h
                           ${CMCORE_TYPE_INCLUDE_PATH}/MemoryAllocator.h)
SOURCE_GROUP("DataType\\Template\\Core" FILES ${CMCORE_TYPE_CORE_FILES})

## Memory 内存管理
SET(CMCORE_TYPE_MEMORY_FILES ${CMCORE_TYPE_INCLUDE_PATH}/MemoryBlock.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/MemoryAllocator.h
                             Other/MemoryBlock.cpp
                             Other/MemoryAllocator.cpp)
SOURCE_GROUP("DataType\\Memory" FILES ${CMCORE_TYPE_MEMORY_FILES})

##==================================================================================================
## String 字符串类
##==================================================================================================
SET(CMCORE_TEXT_STRING_FILES ${CMCORE_TYPE_INCLUDE_PATH}/String.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StringView.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StringViewList.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StringList.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/SplitString.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/MergeString.h
                             ${CMCORE_TYPE_INCLUDE_PATH}/StdString.h)
SOURCE_GROUP("Text\\String" FILES ${CMCORE_TEXT_STRING_FILES})

## Text 文本编码
SET(CMCORE_TEXT_ENCODING_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Endian.h
                               ${CMCORE_ROOT_INCLUDE_PATH}/hgl/utf.h
                               ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Charset.h
                               ${CMCORE_ROOT_INCLUDE_PATH}/hgl/text/FontInfo.h
                               ${CMCORE_TYPE_INCLUDE_PATH}/UnicodeBlocks.h
                               Text/Endian.cpp
                               Text/utf.cpp
                               Text/CodePage.cpp
                               Text/UnicodeBlocks.cpp)
SOURCE_GROUP("Text\\Encoding" FILES ${CMCORE_TEXT_ENCODING_FILES})

##==================================================================================================
## IO 输入输出
##==================================================================================================
SET(CMCORE_IO_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io)

## IO Base 基础
SET(CMCORE_IO_BASE_FILES ${CMCORE_IO_INCLUDE_PATH}/InputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/IOType.h
                         ${CMCORE_IO_INCLUDE_PATH}/OutputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/SeekAccess.h
                         ${CMCORE_IO_INCLUDE_PATH}/MMapFile.h
                         IO/IOType.cpp)
SOURCE_GROUP("IO\\Base" FILES ${CMCORE_IO_BASE_FILES})

## IO DataStream 数据流
SET(CMCORE_IO_DATASTREAM_FILES ${CMCORE_IO_INCLUDE_PATH}/DataInputStream.h
                               ${CMCORE_IO_INCLUDE_PATH}/DataOutputStream.h
                               ${CMCORE_IO_INCLUDE_PATH}/EndianDataInputStream.h
                               ${CMCORE_IO_INCLUDE_PATH}/EndianDataOutputStream.h
                               IO/InputStream.cpp
                               IO/DataInputStream.cpp
                               IO/DataOutputStream.cpp)
SOURCE_GROUP("IO\\DataStream" FILES ${CMCORE_IO_DATASTREAM_FILES})

## IO MemoryStream 内存流
SET(CMCORE_IO_MEMORYSTREAM_FILES ${CMCORE_IO_INCLUDE_PATH}/MemoryInputStream.h
                                 ${CMCORE_IO_INCLUDE_PATH}/MemoryOutputStream.h)
SOURCE_GROUP("IO\\MemoryStream" FILES ${CMCORE_IO_MEMORYSTREAM_FILES})

## IO ValueBuffer 数据数组
SET(CMCORE_IO_DATAARRAY_FILES ${CMCORE_IO_INCLUDE_PATH}/LoadDataArray.h
                              ${CMCORE_IO_INCLUDE_PATH}/SaveDataArray.h)
SOURCE_GROUP("IO\\ValueBuffer" FILES ${CMCORE_IO_DATAARRAY_FILES})

## IO File 文件访问
SET(CMCORE_IO_FILE_FILES ${CMCORE_IO_INCLUDE_PATH}/FileAccess.h
                         ${CMCORE_IO_INCLUDE_PATH}/FileInputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/FileOutputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/RandomAccessFile.h
                         IO/FileAccess.cpp
                         IO/FileInputStream.cpp
                         IO/FileOutputStream.cpp
                         IO/RandomAccessFile.cpp)
SOURCE_GROUP("IO\\File" FILES ${CMCORE_IO_FILE_FILES})

## IO Java Java风格IO
SET(CMCORE_IO_JAVA_FILES ${CMCORE_IO_INCLUDE_PATH}/JavaInputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/JavaOutputStream.h)
SOURCE_GROUP("IO\\Java" FILES ${CMCORE_IO_JAVA_FILES})

## IO Text 文本IO
SET(CMCORE_IO_TEXT_FILES ${CMCORE_IO_INCLUDE_PATH}/TextInputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/TextOutputStream.h
                         ${CMCORE_IO_INCLUDE_PATH}/LoadStringList.h
                         ${CMCORE_IO_INCLUDE_PATH}/SaveStringList.h
                         IO/TextOutputStream.cpp
                         IO/TextInputStream.cpp
                         Text/LoadString.cpp
                         Text/LoadStringList.cpp
                         Text/LoadStringViewList.cpp)
SOURCE_GROUP("IO\\Text" FILES ${CMCORE_IO_TEXT_FILES})

## IO MiniPack 迷你打包
SET(CMCORE_IO_MINIPACK_FILES ${CMCORE_IO_INCLUDE_PATH}/MiniPack.h
                             IO/MiniPackInfoBlock.h
                             IO/MiniPackInfoBlock.cpp
                             IO/MiniPackReader.cpp
                             IO/MiniPackMemory.cpp)
SOURCE_GROUP("IO\\MiniPack" FILES ${CMCORE_IO_MINIPACK_FILES})

## IO Event 输入事件
SET(CMCORE_IO_EVENT_FILES ${CMCORE_IO_INCLUDE_PATH}/event/KeyboardEvent.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/MouseEvent.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/JoystickEvent.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/PointerEvent.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/TouchEvent.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/GestureEvent.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/EventDispatcher.h
                          ${CMCORE_IO_INCLUDE_PATH}/event/InputMapping.h
                          IO/Event/MouseEvent.cpp
                          IO/Event/PointerEvent.cpp
                          IO/Event/TouchEvent.cpp
                          IO/Event/GestureEvent.cpp
                          IO/Event/EventDispatcher.cpp)
SOURCE_GROUP("IO\\Event" FILES ${CMCORE_IO_EVENT_FILES})

## IO Pipe 管道
SET(CMCORE_IO_PIPE_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/proc/Pipe.h
                         ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io/PipeStream.h
                         IO/PipeStream.cpp)
SOURCE_GROUP("IO\\Pipe" FILES ${CMCORE_IO_PIPE_FILES})

## IO Fifo 先进先出
SET(CMCORE_IO_FIFO_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/proc/Fifo.h
                         ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io/FifoStream.h
                         IO/FifoStream.cpp)
SOURCE_GROUP("IO\\Fifo" FILES ${CMCORE_IO_FIFO_FILES})

## IO 汇总
SET(CMCORE_IO_ALL_FILES ${CMCORE_IO_BASE_FILES}
                        ${CMCORE_IO_DATASTREAM_FILES}
                        ${CMCORE_IO_DATAARRAY_FILES}
                        ${CMCORE_IO_MEMORYSTREAM_FILES}
                        ${CMCORE_IO_FILE_FILES}
                        ${CMCORE_IO_JAVA_FILES}
                        ${CMCORE_IO_TEXT_FILES}
                        ${CMCORE_IO_EVENT_FILES}
                        ${CMCORE_IO_PIPE_FILES}
                        ${CMCORE_IO_FIFO_FILES}
                        ${CMCORE_IO_MINIPACK_FILES})

##==================================================================================================
## FileSystem 文件系统
##==================================================================================================
SET(CMCORE_FILESYSTEM_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/filesystem)

SET(CMCORE_FILESYSTEM_FILES ${CMCORE_FILESYSTEM_INCLUDE_PATH}/EnumFile.h
                            ${CMCORE_FILESYSTEM_INCLUDE_PATH}/EnumVolume.h
                            ${CMCORE_FILESYSTEM_INCLUDE_PATH}/FileSystem.h
                            ${CMCORE_FILESYSTEM_INCLUDE_PATH}/Filename.h
                            ${CMCORE_FILESYSTEM_INCLUDE_PATH}/Path.h
                            ${CMCORE_FILESYSTEM_INCLUDE_PATH}/CollectFiles.h
                            FileSystem/FileSystem.cpp
                            FileSystem/EnumFile.cpp
                            FileSystem/Path.cpp
                            FileSystem/CollectFiles.cpp)
SOURCE_GROUP("FileSystem" FILES ${CMCORE_FILESYSTEM_FILES})

##==================================================================================================
## Time 时间
##==================================================================================================
SET(CMCORE_TIME_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/time)

SET(CMCORE_TIME_FILES ${CMCORE_TIME_INCLUDE_PATH}/DateTime.h
                      ${CMCORE_TIME_INCLUDE_PATH}/Time.h
                      ${CMCORE_TIME_INCLUDE_PATH}/TimeID.h
                      ${CMCORE_TIME_INCLUDE_PATH}/Timestamp.h
                      ${CMCORE_TIME_INCLUDE_PATH}/ChineseCalendar.h
                      Time/DateTime.cpp
                      Time/TimeCount.cpp
                      Time/TimeVal.cpp
                      Time/TimeOfDay.cpp
                      Time/CalendarDate.cpp
                      Time/TimeID.cpp
                      Time/Timestamp.cpp
                      Time/TimestampClass.cpp
                      Time/ChineseCalendar.cpp)
SOURCE_GROUP("Time" FILES ${CMCORE_TIME_FILES})

##==================================================================================================
## Thread 线程
##==================================================================================================
SET(CMCORE_THREAD_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/Atomic.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/CondVar.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/DataPost.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/Loader.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/RingBuffer.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/RWLock.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/Semaphore.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/SemLock.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/SwapColl.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/SwapData.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/Thread.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/ThreadMutex.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/Workflow.h
                        Other/ThreadFunc.cpp)
SOURCE_GROUP("Thread" FILES ${CMCORE_THREAD_FILES})

##==================================================================================================
## Event 事件
##==================================================================================================
SET(CMCORE_EVENT_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/event/ASyncEvent.h
                       ${CMCORE_ROOT_INCLUDE_PATH}/hgl/event/EventProc.h)
SOURCE_GROUP("Event" FILES ${CMCORE_EVENT_FILES})

##==================================================================================================
## PlugIn 插件
##==================================================================================================
SET(CMCORE_PLUGIN_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/ExternalPlugIn.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/PlugIn.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/PlugInInterface.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/PlugInManager.h
                        PlugIn/ExternalModule.cpp
                        PlugIn/ExternalPlugIn.cpp
                        PlugIn/PlugInManager.cpp)
SOURCE_GROUP("PlugIn" FILES ${CMCORE_PLUGIN_FILES})

##==================================================================================================
## Log 日志
##==================================================================================================
SET(CMCORE_LOG_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/Log.h
                     ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/Logger.h
                     ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/LogLevel.h
                     ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/LogMessage.h
                     Log/LogFile.cpp
                     Log/Loginfo.cpp
                     Log/ObjectLogger.cpp)
SOURCE_GROUP("Log" FILES ${CMCORE_LOG_FILES})

##==================================================================================================
## Library 库定义
##==================================================================================================
add_cm_library(CMCore "CM"
#                        ${SYSTEM_INFO_SOURCE}

                        ${CMCORE_TYPE_INFO_FILES}
                        ${CMCORE_TYPE_OBJECT_FILES}
                        ${CMCORE_TYPE_BITSARRAY_FILES}
                        ${CMCORE_TEXT_STRING_FILES}
                        ${CMCORE_TEXT_ENCODING_FILES}
                        ${CMCORE_TYPE_ACTIVEMANAGER_FILES}
                        ${CMCORE_TYPE_SMARTPTR_FILES}
                        ${CMCORE_TYPE_MEMORY_MGR_FILES}
                        ${CMCORE_TYPE_CONTAINER_FILES}
                        ${CMCORE_TYPE_MAP_FILES}
                        ${CMCORE_TYPE_BASIC_FILES}
                        ${CMCORE_TYPE_STRING_FILES}
                        ${CMCORE_TYPE_INDEX_FILES}
                        ${CMCORE_TYPE_CACHE_FILES}
                        ${CMCORE_TYPE_CORE_FILES}
                        ${CMCORE_TYPE_COLLECTION_FILES}
                        ${CMCORE_TYPE_DATACHAIN_FILES}
                        ${CMCORE_TYPE_MEMORY_FILES}
                        ${CMCORE_IO_ALL_FILES}
                        ${CMCORE_TYPE_IDNAME_FILES}
                        ${CMCORE_FILESYSTEM_FILES}
                        ${CMCORE_TIME_FILES}
                        ${CMCORE_THREAD_FILES}
                        ${CMCORE_EVENT_FILES}
                        ${CMCORE_PLUGIN_FILES}
                        ${CMCORE_LOG_FILES}
                        )

target_link_libraries(CMCore PRIVATE CMCoreType)

##==================================================================================================
## Platform 平台相关（从CMPlatform合并）
##==================================================================================================
add_subdirectory(Platform)
