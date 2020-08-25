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

#ifndef AD9548_STATUS_H
#define AD9548_STATUS_H

#include <stdbool.h>

#include "ad9548_status_regs.h"
#include "bus/bus_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool ad9548_read_status(BusInterface *bus, AD9548_Status *status);
bool ad9548_read_sysclk_status(BusInterface *bus, AD9548_Status *status);
bool ad9548_isDpllLocked(AD9548_Status *status);
bool ad9548_sysclk_is_locked(AD9548_Sysclk_Status_REG_Type sysclk);

#ifdef __cplusplus
}
#endif

#endif // AD9548_STATUS_H
