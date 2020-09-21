
cmake_minimum_required(VERSION 3.12)

set(BOARD "cru16")

SET(ELF ${BOARD}_rtos.elf)

set(cpu 769ni)
SET(STM32_CHIP STM32F769NIH)

include(../components.cmake)

include(${MY_TOP_DIR}/cmake/gcc_stm32.cmake)
include(${MY_TOP_DIR}/cmake/bmc_target.cmake)

include(${PLATFORM_DIR}/Filelists.cmake)

BMC_TARGET_PRE(${ELF} ${BOARD})

target_link_libraries(${ELF} lwip_${cpu})

#subdirs(${MY_LIB_DIR}/dev)
#subdirs(${MY_LIB_DIR}/drivers)
#subdirs(${MY_SRC_DIR})

set(BSP_INC_DIRS
    ${MY_TOP_DIR}/app/mcb/src
    ${MY_TOP_DIR}/app/mcb/src/bsp
    ${MY_TOP_DIR}/app/mcb/src/bsp/mx_init
    ${MY_TOP_DIR}/app/${BOARD}/src
    ${MY_TOP_DIR}/app/${BOARD}/src/bsp
    ${MY_TOP_DIR}/app/${BOARD}/src/bsp/mx_init
    ${MY_TOP_DIR}/app/${BOARD}/src/rtos
    ${MY_TOP_DIR}/app/${BOARD}/src/tasks
)

file(GLOB_RECURSE BSP_HEADERS
    ${MY_TOP_DIR}/app/mcb/src/*.h
    ${MY_TOP_DIR}/app/${BOARD}/src/*.h
)

file(GLOB_RECURSE BSP_SRCS
    ${MY_TOP_DIR}/app/mcb/src/*.c
    ${MY_TOP_DIR}/app/${BOARD}/src/*.c
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
    ${MY_SRC_DIR}/eth/*.c
    ${MY_SRC_DIR}/ipc/*.c
    ${MY_SRC_DIR}/rtos/*.c
    ${MY_SRC_DIR}/system/*.c
    ${MY_SRC_DIR}/tasks/base/*.c
    ${MY_SRC_DIR}/tasks/http/*.c
#    ${MY_SRC_DIR}/tasks/snmp/*.c
#    ${MY_SRC_DIR}/tasks/snmp/snmp_private_mib/*.c
    ${MY_SRC_DIR}/tasks/sntp/*.c
    ${MY_SRC_DIR}/tasks/tcpip/*.c
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
    ${BSP_INC_DIRS}
    ${MY_LIB_DIR}/dev
    ${MY_LIB_DIR}/dev/common
    ${MY_LIB_DIR}/drivers
    ${MY_LIB_DIR}/drivers/common
    ${MY_SRC_DIR}/app
    ${MY_SRC_DIR}/common
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}/eth
    ${MY_SRC_DIR}/ipc
    ${MY_SRC_DIR}/rtos
    ${MY_SRC_DIR}/system
    ${MY_SRC_DIR}/tasks
    ${PLATFORM_INCLUDE_DIRS}
)
TARGET_LINK_LIBRARIES(${ELF}
    lwip_${cpu}
)
BMC_TARGET_POST(${ELF} ${cpu})
