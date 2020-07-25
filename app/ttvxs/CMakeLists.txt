
cmake_minimum_required(VERSION 3.12)

set(BOARD "ttvxs")

SET(BSP_DIR ${MY_TOP_DIR}/app/ttvxs/src)

SET(ELF ${BOARD}_rtos.elf)

BMC_TARGET_PRE(${ELF})

#subdirs(${MY_LIB_DIR}/dev)
#subdirs(${MY_LIB_DIR}/drivers)
#subdirs(${MY_SRC_DIR})

set(BSP_INC_DIRS
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
)

# required for lwip
include_directories(
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}/common
    ${MY_SRC_DIR}/system
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

file(GLOB_RECURSE Dev_HEADERS
    ${MY_LIB_DIR}/dev/*.h
)

file(GLOB Dev_SRCS
    ${MY_LIB_DIR}/dev/ad9516/*.c
    ${MY_LIB_DIR}/dev/ad9545/*.c
#    ${MY_LIB_DIR}/dev/digipot/*.c
    ${MY_LIB_DIR}/dev/fpga/*.c
    ${MY_LIB_DIR}/dev/powermon/*.c
    ${MY_LIB_DIR}/dev/therm/*.c
    ${MY_LIB_DIR}/dev/ttvxs_clkmux/*.c
    ${MY_LIB_DIR}/dev/vxsiicm/*.c
)

file(GLOB_RECURSE Drivers_HEADERS
    ${MY_LIB_DIR}/drivers/*.h
)

file(GLOB Drivers_SRCS
    ${MY_LIB_DIR}/drivers/ad5141/*.c
    ${MY_LIB_DIR}/drivers/ad9516/*.c
    ${MY_LIB_DIR}/drivers/ad9545/*.c
    ${MY_LIB_DIR}/drivers/mcp23017/*.c
    ${MY_LIB_DIR}/drivers/bus/*.c
    ${MY_LIB_DIR}/drivers/bus/impl/*.c
    ${MY_LIB_DIR}/drivers/debug_helpers.c
    ${MY_LIB_DIR}/drivers/dev_ad9545.c
    ${MY_LIB_DIR}/drivers/dev_common_types.c
    ${MY_LIB_DIR}/drivers/dev_eeprom.c
    ${MY_LIB_DIR}/drivers/dev_leds.c
    ${MY_LIB_DIR}/drivers/dev_leds_types.c
    ${MY_LIB_DIR}/drivers/dev_mcu.c
    ${MY_LIB_DIR}/drivers/dev_sfpiic*.c
    ${MY_LIB_DIR}/drivers/dev_thset.c
    ${MY_LIB_DIR}/drivers/error_handler.c
    ${MY_LIB_DIR}/drivers/error_handler_impl.c
    ${MY_LIB_DIR}/drivers/fpga_spi_hal.c
    ${MY_LIB_DIR}/drivers/i2c_util.c
    ${MY_LIB_DIR}/drivers/ina226/*.c
    ${MY_LIB_DIR}/drivers/led_gpio_hal.c
    ${MY_LIB_DIR}/drivers/mcp23017_i2c_hal.c
    ${MY_LIB_DIR}/drivers/os_serial_tty.c
    ${MY_LIB_DIR}/drivers/powermon_i2c_driver.c
    ${MY_LIB_DIR}/drivers/rtc_util.c
)

#add_library(drivers STATIC ${Drivers_SRCS})
#STM32_SET_TARGET_PROPERTIES(drivers)
#target_include_directories(drivers PRIVATE
#    ${CMSIS_INCLUDE_DIRS}
#    ${STM32HAL_INCLUDE_DIR}
#    ${BSP_INC_DIRS}
#    ${MY_LIB_DIR}/drivers
#)
#TARGET_LINK_LIBRARIES(${ELF} drivers)

file(GLOB_RECURSE Src_HEADERS
    ${MY_SRC_DIR}/*.h
)

file(GLOB Src_SOURCES
    ${MY_SRC_DIR}/common/*.c
    ${MY_SRC_DIR}/common/log/*.c
    ${MY_SRC_DIR}/config/*.c
    ${MY_SRC_DIR}/eth/*.c
    ${MY_SRC_DIR}/rtos/*.c
    ${MY_SRC_DIR}/system/init/*.c
    ${MY_SRC_DIR}/tasks/http/*.c
    ${MY_SRC_DIR}/tasks/snmp/*.c
    ${MY_SRC_DIR}/tasks/snmp/snmp_private_mib/*.c
    ${MY_SRC_DIR}/tasks/sntp/*.c
    ${MY_SRC_DIR}/tasks/tcpip/*.c
)

set(BmcApp_HEADERS
    ${BSP_HEADERS}
    ${Dev_HEADERS}
    ${Drivers_HEADERS}
    ${Src_HEADERS}
)

set(BmcApp_SRCS
    ${BSP_SRCS}
    ${Dev_SRCS}
    ${Drivers_SRCS}
    ${Src_SOURCES}
)

# SET_SOURCE_FILES_PROPERTIES( ${BmcApp_SRCS} PROPERTIES LANGUAGE CXX )
target_sources(${ELF} PRIVATE
    ${BmcApp_HEADERS}
    ${BmcApp_SRCS}
)
TARGET_INCLUDE_DIRECTORIES(${ELF} PRIVATE
    ${LWIP_DIR}/src/include
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
    ${BSP_DIR}/rtos
    ${BSP_DIR}/tasks
    ${MY_LIB_DIR}/dev
    ${MY_LIB_DIR}/drivers
    ${MY_SRC_DIR}/common
    ${MY_SRC_DIR}/eth
    ${MY_SRC_DIR}/rtos
    ${MY_SRC_DIR}/system/init
    ${MY_SRC_DIR}/tasks
)
BMC_TARGET_POST(${ELF})
