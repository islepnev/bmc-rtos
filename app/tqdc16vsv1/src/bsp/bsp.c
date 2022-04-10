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

// dummy board specific functions

#include "bsp.h"

#include <stdbool.h>

int pcb_version = 0;

uint32_t read_pcb_version(void)
{
    return 0;
}

void update_board_version(int powermon_count)
{
    // nothing
}

bool fpga_done_pin_present(void)
{
    return false;
}

void sfpiic_switch_enable(bool enable)
{
    (void) enable;
}

void bsp_update_system_powergood_pin(bool power_good)
{
    // write_gpio_pin(PGOOD_PWR_GPIO_Port,   PGOOD_PWR_Pin, power_good);
}
