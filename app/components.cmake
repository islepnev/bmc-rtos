if(NOT BOARD)
    message(FATAL_ERROR "BOARD not defined")
endif()

set(COMMON_COMPONENTS
    common
    eeprom_config
    thset
)

if(${BOARD} STREQUAL "adc64vev1")
    set(BOARD_COMPONENTS
        ad9548
#        adt7301
        fpga
    )
endif()

if(${BOARD} STREQUAL "adc64ve")
    set(BOARD_COMPONENTS
        ad9516
        ad9545
        adc64ve_clkmux
        fpga
        max31725
        powermon
        sfpiic
        sensors
        tmp421
        vxsiics
    )
endif()

if(${BOARD} STREQUAL "cru16")
    set(BOARD_COMPONENTS
        ad9516
        ad9545
        cru16_clkmux
        ethernet
        fpga
        max31725
        powermon
        sfpiic
        sensors
        tmp421
        vxsiics
    )
endif()

if(${BOARD} STREQUAL "ttvxs")
    set(BOARD_COMPONENTS
        ad9516
        ad9545
        ethernet
        fpga
        max31725
        powermon
        sfpiic
        sensors
        tmp421
        ttvxs_clkmux
        vxsiicm
    )
endif()

if(${BOARD} STREQUAL "tdc64")
    set(BOARD_COMPONENTS
        ad9516
        ad9545
        adt7301
        digipot
        fpga
        max31725
        powermon
        sfpiic
        sensors
        vxsiics
    )
endif()

if(${BOARD} STREQUAL "tdc64vhlev1")
    set(BOARD_COMPONENTS
        ad9548
        adt7301
        fpga
    )
endif()


if(${BOARD} STREQUAL "tdc72")
    set(BOARD_COMPONENTS
        ad9545
        adt7301
        digipot
        fpga
        max31725
        powermon
        sfpiic
        sensors
        vxsiics
    )
endif()

if(${BOARD} STREQUAL "tdc72vhlv3")
    set(BOARD_COMPONENTS
        ad9548
        adt7301
        fpga
        vxsiics
    )
endif()

if(${BOARD} STREQUAL "tdc72vhlv2")
    set(BOARD_COMPONENTS
        ad9548
        adt7301
        fpga
    )
endif()

if(${BOARD} STREQUAL "tqdc16vsv1")
    set(BOARD_COMPONENTS
        ad9548
        adt7301
        fpga
    )
endif()

if(${BOARD} STREQUAL "tdc64vle")
    set(BOARD_COMPONENTS
        ad9516
        ad9545
        digipot
        ethernet
        fpga
        max31725
        powermon
        sfpiic
        sensors
        tmp421
        tqdc_clkmux
        vxsiics
    )
endif()

if(${BOARD} STREQUAL "tqdc")
    set(BOARD_COMPONENTS
        ad9516
        ad9545
        digipot
        ethernet
        fpga
        max31725
        powermon
        sfpiic
        sensors
        tmp421
        tqdc_clkmux
        vxsiics
    )
endif()

list(APPEND BOARD_COMPONENTS ${COMMON_COMPONENTS})
message("${BOARD} components: ${BOARD_COMPONENTS}")
