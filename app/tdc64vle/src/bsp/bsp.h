/*
**    Copyright 2019-2021 Ilja Slepnev
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

#define BOARD_TDC64VLE 1

#define ENABLE_SENSORS   1

#define LED_HEARTBEAT LED_INT_GREEN

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_BUS_INDEX_AD9516 2

#define AUXPLL_AD9516_OUT6_ENABLE 0
#define AUXPLL_AD9516_OUT7_ENABLE 1
#define AUXPLL_AD9516_OUT8_ENABLE 1
#define AUXPLL_AD9516_OUT9_ENABLE 1

void update_board_version(int powermon_count);
bool fpga_done_pin_present(void);
void sfpiic_switch_enable(bool enable);
void bsp_update_system_powergood_pin(bool power_good);

#ifdef __cplusplus
}
#endif

#endif // BSP_H
