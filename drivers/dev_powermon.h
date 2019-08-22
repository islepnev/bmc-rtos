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

#ifndef DEV_POWERMON_H
#define DEV_POWERMON_H

#include <unistd.h>
#include "stm32f7xx_hal_def.h"
#include "dev_types.h"
#include "dev_pm_sensors.h"
#include "dev_pot.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum SwitchOnOff {
    SWITCH_OFF = 0,
    SWITCH_ON = 1,
} SwitchOnOff;

typedef struct pm_switches {
    SwitchOnOff switch_5v;
    SwitchOnOff switch_3v3;
    SwitchOnOff switch_1v5;
    SwitchOnOff switch_1v0;
    SwitchOnOff switch_tdc_a;
    SwitchOnOff switch_tdc_b;
    SwitchOnOff switch_tdc_c;
} pm_switches;

typedef enum MonState {
    MON_STATE_INIT = 0,
    MON_STATE_DETECT = 1,
    MON_STATE_READ = 2,
    MON_STATE_ERROR = 3
} MonState;

typedef struct Dev_powermon {
    MonState monState;
    uint32_t stateStartTick;
    int monErrors;
    int monCycle;
//    DeviceStatus present;
   pm_sensor sensors[POWERMON_SENSORS];
   int vmePresent;
   int fpga_core_pgood;
   int ltm_pgood;
   pm_switches sw;
   pm_switches sw_state;
   Dev_pots pots;
} Dev_powermon;

void struct_powermon_sensors_init(Dev_powermon *d);
void struct_powermon_init(Dev_powermon *d);
//int readPowerGoodFpga();
//int readPowerGood1v5();
int pm_read_liveInsert(Dev_powermon *pm);
void pm_read_pgood(Dev_powermon *pm);
void update_power_switch_state(Dev_powermon *pm);
void update_power_switches(Dev_powermon *pm, SwitchOnOff state);
int monIsOn(const pm_switches *sw, SensorIndex index);
void monClearMinMax(Dev_powermon *d);
void monClearMeasurements(Dev_powermon *d);
int monDetect(Dev_powermon *d);
int monReadValues(Dev_powermon *d);
int pm_sensors_isAllValid(const Dev_powermon *d);
SensorStatus pm_sensors_getStatus(const Dev_powermon *d);
uint32_t getMonStateTicks(const Dev_powermon *pm);
MonState runMon(Dev_powermon *pm);
int get_input_power_valid(const Dev_powermon *pm);
int get_critical_power_valid(const Dev_powermon *pm);
int get_fpga_core_power_present(const Dev_powermon *pm);
PgoodState get_all_pgood(const Dev_powermon *pm);
double pm_get_power_w(const Dev_powermon *pm);
double pm_get_power_max_w(const Dev_powermon *pm);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_H
