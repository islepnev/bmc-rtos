/*
**    Copyright 2019-2020 Ilja Slepnev
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
#include <stdint.h>

#include "bus/bus_types.h"
#include "dev_common_types.h"
#include "devicebase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RAMP_NONE = 0,
    RAMP_UP   = 1,
    RAMP_DOWN = 2
} RampState;

typedef struct pm_sensor_priv {
    int index;
    RampState rampState;
    uint32_t lastStatusUpdatedTick;
    // uint16_t busAddress;
    bool isOptional;
    bool hasShunt;
    double shuntVal;
    double busNomVoltage;
    double current_lsb;
    uint16_t cal;
    const char *label;
    double voltageMarginWarn;
    double voltageMarginCrit;
    // measurements
    bool valid;
    double busVoltage;
    //    double shuntVoltage;
    double current;
    double power;
    // calculated
    double busVoltageMin;
    double busVoltageMax;
    double currentMin;
    double currentMax;
    double powerMax;
} pm_sensor_priv;

typedef struct pm_sensor {
    DeviceBase dev;
    pm_sensor_priv priv;
} pm_sensor;

enum { MAX_POWERMON_SENSORS = 16 };
typedef struct pm_sensors_arr {
    pm_sensor arr[MAX_POWERMON_SENSORS];
    int count;
} pm_sensors_arr;

typedef enum MonState {
    MON_STATE_INIT = 0,
    MON_STATE_DETECT = 1,
    MON_STATE_READ = 2,
    MON_STATE_ERROR = 3
} MonState;

typedef struct Dev_pm_sensors_priv {
    int unused;// TODO
} Dev_pm_sensors_priv;

typedef struct Dev_pm_sensors {
    DeviceBase dev;
    Dev_pm_sensors_priv priv;
} Dev_pm_sensors;

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_TYPES_H
