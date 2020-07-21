#SET(TOOLCHAIN_PREFIX ~/gcc-arm-none-eabi-8-2018-q4-major)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

SET(MY_TOP_DIR ${CMAKE_SOURCE_DIR}/../..)
SET(MY_EXT_DIR ${MY_TOP_DIR}/external)
SET(MY_LIB_DIR ${MY_TOP_DIR})

SET(FREERTOS_DIR ${MY_EXT_DIR}/FreeRTOS)

SET(STM32_CHIP STM32F769NIH)
SET(STM32Cube_DIR ${MY_EXT_DIR}/STM32Cube)
SET(CMAKE_MODULE_PATH ${MY_TOP_DIR}/cmake)
SET(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/gcc_stm32.cmake)

SET(STM32_MIN_HEAP_SIZE 0x20000)
SET(STM32_MIN_STACK_SIZE 0x200)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

# use one of: cru16, tdc64, tdc72, ttvxs
set(BOARD "cru16" CACHE STRING "PCB variant")
set_property(CACHE BOARD PROPERTY STRINGS cru16 tdc64 tdc72 ttvxs)
message(STATUS "BOARD=${BOARD}")

PROJECT(${BOARD}_rtos VERSION 1.17 LANGUAGES C CXX)
set (CMAKE_CXX_STANDARD 11)
ENABLE_LANGUAGE(ASM)

# uncomment to build for old TTVXS v1.0
#add_compile_definitions(TTVXS_1_0)

FIND_PACKAGE(CMSIS REQUIRED)
FIND_PACKAGE(STM32HAL COMPONENTS adc dma eth flash fmc gpio i2c qspi rcc rtc sd sdram sdmmc smbus spi tim uart REQUIRED)

