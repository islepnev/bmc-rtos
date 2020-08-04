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

#ifndef DEV_THSET_H
#define DEV_THSET_H

#include <stdbool.h>
#include <stdint.h>

#include "dev_common_types.h"
#include "dev_thset_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool dev_thset_add(Dev_thset *d, const char *name);

void dev_thset_run(Dev_thset *d);
SensorStatus dev_thset_thermStatus(void);
//int16_t adt7301_convert_temp_adt7301_scale32(int16_t raw);
//thset_state_t thermal_shutdown_check(Dev_thset *d);
void clear_thermal_shutdown(void);
thset_state_t get_thset_state(void);
SensorStatus dev_thset_sensor_status(const GenericSensor *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_THSET_H
