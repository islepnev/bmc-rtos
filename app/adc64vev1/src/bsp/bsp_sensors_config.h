/*
**    Copyright 2019-2021 Ilja Slepnev
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

// dummy definitions

#ifndef BSP_SENSORS_CONFIG_H
#define BSP_SENSORS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

enum {POWERMON_SENSORS = 0};

typedef enum {
    SENSOR_UNUSED
} SensorIndex;

extern const SensorIndex input_power_sensor;

struct pm_sensors_arr;
void bsp_pm_sensors_arr_init(struct pm_sensors_arr *arr);

#ifdef __cplusplus
}
#endif

#endif // BSP_SENSORS_CONFIG_H
