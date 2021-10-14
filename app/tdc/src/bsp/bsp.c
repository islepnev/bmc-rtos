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

// TDC board specific functions

#include "bsp.h"

#include <stdbool.h>

#include "bsp_pin_defs.h"
#include "bsp_sensors_config.h"
#include "gpio.h"
#include "i2c.h"
#include "log/log.h"

int pcb_version = 0;

uint32_t read_pcb_version(void)
{
#if defined(BOARD_TDC72VHLV2) || defined (BOARD_TDC72VHLV3)
    return 0;
#else
    bool a0 = read_gpio_pin(PCB_VER_A0_GPIO_Port, PCB_VER_A0_Pin);
    bool a1 = read_gpio_pin(PCB_VER_A1_GPIO_Port, PCB_VER_A1_Pin);
    return a1 * 2 + a0;
#endif
}

#ifdef BOARD_TDC72

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

#else

void update_board_version(int powermon_count)
{
    // nothing
}

bool fpga_done_pin_present(void)
{
#if defined(BOARD_TDC72VHLV2) || defined (BOARD_TDC72VHLV3)
    return false;
#else
    return true;
#endif
}
#endif

void sfpiic_switch_enable(bool enable)
{
    write_gpio_pin(MON_SMB_SW_RST_B_GPIO_Port,  MON_SMB_SW_RST_B_Pin, enable);
}

void bsp_update_system_powergood_pin(bool power_good)
{
#if defined(BOARD_TDC64) || defined(BOARD_TDC72)
    write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, power_good);
#endif
}
