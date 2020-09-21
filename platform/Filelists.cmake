# This file is indended to be included in end-user CMakeLists.txt
# include(/path/to/Filelists.cmake)
# It assumes the variable PLATFORM_DIR is defined pointing to the
# root path of PLATFORM sources.
#
# This file is NOT designed (on purpose) to be used as cmake
# subdir via add_subdirectory()
# The intention is to provide greater flexibility to users to
# create their own targets using the *_SOURCES variables.

if(NOT STM32_FAMILY)
    message(FATAL_ERROR "STM32_FAMILY not defined")
endif()

STRING(TOLOWER ${STM32_FAMILY} STM32_FAMILY_LOWER)

unset(PLATFORM_HEADERS CACHE)
file(GLOB PLATFORM_HEADERS
    ${PLATFORM_DIR}/stm32/*.h
    ${PLATFORM_DIR}/stm32${STM32_FAMILY_LOWER}/*.h
)

unset(PLATFORM_SOURCES CACHE)
file(GLOB PLATFORM_SOURCES
    ${PLATFORM_HEADERS}
    ${PLATFORM_DIR}/stm32/*.c
    ${PLATFORM_DIR}/stm32${STM32_FAMILY_LOWER}/*.c
)

unset(PLATFORM_INCLUDE_DIRS CACHE)
file(GLOB PLATFORM_INCLUDE_DIRS
    ${PLATFORM_DIR}/stm32
    ${PLATFORM_DIR}/stm32${STM32_FAMILY_LOWER}
)
