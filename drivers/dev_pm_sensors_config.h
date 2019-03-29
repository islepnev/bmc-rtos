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

// TDC72VHL v4 board specific definitions

#ifndef DEV_PM_SENSORS_CONFIG_H
#define DEV_PM_SENSORS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

enum {POWERMON_SENSORS = 13};
#define SENSOR_MINIMAL_SHUNT_VAL 1.0e-6

typedef enum {
    SENSOR_1V5,
    SENSOR_5V,
    SENSOR_VME_5V,
    SENSOR_3V3,
    SENSOR_VME_3V3,
    SENSOR_FPGA_CORE_1V0,
    SENSOR_FPGA_MGT_1V0,
    SENSOR_FPGA_MGT_1V2,
    SENSOR_FPGA_1V8,
    SENSOR_TDC_A,
    SENSOR_TDC_B,
    SENSOR_TDC_C,
    SENSOR_CLOCK_2V5
} SensorIndex;

int monIsOptional(SensorIndex index);
double monShuntVal(SensorIndex index);
double monVoltageMarginWarn(SensorIndex index);
double monVoltageMarginCrit(SensorIndex index);
double monVoltageNom(SensorIndex index);
int sensorBusAddress(SensorIndex index);
const char *monLabel(SensorIndex index);

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_CONFIG_H