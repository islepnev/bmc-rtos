
cmake_minimum_required(VERSION 3.12)

set(BOARD "cru16")

SET(ELF ${BOARD}_rtos.elf)

BMC_TARGET_PRE(${ELF})

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

# required for lwip
include_directories(
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}/common
    ${MY_SRC_DIR}/system
)

file(GLOB_RECURSE BSP_HEADERS
    ${MY_TOP_DIR}/app/mcb/src/*.h
    ${MY_TOP_DIR}/app/${BOARD}/src/*.h
)

file(GLOB_RECURSE BSP_SRCS
    ${MY_TOP_DIR}/app/mcb/src/*.c
    ${MY_TOP_DIR}/app/${BOARD}/src/*.c
)

file(GLOB_RECURSE Dev_HEADERS
    ${MY_LIB_DIR}/dev/*.h
)

file(GLOB Dev_SRCS
    ${MY_LIB_DIR}/dev/common/*.c
    ${MY_LIB_DIR}/dev/ad9516/*.c
    ${MY_LIB_DIR}/dev/ad9545/*.c
    ${MY_LIB_DIR}/dev/cru16_clkmux/*.c
#    ${MY_LIB_DIR}/dev/digipot/*.c
    ${MY_LIB_DIR}/dev/eeprom_config/*.c
    ${MY_LIB_DIR}/dev/fpga/*.c
    ${MY_LIB_DIR}/dev/powermon/*.c
    ${MY_LIB_DIR}/dev/therm/*.c
    ${MY_LIB_DIR}/dev/vxsiics/*.c
)

file(GLOB_RECURSE Drivers_HEADERS
    ${MY_LIB_DIR}/drivers/*.h
)

file(GLOB Drivers_SRCS
    ${MY_LIB_DIR}/drivers/ad5141/*.c
    ${MY_LIB_DIR}/drivers/ad9516/*.c
    ${MY_LIB_DIR}/drivers/ad9545/*.c
    ${MY_LIB_DIR}/drivers/at24c/*.c
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
#    ${MY_SRC_DIR}/tasks/snmp/*.c
#    ${MY_SRC_DIR}/tasks/snmp/snmp_private_mib/*.c
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

target_sources(${ELF} PRIVATE
    ${BmcApp_HEADERS}
    ${BmcApp_SRCS}
)
TARGET_INCLUDE_DIRECTORIES(${ELF} PRIVATE
    ${LWIP_DIR}/src/include
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
    ${BSP_INC_DIRS}
    ${MY_LIB_DIR}/dev
    ${MY_LIB_DIR}/dev/common
    ${MY_LIB_DIR}/drivers
    ${MY_SRC_DIR}/common
    ${MY_SRC_DIR}/eth
    ${MY_SRC_DIR}/rtos
    ${MY_SRC_DIR}/system/init
    ${MY_SRC_DIR}/tasks
)
BMC_TARGET_POST(${ELF})
