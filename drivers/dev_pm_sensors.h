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

#include <unistd.h>
#include "stm32f7xx_hal_def.h"
#include "dev_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {POWERMON_SENSORS = 13};

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

typedef struct {
    SensorIndex index;
    DeviceStatus deviceStatus;
    SensorStatus sensorStatus;
    uint32_t lastStatusUpdatedTick;
    uint16_t busAddress;
    int hasShunt;
    float shuntVal;
    float busNomVoltage;
    const char *label;
    // measurements
    float busVoltage;
    float shuntVoltage;
    float current;
} pm_sensor;

void struct_pm_sensor_clear_measurements(pm_sensor *d);
void struct_pm_sensor_init(pm_sensor *d, SensorIndex index);
SensorStatus pm_sensor_status(const pm_sensor *d);
int pm_sensor_isValid(const pm_sensor *d);
void pm_sensor_print(const pm_sensor *d, int isOn);
int pm_sensor_detect(pm_sensor *d);
int pm_sensor_read(pm_sensor *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_H
