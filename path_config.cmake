macro(CMCoreSetup CMCORE_ROOT_PATH)

    message("CMROOT_ROOT_PATH: " ${CMCORE_ROOT_PATH})

    set(CMCORE_ROOT_INCLUDE_PATH    ${CMCORE_ROOT_PATH}/inc)
    set(CMCORE_ROOT_SOURCE_PATH     ${CMCORE_ROOT_PATH}/src)

    #set(CMAKE_MODULE_PATH ${CMCORE_ROOT_PATH}/cmake)
    #使用CMAKE_MODULE_PATH会引起在多重工程引用时出现一定问题，所以这里直接指定
    include(${CMCORE_ROOT_PATH}/cmake/system_bit.cmake)
    include(${CMCORE_ROOT_PATH}/cmake/version.cmake)
    include(${CMCORE_ROOT_PATH}/cmake/compiler.cmake)
    include(${CMCORE_ROOT_PATH}/cmake/output_path.cmake)

    include_directories(${CMCORE_ROOT_INCLUDE_PATH})
endmacro()