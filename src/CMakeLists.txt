SET(CORE_PLATFORM_INCLUDE_PATH  ${CMCORE_ROOT_INCLUDE_PATH}/hgl/platform)

SET(CORE_PLATFORM_HEADER_FILES  ${CORE_PLATFORM_INCLUDE_PATH}/Platform.h)

SET(TYPE_INCLUDE_PATH   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/type)

FILE(GLOB CORE_PLATFORM_OS_FILES ${CORE_PLATFORM_INCLUDE_PATH}/os/*.h)
FILE(GLOB CORE_PLATFORM_COMPILER_FILES ${CORE_PLATFORM_INCLUDE_PATH}/compiler/*.h)

SOURCE_GROUP("Platform" FILES ${CORE_PLATFORM_HEADER_FILES})
SOURCE_GROUP("Platform\\OS" FILES ${CORE_PLATFORM_OS_FILES})
SOURCE_GROUP("Platform\\Compiler" FILES ${CORE_PLATFORM_COMPILER_FILES})

SET(CORE_PLATFORM_HEADER_FILES  ${CORE_PLATFORM_HEADER_FILES}
                              ${CORE_PLATFORM_OS_FILES}
                              ${CORE_PLATFORM_COMPILER_FILES})

SET(TYPE_INFO_FILE ${TYPE_INCLUDE_PATH}/TypeInfo.h)

SOURCE_GROUP("DataType" FILES ${TYPE_INFO_FILE})

SET(BITS_ARRAY_FILES ${TYPE_INCLUDE_PATH}/BitsArray.h
                    Type/BitsArray.cpp)

SOURCE_GROUP("DataType\\BitsArray" FILES ${BITS_ARRAY_FILES})

SET(TYPE_IDNAME_HEADER_FILES    ${TYPE_INCLUDE_PATH}/ConstStringSet.h
                                ${TYPE_INCLUDE_PATH}/IDName.h)

SET(TYPE_IDNAME_SOURCE_FILES    Text/ConstStringSetSaveToTextStream.cpp
                                Type/IDName.cpp)

SOURCE_GROUP("DataType\\IDName" FILES ${TYPE_IDNAME_HEADER_FILES} ${TYPE_IDNAME_SOURCE_FILES})

SET(TYPE_COLLECTION_SOURCE  ${TYPE_INCLUDE_PATH}/Collection.h
                            Type/Collection.cpp)
                            
SOURCE_GROUP("DataType\\Collection" FILES ${TYPE_COLLECTION_SOURCE})

SET(TYPE_DATA_CHAIN_SOURCE  ${TYPE_INCLUDE_PATH}/DataChain.h
                            Type/DataChain.cpp)

SOURCE_GROUP("DataType\\DataChain" FILES ${TYPE_DATA_CHAIN_SOURCE})

SET(TREE_NODE_FILES ${TYPE_INCLUDE_PATH}/Node.h
                    ${TYPE_INCLUDE_PATH}/NodeManager.h
                    ${TYPE_INCLUDE_PATH}/TreeNode.h
                    Type/Node.cpp
                    Type/NodeManager.cpp)

SOURCE_GROUP("DataType\\Node" FILES ${TREE_NODE_FILES})

SET(ACTIVE_MANAGER_FILES    ${TYPE_INCLUDE_PATH}/ActiveIDManager.h
                            ${TYPE_INCLUDE_PATH}/ActiveMemoryBlockManager.h
                            ${TYPE_INCLUDE_PATH}/ActiveDataManager.h
                            Type/ActiveIDManager.cpp
                            Type/ActiveMemoryBlockManager.cpp)

file(GLOB TYPE_TEMPLATE_HEADER ${TYPE_INCLUDE_PATH}/*.h)

SOURCE_GROUP("DataType\\ActiveManager" FILES ${ACTIVE_MANAGER_FILES})


SET(SYSTEM_INFO_SOURCE  ${CORE_PLATFORM_INCLUDE_PATH}/SystemInfo.h
                        SystemInfo.cpp)

SET(MATH_INCLUDE_PATH   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/math)

SET(BASE_OBJECT_HEADER_FILES    ${TYPE_INCLUDE_PATH}/object/ObjectBaseInfo.h
                                ${TYPE_INCLUDE_PATH}/object/Object.h)

SET(BASE_OBJECT_SOURCE_FILES    Object/ObjectBaseInfo.cpp
                                Object/Object.cpp
                                Object/TickObject.cpp)

SOURCE_GROUP("DataType\\Object" FILES ${BASE_OBJECT_HEADER_FILES}
                                      ${BASE_OBJECT_SOURCE_FILES})

##Math--------------------------------------------------------
SET(MATH_HEADER_FILES   ${MATH_INCLUDE_PATH}/bvec.h
                        ${MATH_INCLUDE_PATH}/FastTriangle.h
                        ${MATH_INCLUDE_PATH}/Bezier.h
                        ${MATH_INCLUDE_PATH}/Area.h
                        ${MATH_INCLUDE_PATH}/Math.h
                        ${MATH_INCLUDE_PATH}/MathConst.h
                        ${MATH_INCLUDE_PATH}/Matrix.h
                        ${MATH_INCLUDE_PATH}/Transform.h
                        ${MATH_INCLUDE_PATH}/Vector.h
                        ${MATH_INCLUDE_PATH}/PrimaryMathematics.h
                        ${MATH_INCLUDE_PATH}/PhysicsConst.h
                        ${MATH_INCLUDE_PATH}/HalfFloat.h
                        ${MATH_INCLUDE_PATH}/LerpType.h
                        ${MATH_INCLUDE_PATH}/Lerp2D.h
                        ${MATH_INCLUDE_PATH}/Lerp3D.h
                        )

SET(MATH_SOURCE_FILES   Math/LAtan.cpp
                        Math/LSinCos.cpp
                        Math/Matrix4f.cpp
                        Math/HalfFloat.cpp
                        Math/Transform.cpp)

SOURCE_GROUP("Math\\Header Files" FILES ${MATH_HEADER_FILES})
SOURCE_GROUP("Math\\Source Files" FILES ${MATH_SOURCE_FILES})

##Color--------------------------------------------------------
FILE(GLOB COLOR_HEADER_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/color/*.h)
FILE(GLOB COLOR_SOURCE_FILES Color/*.cpp)

SOURCE_GROUP("Datatype\\Color\\Header Files" FILES ${COLOR_HEADER_FILES})
SOURCE_GROUP("Datatype\\Color\\Source Files" FILES ${COLOR_SOURCE_FILES})

##Text--------------------------------------------------------
SET(STRING_HEADER_FILES ${TYPE_INCLUDE_PATH}/String.h
                        ${TYPE_INCLUDE_PATH}/StringView.h
                        ${TYPE_INCLUDE_PATH}/StringInstance.h
                        ${TYPE_INCLUDE_PATH}/StringList.h
                        ${TYPE_INCLUDE_PATH}/SplitString.h
                        ${TYPE_INCLUDE_PATH}/MergeString.h
                        ${TYPE_INCLUDE_PATH}/StdString.h
)

SET(TEXT_HEADER_FILES   ${CMCORE_ROOT_INCLUDE_PATH}/hgl/Endian.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/CodePage.h
                        ${CMCORE_ROOT_INCLUDE_PATH}/hgl/text/FontInfo.h
                        ${TYPE_INCLUDE_PATH}/UnicodeBlocks.h
                        ${TYPE_INCLUDE_PATH}/StrChar.h)

SET(TEXT_SOURCE_FILES   Text/Endian.cpp
                        Text/CodePage.cpp
                        Text/UnicodeBlocks.cpp)

SOURCE_GROUP("Text\\String" FILES   ${STRING_HEADER_FILES}
                                    Text/StringList.cpp)

SOURCE_GROUP("Text\\Header Files" FILES ${TEXT_HEADER_FILES})
SOURCE_GROUP("Text\\Source Files" FILES ${TEXT_SOURCE_FILES})

SOURCE_GROUP("Datatype\\Template" FILES ${TYPE_TEMPLATE_HEADER})

SET(BASE_OTHER_SOURCE   ${TYPE_INCLUDE_PATH}/MemoryBlock.h
                        ${TYPE_INCLUDE_PATH}/MemoryAllocator.h
                        Other/MemoryBlock.cpp
                        Other/MemoryAllocator.cpp)
SOURCE_GROUP("DataType\\Template\\Memory" FILES ${BASE_OTHER_SOURCE})

##I/O------------------------------------------------------------
SET(IO_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/io)

SET(IO_BASE_FILES ${IO_INCLUDE_PATH}/InputStream.h
                  ${IO_INCLUDE_PATH}/IOType.h
                  ${IO_INCLUDE_PATH}/OutputStream.h
                  ${IO_INCLUDE_PATH}/SeekAccess.h
                  IO/IOType.cpp)

SET(IO_DATA_FILES ${IO_INCLUDE_PATH}/DataInputStream.h
                  ${IO_INCLUDE_PATH}/DataOutputStream.h
                  ${IO_INCLUDE_PATH}/EndianDataInputStream.h
                  ${IO_INCLUDE_PATH}/EndianDataOutputStream.h
                  IO/InputStream.cpp
                  IO/DataInputStream.cpp
                  IO/DataOutputStream.cpp)

SET(IO_MEMORY_STREAM_FILES  ${IO_INCLUDE_PATH}/MemoryInputStream.h
                            ${IO_INCLUDE_PATH}/MemoryOutputStream.h)

SET(IO_DATA_ARRAY_FILES ${IO_INCLUDE_PATH}/LoadDataArray.h
                        ${IO_INCLUDE_PATH}/SaveDataArray.h)

SET(IO_FILE_FILES ${IO_INCLUDE_PATH}/FileAccess.h
                  ${IO_INCLUDE_PATH}/FileInputStream.h
                  ${IO_INCLUDE_PATH}/FileOutputStream.h
                  ${IO_INCLUDE_PATH}/RandomAccessFile.h
                  IO/FileAccess.cpp
                  IO/FileInputStream.cpp
                  IO/FileOutputStream.cpp
                  IO/RandomAccessFile.cpp)

SET(IO_JAVA_FILES   ${IO_INCLUDE_PATH}/JavaInputStream.h
                    ${IO_INCLUDE_PATH}/JavaOutputStream.h)

SET(IO_TEXT_FILES   IO/TextOutputStream.cpp
                    IO/TextInputStream.cpp
                    ${IO_INCLUDE_PATH}/TextInputStream.h
                    ${IO_INCLUDE_PATH}/TextOutputStream.h
                    Text/LoadString.cpp
                    Text/LoadStringList.cpp

                    ${IO_INCLUDE_PATH}/LoadStringList.h
                    ${IO_INCLUDE_PATH}/SaveStringList.h)

SET(INPUT_EVENT_FILES   ${IO_INCLUDE_PATH}/event/KeyboardEvent.h
                        ${IO_INCLUDE_PATH}/event/MouseEvent.h
                        ${IO_INCLUDE_PATH}/event/JoystickEvent.h
                        ${IO_INCLUDE_PATH}/event/InputEvent.h)

SOURCE_GROUP("IO\\Event"   FILES ${INPUT_EVENT_FILES})

SOURCE_GROUP("IO\\Base"         FILES ${IO_BASE_FILES})
SOURCE_GROUP("IO\\DataIOStream" FILES ${IO_DATA_FILES})
SOURCE_GROUP("IO\\DataArray"    FILES ${IO_DATA_ARRAY_FILES})
SOURCE_GROUP("IO\\MemoryStream" FILES ${IO_MEMORY_STREAM_FILES})
SOURCE_GROUP("IO\\File"         FILES ${IO_FILE_FILES})
SOURCE_GROUP("IO\\Jave"         FILES ${IO_JAVA_FILES})
SOURCE_GROUP("IO\\Text"         FILES ${IO_TEXT_FILES})

SET(IO_SOURCE_FILES   ${IO_BASE_FILES}
                      ${IO_DATA_FILES}
                      ${IO_DATA_ARRAY_FILES}
                      ${IO_MEMORY_STREAM_FILES}
                      ${IO_FILE_FILES}
                      ${IO_JAVA_FILES}
                      ${IO_TEXT_FILES}
                      ${INPUT_EVENT_FILES})

SET(FILESYSTEM_INCLUDE_PATH ${CMCORE_ROOT_INCLUDE_PATH}/hgl/filesystem)

SET(FILESYSTEM_HEADER_FILES ${FILESYSTEM_INCLUDE_PATH}/EnumFile.h
                            ${FILESYSTEM_INCLUDE_PATH}/EnumVolume.h
                            ${FILESYSTEM_INCLUDE_PATH}/FileSystem.h
                            ${FILESYSTEM_INCLUDE_PATH}/Filename.h
)

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


FILE(GLOB BASE_THREAD_HEADER_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/thread/*.h)
SET(BASE_THREAD_SOURCE_FILES   Other/ThreadFunc.cpp)

SOURCE_GROUP("Thread" FILES ${BASE_THREAD_HEADER_FILES}
                            ${BASE_THREAD_SOURCE_FILES})

FILE(GLOB BASE_EVENT_HEADER_FILES ${CMCORE_ROOT_INCLUDE_PATH}/hgl/event/*.*)
FILE(GLOB BASE_EVENT_SOURCE_FILES Event/*.*)

SOURCE_GROUP("Event" FILES  ${BASE_EVENT_HEADER_FILES}
                            ${BASE_EVENT_SOURCE_FILES})

file(GLOB BASE_PLUG_IN_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/*.h)
file(GLOB BASE_PLUG_IN_SOURCE PlugIn/*.cpp)

SOURCE_GROUP("PlugIn\\Header Files" FILES ${BASE_PLUG_IN_HEADER})
SOURCE_GROUP("PlugIn\\Source Files" FILES ${BASE_PLUG_IN_SOURCE})

file(GLOB BASE_LOG_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/*.h)
file(GLOB BASE_LOG_SOURCE Log/*.*)

SOURCE_GROUP("Log\\Header Files" FILES ${BASE_LOG_HEADER})
SOURCE_GROUP("Log\\Source Files" FILES ${BASE_LOG_SOURCE})

add_cm_library(CMCore "CM"  ${CORE_PLATFORM_HEADER_FILES}
                        #${SYSTEM_INFO_SOURCE}

                        ${TYPE_INFO_FILE}
                        ${BASE_OBJECT_HEADER_FILES}
                        ${BASE_OBJECT_SOURCE_FILES}

                        ${BITS_ARRAY_FILES}

                        ${TREE_NODE_FILES}

                        ${COLOR_HEADER_FILES}
                        ${COLOR_SOURCE_FILES}

                        ${STRING_HEADER_FILES}
                        ${TEXT_HEADER_FILES}
                        ${TEXT_SOURCE_FILES}

                        ${ACTIVE_MANAGER_FILES}

                        ${TYPE_TEMPLATE_HEADER}
                        ${TYPE_SOURCE_FILES}

                        ${TYPE_COLLECTION_SOURCE}
                        ${TYPE_DATA_CHAIN_SOURCE}

                        ${IO_SOURCE_FILES}

                        ${TYPE_IDNAME_HEADER_FILES}
                        ${TYPE_IDNAME_SOURCE_FILES}

                        ${MATH_HEADER_FILES}
                        ${MATH_SOURCE_FILES}

                        ${FILESYSTEM_HEADER_FILES}
                        ${FILESYSTEM_SOURCE_FILES}

                        ${BASE_TIME_HEADER_FILES}
                        ${BASE_TIME_SOURCE_FILES}

                        ${BASE_OTHER_SOURCE}
                        
                        ${BASE_THREAD_HEADER_FILES}
                        ${BASE_THREAD_SOURCE_FILES}
                        
                        ${BASE_EVENT_HEADER_FILES}
                        ${BASE_EVENT_SOURCE_FILES}

                        ${BASE_PLUG_IN_HEADER}
                        ${BASE_PLUG_IN_SOURCE}

                        ${BASE_LOG_HEADER}
                        ${BASE_LOG_SOURCE}
                        )
