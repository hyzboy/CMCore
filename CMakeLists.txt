cmake_minimum_required(VERSION 3.28)

project(CMCore)

# 依赖CMMath数学库
find_package(CMMath REQUIRED)

include(path_config.cmake)
CMCoreSetup(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMCORE_ROOT_SOURCE_PATH})
