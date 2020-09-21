/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef AD9548_H
#define AD9548_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"

typedef enum {
    BOARD_PLL_DEFAULT,
    BOARD_PLL_ADC64VE,
    BOARD_PLL_TDC_VHLE,
    BOARD_PLL_TQDC16VS
} AD9548_BOARD_PLL_VARIANT;

#ifdef __cplusplus
extern "C" {
#endif

void ad9548_gpio_init(BusInterface *bus);

bool ad9548_gpio_test(BusInterface *bus)
    __attribute__((warn_unused_result));

bool ad9548_write_reset_pin(BusInterface *bus, bool reset)
    __attribute__((warn_unused_result));

bool ad9548_read_register(BusInterface *bus, uint16_t address, uint8_t *data)
    __attribute__((warn_unused_result));

bool ad9548_write_register(BusInterface *bus, uint16_t address, uint8_t value)
    __attribute__((warn_unused_result));

bool ad9548_ioupdate(BusInterface *bus)
    __attribute__((warn_unused_result));

bool ad9548_detect(BusInterface *bus)
    __attribute__((warn_unused_result));

#ifdef __cplusplus
}
#endif

#endif // AD9548_H
