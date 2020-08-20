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

// TDC72VHL v3 board specific functions

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
    return 0;
}

void update_board_version(int powermon_count)
{
    (void)powermon_count;
    // nothing
}

bool fpga_done_pin_present(void)
{
    return false;
}

void sfpiic_switch_enable(bool enable)
{
    (void)enable;
}
