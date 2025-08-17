cmake_minimum_required(VERSION 3.10)

project(CMCore)

include(path_config.cmake)
CMCoreSetup(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMCORE_ROOT_SOURCE_PATH})
