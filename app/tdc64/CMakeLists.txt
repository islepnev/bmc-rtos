
cmake_minimum_required(VERSION 3.12)

set(BOARD "tdc64")

SET(BSP_DIR ${MY_TOP_DIR}/app/tdc/src)

SET(ELF ${BOARD}_rtos.elf)

set(cpu 769ni)
SET(STM32_CHIP STM32F769NIH)

include(../components.cmake)

include(${MY_TOP_DIR}/cmake/gcc_stm32.cmake)
include(${MY_TOP_DIR}/cmake/bmc_target.cmake)

include(${PLATFORM_DIR}/Filelists.cmake)

BMC_TARGET_PRE(${ELF} ${BOARD})

#subdirs(${MY_LIB_DIR}/dev)
#subdirs(${MY_LIB_DIR}/drivers)
#subdirs(${MY_SRC_DIR})

set(BSP_INC_DIRS
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
)

file(GLOB_RECURSE BSP_HEADERS
    ${BSP_DIR}/*.h
)

file(GLOB BSP_SRCS
    ${BSP_DIR}/*.c
    ${BSP_DIR}/bsp/*.c
    ${BSP_DIR}/bsp/mx_init/*.c
    ${BSP_DIR}/rtos/*.c
    ${BSP_DIR}/tasks/*.c
)

#file(GLOB_RECURSE Dev_HEADERS
#    ${MY_LIB_DIR}/dev/*.h
#)

#file(GLOB_RECURSE Dev_SRCS
#    ${MY_LIB_DIR}/dev/*.c
#)

file(GLOB_RECURSE Drivers_HEADERS
    ${MY_LIB_DIR}/drivers/*.h
)

file(GLOB_RECURSE Drivers_SRCS
    ${MY_LIB_DIR}/drivers/*.c
)

file(GLOB_RECURSE App_SOURCES
    ${MY_SRC_DIR}/app/*.c
)

file(GLOB_RECURSE Src_HEADERS
    ${MY_SRC_DIR}/*.h
)

file(GLOB Src_SOURCES
    ${MY_SRC_DIR}/app/*.c
    ${MY_SRC_DIR}/common/*.c
    ${MY_SRC_DIR}/common/log/*.c
    ${MY_SRC_DIR}/config/*.c
#    ${MY_SRC_DIR}/eth/*.c
    ${MY_SRC_DIR}/ipc/*.c
    ${MY_SRC_DIR}/rtos/*.c
    ${MY_SRC_DIR}/system/*.c
    ${MY_SRC_DIR}/tasks/base/*.c
#    ${MY_SRC_DIR}/tasks/http/*.c
#    ${MY_SRC_DIR}/tasks/snmp/*.c
#    ${MY_SRC_DIR}/tasks/snmp/snmp_private_mib/*.c
#    ${MY_SRC_DIR}/tasks/sntp/*.c
#    ${MY_SRC_DIR}/tasks/tcpip/*.c
)

set(BmcApp_HEADERS
    ${BSP_HEADERS}
#    ${Dev_HEADERS}
    ${Drivers_HEADERS}
    ${Src_HEADERS}
)

set(BmcApp_SRCS
    ${BSP_SRCS}
#    ${Dev_SRCS}
    ${Drivers_SRCS}
    ${Src_SOURCES}
    ${App_SOURCES}
    ${PLATFORM_SOURCES}
)

target_sources(${ELF} PRIVATE
    ${BmcApp_HEADERS}
    ${BmcApp_SRCS}
)
TARGET_INCLUDE_DIRECTORIES(${ELF} PRIVATE
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
    ${BSP_DIR}/rtos
    ${BSP_DIR}/tasks
    ${MY_LIB_DIR}/dev
    ${MY_LIB_DIR}/dev/common
    ${MY_LIB_DIR}/drivers
    ${MY_LIB_DIR}/drivers/common
    ${MY_SRC_DIR}/app
    ${MY_SRC_DIR}/common
#    ${MY_SRC_DIR}/eth
    ${MY_SRC_DIR}/ipc
    ${MY_SRC_DIR}/rtos
    ${MY_SRC_DIR}/system
    ${MY_SRC_DIR}/tasks
    ${PLATFORM_INCLUDE_DIRS}
)
BMC_TARGET_POST(${ELF} ${cpu})
