
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
#SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../../cmake)

set(cpu 303cc)

SET(STM32_CHIP STM32F303CC)

include(${MY_TOP_DIR}/cmake/gcc_stm32.cmake)


### CMSIS

FIND_PACKAGE(CMSIS REQUIRED)

add_library(CMSIS_${cpu} OBJECT ${CMSIS_SOURCES})
TARGET_INCLUDE_DIRECTORIES(CMSIS_${cpu} PUBLIC
    ${CMSIS_INCLUDE_DIRS}
)
STM32_SET_TARGET_PROPERTIES(CMSIS_${cpu})


### STM32HAL, STM32LL

FIND_PACKAGE(STM32HAL COMPONENTS adc dma gpio i2c rcc rtc smbus spi tim uart REQUIRED)
FIND_PACKAGE(STM32LL COMPONENTS gpio usart REQUIRED)

SET(STM32HAL_SOURCES ${STM32HAL_SOURCES} ${STM32LL_SOURCES})

add_library(STM32HAL_${cpu} OBJECT ${STM32HAL_SOURCES})
TARGET_INCLUDE_DIRECTORIES(STM32HAL_${cpu}
    PUBLIC
    ${STM32HAL_INCLUDE_DIR}
)
target_compile_definitions(STM32HAL_${cpu} PUBLIC USE_FULL_LL_DRIVER)
target_link_libraries(STM32HAL_${cpu} CMSIS_${cpu})
STM32_SET_TARGET_PROPERTIES(STM32HAL_${cpu})


### FreeRTOS

FIND_PACKAGE(FreeRTOS REQUIRED)

add_library(FreeRTOS_${cpu} OBJECT ${FreeRTOS_SOURCES})

target_include_directories(FreeRTOS_${cpu} PUBLIC
    ${FreeRTOS_INCLUDE_DIRS}
)

target_link_libraries(FreeRTOS_${cpu} CMSIS_${cpu})
STM32_SET_TARGET_PROPERTIES(FreeRTOS_${cpu})


### lwIP

FIND_PACKAGE(lwip REQUIRED)
add_library(lwip_${cpu} OBJECT ${lwip_SOURCES})
target_compile_definitions(lwip_${cpu} PRIVATE LWIP_DEBUG)
target_include_directories(lwip_${cpu}
    PUBLIC
    ${LWIP_INCLUDE_DIRS}
    PRIVATE
    ${MY_SRC_DIR}/config
    ${MY_SRC_DIR}/system
    ${MY_SRC_DIR}/common
)

target_link_libraries(lwip_${cpu} FreeRTOS_${cpu})


#### init cpu
#
#set(INIT_SOURCES init_sysclk.c)
#set(INIT_HEADERS init_sysclk.h)
#add_library(sysclk_${cpu} OBJECT ${INIT_SOURCES} ${INIT_HEADERS})
#target_link_libraries(sysclk_${cpu} STM32HAL_${cpu})
#target_include_directories(sysclk_${cpu}
#    PUBLIC
#    ./
#)
#STM32_SET_TARGET_PROPERTIES(sysclk_${cpu})
