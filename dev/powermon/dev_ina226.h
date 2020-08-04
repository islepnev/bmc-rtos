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

#ifndef DEV_INA226_H
#define DEV_INA226_H

#include <stdbool.h>

#include "dev_common_types.h"

extern const double SENSOR_MINIMAL_SHUNT_VAL;

struct pm_sensor;
bool dev_ina226_detect(struct pm_sensor *d);
DeviceStatus dev_ina226_read(struct pm_sensor *d);
void pm_sensor_set_sensorStatus(struct pm_sensor *d, SensorStatus status);

#endif // DEV_INA226_H
