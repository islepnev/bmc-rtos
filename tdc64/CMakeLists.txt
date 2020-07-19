#SET(TOOLCHAIN_PREFIX ~/gcc-arm-none-eabi-8-2018-q4-major)

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

SET(FREERTOS_DIR FreeRTOS)

SET(STM32_CHIP STM32F769NIH)
SET(STM32Cube_DIR STM32Cube)
SET(CMAKE_MODULE_PATH cmake)
SET(CMAKE_TOOLCHAIN_FILE ${CMAKE_MODULE_PATH}/gcc_stm32.cmake)

SET(STM32_MIN_HEAP_SIZE 0x20000)
SET(STM32_MIN_STACK_SIZE 0x200)

CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)

PROJECT(tdc72vxs4_rtos VERSION 1.11 LANGUAGES C)

add_compile_definitions(TDC64)

ENABLE_LANGUAGE(ASM)

FIND_PACKAGE(CMSIS REQUIRED)
FIND_PACKAGE(STM32HAL COMPONENTS dma i2c gpio smbus spi tim uart REQUIRED)

SET(HAL_LL_COMPONENTS ${HAL_LL_COMPONENTS} gpio i2c usart)
file(GLOB_RECURSE HAL_LL_EXTRA_INCLUDES
   STM32Cube/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_*.h
)
file(GLOB_RECURSE HAL_LL_EXTRA_SOURCES
   STM32Cube/Drivers/STM32F7xx_HAL_Driver/Src/stm32f7xx_ll_*.c
)

SET(STM32HAL_SOURCES ${STM32HAL_SOURCES} ${HAL_LL_EXTRA_SOURCES})

add_compile_definitions(USE_FULL_LL_DRIVER)
INCLUDE_DIRECTORIES(
#    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMSIS_INCLUDE_DIRS}
    ${STM32HAL_INCLUDE_DIR}
   ${FREERTOS_DIR}/Source/CMSIS_RTOS
   ${FREERTOS_DIR}/Source/include
   ${FREERTOS_DIR}/Source/portable/GCC/ARM_CM7/r0p1
   Inc
   Src
   drivers
   common
   app
)

file(GLOB_RECURSE USER_SOURCES
   Src/*.c
   Src/*.cpp
   common/*.c
   drivers/*.c
   app/*.c
)
file(GLOB_RECURSE USER_INCLUDES
   Inc/*.h
   Src/*.h
   common/*.h
   drivers/*.h
   app/*.h
   ${HAL_LL_EXTRA_INCLUDES}
)

SET(PROJECT_SOURCES
    ${USER_SOURCES}
    ${USER_INCLUDES}
)

file(GLOB_RECURSE FREERTOS_SOURCES
    ${FREERTOS_DIR}/Source/tasks.c
    ${FREERTOS_DIR}/Source/timers.c
    ${FREERTOS_DIR}/Source/queue.c
    ${FREERTOS_DIR}/Source/list.c
    ${FREERTOS_DIR}/Source/CMSIS_RTOS/cmsis_os.c
    ${FREERTOS_DIR}/Source/portable/GCC/ARM_CM7/r0p1/port.c
    ${FREERTOS_DIR}/Source/portable/MemMang/heap_4.c
    ${FREERTOS_DIR}/Source/event_groups.c

)
set(DATA_FILE
   README.md
   app/version.h.in
)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u_printf_float")
SET(ELF ${CMAKE_PROJECT_NAME}.elf)
ADD_EXECUTABLE(${ELF}
    ${CMSIS_SOURCES}
    ${STM32HAL_SOURCES}
    ${FREERTOS_SOURCES}
    ${PROJECT_SOURCES}
    ${DATA_FILE}
)
TARGET_LINK_LIBRARIES(${ELF})
STM32_SET_TARGET_PROPERTIES(${ELF})
STM32_ADD_HEX_BIN_TARGETS(${ELF})
STM32_ADD_DUMP_TARGET(${ELF})
STM32_PRINT_SIZE_OF_TARGETS(${ELF})

# Stack Smashing Protection
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fstack-protector-all -fstack-usage -Wstack-usage=250")
# Extra warnings
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wpedantic -Wextra -Wstrict-prototypes -Wdouble-promotion -Wswitch-enum -Wno-unused-parameter")

# first we can indicate the documentation build as an option and set it to ON by default
option(BUILD_DOC "Build documentation" ON)

# check if Doxygen is installed
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
#    add_custom_target( doc_doxygen ALL
#        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
#        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#        COMMENT "Generating API documentation with Doxygen"
#        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)

find_package (Git)
if (GIT_FOUND)
      message("git found: ${GIT_EXECUTABLE} in version ${GIT_VERSION_STRING}")
endif (GIT_FOUND)

# Get the current working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

configure_file(
  ${CMAKE_SOURCE_DIR}/app/version.h.in
  ${CMAKE_SOURCE_DIR}/app/version.h
)
list(APPEND SOURCES ${CMAKE_SOURCE_DIR}/app/version.h)
