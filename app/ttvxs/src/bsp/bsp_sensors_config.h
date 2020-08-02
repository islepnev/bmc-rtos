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

// TTVXS board specific definitions

#ifndef BSP_SENSORS_CONFIG_H
#define BSP_SENSORS_CONFIG_H

#include <stdbool.h>

#include "bus/bus_types.h"
#ifdef __cplusplus
extern "C" {
#endif

enum {POWERMON_SENSORS_PCB_1_0 = 13};
enum {POWERMON_SENSORS_PCB_1_1 = 16};
#ifdef TTVXS_1_0
enum {POWERMON_SENSORS = POWERMON_SENSORS_PCB_1_0};
#else
enum {POWERMON_SENSORS = POWERMON_SENSORS_PCB_1_1};
#endif

typedef enum {
    SENSOR_VXS_8A_5V,
    SENSOR_VXS_1A5_5V,
    SENSOR_5VPC, // new in TTVXS v1.1
    SENSOR_5V,
    SENSOR_VPC_3V3,
    SENSOR_MCB_4V5, // new in TTVXS v1.1
    SENSOR_MCB_3V3, // new in TTVXS v1.1
    SENSOR_3V3,
    SENSOR_2V5,
    SENSOR_CLOCK_2V5,
    SENSOR_FPGA_CORE_1V0,
    SENSOR_FPGA_MGT_1V0,
    SENSOR_FPGA_MGT_1V2,
    SENSOR_FPGA_1V8,
    SENSOR_FMC_VADJ,
    SENSOR_FMC_12V
} SensorIndex;

extern const SensorIndex input_power_sensor;

struct pm_sensors_arr;
void bsp_pm_sensors_arr_init(struct pm_sensors_arr *arr);

#ifdef __cplusplus
}
#endif

#endif // BSP_SENSORS_CONFIG_H