SET(HAL_LL_COMPONENTS ${HAL_LL_COMPONENTS} gpio usart)
file(GLOB_RECURSE HAL_LL_EXTRA_INCLUDES
   ${STM32Cube_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_*.h
)
file(GLOB_RECURSE HAL_LL_EXTRA_SOURCES
   ${STM32Cube_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_*.c
)

SET(STM32HAL_SOURCES ${STM32HAL_SOURCES} ${HAL_LL_EXTRA_SOURCES})

SET(MY_SRC_DIR ${MY_TOP_DIR}/app/${BOARD}/src)

add_compile_definitions(USE_FULL_LL_DRIVER)

### lwIP
SET(LWIP_DIR ${MY_EXT_DIR}/lwip)
add_compile_definitions(LWIP_DEBUG)
include(${LWIP_DIR}/src/Filelists.cmake)
#include(${LWIP_CONTRIB_DIR}/Filelists.cmake)
SET(LWIP_SOURCES
   ${lwipcore_SRCS}
   ${lwipcore4_SRCS}
   ${lwipapi_SRCS}
   ${lwipnetif_SRCS}
   ${lwipsntp_SRCS}
   ${lwipsnmp_SRCS}
   ${MY_LIB_DIR}/system/OS/sys_arch.c
)

add_library(lwip STATIC ${LWIP_SOURCES})
STM32_SET_TARGET_PROPERTIES(lwip)
target_include_directories(lwip PRIVATE
    ${LWIP_DIR}/src/include
)

include_directories(
    ${MY_SRC_DIR}/config
    ${MY_LIB_DIR}/eth
    ${MY_LIB_DIR}/system
)

include_directories(
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}
    ${MY_LIB_DIR}/common
    ${MY_LIB_DIR}/system/init
)

file(GLOB_RECURSE BSP_SRCS
    ${MY_SRC_DIR}/bsp/*.*
    ${MY_SRC_DIR}/bsp/mx_init/*.*
    ${MY_LIB_DIR}/system/init/*.*
)

set(Dev_INC_DIRS
    ${MY_LIB_DIR}/dev/ad9516
    ${MY_LIB_DIR}/dev/ad9545
    ${MY_LIB_DIR}/dev/fpga
    ${MY_LIB_DIR}/dev/powermon
    ${MY_LIB_DIR}/dev/ttvxs_auxpll
    ${MY_LIB_DIR}/dev/cru16_clkmux
    ${MY_LIB_DIR}/dev/ttvxs_vxsiic
)
file(GLOB_RECURSE Dev_SRCS
    ${MY_LIB_DIR}/dev/ad9516/*
    ${MY_LIB_DIR}/dev/ad9545/*
    ${MY_LIB_DIR}/dev/fpga/*
    ${MY_LIB_DIR}/dev/powermon/*
    ${MY_LIB_DIR}/dev/ttvxs_auxpll/*
    ${MY_LIB_DIR}/dev/cru16_clkmux/*
    ${MY_LIB_DIR}/dev/ttvxs_vxsiic/*
)

set(Drivers_INC_DIRS
    ${MY_LIB_DIR}/drivers
    ${MY_LIB_DIR}/drivers/ina226
    ${MY_LIB_DIR}/drivers/mcp23017
)

file(GLOB_RECURSE Drivers_SRCS
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

add_library(hwlib STATIC ${BSP_SRCS} ${Dev_SRCS} ${Drivers_SRCS})
target_include_directories(hwlib PUBLIC
    ${Dev_INC_DIRS}
    ${Drivers_INC_DIRS}
)
TARGET_INCLUDE_DIRECTORIES(hwlib PRIVATE
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
    ${MY_SRC_DIR}/bsp
    ${MY_SRC_DIR}/bsp/mx_init
    ${MY_LIB_DIR}/init
#    ${MY_LIB_DIR}/drivers
)
STM32_SET_TARGET_PROPERTIES(hwlib)

add_library(CMSIS STATIC ${CMSIS_SOURCES})
TARGET_INCLUDE_DIRECTORIES(CMSIS PRIVATE
    ${CMSIS_INCLUDE_DIRS}
)
STM32_SET_TARGET_PROPERTIES(CMSIS)

add_library(STM32HAL STATIC ${STM32HAL_SOURCES})
TARGET_INCLUDE_DIRECTORIES(STM32HAL PUBLIC
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)
STM32_SET_TARGET_PROPERTIES(STM32HAL)

file(GLOB_RECURSE USER_SOURCES
    ${MY_SRC_DIR}/config/*.*
    ${MY_LIB_DIR}/common/*.*
    ${MY_SRC_DIR}/*.*
    ${MY_LIB_DIR}/eth/*.*
    ${HAL_LL_EXTRA_INCLUDES}
)

SET(PROJECT_SOURCES
    ${USER_SOURCES}
)

file(GLOB_RECURSE FreeRTOS_SRCS
    ${FREERTOS_DIR}/include/*.h
    ${FREERTOS_DIR}/tasks.c
    ${FREERTOS_DIR}/timers.c
    ${FREERTOS_DIR}/queue.c
    ${FREERTOS_DIR}/list.c
    ${FREERTOS_DIR}/CMSIS_RTOS/cmsis_os.*
    ${FREERTOS_DIR}/portable/GCC/ARM_CM7/r0p1/*.c
    ${FREERTOS_DIR}/portable/MemMang/heap_4.c
    ${FREERTOS_DIR}/event_groups.c
)

add_library(FreeRTOS STATIC ${FreeRTOS_SRCS})
include_directories(
    ${FREERTOS_DIR}/CMSIS_RTOS
    ${FREERTOS_DIR}/include
    ${FREERTOS_DIR}/portable/GCC/ARM_CM7/r0p1
)
target_include_directories(FreeRTOS PRIVATE
    ${CMSIS_COMMON_INCLUDE_DIR}
)

set(DATA_FILE
   ${MY_TOP_DIR}/doc/Doxyfile.in
   ${MY_TOP_DIR}/mib/afi-bmc.mib
   ${MY_TOP_DIR}/README.md
   ${MY_SRC_DIR}/version.h.in
)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u_printf_float")
SET(ELF ${CMAKE_PROJECT_NAME}.elf)
ADD_EXECUTABLE(${ELF}
    ${PROJECT_SOURCES}
    ${DATA_FILE}
)
TARGET_INCLUDE_DIRECTORIES(${ELF} PRIVATE
    ${STM32HAL_INCLUDE_DIR}
    ${LWIP_DIR}/src/include
   ${MY_LIB_DIR}/common
   ${MY_SRC_DIR}
   ${MY_SRC_DIR}/bsp
   ${MY_SRC_DIR}/bsp/mx_init
   ${MY_SRC_DIR}/tasks
   ${MY_SRC_DIR}/rtos
)

TARGET_LINK_LIBRARIES(${ELF}
    hwlib
    lwip
    FreeRTOS
    CMSIS
    STM32HAL
)
STM32_SET_TARGET_PROPERTIES(${ELF})
STM32_ADD_HEX_BIN_TARGETS(${ELF})
STM32_ADD_DUMP_TARGET(${ELF})
STM32_PRINT_SIZE_OF_TARGETS(${ELF})

# Stack Smashing Protection
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fstack-protector-all -fstack-usage -Wstack-usage=250")
# Extra warnings
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wpedantic -Wextra -Wstrict-prototypes -Wdouble-promotion -Wswitch-enum -Wno-unused -Wno-unused-parameter")

include(${MY_TOP_DIR}/cmake/Doxygen.cmake)
include(${MY_TOP_DIR}/cmake/git_version.cmake)

configure_file(
  ${MY_SRC_DIR}/version.h.in
  ${MY_SRC_DIR}/version.h
)
configure_file(
  ${MY_TOP_DIR}/build_id.in
  ${MY_TOP_DIR}/build_id
)
list(APPEND SOURCES ${MY_SRC_DIR}/version.h)

add_custom_command(TARGET ${ELF} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${ELF}> ${CMAKE_PROJECT_NAME}-${GIT_DESCR}.elf
)
