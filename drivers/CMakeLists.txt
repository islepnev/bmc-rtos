
file(GLOB Drivers_SRCS
    ad9516/*.c
    ad9516/*.h
    ad9545/*.c
    ad9545/*.h
    bus/*.c
    bus/*.h
    debug_helpers.c
    debug_helpers.h
    dev_ad9545.c
    dev_ad9545.h
    dev_common_types.c
    dev_common_types.h
    dev_eeprom.c
    dev_eeprom.h
    dev_eeprom_types.h
    dev_leds.c
    dev_leds.h
    dev_leds_types.c
    dev_leds_types.h
    dev_mcu.c
    dev_mcu.h
    dev_sfpiic*
    dev_thset.c
    dev_thset.h
    dev_thset_types.h
    error_handler.c
    error_handler.h
    error_handler_impl.c
    error_handler_impl.h
    fpga_spi_hal.c
    fpga_spi_hal.h
    ina226/*.c
    ina226/*.h
    led_gpio_hal.c
    led_gpio_hal.h
    os_serial_tty.c
    os_serial_tty.h
    powermon_i2c_driver.c
    powermon_i2c_driver.h
)

target_include_directories(${ELF} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_sources(${ELF} PRIVATE
    ${Drivers_SRCS}
)
