
CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
cmake_policy(SET CMP0048 NEW)
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(HAVE_FLAG_SEARCH_PATHS_FIRST 0) # workaround for macosx

if ( ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR} )
    message( FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt." )
endif()

set(CMAKE_BUILD_TYPE Debug)

if(DEFINED ENV{TOOLCHAIN_PREFIX})
    set(TOOLCHAIN_PREFIX=ENV{TOOLCHAIN_PREFIX})
endif()

SET(MY_TOP_DIR ${CMAKE_SOURCE_DIR})
SET(MY_EXT_DIR ${MY_TOP_DIR}/external)
SET(MY_LIB_DIR ${MY_TOP_DIR})
SET(MY_SRC_DIR ${MY_TOP_DIR}/src)
SET(PLATFORM_DIR ${MY_TOP_DIR}/platform)

SET(STM32Cube_DIR ${MY_EXT_DIR}/STM32Cube)
SET(FreeRTOS_DIR ${MY_EXT_DIR}/FreeRTOS)
SET(LWIP_DIR ${MY_EXT_DIR}/lwip)

SET(STM32_MIN_HEAP_SIZE 0x2000) # Minimum heap size for error detection at link-time
SET(STM32_MIN_STACK_SIZE 0x400) # Minimum stack size for error detection at link-time
SET(FREERTOS_HEAP_IMPL 4)

PROJECT(bmc_rtos LANGUAGES C CXX)
ENABLE_LANGUAGE(ASM)
set(CMAKE_CXX_STANDARD 11)

# Stack Smashing Protection
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -fstack-protector-all -fstack-usage -Wstack-usage=250")
# Extra warnings
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wpedantic -Wextra -Wstrict-prototypes -Wdouble-promotion -Wswitch-enum -Wno-unused -Wno-unused-parameter")
# treat as errors
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration -Werror=incompatible-pointer-types")

#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -u_printf_float")


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
include_directories(
    ${CMAKE_BINARY_DIR}/generated
    ${MY_SRC_DIR}/config
    ${MY_LIB_DIR}/common
)
add_custom_target(data ALL SOURCES
    ${MY_TOP_DIR}/doc/Doxyfile.in
    ${MY_TOP_DIR}/mib/afi-bmc.mib
    ${MY_TOP_DIR}/README.md
    ${MY_SRC_DIR}/version.h.in
    ${MY_TOP_DIR}/build_id.in
)

subdirs(cpu/stm32f769ni)
subdirs(app/tdc64)
subdirs(app/tdc72)
subdirs(app/ttvxs)
subdirs(app/cru16)

subdirs(cpu/stm32f746ig)
subdirs(app/tdc72vhlv3)

subdirs(cpu/stm32f303cc)
subdirs(app/tdc72vhlv2)
