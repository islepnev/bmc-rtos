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
#ifndef DEV_POWERMON_TYPES_H
#define DEV_POWERMON_TYPES_H

#include <stdint.h>
#include <stdbool.h>
#include "dev_common_types.h"
#include "dev_pm_sensors_config.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    RAMP_NONE = 0,
    RAMP_UP   = 1,
    RAMP_DOWN = 2,
} RampState;

typedef struct {
    SensorIndex index;
    DeviceStatus deviceStatus;
    SensorStatus sensorStatus;
    RampState rampState;
    uint32_t lastStatusUpdatedTick;
    uint16_t busAddress;
    bool isOptional;
    bool hasShunt;
    double shuntVal;
    double busNomVoltage;
    const char *label;
    // measurements
    double busVoltage;
//    double shuntVoltage;
    double current;
    // calculated
    double busVoltageMin;
    double busVoltageMax;
    double currentMin;
    double currentMax;
} pm_sensor;

typedef enum {
    PM_STATE_INIT,
    PM_STATE_WAITINPUT,
    PM_STATE_STANDBY,
    PM_STATE_RAMP,
    PM_STATE_RUN,
    PM_STATE_PWRFAIL,
    PM_STATE_FAILWAIT,
    PM_STATE_SWITCHOFF,
    PM_STATE_ERROR
} PmState;

typedef struct pm_switches {
    bool switch_5v;
    bool switch_5v_fmc;
    bool switch_3v3;
    bool switch_2v5;
    bool switch_1v0_core;
    bool switch_1v0_mgt;
    bool switch_1v2_mgt; // added in TTVXS v1.1
} pm_switches;

typedef enum MonState {
    MON_STATE_INIT = 0,
    MON_STATE_DETECT = 1,
    MON_STATE_READ = 2,
    MON_STATE_ERROR = 3
} MonState;

typedef struct Dev_powermon {
    PmState pmState;
    MonState monState;
    uint32_t stateStartTick;
    int monErrors;
    int monCycle;
//    DeviceStatus present;
   pm_sensor sensors[POWERMON_SENSORS];
   bool vmePresent;
   bool pgood_3v3;
   bool pgood_2v5;
   bool pgood_1v0_core;
   bool pgood_1v0_mgt;
   bool pgood_1v2_mgt;
   bool pgood_3v3_fmc;
   pm_switches sw;
} Dev_powermon;

int monIsOn(const pm_switches *sw, SensorIndex index);
SensorStatus pm_sensors_getStatus(const Dev_powermon *d);
SensorStatus getMonStatus(const Dev_powermon *pm);
SensorStatus getPowermonStatus(const Dev_powermon *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_TYPES_H
