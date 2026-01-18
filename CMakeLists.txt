cmake_minimum_required(VERSION 3.28)

project(CMCore)

include(path_config.cmake)
CMCoreSetup(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(${CMCORE_ROOT_SOURCE_PATH})

add_subdirectory(examples)
