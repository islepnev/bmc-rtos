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

#include "mcb.h"

#include <stdbool.h>

#include "gpio.h"
#include "bsp_pin_defs.h"

uint32_t read_mcb_pcb_ver(void)
{
    bool a0 = read_gpio_pin(PCB_VER_A0_GPIO_Port, PCB_VER_A0_Pin);
    bool a1 = read_gpio_pin(PCB_VER_A1_GPIO_Port, PCB_VER_A1_Pin);
    return a1 * 2 + a0;
}
