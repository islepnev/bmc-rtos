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

#ifndef DEVICES_H
#define DEVICES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "dev_common_types.h"

struct Dev_thset;
struct Dev_at24c;
struct Dev_pll;
struct Dev_powermon;
struct Devices;

void struct_thset_init(struct Dev_thset *d);

DeviceStatus getDeviceStatus(const struct Devices *d);
PgoodState dev_readPgood(struct Dev_powermon *pm);
void dev_thset_read(struct Dev_thset *d);
//void dev_print_thermometers(const Devices *dev);

//void dev_thset_read(Dev_thset *d);
//void dev_thset_print(const Dev_thset d);
SensorStatus dev_thset_thermStatus(const struct Dev_thset *d);

#ifdef __cplusplus
}
#endif

#endif // DEVICES_H
