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

#include <stdbool.h>
#include "dev_powermon_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void struct_powermon_sensors_init(Dev_powermon *d);
void struct_powermon_init(Dev_powermon *d);
bool pm_read_liveInsert(Dev_powermon *pm);
bool update_power_switches(Dev_powermon *pm, bool state);
void monClearMinMax(Dev_powermon *d);
void monClearMeasurements(Dev_powermon *d);
int monDetect(Dev_powermon *d);
int monReadValues(Dev_powermon *d);
bool pm_sensors_isAllValid(const Dev_powermon *d);
uint32_t getMonStateTicks(const Dev_powermon *pm);
MonState runMon(Dev_powermon *pm);
bool get_critical_power_valid(const pm_sensors_arr sensors);
bool get_fpga_core_power_present(const pm_sensors_arr sensors);
bool get_critical_power_failure(const pm_sensors_arr sensors);
void update_system_powergood_pin(const pm_sensors_arr sensors);
double pm_get_power_w(const Dev_powermon *pm);
double pm_get_power_max_w(const Dev_powermon *pm);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_H
