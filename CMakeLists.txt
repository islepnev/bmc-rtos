#SET(TOOLCHAIN_PREFIX ~/gcc-arm-none-eabi)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

set(CMAKE_BUILD_TYPE Debug)

SET(MY_TOP_DIR ${CMAKE_SOURCE_DIR})
SET(MY_EXT_DIR ${MY_TOP_DIR}/external)
SET(MY_LIB_DIR ${MY_TOP_DIR})
SET(MY_SRC_DIR ${MY_TOP_DIR}/src)

SET(FREERTOS_DIR ${MY_EXT_DIR}/FreeRTOS)

SET(STM32_CHIP STM32F769NIH)
#SET(STM32_CHIP_TYPE 769NI)
#SET(STM32_FAMILY F7)
SET(STM32Cube_DIR ${MY_EXT_DIR}/STM32Cube)
#SET(CMAKE_MODULE_PATH ${MY_TOP_DIR}/cmake)
SET(CMAKE_TOOLCHAIN_FILE cmake/gcc_stm32.cmake)

SET(STM32_MIN_HEAP_SIZE 0x20000)
SET(STM32_MIN_STACK_SIZE 0x200)

PROJECT(bmc_rtos VERSION 1.17 LANGUAGES C CXX)
ENABLE_LANGUAGE(ASM)
set(CMAKE_CXX_STANDARD 11)

# Stack Smashing Protection
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fstack-protector-all -fstack-usage -Wstack-usage=250")
# Extra warnings
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wpedantic -Wextra -Wstrict-prototypes -Wdouble-promotion -Wswitch-enum -Wno-unused -Wno-unused-parameter")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u_printf_float")


### CMSIS
FIND_PACKAGE(CMSIS REQUIRED)

add_library(CMSIS STATIC ${CMSIS_SOURCES})
TARGET_INCLUDE_DIRECTORIES(CMSIS PRIVATE
    ${CMSIS_INCLUDE_DIRS}
)
STM32_SET_TARGET_PROPERTIES(CMSIS)


### STM32HAL
FIND_PACKAGE(STM32HAL COMPONENTS adc dma eth flash fmc gpio i2c qspi rcc rtc sd sdram sdmmc smbus spi tim uart REQUIRED)

SET(HAL_LL_COMPONENTS ${HAL_LL_COMPONENTS} gpio i2c usart)
file(GLOB HAL_LL_EXTRA_INCLUDES
   ${STM32Cube_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_*.h
)
file(GLOB HAL_LL_EXTRA_SOURCES
   ${STM32Cube_DIR}/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_*.c
)

SET(STM32HAL_SOURCES ${STM32HAL_SOURCES} ${HAL_LL_EXTRA_SOURCES})

add_library(STM32HAL STATIC ${STM32HAL_SOURCES})
TARGET_INCLUDE_DIRECTORIES(STM32HAL PUBLIC
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)
target_compile_definitions(STM32HAL PUBLIC USE_FULL_LL_DRIVER)
STM32_SET_TARGET_PROPERTIES(STM32HAL)


### FreeRTOS
file(GLOB FreeRTOS_SRCS
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
   ${MY_SRC_DIR}/system/OS/sys_arch.c
)

add_library(lwip STATIC ${LWIP_SOURCES})
STM32_SET_TARGET_PROPERTIES(lwip)
target_include_directories(lwip PRIVATE
    ${LWIP_DIR}/src/include
)
TARGET_LINK_LIBRARIES(${ELF} lwip)

# required for lwip
include_directories(
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}/system
    ${MY_SRC_DIR}/common/log
)


include(cmake/Doxygen.cmake)
include(cmake/git_version.cmake)
configure_file(
  ${MY_TOP_DIR}/build_id.in
  ${MY_TOP_DIR}/build_id
)
configure_file(
  ${MY_SRC_DIR}/version.h.in
  ${CMAKE_BINARY_DIR}/generated/version.h
)
include_directories(${CMAKE_BINARY_DIR}/generated)
add_custom_target(data ALL SOURCES
    ${MY_TOP_DIR}/doc/Doxyfile.in
    ${MY_TOP_DIR}/mib/afi-bmc.mib
    ${MY_TOP_DIR}/README.md
    ${MY_SRC_DIR}/version.h.in
    ${MY_TOP_DIR}/build_id.in
)

include(cmake/bmc_target.cmake)

subdirs(app/tdc64)
subdirs(app/tdc72)
subdirs(app/ttvxs)
subdirs(app/cru16)
