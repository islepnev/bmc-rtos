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

#include "bsp_sensors_config.h"
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
    SensorIndex index;
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

bool pm_sensor_isValid(const pm_sensor *d);
bool pm_sensor_isNormal(const pm_sensor *d);
bool pm_sensor_isWarning(const pm_sensor *d);
bool pm_sensor_isCritical(const pm_sensor *d);
SensorStatus pm_sensor_status(const pm_sensor *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_PM_SENSORS_TYPES_H
