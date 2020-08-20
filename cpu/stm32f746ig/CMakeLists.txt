
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
#SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

set(cpu 746ig)

SET(STM32_CHIP STM32F746IGK)

include(${MY_TOP_DIR}/cmake/gcc_stm32.cmake)


### CMSIS
FIND_PACKAGE(CMSIS REQUIRED)

add_library(CMSIS-${cpu} STATIC ${CMSIS_SOURCES})
TARGET_INCLUDE_DIRECTORIES(CMSIS-${cpu} PRIVATE
    ${CMSIS_INCLUDE_DIRS}
)
STM32_SET_TARGET_PROPERTIES(CMSIS-${cpu})


### STM32HAL, STM32LL
FIND_PACKAGE(STM32HAL COMPONENTS adc dma eth flash fmc gpio i2c qspi rcc rtc sd sdram sdmmc smbus spi tim uart REQUIRED)
FIND_PACKAGE(STM32LL COMPONENTS gpio i2c usart REQUIRED)

SET(STM32HAL_SOURCES ${STM32HAL_SOURCES} ${STM32LL_SOURCES})

add_library(STM32HAL-${cpu} STATIC ${STM32HAL_SOURCES})
TARGET_INCLUDE_DIRECTORIES(STM32HAL-${cpu} PRIVATE
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
)
target_compile_definitions(STM32HAL-${cpu} PUBLIC USE_FULL_LL_DRIVER)
STM32_SET_TARGET_PROPERTIES(STM32HAL-${cpu})


### FreeRTOS

FIND_PACKAGE(FreeRTOS REQUIRED)

add_library(FreeRTOS-${cpu} STATIC ${FreeRTOS_SOURCES})
message("CMSIS_COMMON_INCLUDE_DIR=${CMSIS_COMMON_INCLUDE_DIR}")

target_include_directories(FreeRTOS-${cpu} PUBLIC
    ${FreeRTOS_INCLUDE_DIRS}
)

target_include_directories(FreeRTOS-${cpu} PRIVATE
    ${CMSIS_COMMON_INCLUDE_DIR}
)


### lwIP

FIND_PACKAGE(lwip REQUIRED)
add_compile_definitions(LWIP_DEBUG)
add_library(lwip-${cpu} STATIC ${LWIP_SOURCES})
STM32_SET_TARGET_PROPERTIES(lwip-${cpu})
message("LWIP_INCLUDE_DIRS=${LWIP_INCLUDE_DIRS}")
target_include_directories(lwip-${cpu} PUBLIC
    ${LWIP_INCLUDE_DIRS}
)
target_include_directories(lwip-${cpu} PRIVATE
    ${FreeRTOS_INCLUDE_DIRS}
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}/system
    ${MY_SRC_DIR}/common
)
