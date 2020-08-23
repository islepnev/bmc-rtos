/*
**    Copyright 2020 Ilja Slepnev
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

#ifndef DEV_POWERMON_DISPLAY_H
#define DEV_POWERMON_DISPLAY_H

#include <stdbool.h>

#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

enum { DISPLAY_POWERMON_H = 3 };

struct pm_sensor;
void pm_sensor_print_header(void);
void pm_sensor_print_values(const struct pm_sensor *d, bool isOn);
void print_powermon_box(void);
void print_sensors_box(void);

#ifdef __cplusplus
}
#endif

#endif // DEV_POWERMON_DISPLAY_H
