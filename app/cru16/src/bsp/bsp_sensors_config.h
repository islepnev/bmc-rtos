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

// CRU16 board specific definitions

#ifndef BSP_SENSORS_CONFIG_H
#define BSP_SENSORS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

enum {POWERMON_SENSORS = 14};
#define SENSOR_MINIMAL_SHUNT_VAL 1.0e-6

typedef enum {
    SENSOR_5VPC,
    SENSOR_VME_3V3,
    SENSOR_VME_5V,
    SENSOR_MCB_4V5,
    SENSOR_MCB_3V3,
    SENSOR_5V,
    SENSOR_1V5,
    SENSOR_3V3,
    SENSOR_CLK_3V3,
    SENSOR_CLK_2V5,
    SENSOR_FPGA_CORE_1V0,
    SENSOR_FPGA_MGT_1V0,
    SENSOR_FPGA_MGT_1V2,
    SENSOR_FPGA_1V8
} SensorIndex;

extern const SensorIndex input_power_sensor;

struct pm_sensors_arr;
void bsp_pm_sensors_arr_init(struct pm_sensors_arr *arr);

#ifdef __cplusplus
}
#endif

#endif // BSP_SENSORS_CONFIG_H
