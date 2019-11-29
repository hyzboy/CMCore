file(GLOB TYPE_TEMPLATE_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/type/*.h)

SET(SYSTEM_INFO_SOURCE   SystemInfo.cpp)

SET(BASE_TYPE_SOURCE
#    DataType/Info.cpp
#    DataType/BitArray.cpp
    DataType/Color.cpp
    DataType/Color3f.cpp
    DataType/Color4f.cpp
#     DataType/FontInfo.cpp
    DataType/StringList.cpp
    DataType/Endian.cpp)

SOURCE_GROUP("DataType\\Template" FILES ${TYPE_TEMPLATE_HEADER})
SOURCE_GROUP("DataType" FILES ${BASE_TYPE_SOURCE})

SET(BASE_IO_SOURCE
    IO/DataInputStream.cpp
    IO/DataOutputStream.cpp
    IO/FileAccess.cpp
    IO/FileInputStream.cpp
    IO/FileOutputStream.cpp
    IO/IOType.cpp
    IO/RandomAccessFile.cpp
    IO/TextOutputStream.cpp)

SOURCE_GROUP("IO" FILES ${BASE_IO_SOURCE})

SET(BASE_FILE_SYSTEM_SOURCE
    FileSystem/FileSystem.cpp
    FileSystem/EnumFile.cpp)

SOURCE_GROUP("FileSystem" FILES ${BASE_FILE_SYSTEM_SOURCE})

SET(BASE_OTHER_SOURCE
    Other/CodePage.cpp
    Other/DateTime.cpp
#    Other/ExpendString.cpp
    Other/ThreadFunc.cpp
    Other/TimeCount.cpp
    Other/TimeVal.cpp)

SOURCE_GROUP("Other"                FILES ${BASE_OTHER_SOURCE})

file(GLOB BASE_PLUG_IN_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/plugin/*.h)
file(GLOB BASE_PLUG_IN_SOURCE PlugIn/*.cpp)

SOURCE_GROUP("PlugIn\\Header Files" FILES ${BASE_PLUG_IN_HEADER})
SOURCE_GROUP("PlugIn\\Source Files" FILES ${BASE_PLUG_IN_SOURCE})

file(GLOB BASE_LOG_HEADER ${CMCORE_ROOT_INCLUDE_PATH}/hgl/log/*.h)
file(GLOB BASE_LOG_SOURCE Log/*.*)

SOURCE_GROUP("Log\\Header Files" FILES ${BASE_LOG_HEADER})
SOURCE_GROUP("Log\\Source Files" FILES ${BASE_LOG_SOURCE})

add_cm_library(CMCore "CM"  #${SYSTEM_INFO_SOURCE}
                            ${TYPE_TEMPLATE_HEADER}
                            ${BASE_TYPE_SOURCE}
                            ${BASE_IO_SOURCE}
                            ${BASE_FILE_SYSTEM_SOURCE}
                            ${BASE_OTHER_SOURCE}

							${BASE_PLUG_IN_HEADER}
                            ${BASE_PLUG_IN_SOURCE}

							${BASE_LOG_HEADER}
							${BASE_LOG_SOURCE}
							)
