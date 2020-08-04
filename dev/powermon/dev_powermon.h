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

#ifndef DEV_POWERMON_H
#define DEV_POWERMON_H

#include <stdbool.h>
#include "bus/bus_types.h"
#include "dev_powermon_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void create_sensor_subdevices(Dev_powermon *d);
void struct_powermon_sensors_clear(Dev_powermon_priv *p);
void pm_clear_all(Dev_powermon_priv *p);
bool pm_read_liveInsert(Dev_powermon_priv *p);
bool update_power_switches(Dev_powermon_priv *p, bool state);
void monClearMinMax(Dev_powermon *d);
void monClearMeasurements(Dev_powermon *d);
int monDetect(Dev_powermon *d);
int monReadValues(Dev_powermon *d);
//bool pm_sensors_isAllValid(const Dev_powermon *d);
uint32_t getMonStateTicks(const Dev_powermon *pm);
MonState runMon(Dev_powermon *pm);
bool get_critical_power_valid(const pm_sensors_arr *sensors);
bool get_fpga_core_power_present(const pm_sensors_arr *sensors);
bool get_critical_power_failure(const pm_sensors_arr *sensors);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_H
