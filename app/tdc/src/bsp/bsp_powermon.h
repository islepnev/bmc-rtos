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

#ifndef BSP_POWERMON_H
#define BSP_POWERMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dev_pm_sensors_config.h"
#include "powermon/dev_powermon_types.h"

struct pm_switches;

int monIsOn(const pm_switches sw, SensorIndex index);

bool readLiveInsertPin(void);

void init_power_switches(pm_switches sw);
void read_power_switches_state(pm_switches sw_state);
void write_power_switches(pm_switches sw);

void init_pgood(pm_pgoods pgood);
void pm_read_pgood(pm_pgoods pgood);
bool get_all_pgood(const pm_pgoods pgood);

bool get_input_power_valid(const pm_sensors_arr sensors);
bool get_input_power_normal(const pm_sensors_arr sensors);
bool get_input_power_failed(const pm_sensors_arr sensors);

double pm_get_power_w(const Dev_powermon *pm);
double pm_get_power_max_w(const Dev_powermon *pm);

void bsp_update_system_powergood_pin(bool power_good);

void switch_power(Dev_powermon *pm, bool state);

#ifdef __cplusplus
}
#endif

#endif // BSP_POWERMON_H
