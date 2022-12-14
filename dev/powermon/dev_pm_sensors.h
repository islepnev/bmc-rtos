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

#ifndef DEV_PM_SENSORS_H
#define DEV_PM_SENSORS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//#include "bsp_sensors_config.h"
#include "dev_pm_sensors_types.h"

void struct_pm_sensor_clear_minmax(pm_sensor_priv *d);
void struct_pm_sensor_clear_measurements(pm_sensor_priv *d);
void struct_pm_sensor_clear(pm_sensor *d);

uint32_t pm_sensor_get_sensorStatus_Duration(const pm_sensor_priv *d);
SensorStatus pm_sensor_compute_status(const pm_sensor *d);

double get_sensor_power_w(const pm_sensor *d);

SensorStatus pm_sensor_status(const pm_sensor *d);
bool pm_sensor_isValid(const pm_sensor *d);
bool pm_sensor_isNormal(const pm_sensor *d);
bool pm_sensor_isWarning(const pm_sensor *d);
bool pm_sensor_isCritical(const pm_sensor *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_H
