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
#define SENSOR_MINIMAL_SHUNT_VAL 1.0e-6

typedef enum {
    SENSOR_5VPC, // new in TTVXS v1.1
    SENSOR_VPC_3V3,
    SENSOR_VXS_5V,
    SENSOR_MCB_4V5, // new in TTVXS v1.1
    SENSOR_MCB_3V3, // new in TTVXS v1.1
    SENSOR_5V,
    SENSOR_2V5,
    SENSOR_3V3,
    SENSOR_CLOCK_2V5,
    SENSOR_FPGA_CORE_1V0,
    SENSOR_FPGA_MGT_1V0,
    SENSOR_FPGA_MGT_1V2,
    SENSOR_FPGA_1V8,
    SENSOR_VADJ,
    SENSOR_FMC_5V,
    SENSOR_FMC_12V
} SensorIndex;

extern const SensorIndex input_power_sensor;

bool monIsOptional(SensorIndex index);
double monShuntVal(SensorIndex index);
double monVoltageMarginWarn(SensorIndex index);
double monVoltageMarginCrit(SensorIndex index);
double monVoltageNom(SensorIndex index);
int sensorBusNumber(SensorIndex index);
int sensorBusAddress(SensorIndex index);
const char *monLabel(SensorIndex index);

#ifdef __cplusplus
}
#endif

#endif // BSP_SENSORS_CONFIG_H
