/*
**    Copyright 2019-2020 Ilja Slepnev
**
**    This program is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    This program is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "init_periph.h"

#include "adc.h"
#include "rtc.h"
#include "spi.h"
#include "usart.h"

#include "ansi_escape_codes.h"
#include "bus/spi_driver.h"
#include "adc_driver.h"
#include "debug_helpers.h"

void init_periph(void)
{
    debug_print("ADC init...");
    MX_ADC1_Init();
    debug_print(" Ok\n");

    debug_print("RTC init...");
    MX_RTC_Init();
    debug_print(" Ok\n");

    // i2c_driver_init();

    debug_print("SPI init...");
    spi_driver_init();
    init_spi_peripherals();
    debug_print(" Ok\n");
}
