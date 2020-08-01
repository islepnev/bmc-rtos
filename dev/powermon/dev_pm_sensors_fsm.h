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

#ifndef DEV_PM_SENSORS_FSM_H
#define DEV_PM_SENSORS_FSM_H

#include <stdbool.h>
#include "bus/bus_types.h"
#include "dev_powermon_types.h"

#ifdef __cplusplus
extern "C" {
#endif

MonState runMon(Dev_powermon *pm);

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_FSM_H
