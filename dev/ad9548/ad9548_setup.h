/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef AD9548_SETUP_H
#define AD9548_SETUP_H

#include <stdbool.h>

#include "bus/bus_types.h"
#include "ad9548.h"
#include "ad9548_setup_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ad9548_software_reset(BusInterface *bus);
void ad9548_output_sync(BusInterface *bus);
void ad9548_poll_irq_state(BusInterface *bus);
bool ad9548_setup_sysclk(BusInterface *bus, ad9548_setup_t *reg);
bool ad9548_calibrate_sysclk(BusInterface *bus, ad9548_setup_t *reg);
bool ad9548_setup(BusInterface *bus, ad9548_setup_t *reg);
//bool ad9548_ProfileConfig(BusInterface *bus, ad9548_setup_t *reg);
//void ad9548_Phase_Shift_Right(BusInterface *bus);
//void ad9548_Phase_Shift_Left(BusInterface *bus);
//void ad9548_Phase_Reset(BusInterface *bus);
//void ad9548_setProfile(ad9548_setup_t *reg, AD9548_BOARD_PLL_VARIANT variant);
void init_ad9548_setup(ad9548_setup_t *setup);

#ifdef __cplusplus
}
#endif

#endif // AD9548_SETUP_H
