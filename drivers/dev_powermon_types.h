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

#include "dev_pot.h"

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
    double current_lsb;
    uint16_t cal;
    const char *label;
    // measurements
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
} pm_sensor;

typedef enum {
    PM_STATE_INIT,
    PM_STATE_STANDBY,
    PM_STATE_RAMP,
    PM_STATE_RUN,
    PM_STATE_OFF,
    PM_STATE_PWRFAIL,
    PM_STATE_OVERHEAT
} PmState;

typedef struct pm_switches {
    bool switch_5v;
    bool switch_3v3;
    bool switch_1v5;
    bool switch_1v0;
    bool switch_tdc_a;
    bool switch_tdc_b;
    bool switch_tdc_c;
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
   bool fpga_core_pgood;
   bool ltm_pgood;
   pm_switches sw;
   pm_switches sw_state;
   Dev_pots pots;
} Dev_powermon;

SensorStatus pm_sensors_getStatus(const Dev_powermon *d);
SensorStatus getMonStatus(const Dev_powermon *pm);
SensorStatus getPowermonStatus(const Dev_powermon *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_TYPES_H
