macro(CMCoreSetup CMCORE_ROOT_PATH)

    message("CMROOT_ROOT_PATH: " ${CMCORE_ROOT_PATH})

    set(CMCORE_ROOT_INCLUDE_PATH    ${CMCORE_ROOT_PATH}/inc)
    set(CMCORE_ROOT_SOURCE_PATH     ${CMCORE_ROOT_PATH}/src)

    include_directories(${CMCORE_ROOT_INCLUDE_PATH})
endmacro()