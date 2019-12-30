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

// TDC72VHL v4 board specific functions

#include "bsp.h"

#include <stdbool.h>

#include "bsp_pin_defs.h"
#include "stm32f7xx_hal_gpio.h"
#include "i2c.h"
#include "bus/i2c_driver.h"
#include "spi.h"
#include "dev_pm_sensors_config.h"
#include "dev_pot.h"
#include "logbuffer.h"

board_version_t board_version = PCB_4_1;
void update_board_version(int powermon_count, int pots_count)
{
    if ((pots_count == 0) && (powermon_count <= POWERMON_SENSORS_PCB_4_1))
        board_version = PCB_4_1;
    if ((pots_count > 0) && (powermon_count > POWERMON_SENSORS_PCB_4_1))
        board_version = PCB_4_2;
    if (pots_count == DEV_POT_COUNT)
        board_version = PCB_4_2;
    if (powermon_count == POWERMON_SENSORS_PCB_4_2)
        board_version = PCB_4_2;
}
