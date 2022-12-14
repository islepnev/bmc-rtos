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

#ifndef DISPLAY_COMMON_H
#define DISPLAY_COMMON_H

#include <stdbool.h>

#include "dev_common_types.h"

static const int DISPLAY_PAGE_Y = 2;

void display_clear_page(void);
void display_devices_page(int y, bool repaint);
void print_clock(void);
void print_header_line(bool repaint);
void print_footer_line(void);
SensorStatus get_device_sensor_status(DeviceClass device_class);
const char *device_sensor_status_ansi_str(DeviceClass device_class);
void display_device_sensor_ansi_str(const char *name, DeviceClass device_class);
void dev_eeprom_config_print(void);

#endif // DISPLAY_COMMON_H

