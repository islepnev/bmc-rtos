function(ADD_COMPONENT TARGET COMPONENT)
    string(TOUPPER ${COMPONENT} COMPONENT_UPPER)
    target_compile_definitions(${TARGET} PRIVATE
        "ENABLE_${COMPONENT_UPPER}"
    )
    unset(SOURCES CACHE)
    file(GLOB SOURCES
        ${MY_LIB_DIR}/drivers/${COMPONENT}/*.c
        ${MY_LIB_DIR}/drivers/${COMPONENT}/*.h
        ${MY_LIB_DIR}/dev/${COMPONENT}/*.c
        ${MY_LIB_DIR}/dev/${COMPONENT}/*.h
    )
    target_sources(${TARGET} PRIVATE
        ${SOURCES}
    )
endfunction()

function(BMC_TARGET_ADD_BOARD_COMPONENTS TARGET BOARD)
    if(NOT BOARD_COMPONENTS)
        message(FATAL_ERROR "BOARD_COMPONENTS not set")
    endif()
    unset(COMPONENT_SOURCES CACHE)
    foreach(COMPONENT ${BOARD_COMPONENTS})
        ADD_COMPONENT(${TARGET} ${COMPONENT})
    endforeach()
endfunction()

FUNCTION(BMC_TARGET_PRE TARGET BOARD)
    set(OUTPUT_NAME ${TARGET})
    ADD_EXECUTABLE(${TARGET})

    STRING(TOUPPER ${BOARD} BOARD_UPPER)
    target_compile_definitions(${TARGET} PRIVATE
        BOARD_${BOARD_UPPER}
    )
    # target_compile_definitions(${TARGET} PRIVATE -DENABLE_LWIP=${ENABLE_LWIP})
    BMC_TARGET_ADD_BOARD_COMPONENTS(${TARGET} ${BOARD})
ENDFUNCTION()

FUNCTION(BMC_TARGET_POST TARGET cpu)
    TARGET_LINK_LIBRARIES(${TARGET}
        CMSIS_${cpu}
        STM32HAL_${cpu}
        FreeRTOS_${cpu}
#        sysclk_${cpu}
    )
    STM32_SET_TARGET_PROPERTIES(${TARGET})
    STM32_ADD_HEX_BIN_TARGETS(${TARGET})
    STM32_ADD_DUMP_TARGET(${TARGET})
    STM32_PRINT_SIZE_OF_TARGETS(${TARGET})
    set(DIST_NAME ${BOARD}_rtos-${GIT_DESCR})
    add_custom_command(OUTPUT ${DIST_NAME}.elf
        DEPENDS ${TARGET}
        COMMAND ${CMAKE_COMMAND} -E copy ${TARGET} ${CMAKE_BINARY_DIR}/${DIST_NAME}.elf
    )
    add_custom_command(OUTPUT ${DIST_NAME}.bin
        DEPENDS ${TARGET}.bin
        COMMAND ${CMAKE_COMMAND} -E copy ${TARGET}.bin ${CMAKE_BINARY_DIR}/${DIST_NAME}.bin
    )
    add_custom_target(${BOARD}-all-generated ALL
        DEPENDS ${TARGET} ${DIST_NAME}.elf ${DIST_NAME}.bin
    )
ENDFUNCTION()
