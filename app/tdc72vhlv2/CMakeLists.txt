
cmake_minimum_required(VERSION 3.12)

set(BOARD "tdc72vhlv2")

SET(BSP_DIR ${MY_TOP_DIR}/app/tdc72vhlv2/src)

SET(ELF ${BOARD}_rtos.elf)

set(cpu 303cc)
SET(STM32_CHIP STM32F303CCT)

include(${MY_TOP_DIR}/cmake/gcc_stm32.cmake)
include(${MY_TOP_DIR}/cmake/bmc_target.cmake)

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

file(GLOB_RECURSE Dev_HEADERS
    ${MY_LIB_DIR}/dev/adt7301/*.h
    ${MY_LIB_DIR}/dev/common/*.h
    ${MY_LIB_DIR}/dev/fpga/*.h
    ${MY_LIB_DIR}/dev/thset/*.h
)

file(GLOB_RECURSE Dev_SRCS
    ${MY_LIB_DIR}/dev/adt7301/*.c
    ${MY_LIB_DIR}/dev/common/*.c
    ${MY_LIB_DIR}/dev/fpga/*.c
    ${MY_LIB_DIR}/dev/thset/*.c
)

set(Drivers_HEADERS
    ${MY_LIB_DIR}/drivers/bus/bus_types.h
    ${MY_LIB_DIR}/drivers/bus/spi_driver.h
    ${MY_LIB_DIR}/drivers/bus/impl/spi_driver_callbacks.h
    ${MY_LIB_DIR}/drivers/bus/impl/spi_driver_impl.h
    ${MY_LIB_DIR}/drivers/bus/impl/spi_driver_util.h
    ${MY_LIB_DIR}/drivers/clock.h
    ${MY_LIB_DIR}/drivers/debug_helpers.h
    ${MY_LIB_DIR}/drivers/dev_leds.h
    ${MY_LIB_DIR}/drivers/dev_leds_types.h
    ${MY_LIB_DIR}/drivers/dev_mcu.h
    ${MY_LIB_DIR}/drivers/error_handler.h
    ${MY_LIB_DIR}/drivers/error_handler_impl.h
    ${MY_LIB_DIR}/drivers/fpga_spi_hal.h
    ${MY_LIB_DIR}/drivers/led_gpio_hal.h
    ${MY_LIB_DIR}/drivers/os_serial_tty.h
    ${MY_LIB_DIR}/drivers/rtc_util.h
)

set(Drivers_SRCS
    ${MY_LIB_DIR}/drivers/bus/bus_types.c
    ${MY_LIB_DIR}/drivers/bus/spi_driver.c
    ${MY_LIB_DIR}/drivers/bus/impl/spi_driver_callbacks.c
    ${MY_LIB_DIR}/drivers/bus/impl/spi_driver_impl.c
    ${MY_LIB_DIR}/drivers/bus/impl/spi_driver_util.c
    ${MY_LIB_DIR}/drivers/clock.c
    ${MY_LIB_DIR}/drivers/debug_helpers.c
    ${MY_LIB_DIR}/drivers/dev_leds.c
    ${MY_LIB_DIR}/drivers/dev_leds_types.c
    ${MY_LIB_DIR}/drivers/dev_mcu.c
    ${MY_LIB_DIR}/drivers/error_handler.c
    ${MY_LIB_DIR}/drivers/error_handler_impl.c
    ${MY_LIB_DIR}/drivers/fpga_spi_hal.c
    ${MY_LIB_DIR}/drivers/led_gpio_hal.c
    ${MY_LIB_DIR}/drivers/os_serial_tty.c
    ${MY_LIB_DIR}/drivers/rtc_util.c
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
    ${MY_SRC_DIR}/system/init/*.c
    ${MY_SRC_DIR}/tasks/base/*.c
#    ${MY_SRC_DIR}/tasks/http/*.c
#    ${MY_SRC_DIR}/tasks/snmp/*.c
#    ${MY_SRC_DIR}/tasks/snmp/snmp_private_mib/*.c
#    ${MY_SRC_DIR}/tasks/sntp/*.c
#    ${MY_SRC_DIR}/tasks/tcpip/*.c
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
    ${App_SOURCES}
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
    ${MY_SRC_DIR}/app
    ${MY_SRC_DIR}/common
#    ${MY_SRC_DIR}/eth
    ${MY_SRC_DIR}/ipc
    ${MY_SRC_DIR}/rtos
    ${MY_SRC_DIR}/system/init
    ${MY_SRC_DIR}/tasks
)
BMC_TARGET_POST(${ELF} ${cpu})
