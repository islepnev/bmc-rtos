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

#include "dev_powermon_types.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void struct_powermon_sensors_init(Dev_powermon *d);
void struct_powermon_init(Dev_powermon *d);
//int readPowerGoodFpga();
//int readPowerGood1v5();
int pm_read_liveInsert(Dev_powermon *pm);
void pm_read_pgood(Dev_powermon *pm);
bool update_power_switches(Dev_powermon *pm, bool state);
//int monIsOn(const pm_switches *sw, SensorIndex index);
void monClearMeasurements(Dev_powermon *d);
int monDetect(Dev_powermon *d);
int monReadValues(Dev_powermon *d);
int pm_sensors_isAllValid(const Dev_powermon *d);
uint32_t getMonStateTicks(const Dev_powermon *pm);
MonState runMon(Dev_powermon *pm);
bool get_input_power_valid(const Dev_powermon *pm);
bool get_input_power_normal(const Dev_powermon *pm);
bool get_input_power_failed(const Dev_powermon *pm);
bool get_critical_power_valid(const Dev_powermon *pm);
bool get_critical_power_failure(const Dev_powermon *pm);
bool get_all_pgood(const Dev_powermon *pm);
void update_system_powergood_pin(const Dev_powermon *pm);
double pm_get_power_w(const Dev_powermon *pm);
double pm_get_power_max_w(const Dev_powermon *pm);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_H
