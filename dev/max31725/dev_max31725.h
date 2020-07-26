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

#ifndef DEV_MAX31725_H
#define DEV_MAX31725_H

#include <stdbool.h>
#include <stdint.h>

#include "bus/bus_types.h"
#include "dev_common_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Dev_max31725 {
    BusInterface bus;
    DeviceStatus present;
    double temp;
} Dev_max31725;

void dev_max31725_init(BusInterface *bus);
bool dev_max31725_detect(Dev_max31725 *d);
bool dev_max31725_read(Dev_max31725 *d);

#ifdef __cplusplus
}
#endif

#endif // DEV_MAX31725_H
