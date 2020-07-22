
cmake_minimum_required(VERSION 3.12)

set(BOARD "tdc64")

SET(BSP_DIR ${MY_TOP_DIR}/app/tdc/src)

SET(ELF ${BOARD}_rtos.elf)

BMC_TARGET_PRE(${ELF})

subdirs(${MY_LIB_DIR}/dev)
subdirs(${MY_LIB_DIR}/drivers)
subdirs(${MY_SRC_DIR})

set(BSP_INC_DIRS
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
)

file(GLOB_RECURSE BSP_SRCS
    ${BSP_DIR}/bsp/*.*
    ${BSP_DIR}/bsp/mx_init/*.*
    ${MY_LIB_DIR}/system/init/*.*
)

file(GLOB_RECURSE BmcApp_SRCS
    ${BSP_SRCS}
    ${Dev_SRCS}
    ${Drivers_SRCS}
    ${MY_LIB_DIR}/common/*.*
    ${BSP_DIR}/*.*
    ${MY_SRC_DIR}/*.*
    ${MY_SRC_DIR}/rtos/*.*
    ${MY_SRC_DIR}/tasks/*.*
)

file(GLOB_RECURSE PROJECT_SOURCES
    ${BmcApp_SRCS}
    ${MY_LIB_DIR}/system/init/*.*
)

target_sources(${ELF} PRIVATE
    ${PROJECT_SOURCES}
)
TARGET_INCLUDE_DIRECTORIES(${ELF} PRIVATE
#    ${STM32HAL_INCLUDE_DIR}
    ${MY_LIB_DIR}/common
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
    ${BSP_DIR}/tasks
    ${BSP_DIR}/rtos
    ${MY_SRC_DIR}
    ${MY_LIB_DIR}/system/init
)
BMC_TARGET_POST(${ELF})
