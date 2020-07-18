//
//    Copyright 2019 Ilja Slepnev
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef DEV_PM_SENSORS_H
#define DEV_PM_SENSORS_H

#include <stdint.h>

#include "dev_powermon_types.h"

void struct_pm_sensor_clear_minmax(pm_sensor *d);
void struct_pm_sensor_clear_measurements(pm_sensor *d);
void struct_pm_sensor_init(pm_sensor *d, SensorIndex index);
uint32_t pm_sensor_get_sensorStatus_Duration(const pm_sensor *d);
DeviceStatus pm_sensor_detect(pm_sensor *d);
DeviceStatus pm_sensor_read(pm_sensor *d);

#endif // DEV_PM_SENSORS_H
