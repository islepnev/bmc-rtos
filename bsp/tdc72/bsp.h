/*
**    Copyright 2019 Ilja Slepnev
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

// TDC72VHL v4 board specific definitions

#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LED_HEARTBEAT LED_INT_GREEN


#define ad9545_deviceAddr 0x4A

#define hi2c_ad9545 hi2c2
#define hi2c_sensors hi2c4
#define hi2c_sfpiic hi2c4

#define fpga_spi hspi1
#define therm_spi hspi4

#define hi2c_eeprom_cfg hi2c2
#define eeprom_cfg_deviceAddr 0x50

typedef enum {
    PCB_4_1 = 0,
    PCB_4_2 = 1
} board_version_t;

// run-time detected board version (see i2c supply sensors)
extern board_version_t board_version;

void update_board_version(int powermon_count, int pots_count);

#ifdef __cplusplus
}
#endif

#endif // BSP_H
