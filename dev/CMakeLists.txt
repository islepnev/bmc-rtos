file(GLOB_RECURSE Dev_HEADERS
    ${MY_LIB_DIR}/dev/*.h
)

file(GLOB_RECURSE Dev_SRCS
    ${MY_LIB_DIR}/dev/*.c
)

#file(GLOB Dev_SRCS
#    ${MY_LIB_DIR}/dev/ad9516/*.c
#    ${MY_LIB_DIR}/dev/ad9545/*.c
#    ${MY_LIB_DIR}/dev/adt7301/*.c
#    ${MY_LIB_DIR}/dev/clkmux/*.c
#    ${MY_LIB_DIR}/dev/common/*.c
#    ${MY_LIB_DIR}/dev/digipot/*.c
#    ${MY_LIB_DIR}/dev/eeprom_config/*.c
#    ${MY_LIB_DIR}/dev/fpga/*.c
#    ${MY_LIB_DIR}/dev/max31725/*.c
#    ${MY_LIB_DIR}/dev/powermon/*.c
#    ${MY_LIB_DIR}/dev/sfpiic/*.c
#    ${MY_LIB_DIR}/dev/thset/*.c
#    ${MY_LIB_DIR}/dev/tmp421/*.c
#    ${MY_LIB_DIR}/dev/ttvxs_clkmux/*.c
#    ${MY_LIB_DIR}/dev/vxsiicm/*.c
#)

add_library(dev STATIC ${Dev_SRCS} ${Dev_HEADERS})
STM32_SET_TARGET_PROPERTIES(dev)
target_include_directories(dev PRIVATE
    ${CMSIS_INCLUDE_DIRS}
    ${BSP_INC_DIRS}
    ${MY_LIB_DIR}/src/common
    ${MY_LIB_DIR}/dev/common
    ${MY_LIB_DIR}/drivers
    ${MY_SRC_DIR}/ipc
)
TARGET_LINK_LIBRARIES(${ELF} dev)

#set_property(TARGET ${ELF} APPEND PROPERTY SOURCES ${Dev_SRCS})

#target_include_directories(${ELF} PRIVATE
#    ${CMAKE_CURRENT_SOURCE_DIR}
#)

#target_sources(${ELF} PRIVATE
#    ${Dev_SRCS}
#)
