cmake_minimum_required(VERSION 3.0)

project(CMCore)

set(CMCORE_ROOT_PATH            ${CMAKE_CURRENT_SOURCE_DIR})
set(CMCORE_ROOT_INCLUDE_PATH    ${CMCORE_ROOT_PATH}/inc)
set(CMCORE_ROOT_SOURCE_PATH     ${CMCORE_ROOT_PATH}/src)

set(CMAKE_MODULE_PATH ${CMCORE_ROOT_PATH}/cmake)

include(CMCoreCheck)

include_directories(${CMCORE_ROOT_INCLUDE_PATH})
add_subdirectory(${CMCORE_ROOT_SOURCE_PATH})
