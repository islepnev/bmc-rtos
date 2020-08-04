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

#ifndef DEV_PLL_H
#define DEV_PLL_H

#include <stdbool.h>

#include "ad9545_setup_regs.h"
#include "ad9545_status_regs.h"
#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_ad9545_setup(ad9545_setup_t *setup);
bool ad9545_detect(BusInterface *bus);
bool ad9545_setup(BusInterface *bus, const ad9545_setup_t *setup);

bool ad9545_software_reset(BusInterface *bus);
bool ad9545_setup_sysclk(BusInterface *bus, const PllSysclkSetup_TypeDef *sysclkSetup);
bool ad9545_calibrate_sysclk(BusInterface *bus);
bool ad9545_read_status(BusInterface *bus, AD9545_Status *status);
bool ad9545_read_sysclk_status(BusInterface *bus, AD9545_Status *status);

void ad9545_reset(BusInterface *bus);
bool ad9545_gpio_test(BusInterface *bus);
void ad9545_gpio_init(BusInterface *bus);

#ifdef __cplusplus
}
#endif

#endif // DEV_PLL_H
