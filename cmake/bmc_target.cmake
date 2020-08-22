FUNCTION(BMC_TARGET_PRE TARGET BOARD)
    set(OUTPUT_NAME ${TARGET})
    ADD_EXECUTABLE(${TARGET})

    STRING(TOUPPER ${BOARD} BOARD_UPPER)
    target_compile_definitions(${TARGET} PRIVATE
        BOARD_${BOARD_UPPER}
    )
ENDFUNCTION()

FUNCTION(BMC_TARGET_POST TARGET cpu)
    TARGET_LINK_LIBRARIES(${TARGET}
        CMSIS_${cpu}
        STM32HAL_${cpu}
        FreeRTOS_${cpu}
#        sysclk_${cpu}
        lwip_${cpu}
    )
    STM32_SET_TARGET_PROPERTIES(${TARGET})
    STM32_ADD_HEX_BIN_TARGETS(${TARGET})
    STM32_ADD_DUMP_TARGET(${TARGET})
    STM32_PRINT_SIZE_OF_TARGETS(${TARGET})
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET}> ${CMAKE_BINARY_DIR}/${BOARD}_rtos-${GIT_DESCR}.elf
    )
ENDFUNCTION()
