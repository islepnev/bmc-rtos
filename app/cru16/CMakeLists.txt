
cmake_minimum_required(VERSION 3.12)

set(BOARD "cru16")

SET(BSP_DIR ${MY_TOP_DIR}/app/cru16/src)

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
    ${MY_TOP_DIR}/common
    ${MY_TOP_DIR}/system
    ${MY_LIB_DIR}/eth
)

file(GLOB BSP_SRCS
    ${BSP_DIR}/*.*
    ${BSP_DIR}/bsp/*.*
    ${BSP_DIR}/bsp/mx_init/*.*
    ${BSP_DIR}/rtos/*.*
    ${BSP_DIR}/tasks/*.*
    ${MY_LIB_DIR}/system/init/*.*
)

file(GLOB Dev_SRCS
    ${MY_LIB_DIR}/dev/ad9516/*
    ${MY_LIB_DIR}/dev/ad9545/*
    ${MY_LIB_DIR}/dev/fpga/*
    ${MY_LIB_DIR}/dev/powermon/*
    ${MY_LIB_DIR}/dev/ttvxs_clkmux/*
    ${MY_LIB_DIR}/dev/vxsiics/*
)

file(GLOB Drivers_SRCS
    ${MY_LIB_DIR}/drivers/ad9516/*.c
    ${MY_LIB_DIR}/drivers/ad9516/*.h
    ${MY_LIB_DIR}/drivers/ad9545/*.c
    ${MY_LIB_DIR}/drivers/ad9545/*.h
    ${MY_LIB_DIR}/drivers/mcp23017/*.c
    ${MY_LIB_DIR}/drivers/mcp23017/*.h
    ${MY_LIB_DIR}/drivers/bus/*.c
    ${MY_LIB_DIR}/drivers/bus/*.h
    ${MY_LIB_DIR}/drivers/debug_helpers.c
    ${MY_LIB_DIR}/drivers/debug_helpers.h
    ${MY_LIB_DIR}/drivers/dev_ad9545.c
    ${MY_LIB_DIR}/drivers/dev_ad9545.h
    ${MY_LIB_DIR}/drivers/dev_common_types.c
    ${MY_LIB_DIR}/drivers/dev_common_types.h
    ${MY_LIB_DIR}/drivers/dev_eeprom.c
    ${MY_LIB_DIR}/drivers/dev_eeprom.h
    ${MY_LIB_DIR}/drivers/dev_eeprom_types.h
    ${MY_LIB_DIR}/drivers/dev_leds.c
    ${MY_LIB_DIR}/drivers/dev_leds.h
    ${MY_LIB_DIR}/drivers/dev_leds_types.c
    ${MY_LIB_DIR}/drivers/dev_leds_types.h
    ${MY_LIB_DIR}/drivers/dev_mcu.c
    ${MY_LIB_DIR}/drivers/dev_mcu.h
    ${MY_LIB_DIR}/drivers/dev_sfpiic*
    ${MY_LIB_DIR}/drivers/dev_thset.c
    ${MY_LIB_DIR}/drivers/dev_thset.h
    ${MY_LIB_DIR}/drivers/dev_thset_types.h
    ${MY_LIB_DIR}/drivers/error_handler.c
    ${MY_LIB_DIR}/drivers/error_handler.h
    ${MY_LIB_DIR}/drivers/error_handler_impl.c
    ${MY_LIB_DIR}/drivers/error_handler_impl.h
    ${MY_LIB_DIR}/drivers/fpga_spi_hal.c
    ${MY_LIB_DIR}/drivers/fpga_spi_hal.h
    ${MY_LIB_DIR}/drivers/i2c_util.c
    ${MY_LIB_DIR}/drivers/i2c_util.h
    ${MY_LIB_DIR}/drivers/ina226/*.c
    ${MY_LIB_DIR}/drivers/ina226/*.h
    ${MY_LIB_DIR}/drivers/led_gpio_hal.c
    ${MY_LIB_DIR}/drivers/led_gpio_hal.h
    ${MY_LIB_DIR}/drivers/mcp23017_i2c_hal.c
    ${MY_LIB_DIR}/drivers/mcp23017_i2c_hal.h
    ${MY_LIB_DIR}/drivers/os_serial_tty.c
    ${MY_LIB_DIR}/drivers/os_serial_tty.h
    ${MY_LIB_DIR}/drivers/powermon_i2c_driver.c
    ${MY_LIB_DIR}/drivers/powermon_i2c_driver.h
    ${MY_LIB_DIR}/drivers/rtc_util.c
    ${MY_LIB_DIR}/drivers/rtc_util.h
)

file(GLOB BmcApp_SRCS
    ${BSP_SRCS}
    ${Dev_SRCS}
    ${Drivers_SRCS}
    ${MY_LIB_DIR}/common/*.*
    ${MY_LIB_DIR}/common/log/*.*
    ${MY_LIB_DIR}/eth/*.*
    ${MY_LIB_DIR}/system/init/*.*
    ${MY_SRC_DIR}/config/*.*
    ${MY_SRC_DIR}/*.*
    ${MY_SRC_DIR}/rtos/*.*
    ${MY_SRC_DIR}/tasks/http/*.*
#    ${MY_SRC_DIR}/tasks/snmp/*.*
#    ${MY_SRC_DIR}/tasks/snmp/snmp_private_mib/*.*
    ${MY_SRC_DIR}/tasks/tcpip/*.*
    ${MY_SRC_DIR}/tasks/sntp/*.*
)

target_sources(${ELF} PRIVATE
    ${BmcApp_SRCS}
)
TARGET_INCLUDE_DIRECTORIES(${ELF} PRIVATE
    ${LWIP_DIR}/src/include
    ${BSP_DIR}
    ${BSP_DIR}/bsp
    ${BSP_DIR}/bsp/mx_init
    ${BSP_DIR}/rtos
    ${BSP_DIR}/tasks
    ${MY_LIB_DIR}/common
    ${MY_LIB_DIR}/dev
    ${MY_LIB_DIR}/drivers
    ${MY_LIB_DIR}/system/init
    ${MY_SRC_DIR}
    ${MY_SRC_DIR}/rtos
    ${MY_SRC_DIR}/tasks
)
BMC_TARGET_POST(${ELF})
