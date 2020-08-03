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

#ifndef BSP_H
#define BSP_H

#include <stdint.h>
#include <stdbool.h>

#include "mcb.h"

#define BOARD_TTVXS 1

#define LED_HEARTBEAT LED_INT_GREEN

#ifdef __cplusplus
extern "C" {
#endif

//#define hi2c_ad9545 hi2c4
//#define hi2c_sensors hi2c2
//#define hi2c_sfpiic hi2c2
//#define vxsiic_hi2c hi2c1
//#define hi2c_eeprom_cfg hi2c3 // mezzanine onboard eeprom

//#define ad9545_deviceAddr 0x4A
//#define eeprom_cfg_deviceAddr 0x50

#ifdef TTVXS_1_0
#define therm_spi hspi2
#else
#define AD9516_SPI_BUS_INDEX 2
#endif

#define AUXPLL_AD9516_OUT6_ENABLE 1
#define AUXPLL_AD9516_OUT7_ENABLE 0
#define AUXPLL_AD9516_OUT8_ENABLE 1
#define AUXPLL_AD9516_OUT9_ENABLE 1

void update_board_version(int powermon_count);
bool fpga_done_pin_present(void);
void sfpiic_switch_enable(bool enable);

#ifdef __cplusplus
}
#endif

#endif // BSP_H
