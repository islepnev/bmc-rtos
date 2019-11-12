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
#ifndef DEV_PM_SENSORS_TYPES_H
#define DEV_PM_SENSORS_TYPES_H

#include <stdbool.h>

#include "dev_common_types.h"
#include "dev_powermon_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool pm_sensor_isValid(const pm_sensor *d);
bool pm_sensor_isNormal(const pm_sensor *d);
bool pm_sensor_isWarning(const pm_sensor *d);
bool pm_sensor_isCritical(const pm_sensor *d);
SensorStatus pm_sensor_status(const pm_sensor *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_TYPES_H
