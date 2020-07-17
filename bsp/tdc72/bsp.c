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
#include "dev_pm_sensors_config.h"
#include "logbuffer.h"

uint32_t detect_pcb_version(void)
{
    bool a0 = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PCB_VER_A0_GPIO_Port, PCB_VER_A0_Pin));
    bool a1 = (GPIO_PIN_SET == HAL_GPIO_ReadPin(PCB_VER_A1_GPIO_Port, PCB_VER_A1_Pin));
    return a1 * 2 + a0;
}

board_version_t board_version = PCB_4_1;
void update_board_version(int powermon_count)
{
    if (powermon_count <= POWERMON_SENSORS_PCB_4_1)
        board_version = PCB_4_1;
    else
        board_version = PCB_4_2;
}

bool fpga_done_pin_present(void)
{
    return board_version >= PCB_4_2;
}
